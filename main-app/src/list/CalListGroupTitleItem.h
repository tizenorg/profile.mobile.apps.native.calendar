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

#ifndef _CAL_LIST_GROUP_TITLE_ITEM_H_
#define _CAL_LIST_GROUP_TITLE_ITEM_H_

#include <list>
#include "CalListControl.h"
#include "CalEventItem.h"
#include "CalScheduleListControl.h"

class CalListGroupTitleItem : public CalListControl::Item
{
public:
	CalListGroupTitleItem(CalScheduleListControl* parent, const CalDateTime& date);
	virtual ~CalListGroupTitleItem();
public:
	CalListGroupTitleItem* search(const CalDateTime& date);
	void addItem(const std::shared_ptr<CalSchedule>& schedule, bool isCheckboxRequired, bool isCheckboxSelected, const std::string& searchText, int dir);
	int getSelectedItemsCount();
	void selectAllItems(bool select);
	void getSelectedSchedules(std::list<std::shared_ptr<CalSchedule>>& list);
private:
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
private:
	WDISABLE_COPY_AND_ASSIGN(CalListGroupTitleItem);
private:
	CalScheduleListControl* __parent;
	std::string __weekDay;
	std::string __monthDay;
	std::list<CalEventItem*> __listEvents;
};

#endif /* _CAL_LIST_GROUP_TITLE_ITEM_H_ */
