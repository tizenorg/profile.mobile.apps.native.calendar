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

CalBook::CalBook()
{
	__book = cal_book_create();
	WASSERT(__book);
}

CalBook::~CalBook()
{
	if (__book)
	{
		cal_book_destroy(__book);
	}
}

CalBook::CalBook(const CalBook& obj)
{
	__book = cal_book_clone(obj.__book);
}

CalBook::CalBook(const cal_book_h book)
{
	__book = cal_book_clone(book);
}

const CalBook& CalBook::operator=(const CalBook& obj)
{
	if (this != &obj)
	{
		cal_book_destroy(__book);
		__book = cal_book_clone(obj.__book);
	}

	return *this;
}

const cal_book_h CalBook::getBook() const
{
	return __book;
}

int CalBook::getIndex() const
{
	return cal_book_get_id(__book);
}

const char* CalBook::getUid() const
{
	return cal_book_get_uid(__book);
}

const char* CalBook::getName() const
{
	return cal_book_get_name(__book);
}

const char* CalBook::getDescription() const
{
	return cal_book_get_description(__book);
}

void CalBook::getColor(int& r, int& g, int& b, int& a) const
{
	cal_book_get_color_rgb(__book, &r, &g, &b, &a);
}

const char* CalBook::getLocation() const
{
	return cal_book_get_location(__book);
}

bool CalBook::getVisibility() const
{
	return (bool)cal_book_get_visibility(__book);
}

int CalBook::getAccountId() const
{
	return cal_book_get_account_id(__book);
}

int CalBook::getStoreType() const
{
	return cal_book_get_store_type(__book);
}

const char* CalBook::getSyncData1() const
{
	return cal_book_get_sync_data1(__book);
}

const char* CalBook::getSyncData2() const
{
	return cal_book_get_sync_data2(__book);
}

const char* CalBook::getSyncData3() const
{
	return cal_book_get_sync_data3(__book);
}

const char* CalBook::getSyncData4() const
{
	return cal_book_get_sync_data4(__book);
}

int CalBook::getMode() const
{
	return cal_book_get_mode(__book);
}

void CalBook::setUid(const char* uid)
{
	cal_book_set_uid(__book, uid);
}

void CalBook::setName(const char* name)
{
	cal_book_set_name(__book, name);
}

void CalBook::setDescription(const char* description)
{
	cal_book_set_description(__book, description);
}

void CalBook::setColor(int r, int g, int b, int a)
{
	cal_book_set_color_rgb(__book, r, g, b, a);
}

void CalBook::setLocation(const char* location)
{
	cal_book_set_location(__book, location);
}

void CalBook::setVisibility(bool visibility)
{
	cal_book_set_visibility(__book, (Eina_Bool)visibility);
}

void CalBook::setAccountId(int accountId)
{
	cal_book_set_account_id(__book, accountId);
}

void CalBook::setStoreType(int storeType)
{
	cal_book_set_store_type(__book, storeType);
}

void CalBook::setSyncData1(const char* syncData)
{
	cal_book_set_sync_data1(__book, syncData);
}

void CalBook::setSyncData2(const char* syncData)
{
	cal_book_set_sync_data2(__book, syncData);
}

void CalBook::setSyncData3(const char* syncData)
{
	cal_book_set_sync_data3(__book, syncData);
}

void CalBook::setSyncData4(const char* syncData)
{
	cal_book_set_sync_data4(__book, syncData);
}

void CalBook::setMode(int mode)
{
	cal_book_set_mode(__book, mode);
}

bool CalBook::isReadOnly() const
{
	if (getMode() != CALENDAR_BOOK_MODE_NONE || getIndex() == DEFAULT_BIRTHDAY_CALENDAR_BOOK_ID)
	{
		return true;
	}

	return false;
}
