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

#ifndef _CAL_NUMBER_PICKER_POPUP_H_
#define _CAL_NUMBER_PICKER_POPUP_H_

#include <memory>
#include "WPopup.h"
#include "CalSchedule.h"
#include <efl_extension.h>

/**
 * @brief Popup for choose repeat reminder
 *
 * @see WPopup
 */
class CalNumberPickerPopup: public WPopup
{
public:
	/**
	 * @brief Create popup
	 *
	 * @param repeat Repeat info
	 * @param isTime Is time
	 *
	 */
	CalNumberPickerPopup(const CalScheduleRepeat& repeat, bool isTime);

	/**
	 * @brief Destructor
	 */
	virtual ~CalNumberPickerPopup();

	/**
	 * @breif Set done callback
	 *
	 * @param doneCb Done callback
	 *
	 */
	void setDoneCb(std::function<void (const CalScheduleRepeat& repeat)> doneCb);

	/**
	 * @brief Set cancel callback
	 *
	 * @param canceldCb Cancel callback
	 *
	 */
	void setCancelCb(std::function<void (void)> canceldCb);
private:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);
	virtual void onDestroy();
	Evas_Object* __createContent(Evas_Object *parent);
	void __getRepeat( bool isTime);

private:
	std::function<void (const CalScheduleRepeat& repeat)> __doneCb;
	std::function<void (void)> __cancelCb;
	CalScheduleRepeat __repeat;
	Evas_Object* __unitValueSp;
	bool __isTime;
	int __number;
};

#endif

