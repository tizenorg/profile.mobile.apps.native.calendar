/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "cal.h"
#include "cal_books.h"

struct _acct {
    int id;
    char *name;
    char *icon;
    char *domain;
    char *package_name;
    void *data;
};

typedef enum {
	CAL_BOOKS_ACCOUNT_DIRTY_NONE = 0,
	CAL_BOOKS_ACCOUNT_DIRTY_UPDATE,
	CAL_BOOKS_ACCOUNT_DIRTY_CHECKED
} cal_books_account_dirty_e;

typedef struct {
	int id;
	cal_books_account_dirty_e dirty;
	Eina_List* books;
} cal_books_account_s;

typedef struct {
	Eina_List* accounts; /* cal_books_account_s* */
	GHashTable* map;
} cal_books_s;

static inline GHashTable* __initialize_map()
{
	return g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
}

static inline void __finalize_map(GHashTable* map)
{
	if (map)
		g_hash_table_destroy(map);
}

static inline void __add_book_to_map(GHashTable* map, int id, cal_book_h book)
{
	g_hash_table_insert(map, GINT_TO_POINTER(id), book);
}

static inline void __remove_book_from_map(GHashTable* map, int id)
{
	g_hash_table_remove(map, (gconstpointer)id);
}

static inline cal_book_h __find_book_from_map_with_deleted(GHashTable* map, int id)
{
	return g_hash_table_lookup(map, GINT_TO_POINTER(id));
}

static inline cal_book_h __find_book_from_map(GHashTable* map, int id)
{
	cal_book_h lbook = __find_book_from_map_with_deleted(map, id);
	if (lbook && cal_book_get_dirty(lbook) != CAL_BOOK_DIRTY_DELETE)
		return lbook;
	return NULL;
}

static inline cal_book_h __clone_book(const cal_book_h book)
{
	calendar_record_h record = NULL;
	int ret = -1;
	ret = calendar_record_clone((calendar_record_h)cal_book_get_record(book), &record);
	CAL_ASSERT(ret == CALENDAR_ERROR_NONE);
	return cal_book_create_with_record(record);
}

static Eina_Bool __compare_book(const cal_book_h book1, const cal_book_h book2)
{
	CAL_ASSERT(book1 && book2);
	CAL_ASSERT(cal_book_get_id(book1) == cal_book_get_id(book2));
	//CAL_ASSERT(cal_book_get_account_id(book1) == cal_book_get_account_id(book2));
	//CAL_ASSERT(cal_book_get_store_type(book1) == cal_book_get_store_type(book2));
	//CAL_ASSERT(cal_book_get_mode(book1) == cal_book_get_mode(book2));
	//CAL_ASSERT(cal_book_get_sync_data1(book1), cal_book_get_sync_data1(book2) == 0);
	//CAL_ASSERT(cal_book_get_sync_data2(book1), cal_book_get_sync_data2(book2) == 0);
	//CAL_ASSERT(cal_book_get_sync_data3(book1), cal_book_get_sync_data3(book2) == 0);
	//CAL_ASSERT(cal_book_get_sync_data4(book1), cal_book_get_sync_data4(book2) == 0);

	if (cal_book_get_visibility(book1) != cal_book_get_visibility(book2))
		return EINA_FALSE;
	if (g_strcmp0(cal_book_get_name(book1), cal_book_get_name(book2)) != 0)
		return EINA_FALSE;
	if (g_strcmp0(cal_book_get_color(book1), cal_book_get_color(book2)) != 0)
		return EINA_FALSE;
	if (g_strcmp0(cal_book_get_uid(book1), cal_book_get_uid(book2)) != 0)
		return EINA_FALSE;
	if (g_strcmp0(cal_book_get_description(book1), cal_book_get_description(book2)) != 0)
		return EINA_FALSE;
	if (g_strcmp0(cal_book_get_location(book1), cal_book_get_location(book2)) != 0)
		return EINA_FALSE;

	return EINA_TRUE;
}

