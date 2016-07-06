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

#include "CalSchedule.h"

class CalAlertNotificationItem
{
public:
	CalAlertNotificationItem(int recordIndex);
	virtual ~CalAlertNotificationItem();

	/**
	 * @brief Set alert to snoozed.
	 * @param [in]	isSnoozed	Is snoozed.
	 */
	void setSnoozed(bool isSnoozed);

	/**
	 * @brief Set alarm ID.
	 * @param [in]	alarmId	Alarm ID.
	 */
	void setAlarmId(int alarmId);

	/**
	 * @brief Get snooze status.
	 * @return Snooze status value.
	 */
	bool isSnoozed();

	/**
	 * @brief Get schedule ID.
	 * @return Schedule ID.
	 */
	int getScheduleId();

	/**
	 * @brief Get saved record index.
	 * @return Record index.
	 */
	int getRecordIndex();

	/**
	 * @brief Get alarm ID.
	 * @return Alarm ID.
	 */
	int getAlarmId();

	/**
	 * @brief Get schedule instance.
	 * @return Schedule instance.
	 */
	const std::shared_ptr<CalSchedule> getSchedule();

	/**
	 * @brief Get formated event name.
	 * @remarks Result mast be freed.
	 * @return Copy of formated event name.
	 */
	char *getEventName();

	/**
	 * @brief Get original event name.
	 * @remarks Result mast be freed.
	 * @return Copy of original event name.
	 */
	char *getEventOriginalName();

	/**
	 * @brief Get event start time.
	 * @remarks Result mast be freed.
	 * @return Copy of event start time.
	 */
	char *getStartTime();

	/**
	 * @brief Get event end time.
	 * @remarks Result mast be freed.
	 * @return Copy of event end time.
	 */
	char *getEndTime();

	/**
	 * @brief Get event location.
	 * @remarks Result mast be freed.
	 * @return Copy of event location.
	 */
	char *getLocation();

private:
	WDISABLE_COPY_AND_ASSIGN(CalAlertNotificationItem);
	std::shared_ptr<CalSchedule> __schedule;
	bool __isSnoozed;
	int __alarmId;
	int __recordIndex;

	void __getRecord(int recordIndex);
};

