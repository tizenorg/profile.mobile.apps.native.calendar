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

#ifndef _CAL_DIALOG_EDIT_REPEAT_UNTIL_DUE_DATE_ITEM_H_
#define _CAL_DIALOG_EDIT_REPEAT_UNTIL_DUE_DATE_ITEM_H_

#include "CalDialogControl.h"
#include "CalDateTime.h"

class CalDialogEditRepeatUntilDueDateItem : public CalDialogControl::Item
{
public:
	CalDialogEditRepeatUntilDueDateItem(std::function<void (CalScheduleRepeat::UntilType untilType, CalScheduleRepeat::UntilInfo untilInfo)> ChangedCb, const CalScheduleRepeat repeatOri, int radioIndex, const CalDateTime& dateTime);
	virtual ~CalDialogEditRepeatUntilDueDateItem();
	void setDateButtonClickedCb(std::function<void (Evas_Object* dateTime)> dateButtonClickedCb);
	void setDateButtonTime(const CalScheduleRepeat repeat);
	CalDateTime& getUntilDate();
private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogEditRepeatUntilDueDateItem);
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
	static void __onDateButtonClicked(void* data, Evas_Object* obj, void* event_info);
private:
	std::function<void (Evas_Object* dateTime)> __dateButtonClickedCb;
	std::function<void (CalScheduleRepeat::UntilType untilType, CalScheduleRepeat::UntilInfo untilInfo)> __changedCb;
	const CalScheduleRepeat __repeat;
	int __radioIndex;
	CalScheduleRepeat::UntilInfo __untilInfo;
	CalDateTime __dateTime;
	Evas_Object *__radio;
	Evas_Object *__button;
};

#endif