static calendar_list_h __get_books_from_db_by_account_id(int account_id)
{
	CAL_FN_START;
	CAL_ASSERT(account_id >= -1);

	calendar_error_e error = CALENDAR_ERROR_NONE;
	calendar_filter_h filter = NULL;
	calendar_query_h query = NULL;
	calendar_list_h list = NULL;

	do
	{
		error = calendar_query_create(_calendar_book._uri, &query);
		if (error != CALENDAR_ERROR_NONE)
			break;

		if (account_id != _CALENDAR_ALL_ACCOUNT_ID)
		{
			error = calendar_filter_create(_calendar_book._uri, &filter);
			if (error != CALENDAR_ERROR_NONE)
				break;
			error = calendar_filter_add_int(filter, _calendar_book.account_id, CALENDAR_MATCH_EQUAL, account_id);
			if (error != CALENDAR_ERROR_NONE)
				break;
			error = calendar_query_set_filter(query, filter);
			if (error != CALENDAR_ERROR_NONE)
				break;
		}

		error = calendar_db_get_records_with_query(query, 0, 0, &list);
		if (error != CALENDAR_ERROR_NONE)
			break;
	}
	while(0);

	if (filter)
		calendar_filter_destroy(filter);

	if (query)
		calendar_query_destroy(query);

	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return list;
}

static cal_book_h __insert_book_to_memory(cal_books_s* books, cal_books_account_s* account, const cal_book_h book)
{
	CAL_FN_START;
	cal_book_h lbook = __clone_book(book);
	cal_book_set_account_id(lbook, account->id);

	// add it to the list
	account->books = eina_list_append(account->books, lbook);

	// add it to the map
	__add_book_to_map(books->map, cal_book_get_id(lbook), lbook);

	return lbook;
}

static Eina_Bool __update_book_to_memory(cal_books_s* books, cal_book_h book, const cal_book_h changedbook)
{
	CAL_FN_START;
	if (__compare_book(book, changedbook)) /* Their records are same */
		return EINA_FALSE;

	calendar_record_h record = NULL;
	int ret = calendar_record_clone((calendar_record_h)cal_book_get_record(changedbook), &record);
	CAL_ASSERT(ret == CALENDAR_ERROR_NONE);
	cal_book_set_record(book, record);
	return EINA_TRUE;
}

static void __delete_book_from_memory_by_node(cal_books_s* books, cal_books_account_s* account, Eina_List* node)
{
	CAL_FN_START;
	cal_book_h lbook = eina_list_data_get(node);
	CAL_ASSERT(lbook);

	// remove it from the map
	__remove_book_from_map(books->map, cal_book_get_id(lbook));

	// remove it from the list
	account->books = eina_list_remove_list(account->books, node);
	cal_book_destroy(lbook);
}

static Eina_Bool __delete_unchecked_books_from_memory(cal_books_s* books, cal_books_account_s* account)
{
	CAL_FN_START;
	Eina_Bool is_changed = EINA_FALSE;
	Eina_List* node = NULL;
	Eina_List* current_node = NULL;
	cal_book_h book = NULL;
	cal_book_dirty_e dirty = CAL_BOOK_DIRTY_NONE;
	for (node = account->books; node != NULL; )
	{
		current_node = node;
		node = eina_list_next(node);
		book = (cal_book_h)eina_list_data_get(current_node);
		dirty = cal_book_get_dirty(book);
		switch (dirty)
		{
			case CAL_BOOK_DIRTY_NONE:
				__delete_book_from_memory_by_node(books, account, current_node);
				is_changed = EINA_TRUE;
				break;
			case CAL_BOOK_DIRTY_CHECKED:
				break;
			default:
				ERR("Internal error");
				CAL_ASSERT(0);
				break;
		}
	}
	return is_changed;
}

static cal_books_account_s*	__add_account(cal_books_s* books, int account_id)
{
	CAL_FN_START;
	cal_books_account_s* account = (cal_books_account_s*)g_new0(cal_books_account_s, 1);
	account->id = account_id;
	books->accounts = eina_list_append(books->accounts, account);
	return account;
}

