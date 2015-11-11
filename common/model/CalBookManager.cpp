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

#include "cal_book.h"
#include "CalBookManager.h"
#include "CalCommon.h"
#include "CalEventManager.h"
#include "CalDataManager.h"

SINGLETON_INSTANCE(CalBookManager);

CalBookManager::CalBookManager() :
	__books(NULL)
{
	WENTER();
}

CalBookManager::~CalBookManager()
{
	WENTER();
	__destroyBookMap();

	calendar_db_remove_changed_cb(_calendar_book._uri, __onChanged, NULL);
	if (__books)
	{
		cal_books_finalize(__books);
	}
}

void CalBookManager::init()
{
	WENTER();

	int error = calendar_db_add_changed_cb(_calendar_book._uri, __onChanged, NULL);
	WASSERT(error == CALENDAR_ERROR_NONE);

	__books = cal_books_initialize();
	WASSERT(__books);

	__createBookMap();
}

std::shared_ptr<CalBook> CalBookManager::getBook(int index)
{
	WENTER();
	auto finder = __bookMap.find(index);
	if (finder == __bookMap.end())
	{
		WDEBUG("null");
		return nullptr;
	}

	auto instance = finder->second;
	WLEAVE();
	return instance;
}

std::shared_ptr<CalBook> CalBookManager::findBook(const char* name)
{
	if (!name)
	{
		return nullptr;
	}
	for (auto it = __bookMap.begin(); it != __bookMap.end(); ++it)
	{
		auto instance = it->second;
		const char* bookName = instance->getName();
		if (bookName && !strcmp(name, bookName))
		{
			return instance;
		}
	}
	return nullptr;
}

std::shared_ptr<CalBook> CalBookManager::findBook(const char* name, int r, int g, int b, int a)
{
	for (auto it = __bookMap.begin(); it != __bookMap.end(); ++it)
	{
		auto instance = it->second;
		int red;
		int green;
		int blue;
		int apl;
		const char* bookName = instance->getName();
		instance->getColor(red, green, blue, apl);
		if (bookName && strcmp(name, bookName) == 0 && red == r && green == g && blue == b && apl == a)
		{
			return instance;
		}
	}

	return nullptr;
}

std::shared_ptr<CalBook> CalBookManager::findBook(int r, int g, int b, int a)
{
	for (auto it = __bookMap.begin(); it != __bookMap.end(); ++it)
	{
		auto instance = it->second;
		int red;
		int green;
		int blue;
		int apl;
		instance->getColor(red, green, blue, apl);
		if (red == r && green == g && blue == b && apl == a)
		{
			return instance;
		}
	}

	return nullptr;
}

bool CalBookManager::isAllVisible()
{
	for (auto it = __bookMap.begin(); it != __bookMap.end(); ++it)
	{
		auto instance = it->second;
		if (instance->getVisibility() == false)
		{
			return false;
		}
	}
	return true;
}

void CalBookManager::getBooks(std::map<int, std::shared_ptr<CalBook>>& bookMap)
{
	WENTER();
	__addLocalBooks(bookMap);
}

int CalBookManager::insertBook(const std::shared_ptr<CalBook>& book)
{
	WENTER();
	cal_book_h newBook = cal_books_insert_book_without_handle(_CALENDAR_LOCAL_ACCOUNT_ID, book->getBook());
	if (!newBook)
	{
		WERROR("cal_books_insert_book_without_handle failed");
		return -1;
	}
	WDEBUG("inserted book id = %d", cal_book_get_id(newBook));
	cal_books_update_from_db(__books);
	std::shared_ptr<CalBook> instance(new CalBook(newBook));
	cal_book_destroy(newBook);
	__bookMap.insert(std::pair<int, std::shared_ptr<CalBook>>(instance->getIndex(), instance));
	__notify(CalEvent::LOCAL);
	WLEAVE();
	return 0;
}

int CalBookManager::deleteBook(const std::shared_ptr<CalBook>& book)
{
	WENTER();
	int ret = cal_books_delete_book(__books, book->getIndex());
	if (ret != 0)
	{
		WERROR("cal_books_delete_book(%d) failed : %d", book->getIndex(), ret);
		return -1;
	}
	cal_books_commit_to_db(__books);
	__bookMap.erase(book->getIndex());
	__notify(CalEvent::LOCAL);
	WLEAVE();
	return 0;
}

int CalBookManager::updateBook(const std::shared_ptr<CalBook>& book)
{
	WENTER();
	int ret = cal_books_update_book(__books, book->getBook());
	if (ret != 0)
	{
		WERROR("cal_books_update_book failed : %d", ret);
		return -1;
	}
	cal_books_commit_to_db(__books);
	__bookMap.erase(book->getIndex());
	std::shared_ptr<CalBook> instance(new CalBook(*book));
	__bookMap.insert(std::pair<int, std::shared_ptr<CalBook>>(instance->getIndex(), instance));
	__notify(CalEvent::LOCAL);
	WLEAVE();
	return 0;
}

void CalBookManager::__notify(CalEvent::Source source)
{
	CalEvent event1(CalEvent::DB_CHANGED, source);
	CalEventManager::getInstance().notify(event1);
	CalEvent event2(CalEvent::BOOK_DB_CHANGED, source);
	CalEventManager::getInstance().notify(event2);
}

void CalBookManager::__onChanged(const char* uri, void* userData)
{
	WENTER();
	CalBookManager& self = CalBookManager::getInstance();
	if (!cal_books_update_from_db(self.__books))
	{
		WDEBUG("already updated");
		return;
	}

	// update book maps
	self.__destroyBookMap();
	self.__createBookMap();

	self.__notify(CalEvent::REMOTE);
}

void CalBookManager::__addLocalBooks(std::map<int, std::shared_ptr<CalBook>>& bookMap)
{
	WENTER();
	Eina_List* bookList = cal_books_get_books_by_accound_id_p(__books, _CALENDAR_LOCAL_ACCOUNT_ID);
	cal_book_h book = NULL;
	Eina_List* l = NULL;
	EINA_LIST_FOREACH(bookList, l, book)
	{
		if (cal_book_get_store_type(book) == CALENDAR_BOOK_TYPE_TODO)
		{
			continue;
		}

		std::shared_ptr<CalBook> instance(new CalBook(book));
		bookMap.insert(std::pair<int, std::shared_ptr<CalBook>>(instance->getIndex(), instance));
	}
}

void CalBookManager::__createBookMap()
{
	WENTER();
	__addLocalBooks(__bookMap);
}

void CalBookManager::__destroyBookMap()
{
	WENTER();
	__bookMap.clear();
}

