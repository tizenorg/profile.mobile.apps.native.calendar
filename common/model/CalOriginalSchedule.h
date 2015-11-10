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

#include <calendar.h>
#include "CalSchedule.h"
#include "CalScheduleReminder.h"
#include "CalScheduleRepeat.h"

class WAPP_ASSIST_EXPORT CalOriginalSchedule: public CalSchedule
{
public:
	CalOriginalSchedule(int year, int month);
	CalOriginalSchedule(calendar_record_h record);
	virtual ~CalOriginalSchedule();

	CalOriginalSchedule(const CalOriginalSchedule&);
	const CalOriginalSchedule& operator=(const CalOriginalSchedule&);

public:
	virtual void setStart(const CalDateTime& startTime);

	virtual const int getRemindersCount() const;
	virtual const CalScheduleRepeat& getRepeat() const;
	virtual void setRepeat(const CalScheduleRepeat& repeat);
	virtual void getReminder(const int nth, CalScheduleReminder& reminder);
	virtual void setReminder(const int nth, const CalScheduleReminder& reminder);
	virtual int addReminder(const CalScheduleReminder& reminder);
	virtual void removeReminder(const int nth);
	virtual void initialize(const char* tzid, const CalDateTime& startTime);

protected:
	int getInstanceCountBeforeDate(const CalDateTime& date) const;
	void truncateRepeatToBeforeDate(const CalDateTime& date);

protected:
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
