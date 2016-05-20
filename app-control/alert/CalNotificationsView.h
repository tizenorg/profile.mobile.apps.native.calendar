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

#ifndef _CAL_NOTIFICATIONS_VIEW_H_
#define _CAL_NOTIFICATIONS_VIEW_H_

#include <map>
#include "CalView.h"
#include "CalAlertModel.h"
#include "CalBookManager.h"
#include "CalEventManager.h"
#include "CalNotificationsSelectAllItem.h"
#include "CalAlertItem.h"
#include "CalNaviframe.h"

class CalNotificationsView : public CalView
{
public:
	CalNotificationsView(std::shared_ptr<CalAlertData> alertData);
	virtual ~CalNotificationsView();
private:
	WDISABLE_COPY_AND_ASSIGN(CalNotificationsView);
public:
	void replaceAlertData(std::shared_ptr<CalAlertData> alertData);
private:
	virtual Evas_Object* onCreate( Evas_Object* parent, void* viewParam );
	virtual void onDestroy();
	virtual void onBecomeTop();
	virtual void onExpose();
	virtual void onPushed(Elm_Object_Item* naviItem);
	virtual void onEvent(const CalEvent& event);
	void __createAlertEventGenlist(Evas_Object* parent, void* viewParam);
	void __update();
	void __updateSelectAllItems();
	void __launchDetailApp(const std::shared_ptr<CalAlertNotificationItem> &alertItem);
	void __updateCheckStatus();
	void __updateSelectAllCheck(void);
	void __updateButtonStatus(bool leftStatus, bool rightStatus);
private:
	bool __isAllVisible;
	CalAlertModel __model;
	CalDialogControl* __dialog;
	CalNotificationsSelectAllItem *__selectAllItem;
	Evas_Object* __left_button;
	Evas_Object* __right_button;
	std::map<int, CalAlertItem*> __itemMap;
};


#endif
