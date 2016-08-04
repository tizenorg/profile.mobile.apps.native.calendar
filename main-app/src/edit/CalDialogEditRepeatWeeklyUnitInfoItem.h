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

#ifndef _CAL_DIALOG_EDIT_REPEAT_WEEKLY_UNIT_INFO_ITEM_H_
#define _CAL_DIALOG_EDIT_REPEAT_WEEKLY_UNIT_INFO_ITEM_H_
#include "CalDateTime.h"

/**
 * @brief Repeat weekly radio item
 */
class CalDialogEditRepeatWeeklyUnitInfoItem : public CalDialogControl::Item
{
public:
	/**
	 * @brief Create item
	 *
	 * @param changedCb Changed callback
	 * @param repeatOri Repeat info
	 * @param startDate Start date
	 * @param timezone  Time zone as string
	 *
	 */
	CalDialogEditRepeatWeeklyUnitInfoItem(std::function<void (CalScheduleRepeat::UnitInfo unitInfo)> changedCb, const CalScheduleRepeat repeatOri, const CalDateTime & startDate, const std::string& timezone);

	/**
	 * @brief Destructor
	 */
	virtual ~CalDialogEditRepeatWeeklyUnitInfoItem();
private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogEditRepeatWeeklyUnitInfoItem);
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
	Evas_Object* createCheck(Evas_Object *parent, const char *text, int index, CalDialogEditRepeatWeeklyUnitInfoItem *item, long long day);
private:
	std::function<void (CalScheduleRepeat::UnitInfo unitInfo)> __changedCb;
	const CalScheduleRepeat __repeat;
	CalScheduleRepeat::UnitInfo __unitInfo;
	CalDateTime __startDate;
	std::string __timezone;
};

#endif

