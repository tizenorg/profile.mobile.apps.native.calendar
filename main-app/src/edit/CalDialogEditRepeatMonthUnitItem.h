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

#ifndef _CAL_DIALOG_EDIT_REPEAT_MONTH_UNIT_ITEM_H_
#define _CAL_DIALOG_EDIT_REPEAT_MONTH_UNIT_ITEM_H_

class CalDialogEditRepeatMonthUnitItem : public CalDialogControl::Item
{
public:
	CalDialogEditRepeatMonthUnitItem(std::function<void (CalScheduleRepeat::MonthlyType monthlyType)> changedCb, const CalScheduleRepeat repeat, const CalDateTime start, const std::string& timezone, int radioIndex, CalScheduleRepeat::MonthlyType monthlyType);
	virtual ~CalDialogEditRepeatMonthUnitItem();
private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogEditRepeatMonthUnitItem);
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
	void onSelect();
private:
	std::function<void (CalScheduleRepeat::MonthlyType monthlyType)> __changedCb;
	const CalScheduleRepeat __repeat;
	int __radioIndex;
	CalScheduleRepeat::MonthlyType __monthlyType;
	CalDateTime __startTime;
	std::string __timezone;
};

#endif

