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

#include "CalBook.h"
#include "CalCommon.h"

#define COLOR_BUFFER 17

typedef calendar_record_h cal_book_s;

CalBook::CalBook()
{
	cal_book_s *lbook = (cal_book_s*) calloc(1, sizeof(cal_book_s));
	calendar_record_create(_calendar_book._uri, lbook);
	__book = (cal_book_h) lbook;
	WASSERT(__book);
}

CalBook::~CalBook()
{
	destroyBook();
}

CalBook::CalBook(const CalBook& obj)
{
	cal_book_s *lbook = (cal_book_s*) obj.__book;
	cal_book_s *copied_book = (cal_book_s*) calloc(1, sizeof(cal_book_s));
	calendar_record_clone(*lbook, copied_book);
	__book = (cal_book_h) copied_book;
}

CalBook::CalBook(const cal_book_h book)
{
	cal_book_s *lbook = (cal_book_s*) book;
	cal_book_s *copied_book = (cal_book_s*) calloc(1, sizeof(cal_book_s));
	calendar_record_clone(*lbook, copied_book);
	__book = (cal_book_h) copied_book;
}

const CalBook& CalBook::operator=(const CalBook& obj)
{
	if (this != &obj)
	{
		this->destroyBook();
		return *(obj.cloneBook());
	}

	return *this;
}

CalBook* CalBook::cloneBook() const
{
	return new CalBook(*this);
}

void CalBook::destroyBook()
{
	cal_book_s *lbook = (cal_book_s*) __book;
	if (lbook)
	{
		calendar_record_destroy(*lbook, true);
		free(lbook);
	}
}

const cal_book_h CalBook::getBook() const
{
	return __book;
}

int CalBook::getIndex() const
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook);
	int id = -1;
	int error = calendar_record_get_int(*lbook, _calendar_book.id, &id);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return id;
}

const char* CalBook::getName() const
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook);
	char* name = NULL;
	int error = calendar_record_get_str_p(*lbook, _calendar_book.name, &name);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return name;
}

void CalBook::parseColorString(char *color, int &r, int &g, int &b, int &a) const
{
	WPRET_M(!color, "pass null pointer");
	r = g = b = a = -1;
	char *pch = NULL;
	const char *delim = ".";
	pch = strtok(color, delim);
	int step = 0;
	while (pch != NULL) {
		int tcolor = atoi(pch);
		switch (step) {
		case 0:
			r = tcolor;
			break;
		case 1:
			g = tcolor;
			break;
		case 2:
			b = tcolor;
			break;
		case 3:
			a = tcolor;
			break;
		default:
			CAL_ASSERT("wrong color parsing step");
			break;
		}
		pch = strtok(NULL, delim);
		++step;
	}
}


void CalBook::getColor(int& r, int& g, int& b, int& a) const
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook);
	char *color = NULL;
	int error = calendar_record_get_str_p(*lbook, _calendar_book.color, &color);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	char *temp = strdup(color);
	parseColorString(temp, r, g, b, a);
	free(temp);
}

const char* CalBook::getLocation() const
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook);
	char *location = NULL;
	int error = calendar_record_get_str_p(*lbook, _calendar_book.location, &location);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return location;
}

bool CalBook::getVisibility() const
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook);
	int visibility = EINA_FALSE;
	int error = calendar_record_get_int(*lbook, _calendar_book.visibility, &visibility);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return (bool) visibility;
}

int CalBook::getAccountId() const
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook);
	int account_id = -1;
	int error = calendar_record_get_int(*lbook, _calendar_book.account_id, &account_id);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return account_id;
}

int CalBook::getStoreType() const
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook);
	int store_type = -1;
	int error = calendar_record_get_int(*lbook, _calendar_book.store_type, &store_type);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return store_type;
}

