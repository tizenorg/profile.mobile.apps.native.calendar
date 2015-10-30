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

#ifndef _CAL_DIALOG_DATE_TIME_ITEM_H_
#define _CAL_DIALOG_DATE_TIME_ITEM_H_

#include <functional>
#include <Elementary.h>

#include "CalDialogControl.h"
#include "CalDateTime.h"

class CalDialogEditDateTimeItem : public CalDialogControl::Item
{
public:
	CalDialogEditDateTimeItem( int sortIndex,const CalDateTime &startDateTime, const CalDateTime &endDateTime);
	CalDialogEditDateTimeItem();
	virtual ~CalDialogEditDateTimeItem();

public:
	void updateValueFromPickerPopup(Evas_Object* dateTime);
	void setTitle(const char* title);
	void updateStartAndEndTime(const CalDateTime & startDateTime, const CalDateTime &endDateTime);
	void setChangeCb(std::function<void (CalDateTime &startDateTime,CalDateTime &endDateTime)> changedCb);
	void setDateButtonClickedCb(std::function<void (Evas_Object* dateTime)> dateButtonClickedCb);
	void setTimeButtonClickedCb(std::function<void (Evas_Object* dateTime)> timeButtonClickedCb);

	void setTimeZone(const std::string& timezone);
	void getDateTime(CalDateTime* startDateTime, CalDateTime* endDateTime, Eina_Bool* allDay);
	void setDate(const CalDateTime & date);
	void setTime(const CalDateTime & time);
	bool isStartSelected();
	bool isAllDay();
	bool isDateChanged();
	void update();

private:
	virtual Elm_Genlist_Item_Class* getItemClassStatic();

private:
	Evas_Object* onCreateDateTime(Evas_Object* parent, bool start);
	Evas_Object* onCreateAllDayCheckBox(Evas_Object* parent);

	static void __onTimeButtonClicked(void* data, Evas_Object* obj, void* event_info);
	static void __onDateButtonClicked(void* data, Evas_Object* obj, void* event_info);
	static void __onStartDateClicked(void* data, Evas_Object* obj, void* event_info);
	static void __onEndDateClicked(void* data, Evas_Object* obj, void* event_info);
	static void __onStartTimeClicked(void* data, Evas_Object* obj, void* event_info);
	static void __onEndTimeClicked(void* data, Evas_Object* obj, void* event_info);

private:
	std::function<void (CalDateTime &startDateTime,CalDateTime &endDateTime)> __changedCb;
	std::function<void (Evas_Object* dateTime)> __dateButtonClickedCb;
	std::function<void (Evas_Object* dateTime)> __timeButtonClickedCb;
	Evas_Object* __parent;
	Evas_Object* __startDateTimeBox;
	Evas_Object* __endDateTimeBox;
	Evas_Object* __allDayCheckBox;
	Evas_Object* __timeStartButton;
	Evas_Object* __timeEndButton;
	CalDateTime __start;
	CalDateTime __end;
	CalDateTime __startAllDay;
	CalDateTime __endAllDay;

	bool __isDateChanged;
	bool __isAllDay;
	bool __isStartSelected;
};

#endif /* _CAL_DIALOG_DATE_TIME_ITEM_H_ */
