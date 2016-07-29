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

#ifndef _CAL_ALERT_MODEL_H_
#define _CAL_ALERT_MODEL_H_

#include "WApp.h"
#include "CalAlertData.h"

/**
 * @brief Alert model class
 *
 */
class CalAlertModel
{
public:
	CalAlertModel(std::shared_ptr<CalAlertData> alertData);
	virtual ~CalAlertModel();

	/**
	 * @brief Append alert data to alert model.
	 *
	 * @param alertData    alert data to append.
	 *
	 */
	void addAlertData(const std::shared_ptr<CalAlertData>& alertData);

	/**
	 * @brief Replace alert data to alert model.
	 *
	 * @param alertData    alert data to relace.
	 *
	 */
	void relaceAlertData(const std::shared_ptr<CalAlertData>& alertData);

	/**
	 * @brief Get count of alert data items.
	 *
	 * @return alert data count.
	 *
	 */
	int getCount(void);

	/**
	 * @brief Get alert notification item by number.
	 *
	 * @param nth    number of item.
	 *
	 * @return alert notification item.
	 *
	 */
	std::shared_ptr<CalAlertNotificationItem> getAt(int nth);

	/**
	 * @brief Snooze all alerts.
	 *
	 */
	void snoozeAll(void);

	/**
	 * @brief Snooze alert by number.
	 *
	 * @param nth    number of alert to snooze.
	 *
	 */
	void snooze(int nth);

	/**
	 * @brief Dissmiss all alerts.
	 *
	 */
	void dismissAll(void);

	/**
	 * @brief Dissmiss alert by number.
	 *
	 * @param nth    number of alert to dismiss.
	 *
	 */
	void dismiss(int nth);

	/**
	 * @brief Dissmiss alerts.
	 *
	 * @param nths    alerts to dissmiss.
	 *
	 */
	void dismiss(std::vector<int> &nths);

	/**
	 * @brief Remove alert by number.
	 *
	 * @param nth    number of alert to remove.
	 *
	 */
	void remove(int nth);

	/**
	 * @brief Turn ON display.
	 *
	 */
	void turnOnLcd(void);

	/**
	 * @brief Turn OFF display.
	 *
	 */
	void turnOffLcd(void);

	/**
	 * @brief Check whether device is locked.
	 *
	 * @return lock state.
	 *
	 */
	static bool isDeviceLocked(void);

	/**
	 * @brief Set snooze timeout preference.
	 *
	 * @param min     snooze timeout.
	 *
	 */
	void setSnoozeMinute(const int min);

	/**
	 * @brief Get snooze time out preference's value.
	 *
	 * @return snooze timeout.
	 *
	 */
	int getSnoozeMinute(void);

private:
	WDISABLE_COPY_AND_ASSIGN(CalAlertModel);

	void __snooze(std::shared_ptr<CalAlertNotificationItem> &item);
	void __dismiss(std::shared_ptr<CalAlertNotificationItem> &item);

	std::shared_ptr<CalAlertData> __alertData;
};

#endif