static void	__remove_account_by_node(cal_books_s* books, Eina_List* node)
{
	CAL_FN_START;
	cal_books_account_s* account = (cal_books_account_s*)eina_list_data_get(node);
	CAL_ASSERT(account->books == NULL);
	g_free(account);
	books->accounts = eina_list_remove_list(books->accounts, node);
}

static cal_books_account_s* __find_account_by_id(cal_books_s* books, int account_id)
{
	CAL_FN_START;
	Eina_List* node = NULL;
	cal_books_account_s* account = NULL;
	EINA_LIST_FOREACH(books->accounts, node, account)
	{
		if (account->id == account_id)
			return account;
	}
	return NULL;
}

static Eina_Bool __update_books_from_db_by_account_id(cal_books_s* books, int account_id)
{
	CAL_FN_START;
	Eina_Bool is_changed = EINA_FALSE;
	Eina_Bool is_changed2 = EINA_FALSE;
	DBG("updateing an account(id = %d)", account_id);
	cal_books_account_s* account = __find_account_by_id(books, account_id);
	if (account == NULL)
	{
		// add an account
		account = __add_account(books, account_id);
		DBG("A new account(id = %d) has been added", account_id);
	}

	account->dirty = CAL_BOOKS_ACCOUNT_DIRTY_CHECKED;
	calendar_list_h book_list = __get_books_from_db_by_account_id(account_id);
	if (book_list)
	{
		int count = 0;
		calendar_list_get_count(book_list, &count);
		if (count != eina_list_count(account->books))
			is_changed = EINA_TRUE;
		DBG("book count of the acount(id = %d) = %d:%d ", account_id, count, eina_list_count(account->books));

		int i = 0;
		int id = -1;
		cal_book_h lbook = NULL;
		cal_book_h copied_lbook = cal_book_create();
		calendar_record_h record = NULL;
		for (i = 0; i < count; i++)
		{
			calendar_list_get_current_record_p(book_list, &record);
			CAL_ASSERT(record);

			cal_book_set_record(copied_lbook, record); /* The previous record is released at the same time */

			calendar_record_get_int(record, _calendar_book.id, &id);
			lbook = __find_book_from_map(books->map, id);
			if (lbook)
			{
				is_changed2 = __update_book_to_memory(books, lbook, copied_lbook);
				if (is_changed2)
					is_changed = EINA_TRUE;
			}
			else
			{
				lbook = __insert_book_to_memory(books, account, copied_lbook);
				is_changed = EINA_TRUE;
			}
			CAL_ASSERT(cal_book_get_dirty(lbook) == CAL_BOOK_DIRTY_NONE);
			cal_book_set_dirty(lbook, CAL_BOOK_DIRTY_CHECKED);
			calendar_list_next(book_list);
		}
		cal_book_destroy(copied_lbook); /* The last record is released at the same time */
		calendar_list_destroy(book_list, false);
	}

	is_changed2 = __delete_unchecked_books_from_memory(books, account);
	if (is_changed2)
		is_changed = EINA_TRUE;

	return is_changed;
}

static cal_book_rgb_color_s __books_default_rgb_color[] = {
	{188, 229, 92, 255,},
	{221, 129, 0, 255,},
	{0, 182, 252, 255,},
	{122, 52, 122, 255,},
	{22, 139, 121, 255,},
	{40, 81, 163, 255,},
};
static const cal_book_rgb_color_s* __get_random_rgb_color(int id)
{
	return &(__books_default_rgb_color[id % 6]);
}

static void __set_color_of_colorless_books(cal_books_s* books)
{
	Eina_List* node = NULL;
	cal_books_account_s* account = NULL;
	cal_book_h book = NULL;
	Eina_List* node2 = NULL;
	int id = -1;
	const char* color = NULL;
	const cal_book_rgb_color_s* rgb_color = NULL;
	EINA_LIST_FOREACH(books->accounts, node, account)
	{
		EINA_LIST_FOREACH(account->books, node2, book)
		{
			id = cal_book_get_id(book);
			color = cal_book_get_color(book);
			DBG("book id = %d, color = %s", id, color);
			if (color == NULL || strlen(color) == 0)
			{
				DBG("need to set a color for a book(id = %d)", id);
				rgb_color = __get_random_rgb_color(id);
				cal_book_set_color_rgb_struct(book, *rgb_color);
				cal_books_update_book(books, book);
			}
		}
	}
	cal_books_commit_to_db(books);
}

