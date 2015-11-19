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

#ifndef _CAL_ALERT_APP_H_
#define _CAL_ALERT_APP_H_

#include "CalAlertModel.h"

class CalAlertView;
class CalNotificationsView;

class CalAlertApp
{
public:
	CalAlertApp();
	virtual ~CalAlertApp();

	enum Mode
	{
		CALALERT_NONE = 0,
		CALALERT_VIEW,
		CALALERT_ACTIVE_NOTIFICATION,
		CALALERT_DISMISS,
		CALALERT_SNOOZE,
		CALALERT_SHOW_NOTIFICATION_LIST,
		CALALERT_UPDATE_STATUSBAR
	};

	void onAppControl(app_control_h request, bool firstLaunch);

private:
	void __createWindowSafe(bool isFirstLaunch, bool isAlertPopup);
	void __createWindow(bool isAlertPopup);
	void __checkRequest(const app_control_h request);
	void __launchAlertView(std::shared_ptr<CalAlertData> alertData);
	void __launchNotificationView(std::shared_ptr<CalAlertData> alertData);
	void __setStatusBar(std::shared_ptr<CalAlertData> alertData);
	void __exit();
	void __stopExit();

	CalAlertView* __alertView;
	CalNotificationsView* __notificationsView;
	Ecore_Timer* __timer;

	Mode __mode;
	std::shared_ptr<CalAlertData> __alertData;
	std::shared_ptr<CalAlertModel> __model;
};
#endif /* _CAL_ALERT_APP_H_ */
