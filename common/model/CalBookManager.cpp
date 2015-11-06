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

#include "CalBookManager.h"
#include "CalCommon.h"
#include "CalEventManager.h"
#include "CalDataManager.h"
#include <vector>

typedef struct {
	int r;
	int g;
	int b;
	int a;
} cal_book_color_s;

static const cal_book_color_s __books_default_rgb_color[] = {
	{188, 229, 92, 255},
	{221, 129, 0, 255},
	{0, 182, 252, 255},
	{122, 52, 122, 255},
	{22, 139, 121, 255},
	{40, 81, 163, 255}
};

#define COLOR_ARRAY_LENTH sizeof(__books_default_rgb_color)/sizeof(*__books_default_rgb_color)

#define _CALENDAR_ALL_ACCOUNT_ID 0
#define _CALENDAR_LOCAL_ACCOUNT_ID -1

SINGLETON_INSTANCE(CalBookManager);

CalBookManager::CalBookManager()
{
	WENTER();
}

CalBookManager::~CalBookManager()
{
	WENTER();
	__destroyBookMap();

	calendar_db_remove_changed_cb(_calendar_book._uri, __onChanged, NULL);
}

void CalBookManager::init()
{
	WENTER();

	int error = calendar_db_add_changed_cb(_calendar_book._uri, __onChanged, NULL);
	WASSERT(error == CALENDAR_ERROR_NONE);

	__createBookMap();
	__fillColorlessBook(__bookMap);
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

void CalBookManager::getBooks(std::map<int, std::shared_ptr<CalBook>>& bookMap)
{
	WENTER();
	__addLocalBooks(bookMap);
}

int CalBookManager::updateBook(const std::shared_ptr<CalBook>& book)
{
	WENTER();
	book->updateDbRecord();
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

	// update book maps
	self.__destroyBookMap();
	self.__createBookMap();

	self.__notify(CalEvent::REMOTE);
}

void CalBookManager::__addLocalBooks(std::map<int, std::shared_ptr<CalBook>>& bookMap)
{
	WENTER();

	int error = CALENDAR_ERROR_NONE;
	calendar_filter_h filter = NULL;
	calendar_query_h query = NULL;
	calendar_list_h list = NULL;
	int account_id = _CALENDAR_LOCAL_ACCOUNT_ID;
	do
	{
		error = calendar_query_create(_calendar_book._uri, &query);
		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}

		if (account_id != _CALENDAR_ALL_ACCOUNT_ID)
		{
			error = calendar_filter_create(_calendar_book._uri, &filter);
			if (error != CALENDAR_ERROR_NONE)
			{
				break;
			}
			error = calendar_filter_add_int(filter, _calendar_book.account_id, CALENDAR_MATCH_EQUAL, account_id);
			if (error != CALENDAR_ERROR_NONE)
			{
				break;
			}
			error = calendar_query_set_filter(query, filter);
			if (error != CALENDAR_ERROR_NONE)
			{
				break;
			}
		}

		error = calendar_db_get_records_with_query(query, 0, 0, &list);
		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}
	}
	while(0);

	if (filter)
	{
		calendar_filter_destroy(filter);
	}

	if (query)
	{
		calendar_query_destroy(query);
	}

	CAL_ASSERT(error == CALENDAR_ERROR_NONE);

	int count = 0;
	calendar_list_get_count(list, &count);

	calendar_record_h record = NULL;
	for (int i = 0; i < count; ++i)
	{
		calendar_list_get_current_record_p(list, &record);
		std::shared_ptr<CalBook> instance(new CalBook((cal_book_h) &record));
		if (instance->getStoreType() != CALENDAR_BOOK_TYPE_TODO)
		{
			bookMap.insert(std::pair<int, std::shared_ptr<CalBook>>(instance->getIndex(), instance));
		}
		calendar_list_next(list);
	}
	calendar_list_destroy(list, true);
	WLEAVE();
	return;
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

void CalBookManager::__fillColorlessBook(std::map<int, std::shared_ptr<CalBook>>& bookMap)
{
	WENTER();

	cal_book_color_s color = {0};
	std::vector<int> ids;
	for (auto it = bookMap.begin(); it != bookMap.end(); ++it)
	{
		auto instance = it->second;

		instance->getColor(color.r, color.g, color.b, color.a);
		if (color.r < 0 || color.g < 0 || color.b < 0 || color.a < 0)
		{
			int id = instance->getIndex();
			WDEBUG("need to set a color for a book(id = %d)", id);
			color = __books_default_rgb_color[id % COLOR_ARRAY_LENTH];
			instance->setColor(color.r, color.g, color.b, color.a);
			instance->updateDbRecord();
			ids.push_back(id);
		}
	}

	for (auto it = ids.begin(); it != ids.end(); ++it)
	{
			auto entity = bookMap.find(*it);
			if (entity != bookMap.end())
			{
				std::shared_ptr<CalBook> instance = entity->second;
				bookMap.erase(*it);
				std::shared_ptr<CalBook> instanceNew(new CalBook(*instance));
				bookMap.insert(std::pair<int, std::shared_ptr<CalBook>>(instanceNew->getIndex(), instanceNew));
			}
	}

	WLEAVE();
}
