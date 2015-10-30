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

#ifndef _CAL_EDIT_REMINDER_VIEW_H_
#define _CAL_EDIT_REMINDER_VIEW_H_

#include <Elementary.h>
#include <memory>

#include "WPopup.h"
#include "CalView.h"
#include "CalSchedule.h"
#include "CalDialogControl.h"
#include "CalDialogEditReminderRadioItem.h"
#include "CalEditCustomReminderPopup.h"

class CalEditReminderPopup : public WPopup
{
public:
	CalEditReminderPopup(const CalScheduleReminder& reminderOri, bool isEditReminder);
	virtual ~CalEditReminderPopup();
	void setDoneCb(std::function<void (const CalScheduleReminder& reminder, int radio_index)> doneCb);
	void setCustomSelectedCb(std::function<void (const CalScheduleReminder& reminder, int radio_index)> customSelectedCb);
private:
	WDISABLE_COPY_AND_ASSIGN(CalEditReminderPopup);
private:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);
	virtual void onDestroy();
	Evas_Object* __createContent(Evas_Object *parent);
private:
	void __addRadioItem(CalDialogControl *dialog, Evas_Object* radioObj, int& radioIndex, CalScheduleReminder::UnitType type, int unitValue);
	void __addRadioCustomItem(CalDialogControl *dialog, Evas_Object* radioObj, int& radioIndex);
private:
	std::function<void (const CalScheduleReminder& reminder, int radio_index)> __doneCb;
	std::function<void (const CalScheduleReminder& reminder, int radio_index)> __customSelectedCb;
	CalScheduleReminder __reminder;
	CalDialogEditReminderRadioItem* __customItem;
	bool __isEditReminder;

	int __radio_index;
};

#endif

