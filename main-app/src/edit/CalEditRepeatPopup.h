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

#ifndef _CAL_EDIT_REPEAT_POPUP_H_
#define _CAL_EDIT_REPEAT_POPUP_H_

#include "WPopup.h"
#include "CalDialogEditRepeatRadioItem.h"

class CalEditRepeatPopup: public WPopup
{
public:
	CalEditRepeatPopup(const CalScheduleRepeat::UnitType defaultUnitType, std::function<void (const CalScheduleRepeat::UnitType repeat)> doneCb);
	virtual ~CalEditRepeatPopup();
private:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);
	virtual void onDestroy();

	CalDialogEditRepeatRadioItem* __addRadioItem(Evas_Object *radio_main, int radio_index, CalScheduleRepeat::UnitType unitType);
	Evas_Object* __createContent(Evas_Object *parent);

private:
	std::function<void (const CalScheduleRepeat::UnitType repeatUnitType)> __doneCb;
	CalScheduleRepeat::UnitType __repeatUnitType;
	//CalScheduleRepeat __repeat;
	//CalDateTime __startTime;
	//const char* __timezone;
};

#endif

