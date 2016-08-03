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

#ifndef _CAL_SETTING_ALERT_POPUP_H_
#define _CAL_SETTING_ALERT_POPUP_H_

#include <memory>
#include "CalEvent.h"
#include "CalCommon.h"
#include "WPopup.h"

/**
 * @brief Calendar settings alert popup
 */
class CalSettingAlertPopup: public WPopup
{
public:
	/**
	 * @brief Construct alert popup
	 *
	 * @param callbackFun Popup callback
	 */
	CalSettingAlertPopup(std::function<void ()> callbackFun);

	/**
	 * @brief Destructor
	 */
	virtual ~CalSettingAlertPopup();
private:
	static void __dialogDeleteCallback(void *data, Evas *e, Evas_Object *obj, void *event_info);
	Evas_Object* __createContent(Evas_Object* parent);
private:
	Evas_Object* onCreate(Evas_Object* parent, void* viewParam);
	virtual void onDestroy();
private:
	std::function<void ()> __callbackFun;
};

#endif /* _CAL_SETTING_ALERT_POPUP_H_ */

