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

#ifndef _CAL_ALERT_DATA_H_
#define _CAL_ALERT_DATA_H_

#include <vector>
#include "WApp.h"
#include "CalAlertNotificationItem.h"

/**
 * @brief Alert data class.
 *
 */
class CalAlertData
{
public:
	CalAlertData(app_control_h request);
	CalAlertData();
	CalAlertData(const CalAlertData&);
	const CalAlertData& operator=(const CalAlertData&);
	virtual ~CalAlertData();

	/**
	 * @brief Get tick.
	 *
	 * @return tick from alert data.
	 *
	 */
	int getTick(void);

	/**
	 * @brief Get units.
	 *
	 * @return unit from alert data.
	 *
	 */
	int getUnit(void);

	/**
	 * @brief Get notification items count.
	 *
	 * @return count of notification items.
	 *
	 */
	int getCount(void);

	/**
	 * @brief Get element by number.
	 *
	 * @param nth    number of item.
	 *
	 * @return notification item.
	 *
	 */
	std::shared_ptr<CalAlertNotificationItem> getAt(int nth);

	/**
	 * @brief Check whether notification item is snoozed.
	 *
	 * @param nth    number of item.
	 *
	 * @return snoozed state.
	 *
	 */
	bool isSnoozed(int nth);

	/**
	 * @brief Remove notification item.
	 *
	 * @param nth    number of item.
	 *
	 * @return true if removed successfully, false otherwise.
	 *
	 */
	void remove(int nth);

	/**
	 * @brief Remove notification item by event id.
	 *
	 * @param id    event id.
	 *
	 * @return true if removed successfully, false otherwise.
	 *
	 */
	bool removeById(int id);

	/**
	 * @brief Clear alert list.
	 *
	 */
	void clear();

	/**
	 * @brief Append alert data.
	 *
	 * @param data    alert data to append.
	 *
	 */
	void add(const CalAlertData& data);

	/**
	 * @brief Replace alert data.
	 *
	 * @param data    alert data to replace.
	 *
	 */
	void replace(const CalAlertData& data);

private:
	void __addRecord(int recordIndex, bool isSnoozed = false);

	int __tick;
	int __unit;
	std::vector<std::shared_ptr<CalAlertNotificationItem>> __alerts;
};

#endif
