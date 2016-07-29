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

#ifndef _CAL_ALERT_VIEW_H_
#define _CAL_ALERT_VIEW_H_

#include "CalView.h"
#include "CalAlertData.h"
#include "CalAlertModel.h"

/**
 * @brief Alert main view class.
 *
 */
class CalAlertView : public CalView
{
public:
	CalAlertView(std::shared_ptr<CalAlertData> alertData);
	virtual ~CalAlertView();

	/**
	 * @brief Replace alert view model.
	 *
	 * @param alertData     alert view model.
	 *
	 */
	void replaceAlertData(std::shared_ptr<CalAlertData> alertData);

private:
	WDISABLE_COPY_AND_ASSIGN(CalAlertView);

	virtual Evas_Object* onCreate( Evas_Object* parent, void* viewParam );
	virtual void onDestroy();
	virtual void onBecomeTop();
	virtual void onExpose();
	virtual void onPushed(Elm_Object_Item* naviItem);
	virtual void onEvent(const CalEvent& event);
	virtual void onMenuButton();

	CalAlertModel __model;
	bool __dismissDown;
	bool __snoozeDown;
	bool __actionLock;
	Ecore_Timer* __timer;
	Evas_Object* __dismissImageLine;
	Evas_Object* __snoozeImageLine;
	int __startXPostion;
	int __startYPostion;
	Evas_Object* __dismiss;
	Evas_Object* __snooze;
};


#endif
