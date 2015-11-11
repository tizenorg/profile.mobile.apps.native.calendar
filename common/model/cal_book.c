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
#include "cal_book.h"

typedef struct {
	calendar_record_h record;
	cal_book_dirty_e dirty;
} cal_book_s;

cal_book_h cal_book_create()
{
	cal_book_s* lbook = g_new0(cal_book_s, 1);
	calendar_record_create(_calendar_book._uri, &lbook->record);
	return (cal_book_h)lbook;
}

cal_book_h cal_book_create_with_record(void* record)
{
	cal_book_s* lbook = g_new0(cal_book_s, 1);
	lbook->record = record;
	return (cal_book_h)lbook;
}

void cal_book_destroy(cal_book_h book)
{
	cal_book_s* lbook = (cal_book_s*)book;
	if (!lbook)
		return;
	calendar_record_destroy(lbook->record, true);
	g_free(lbook);
}

cal_book_h cal_book_clone(const cal_book_h book)
{
	cal_book_s* lbook = (cal_book_s*)book;
	if (!lbook)
		return NULL;
	cal_book_s* copied_book = g_new0(cal_book_s, 1);
	calendar_record_clone(lbook->record, &copied_book->record);
	return (cal_book_h)copied_book;
}

void cal_book_set_record(cal_book_h book, void* record)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook && record);
	calendar_record_destroy(lbook->record, true);
	lbook->record = record;
}

void cal_book_set_uid(cal_book_h book, const char* uid)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook && uid);
	calendar_error_e error = calendar_record_set_str(lbook->record, _calendar_book.uid, uid);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void cal_book_set_name(cal_book_h book, const char* name)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook && name);
	calendar_error_e error = calendar_record_set_str(lbook->record, _calendar_book.name, name);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void cal_book_set_description(cal_book_h book, const char* description)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook && description);
	calendar_error_e error = calendar_record_set_str(lbook->record, _calendar_book.description, description);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void cal_book_set_color(cal_book_h book, const char* color)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook && color);
	calendar_error_e error = calendar_record_set_str(lbook->record, _calendar_book.color, color);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void cal_book_set_color_rgb(const cal_book_h book, int r, int g, int b, int a)
{
	#define COLOR_BUFFER 16
	char color[COLOR_BUFFER] = {0};
	snprintf(color, sizeof(color), "%d.%d.%d.%d", r, g, b, a);
	cal_book_set_color(book, (const char*)color);
}

void cal_book_set_color_rgb_struct(const cal_book_h book, cal_book_rgb_color_s rgb_color)
{
	cal_book_set_color_rgb(book, rgb_color.red, rgb_color.green, rgb_color.blue, rgb_color.alpha);
}

void cal_book_set_location(cal_book_h book, const char* location)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook && location);
	calendar_error_e error = calendar_record_set_str(lbook->record, _calendar_book.location, location);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void cal_book_set_visibility(cal_book_h book, Eina_Bool visibility)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	calendar_error_e error = calendar_record_set_int(lbook->record, _calendar_book.visibility, (int)visibility);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void cal_book_set_account_id(cal_book_h book, int account_id)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);

	int id = cal_book_get_id(book);
	if (id <= 0) {
		int record_id = 0;
		calendar_error_e error = calendar_record_get_int(lbook->record, _calendar_book.account_id, &record_id);
		if(error == CALENDAR_ERROR_NONE && record_id != account_id) {
			calendar_record_set_int(lbook->record, _calendar_book.account_id, account_id);
		}
	} else {
		ERR("already book have id(%d), (%d) ", id, account_id);
	}
}

void cal_book_set_store_type(cal_book_h book, int store_type)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	calendar_error_e error = calendar_record_set_int(lbook->record, _calendar_book.store_type, store_type);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void cal_book_set_sync_data1(cal_book_h book, const char* sync_data1)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook && sync_data1);
	calendar_error_e error = calendar_record_set_str(lbook->record, _calendar_book.sync_data1, sync_data1);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void cal_book_set_sync_data2(cal_book_h book, const char* sync_data2)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook && sync_data2);
	calendar_error_e error = calendar_record_set_str(lbook->record, _calendar_book.sync_data2, sync_data2);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void cal_book_set_sync_data3(cal_book_h book, const char* sync_data3)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook && sync_data3);
	calendar_error_e error = calendar_record_set_str(lbook->record, _calendar_book.sync_data3, sync_data3);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void cal_book_set_sync_data4(cal_book_h book, const char* sync_data4)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook && sync_data4);
	calendar_error_e error = calendar_record_set_str(lbook->record, _calendar_book.sync_data4, sync_data4);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void cal_book_set_mode(cal_book_h book, int mode)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	calendar_error_e error = calendar_record_set_int(lbook->record, _calendar_book.mode, mode);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void cal_book_set_dirty(cal_book_h book, cal_book_dirty_e dirty)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	lbook->dirty = dirty;
}

