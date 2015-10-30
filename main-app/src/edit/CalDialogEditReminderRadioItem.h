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

#ifndef _CAL_DIALOG_EDIT_REMINDER_RADIO_ITEM_H_
#define _CAL_DIALOG_EDIT_REMINDER_RADIO_ITEM_H_

#include "CalScheduleReminder.h"
#include "CalDialogControl.h"

class CalDialogEditReminderRadioItem : public CalDialogControl::Item
{
public:
	CalDialogEditReminderRadioItem(const CalScheduleReminder& reminderOri, int radIndex, bool setRadio, bool isOffItem);
	CalDialogEditReminderRadioItem(const CalScheduleReminder& reminderOri, int radIndex, CalScheduleReminder::UnitType unitType, int unitValue, bool setRadio, bool isOffItem);
	virtual ~CalDialogEditReminderRadioItem();
	void setSelectCb(std::function<void (void)> selectCb);
	void setRadioChangeCb(std::function<void (const CalScheduleReminder& reminder)> changeCb);
private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogEditReminderRadioItem);
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
	virtual void onSelect();
private:
	std::function<void (const CalScheduleReminder reminder)> __changeCb;
	const CalScheduleReminder __reminderOri;
	int __radioIndex;
	CalScheduleReminder __reminder;
	std::function<void (void)> __selectCb;
	bool __setRadio;
};

#endif

