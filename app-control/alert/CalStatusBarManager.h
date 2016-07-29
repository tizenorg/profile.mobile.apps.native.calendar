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

#ifndef _CAL_STATUS_BAR_MANAGER_H_
#define _CAL_STATUS_BAR_MANAGER_H_

#include <notification.h>
#include <string.h>
#include <vector>
#include "CalSingleton.h"
#include "CalAlertData.h"

/**
 * @brief This class manage status bar natification.
 */
class CalStatusBarManager
{
SINGLETON_IDIOM(CalStatusBarManager);

public:

	/**
	 * @brief Push notification to Status-Bar.
	 *
	 * @param [in]	alertData	Alert descriptor.
	 * @param [in]	addSound	Play sound while notification.
	 *
	 */
	void pushNotification(std::shared_ptr<CalAlertData>& alertData, bool addSound);


	/**
	 * @brief Push Active Notification.
	 *
	 * @param [in]	alertData	Alert descriptor.
	 * @param [in]	addSound	Play sound while notification.
	 *
	 */
	void pushActiveNotification(const std::shared_ptr<CalAlertData>& alertData, bool addSound);

	/**
	 *
	 * @brief Update Status-Bar notifications app-control.
	 *
	 * @param [in]	alertData	New alert data.
	 *
	 */
	void update(std::shared_ptr<CalAlertData> &alertData);

	/**
	 * @brief Remove infromation about event with ID from Status-Bar.
	 *
	 * @param [in]	id	Event ID.
	 *
	 */
	void removeFromNotification(const int id);

	/**
	 * @brief Remove notification from Status-Bar.
	 *
	 * @param [in]	notification	Status-Bar notification handle.
	 *
	 */
	void removeNotification();


	/**
	 * @brief Get IDs of events from Status-Bar notification.
	 *
	 * @param [out]	items		Array of alert items.
	 *
	 */
	void getAllStatusBar(std::vector<std::shared_ptr<CalAlertNotificationItem> > &items);


	/**
	 * @brief Check deleted events on Status-Bar notification.
	 */
	void checkDeletedEvent(void);


private:
	CalStatusBarManager();
	virtual ~CalStatusBarManager();

	WDISABLE_COPY_AND_ASSIGN(CalStatusBarManager);

	void __addVib(notification_h handle);
	void __addSound(notification_h handle);
	void __getSound(std::string& sound);
	void __setupStatusBarNotification(notification_h notification, const std::shared_ptr<CalAlertData> &alertData);
	void __pushActiveNotification(const std::shared_ptr<CalSchedule> &schedule, bool addSound);
	void __postNotification(notification_h notification);

	app_control_h __createAppControl();
	void __setNotificationEventCallback(notification_h notification, notification_event_type_e type_button, std::shared_ptr<CalSchedule> schedule, int action);

	notification_h __getHandle();
	void __setTag(notification_h notification);
	void __getNotificationData(std::shared_ptr<CalAlertData>& alertData);
	bool __setNotificationData(app_control_h service, std::shared_ptr<CalAlertData>& alertData);
	void __setNotificationTitle(notification_h notification, const std::shared_ptr<CalAlertData> &alertData);
};

#endif
