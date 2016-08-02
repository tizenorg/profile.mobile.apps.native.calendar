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

#ifndef _CAL_EDIT_REPEAT_WEEKLY_VIEW_H_
#define _CAL_EDIT_REPEAT_WEEKLY_VIEW_H_

#include "CalSchedule.h"
#include "CalDialogControl.h"

/**
 * @brief View for editing repeat weekly functionality
 *
 * @see CalView
 *
 */
class CalEditRepeatWeeklyView : public CalView
{
public:
	/**
	 * @brief Create view
	 *
	 * @param repeatOri Repeat info
	 * @param dateTime Date
	 * @param timezone Timezone
	 * @param doneCb Done callback
	 *
	 */
	CalEditRepeatWeeklyView(const CalScheduleRepeat repeatOri, const CalDateTime& dateTime, const char* timezone, std::function<void (const CalScheduleRepeat repeat)> doneCb);

	/**
	 * @brief Destructor
	 */
	virtual ~CalEditRepeatWeeklyView();
private:
	WDISABLE_COPY_AND_ASSIGN(CalEditRepeatWeeklyView);
private:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);
	virtual void onPushed(Elm_Object_Item* naviItem);
	virtual void onEvent(const CalEvent& event);
	void __update();
	void __createGenlist();
private:
	std::function<void (const CalScheduleRepeat repeat)> __doneCb;
	CalScheduleRepeat __repeat;
	CalDateTime __startTime;
	std::string __timezone;
	CalDialogControl *__dialog;
	Evas_Object* __doneButton;
	Evas_Object* __genlist;
};

#endif