cal_books_h cal_books_initialize()
{
	CAL_FN_START;
	cal_books_s* books = (cal_books_s*)g_new0(cal_books_s, 1);
	books->map = __initialize_map();
	cal_books_update_from_db(books);
	return (cal_books_h)books;
}

static void __destroy_books(Eina_List* books)
{
	CAL_FN_START;
	if (!books)
		return;

	cal_book_h book = NULL;
	Eina_List* node = NULL;
	EINA_LIST_FOREACH(books, node, book)
	{
		cal_book_destroy(book);
	}
	eina_list_free(books);
}

void cal_books_finalize(cal_books_h books)
{
	CAL_FN_START;
	cal_books_s* lbooks = (cal_books_s*)books;
	if (!lbooks)
		return;

	Eina_List* node = NULL;
	cal_books_account_s* account = NULL;
	EINA_LIST_FOREACH(lbooks->accounts, node, account)
	{
		// release map
		__remove_book_from_map(lbooks->map, account->id);
		// release books
		__destroy_books(account->books);
		g_free(account);
	}
	eina_list_free(lbooks->accounts);
	__finalize_map(lbooks->map);
	g_free(lbooks);
}

static Eina_Bool __delete_unchecked_account_from_memory(cal_books_s* books)
{
	CAL_FN_START;
	Eina_Bool is_changed = EINA_FALSE;
	Eina_List* node = NULL;
	Eina_List* current_node = NULL;
	cal_books_account_s* account = NULL;
	for (node = books->accounts; node != NULL; )
	{
		current_node = node;
		node = eina_list_next(node);
		account = (cal_books_account_s*)eina_list_data_get(current_node);
		DBG("account->dirty = %d, account->books = %p", account->dirty, account->books);
		switch (account->dirty)
		{
			case CAL_BOOKS_ACCOUNT_DIRTY_NONE:
				__update_books_from_db_by_account_id(books, account->id);
				/* just remove an account having no books */
				if (account->books == NULL)
				{
					// remove this account
					__remove_account_by_node(books, current_node);
					is_changed = EINA_TRUE;
				}
				break;
			case CAL_BOOKS_ACCOUNT_DIRTY_CHECKED:
				break;
			default:
				ERR("Internal error");
				CAL_ASSERT(0);
				break;
		}
	}
	return is_changed;
}

static void __print_account(cal_books_s* books)
{
	Eina_List* node = NULL;
	cal_books_account_s* account = NULL;
	cal_book_h book = NULL;
	Eina_List* node2 = NULL;
	EINA_LIST_FOREACH(books->accounts, node, account)
	{
		DBG("account id = %d, dirty = %d", account->id, account->dirty);
		EINA_LIST_FOREACH(account->books, node2, book)
		{
			DBG("book id = %d, name = %s, dirty = %d, visibility = %d",
					cal_book_get_id(book),
					cal_book_get_name(book),
					cal_book_get_dirty(book),
					cal_book_get_visibility(book));
		}
	}
}

Eina_Bool cal_books_update_from_db(cal_books_h books)
{
	CAL_FN_START;
	Eina_Bool is_changed = EINA_FALSE;
	cal_books_s* lbooks = (cal_books_s*)books;
	if (!lbooks)
		return is_changed;

	Eina_List* accounts = NULL;
	struct _acct* account = NULL;

	cal_books_commit_to_db(books);

	// add the local account
	account = (struct _acct*)g_new0(struct _acct, 1);
	account->id = _CALENDAR_LOCAL_ACCOUNT_ID;
	accounts = eina_list_append(accounts, account);

    Eina_List* node = NULL;
	Eina_Bool is_changed2 = EINA_FALSE;
	EINA_LIST_FOREACH(accounts, node, account)
	{
		is_changed2 = __update_books_from_db_by_account_id(lbooks, account->id);
		if (is_changed2)
			is_changed = EINA_TRUE;
	}

	is_changed2 = __delete_unchecked_account_from_memory(lbooks); /* just remove an account having no books */
	if (is_changed2)
		is_changed = EINA_TRUE;

	__set_color_of_colorless_books(lbooks);

	__print_account(lbooks);

	return is_changed;
}

