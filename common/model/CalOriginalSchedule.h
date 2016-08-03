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

#ifndef _CAL_ORIGINAL_SCHEDULE_H_
#define _CAL_ORIGINAL_SCHEDULE_H_

#include "CalSchedule.h"
#include "CalScheduleReminder.h"
#include "CalScheduleRepeat.h"

/**
 * @brief The CalOriginalSchedule class represents original event record.
 *
 */
class WAPP_ASSIST_EXPORT CalOriginalSchedule: public CalSchedule
{
public:
	CalOriginalSchedule(int year, int month);
	CalOriginalSchedule(calendar_record_h record);
	CalOriginalSchedule(const CalOriginalSchedule&);
	virtual ~CalOriginalSchedule();

	const CalOriginalSchedule& operator=(const CalOriginalSchedule& obj);

	/**
	 * @brief Set start time to event.
	 *
	 * @param[in] startTime      event start time
	 *
	 */
	virtual void setStart(const CalDateTime &startTime);

	/**
	 * @brief Get reminders count.
	 *
	 * @return reminders count.
	 *
	 */
	virtual const int getRemindersCount() const;

	/**
	 * @brief Get event repeat.
	 *
	 * @return event repeat.
	 *
	 */
	virtual const CalScheduleRepeat &getRepeat() const;

	/**
	 * @brief Set repeat to event.
	 *
	 * @param[in] repeat      event repeat
	 *
	 */
	virtual void setRepeat(const CalScheduleRepeat &repeat);

	/**
	 * @brief Get event reminder by index.
	 *
	 * @param[in]  nth        reminder index
	 * @param[out] reminder   reminder
	 *
	 */
	virtual void getReminder(const int nth, CalScheduleReminder &reminder);

	/**
	 * @brief Set event reminder by index.
	 *
	 * @param[in] nth        reminder index
	 * @param[in] reminder   reminder to set
	 */
	virtual void setReminder(const int nth, const CalScheduleReminder &reminder);

	/**
	 * @brief Add reminder to event.
	 *
	 * @param[in] reminder   reminder to add
	 *
	 * @return 0 - success, -1 if failed.
	 *
	 */
	virtual int addReminder(const CalScheduleReminder &reminder);

	/**
	 * @brief Remove reminder by index.
	 *
	 * @param[in] nth        reminder index
	 *
	 */
	virtual void removeReminder(const int nth);

	/**
	 * @brief Initialization.
	 *
	 * @param[in] tzid        time zone
	 * @param[in] startTime   event start time
	 *
	 */
	virtual void initialize(const char *tzid, const CalDateTime &startTime);

protected:
	int getInstanceCountBeforeDate(const CalDateTime& date) const;
	void truncateRepeatToBeforeDate(const CalDateTime& date);

	virtual unsigned int getIndexProperty(void) const;
	virtual unsigned int getStartProperty(void) const;
	virtual unsigned int getEndProperty(void) const;
	virtual unsigned int getSummaryProperty(void) const;
	virtual unsigned int getLocationProperty(void) const;
	virtual unsigned int getDescriptionProperty(void) const;
	virtual unsigned int getBookIdProperty(void) const;
	virtual unsigned int getHasReminderProperty(void) const;
	virtual unsigned int getHasRepeatProperty(void) const;
	virtual unsigned int getStartTimezoneProperty(void) const;
	virtual unsigned int getEndTimezoneProperty(void) const;
	virtual unsigned int getSyncData1Property(void) const;
	virtual unsigned int getOriginalEventIdProperty() const;
	virtual unsigned int getRecurranceIdProperty() const;
	virtual unsigned int getUidProperty() const;
	virtual unsigned int getSyncData2Property() const;

private:
	void updateRepeat(void);
	void __convertAlarmToReminder(const calendar_record_h alarm, CalScheduleReminder& reminder);
	void __convertReminderToAlarm(const CalScheduleReminder& reminder, calendar_record_h alarm);
	calendar_record_h __convertReminderToAlarm(const CalScheduleReminder& reminder);

private:
	CalScheduleRepeat __repeat;

friend class CalDataManager;
};

#endif /* _CAL_ORIGINAL_SCHEDULE_H_ */
