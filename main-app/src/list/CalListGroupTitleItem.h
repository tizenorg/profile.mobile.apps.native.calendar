/*
 * Copyright (c) 2009-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

/**
 * @brief Calendar list group title item.
 *
 * @see CalListControl::Item
 */
class CalListGroupTitleItem : public CalListControl::Item
{
public:
	/**
	 * @brief Create calendar list group title item.
	 *
	 * @param[in]   parent The parent.
	 * @param[in]   date   The date.
	 *
	 * @see CalScheduleListControl
	 * @see CalDate
	 */
	CalListGroupTitleItem(CalScheduleListControl *parent, const CalDate &date);
	virtual ~CalListGroupTitleItem();

	/**
	 * @brief Search list group title item.
	 *
	 * @param[in]   date   The date.
	 *
	 * @return the list group title item.
	 *
	 * @see CalScheduleListControl
	 * @see CalDate
	 */
	CalListGroupTitleItem *search(const CalDate &date);

	/**
	 * @brief Add an item.
	 *
	 * @param[in]   schedule            The schedule.
	 * @param[in]   isCheckboxRequired  Is checkbox required or not.
	 * @param[in]   isCheckboxSelected  Is checkbox selected or not.
	 * @param[in]   searchText          The search text.
	 * @param[in]   dir                 The dir.
	 *
	 * @see CalSchedule
	 */
	void addItem(const std::shared_ptr<CalSchedule> &schedule, bool isCheckboxRequired,
			bool isCheckboxSelected, const std::string &searchText, int dir);

	/**
	 * @brief Get selected item count.
	 *
	 * @return the selected item count.
	 */
	int getSelectedItemsCount();

	/**
	 * @brief Select/Deselect all items.
	 *
	 * @param[in]   select  True if all items should be selected, false otherwise.
	 */
	void selectAllItems(bool select);

	/**
	 * @brief Get selected schedules.
	 *
	 * @param[out]  list    The list of selected schedules.
	 */
	void getSelectedSchedules(std::list<std::shared_ptr<CalSchedule>> &list);

private:
	virtual Elm_Genlist_Item_Class *getItemClassStatic();
	void __addToBox(Evas_Object *box, int &height, CalEventItem *eventItem);

	WDISABLE_COPY_AND_ASSIGN(CalListGroupTitleItem);

	CalScheduleListControl *__parent;
	std::string __weekDay;
	std::string __monthDay;
	std::list<CalEventItem *> __normalEvents;
	std::list<CalEventItem *> __allDayEvents;
};

#endif /* _CAL_LIST_GROUP_TITLE_ITEM_H_ */
