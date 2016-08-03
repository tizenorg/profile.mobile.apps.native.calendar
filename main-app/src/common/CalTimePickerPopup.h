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

#ifndef _CAL_DATE_TIME_PICKER_POPUP_H_
#define _CAL_DATE_TIME_PICKER_POPUP_H_

#include "WPopup.h"

#include <functional>
#include <Elementary.h>
#include <string>
#include "CalDateTime.h"

/**
 * @brief Calendar time picker pop-up.
 *
 * @see WPopup.
 */
class CalTimePickerPopup: public WPopup
{
public:
	/**
	 * @brief Create time picker pop-up.
	 *
	 * @param[in]   dateTime        Date and time.
	 * @param[in]   title           Title.
	 * @param[in]   startDateTime   Start date and time.
	 * @param[in]   isRepeat        Repeat state.
	 *
	 * @see CalDateTime
	 */
	CalTimePickerPopup(Evas_Object* dateTime, const char* title = NULL, CalDateTime startDateTime = CalDateTime(1902,1,1), bool isRepeat = false);

	/**
	 * @brief Set change callback.
	 *
	 * @param[in]   changedCb   Change callback.
	 */
	void setChangeCb(std::function<void (CalDateTime& newTime)> changedCb);

	/**
	 * @brief Set timezone.
	 *
	 * @param[in]   timezone   Timezone.
	 */
	void setTimeZone(const std::string& timezone);

protected:
	virtual ~CalTimePickerPopup();

private:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);
	virtual void onDestroy();
	Evas_Object* __createContent(Evas_Object *parent);
	CalTimePickerPopup();

	std::function<void (CalDateTime& newTime)> __changedCb;
	Evas_Object* __dateTime;
	CalDateTime __startDateTime;
	bool __isRepeat;
	std::string __title;
};

#endif