const char* CalBook::getSyncData(SyncDataType type) const
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook);
	char *syncData = NULL;
	calendar_error_e error = CALENDAR_ERROR_NONE;
	switch (type) {
	case SYNC_DATA_1:
		error = (calendar_error_e) calendar_record_get_str_p(*lbook, _calendar_book.sync_data1, &syncData);
		break;
	case SYNC_DATA_2:
		error = (calendar_error_e) calendar_record_get_str_p(*lbook, _calendar_book.sync_data2, &syncData);
		break;
	case SYNC_DATA_3:
		error = (calendar_error_e) calendar_record_get_str_p(*lbook, _calendar_book.sync_data3, &syncData);
		break;
	case SYNC_DATA_4:
		error = (calendar_error_e) calendar_record_get_str_p(*lbook, _calendar_book.sync_data4, &syncData);
		break;
	default:
		assert(true);
		break;
	}
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return syncData;
}

int CalBook::getMode() const
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook);
	int mode = -1;
	calendar_error_e error = (calendar_error_e)  calendar_record_get_int(*lbook, _calendar_book.mode, &mode);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
	return mode;
}

void CalBook::setUid(const char* uid)
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook && uid);
	int error = calendar_record_set_str(*lbook, _calendar_book.uid, uid);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void CalBook::setName(const char* name)
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook && name);
	int error = calendar_record_set_str(*lbook, _calendar_book.name, name);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void CalBook::setDescription(const char* description)
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook && description);
	int error = calendar_record_set_str(*lbook, _calendar_book.description, description);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void CalBook::setColor(int r, int g, int b, int a)
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook);
	char color[COLOR_BUFFER] = {0};
	snprintf(color, sizeof(color), "%d.%d.%d.%d", r, g, b, a);
	int error = calendar_record_set_str(*lbook, _calendar_book.color, color);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void CalBook::setLocation(const char* location)
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook && location);
	int error = calendar_record_set_str(*lbook, _calendar_book.location, location);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void CalBook::setVisibility(bool visibility)
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook);
	int error = calendar_record_set_int(*lbook, _calendar_book.visibility, visibility);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void CalBook::setAccountId(int account_id)
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook);

	int id = getIndex();
	if (id <= 0) {
		int record_id = 0;
		int error = calendar_record_get_int(*lbook, _calendar_book.account_id, &record_id);
		if(error == CALENDAR_ERROR_NONE && record_id != account_id) {
			calendar_record_set_int(*lbook, _calendar_book.account_id, account_id);
		}
	} else {
		ERR("already book have id(%d), (%d) ", id, account_id);
	}
}

void CalBook::setStoreType(int storeType)
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook);
	int error = calendar_record_set_int(*lbook, _calendar_book.store_type, storeType);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void CalBook::setSyncData(SyncDataType type, const char *syncData)
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook && syncData);
	int error = CALENDAR_ERROR_NONE;
	switch (type) {
	case SYNC_DATA_1:
		error = calendar_record_set_str(*lbook, _calendar_book.sync_data1, syncData);
		break;
	case SYNC_DATA_2:
		error = calendar_record_set_str(*lbook, _calendar_book.sync_data3, syncData);
		break;
	case SYNC_DATA_3:
		error = calendar_record_set_str(*lbook, _calendar_book.sync_data3, syncData);
		break;
	case SYNC_DATA_4:
		error = calendar_record_set_str(*lbook, _calendar_book.sync_data4, syncData);
		break;
	default:
		assert(true);
		break;
	}
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

void CalBook::setMode(int mode)
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook);
	int error = calendar_record_set_int(*lbook, _calendar_book.mode, mode);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}

bool CalBook::isReadOnly() const
{
	if (getMode() != CALENDAR_BOOK_MODE_NONE || getIndex() == DEFAULT_BIRTHDAY_CALENDAR_BOOK_ID)
	{
		return true;
	}

	return false;
}

void CalBook::updateDbRecord()
{
	cal_book_s *lbook = (cal_book_s*) __book;
	CAL_ASSERT(lbook);
	int error = calendar_db_update_record(*lbook);
	CAL_ASSERT(error == CALENDAR_ERROR_NONE);
}