static void __insert_book_to_db(cal_book_h book)
{
	CAL_FN_START;
	calendar_record_h record = (calendar_record_h)cal_book_get_record(book);
	int id = -1;
	calendar_error_e error = calendar_db_insert_record(record, &id);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	error = calendar_db_get_record(_calendar_book._uri, id, &record);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	cal_book_set_record(book, record);
}

static void __delete_book_from_db(int id)
{
	CAL_FN_START;
	calendar_error_e error = calendar_db_delete_record(_calendar_book._uri, id);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

static void __update_book_to_db(const cal_book_h book)
{
	CAL_FN_START;
	calendar_error_e error = calendar_db_update_record((calendar_record_h)cal_book_get_record(book));
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

static void __commit_books_to_db_by_account(cal_books_s* books, cal_books_account_s* account)
{
	CAL_FN_START;
	if (account->dirty != CAL_BOOKS_ACCOUNT_DIRTY_UPDATE)
		return;

	//cal_self_update_set();

	Eina_List* node = NULL;
	Eina_List* current_node = NULL;
	cal_book_h book = NULL;
	cal_book_dirty_e dirty = CAL_BOOK_DIRTY_NONE;
	for (node = account->books; node != NULL; )
	{
		current_node = node;
		node = eina_list_next(node);
		book = (cal_book_h)eina_list_data_get(current_node);
		dirty = cal_book_get_dirty(book);
		switch (dirty)
		{
			case CAL_BOOK_DIRTY_INSERT:
				__insert_book_to_db(book);
				break;
			case CAL_BOOK_DIRTY_DELETE:
				__delete_book_from_db(cal_book_get_id(book));
				__delete_book_from_memory_by_node(books, account, current_node);
				break;
			case CAL_BOOK_DIRTY_UPDATE:
				__update_book_to_db(book);
				break;
			default:
				DBG("Not changed");
				break;
		}
	}
}

static void __clear_accounts_books_all_dirty(cal_books_s* books)
{
	CAL_FN_START;
	Eina_List* node = NULL;
	cal_books_account_s* account = NULL;
	cal_book_h book = NULL;
	Eina_List* node2 = NULL;
	EINA_LIST_FOREACH(books->accounts, node, account)
	{
		account->dirty = CAL_BOOKS_ACCOUNT_DIRTY_NONE;
		EINA_LIST_FOREACH(account->books, node2, book)
		{
			cal_book_set_dirty(book, CAL_BOOK_DIRTY_NONE);
		}
	}
}

void cal_books_commit_to_db(cal_books_h books)
{
	CAL_FN_START;
	cal_books_s* lbooks = (cal_books_s*)books;
	if (!lbooks)
		return;

	Eina_List* node = NULL;
	cal_books_account_s* account = NULL;
	EINA_LIST_FOREACH(lbooks->accounts, node, account)
	{
		__commit_books_to_db_by_account(lbooks, account);
	}

	// clear the dirty flag of all accounts and their books
	__clear_accounts_books_all_dirty(lbooks);
}

Eina_List* cal_books_get_books_by_accound_id_p(cal_books_h books, int account_id)
{
	CAL_FN_START;
	cal_books_s* lbooks = (cal_books_s*)books;
	if (!lbooks)
	{
		ERR("Invalid argument");
		return NULL;
	}

	cal_books_account_s* account = __find_account_by_id(lbooks, account_id);
	if (!account)
	{
		ERR("Unable to find the account(id = %d)", account_id);
		return NULL;
	}

	return account->books;
}

void __set_account_dirty_by_book(cal_books_s* books, const cal_book_h book, cal_books_account_dirty_e dirty)
{
	CAL_FN_START;
	cal_books_account_s* account = __find_account_by_id(books, cal_book_get_account_id(book));
	if (account)
		account->dirty = dirty;
	else
		ERR("account NULL");
}

int cal_books_insert_book(cal_books_h books, int account_id, const cal_book_h book)
{
	CAL_FN_START;
	cal_books_s* lbooks = (cal_books_s*)books;
	if (!lbooks || !book)
	{
		ERR("Invalid argument");
		return -1;
	}

	cal_books_account_s* account = __find_account_by_id(lbooks, account_id);
	CAL_ASSERT(account);
	cal_book_h lbook = __insert_book_to_memory(lbooks, account, book);
	cal_book_set_dirty(lbook, CAL_BOOK_DIRTY_INSERT);
	__set_account_dirty_by_book(lbooks, lbook, CAL_BOOKS_ACCOUNT_DIRTY_UPDATE);
	return 0;
}

const cal_book_h cal_books_get_book_p(cal_books_h books, int id)
{
	cal_books_s* lbooks = (cal_books_s*)books;
	if (!lbooks)
	{
		ERR("Invalid argument");
		return NULL;
	}

	return __find_book_from_map(lbooks->map, id);
}

int cal_books_update_book(cal_books_h books, const cal_book_h book)
{
	CAL_FN_START;
	cal_books_s* lbooks = (cal_books_s*)books;
	if (!lbooks || !book)
	{
		ERR("Invalid argument");
		return -1;
	}

	int id = cal_book_get_id(book);
	cal_book_h lbook = __find_book_from_map(lbooks->map, id);
	if (!lbook)
	{
		ERR("Unable to find the book(id=%u)", id);
		return -1;
	}

	if (lbook == book)
	{
		DBG("Original changed");
		cal_book_set_dirty(lbook, CAL_BOOK_DIRTY_UPDATE);
		__set_account_dirty_by_book(lbooks, lbook, CAL_BOOKS_ACCOUNT_DIRTY_UPDATE);
	}
	else if (__update_book_to_memory(lbooks, lbook, book))
	{ /* changed */
		DBG("Changed");
		cal_book_set_dirty(lbook, CAL_BOOK_DIRTY_UPDATE);
		__set_account_dirty_by_book(lbooks, lbook, CAL_BOOKS_ACCOUNT_DIRTY_UPDATE);
	}
	return 0;
}

int cal_books_delete_book(cal_books_h books, int id)
{
	CAL_FN_START;
	cal_books_s* lbooks = (cal_books_s*)books;
	if (!lbooks)
	{
		ERR("Invalid argument");
		return -1;
	}

	cal_book_h lbook = __find_book_from_map_with_deleted(lbooks->map, id);
	if (!lbook)
	{
		ERR("Unable to find the book(id=%u)", id);
		return -1;
	}

	cal_book_set_dirty(lbook, CAL_BOOK_DIRTY_DELETE);
	__set_account_dirty_by_book(lbooks, lbook, CAL_BOOKS_ACCOUNT_DIRTY_UPDATE);
	return 0;
}

cal_book_h cal_books_insert_book_without_handle(int account_id, const cal_book_h book)
{
	CAL_FN_START;
	if (!book)
	{
		ERR("Invalid argument");
		return NULL;
	}

	cal_book_h lbook = __clone_book(book);
	cal_book_set_account_id(lbook, account_id);
	__insert_book_to_db(lbook);

	return lbook;
}

Eina_Bool cal_books_exist_name(cal_books_h books, int account_id, int store_type, const char* name)
{
	CAL_FN_START;
	c_retvm_if((name==NULL || strlen(name)==0), EINA_FALSE, "invalid parameter");
	Eina_Bool ret = EINA_FALSE;
	Eina_List* lists = cal_books_get_books_by_accound_id_p(books, account_id);
	cal_book_h book = NULL;
	Eina_List* l = NULL;
	int book_store_type = -1;
	const char *book_name = NULL;

	EINA_LIST_FOREACH(lists, l, book) {
		book_store_type = cal_book_get_store_type(book);
		book_name = cal_book_get_name(book);
		if (book_store_type == store_type && strcmp(name, book_name) == 0) {
			ret = EINA_TRUE;
			break;
		}
	}

	return ret;
}