const void* cal_book_get_record(cal_book_h book)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	return lbook->record;
}

int cal_book_get_id(const cal_book_h book)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	int id = -1;
	calendar_error_e error = calendar_record_get_int(lbook->record, _calendar_book.id, &id);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return id;
}

const char* cal_book_get_uid(const cal_book_h book)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	char* uid = NULL;
	calendar_error_e error = calendar_record_get_str_p(lbook->record, _calendar_book.uid, &uid);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return uid;
}

const char* cal_book_get_name(const cal_book_h book)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	char* name = NULL;
	calendar_error_e error = calendar_record_get_str_p(lbook->record, _calendar_book.name, &name);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return name;
}

const char* cal_book_get_description(const cal_book_h book)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	char* description = NULL;
	calendar_error_e error = calendar_record_get_str_p(lbook->record, _calendar_book.description, &description);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return description;
}

static void __get_rgb_from_color_string(const char* color, int* r, int* g, int* b, int* a)
{
	if (!color || !r || !g || !b || !a)
		return;

	char* copied_color = g_strdup(color);
	char* token = NULL;
	char* temp = NULL;

	token = strtok_r(copied_color, ".", &temp);
	if (token)
		*r = atoi(token);
	else
		*r = 0;

	token = strtok_r(NULL, ".", &temp);
	if (token)
		*g = atoi(token);
	else
		*g = 0;

	token = strtok_r(NULL, ".", &temp);
	if (token)
		*b = atoi(token);
	else
		*b = 0;

	token = strtok_r(NULL, ".", &temp);
	if (token)
		*a = atoi(token);
	else
		*a = 255;

	*a = 255;

	g_free(copied_color);
}

const char* cal_book_get_color(const cal_book_h book)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	char* color = NULL;
	calendar_error_e error = calendar_record_get_str_p(lbook->record, _calendar_book.color, &color);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return color;
}

void cal_book_get_color_rgb(const cal_book_h book, int* r, int* g, int* b, int* a)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	char* color = NULL;
	calendar_error_e error = calendar_record_get_str_p(lbook->record, _calendar_book.color, &color);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	__get_rgb_from_color_string(color, r, g, b, a);
}

cal_book_rgb_color_s cal_book_get_color_rgb_struct(const cal_book_h book)
{
	cal_book_rgb_color_s rgb_color = {0, };
	cal_book_get_color_rgb(book, &rgb_color.red, &rgb_color.green, &rgb_color.blue, &rgb_color.alpha);
	return rgb_color;
}

const char* cal_book_get_location(const cal_book_h book)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	char* location = NULL;
	calendar_error_e error = calendar_record_get_str_p(lbook->record, _calendar_book.location, &location);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return location;
}

Eina_Bool cal_book_get_visibility(const cal_book_h book)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	int visibility = EINA_FALSE;
	calendar_error_e error = calendar_record_get_int(lbook->record, _calendar_book.visibility, &visibility);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return (Eina_Bool)visibility;
}

int cal_book_get_account_id(const cal_book_h book)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	int account_id = -1;
	calendar_error_e error = calendar_record_get_int(lbook->record, _calendar_book.account_id, &account_id);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return account_id;
}

int cal_book_get_store_type(const cal_book_h book)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	int store_type = -1;
	calendar_error_e error = calendar_record_get_int(lbook->record, _calendar_book.store_type, &store_type);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return store_type;
}

const char* cal_book_get_sync_data1(const cal_book_h book)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	char* sync_data1 = NULL;
	calendar_error_e error = calendar_record_get_str_p(lbook->record, _calendar_book.sync_data1, &sync_data1);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return sync_data1;
}

const char* cal_book_get_sync_data2(const cal_book_h book)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	char* sync_data2 = NULL;
	calendar_error_e error = calendar_record_get_str_p(lbook->record, _calendar_book.sync_data2, &sync_data2);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return sync_data2;
}

const char* cal_book_get_sync_data3(const cal_book_h book)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	char* sync_data3 = NULL;
	calendar_error_e error = calendar_record_get_str_p(lbook->record, _calendar_book.sync_data3, &sync_data3);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return sync_data3;
}

const char* cal_book_get_sync_data4(const cal_book_h book)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	char* sync_data4 = NULL;
	calendar_error_e error = calendar_record_get_str_p(lbook->record, _calendar_book.sync_data4, &sync_data4);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return sync_data4;
}

int cal_book_get_mode(const cal_book_h book)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	int mode = -1;
	calendar_error_e error = calendar_record_get_int(lbook->record, _calendar_book.mode, &mode);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return mode;
}

cal_book_dirty_e cal_book_get_dirty(cal_book_h book)
{
	cal_book_s* lbook = (cal_book_s*)book;
	CAL_ASSERT(lbook);
	return lbook->dirty;
}
