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

/**
 * @brief Item with date picker
 */
class CalDialogEditDateTimeItem : public CalDialogControl::Item
{
public:
	/**
	 * @brief Create item
	 *
	 * @param sortIndex     Item sort index
	 * @param startDateTime Default start date
	 * @param endDateTime   Default end date
	 *
	 */
	CalDialogEditDateTimeItem( int sortIndex,const CalDateTime &startDateTime, const CalDateTime &endDateTime);

	/**
	 * @brief Default constructor
	 */
	CalDialogEditDateTimeItem();

	/**
	 * @brief Destructor
	 */
	virtual ~CalDialogEditDateTimeItem();

public:
	/**
	 * @brief Update value from picker popup
	 *
	 * @param dateTime Date picker
	 *
	 */
	void updateValueFromPickerPopup(Evas_Object* dateTime);

	/**
	 * @brief Set item text
	 *
	 * @param title Title text
	 */
	void setTitle(const char* title);

	/**
	 * @brief Update date pickers values
	 *
	 * @param startDateTime Start date picker control
	 * @param endDateTime   End date picker control
	 *
	 */
	void updateStartAndEndTime(const CalDateTime & startDateTime, const CalDateTime &endDateTime);

	/**
	 * @brief Set change callback
	 *
	 * @param changedCb Changed callback
	 *
	 */
	void setChangeCb(std::function<void (CalDateTime &startDateTime,CalDateTime &endDateTime)> changedCb);

	/**
	 * @brief Set date clicked callback
	 *
	 * @param dateButtonClickedCb Clicked callback
	 *
	 */
	void setDateButtonClickedCb(std::function<void (Evas_Object* dateTime)> dateButtonClickedCb);

	/**
	 * @brief Set time clicked callback
	 * @param timeButtonClickedCb Clicked callback
	 *
	 */
	void setTimeButtonClickedCb(std::function<void (Evas_Object* dateTime)> timeButtonClickedCb);

	/**
	 * @brief Set time zone
	 *
	 * @param timezone Time zone string representation
	 *
	 */
	void setTimeZone(const std::string& timezone);

	/**
	 * @brief Get date, time and isAllday flag
	 *
	 * @param[out] startDateTime Start time
	 * @param[out] endDateTime   End time
	 * @param[out] allDay        Is all day
	 *
	 */
	void getDateTime(CalDateTime* startDateTime, CalDateTime* endDateTime, Eina_Bool* allDay);

	/**
	 * @brief Set date
	 *
	 * @param date Date to set
	 *
	 */
	void setDate(const CalDateTime & date);

	/**
	 * @brief Set time
	 * @param time Time to set
	 *
	 */
	void setTime(const CalDateTime & time);

	/**
	 * @brief Is start selected
	 *
	 * @return true if selected, otherwise false
	 */
	bool isStartSelected();

	/**
	 * @brief Determines whether all day is checked
	 *
	 * @return true is checked, otherwise false
	 */
	bool isAllDay();

	/**
	 * @brief Determines if date is changed
	 *
	 * @return true if it changed, otherwise false
	 */
	bool isDateChanged();

	/**
	 * @brief Updates item
	 */
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

	bool __isDateChanged;
	bool __isAllDay;
	bool __isStartSelected;
};

#endif /* _CAL_DIALOG_DATE_TIME_ITEM_H_ */
