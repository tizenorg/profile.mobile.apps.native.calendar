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

#ifndef _CAL_EDIT_CUSTOM_REMINDER_POPUP_H_
#define _CAL_EDIT_CUSTOM_REMINDER_POPUP_H_

#include <memory>
#include "WPopup.h"
#include "CalSchedule.h"

class CalEditCustomReminderPopup: public WPopup
{
public:
	CalEditCustomReminderPopup(const CalScheduleReminder& reminder);
	virtual ~CalEditCustomReminderPopup();
	void setDoneCb(std::function<void (const CalScheduleReminder& reminder)> doneCb);
	void setCancelCb(std::function<void (void)> canceldCb);
private:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);
	virtual void onDestroy();
	Evas_Object* __createContent(Evas_Object *parent);
	void __getReminder();

private:
	std::function<void (const CalScheduleReminder& reminder)> __doneCb;
	std::function<void (void)> __cancelCb;
	CalScheduleReminder __reminder;
	Evas_Object* __unitTypeSp;
	Evas_Object* __unitValueSp;
};

#endif
