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

#include <glib.h>
#include <string>
#include "CalOriginalSchedule.h"
#include "CalCommon.h"
#include "CalLocaleManager.h"

#define WEEKDAY_BUFFER 32

CalOriginalSchedule::CalOriginalSchedule(int year, int month)
{
}

CalOriginalSchedule::CalOriginalSchedule(calendar_record_h record) :
	CalSchedule(record)
{
	char *view_uri = NULL;
	int error = CALENDAR_ERROR_NONE;
	error =  calendar_record_get_uri_p(record, &view_uri);
	if (error != CALENDAR_ERROR_NONE)
	{
		WASSERT(0);
	}

	if(g_strcmp0(view_uri, _calendar_event._uri) == 0)
	{
		__type = CalSchedule::ORIGINAL;
	}
	else
	{
		__type = CalSchedule::INVALID;
	}

	updateRepeat();
}

CalOriginalSchedule::~CalOriginalSchedule()
{
}

CalOriginalSchedule::CalOriginalSchedule(const CalOriginalSchedule& obj) :
	CalSchedule(obj)
{
	__repeat = obj.__repeat;
}

const CalOriginalSchedule& CalOriginalSchedule::operator=(const CalOriginalSchedule& obj)
{
	CalSchedule::operator=(obj);
	if (this != &obj)
	{
		__repeat = obj.__repeat;
	}
	return *this;
}

void CalOriginalSchedule::setStart(const CalDateTime& startTime)
{
	const unsigned int propertyId = getStartProperty();
	int error = calendar_record_set_caltime(__record, propertyId, getCaltime(startTime));
	WPRET_M((error != CALENDAR_ERROR_NONE),"set_caltime fail");

	updateRepeat();
}

const int CalOriginalSchedule::getRemindersCount() const
{
	int error = 0;
	unsigned int alarmCount = 0;
	error = calendar_record_get_child_record_count(getRecord(), _calendar_event.calendar_alarm, &alarmCount);
	WPRET_VM((error!=CALENDAR_ERROR_NONE), 0, "get child count fail");
	return alarmCount;
}

const CalScheduleRepeat& CalOriginalSchedule::getRepeat() const
{
	return __repeat;
}

void CalOriginalSchedule::setRepeat(const CalScheduleRepeat& repeat)
{
	int error = 0;

	// update
	__repeat = repeat;

	// set record
	switch(repeat.unitType)
	{
	case CalScheduleRepeat::UnitType::ONE_TIME:
	{
		error = calendar_record_set_int(getRecord(), _calendar_event.freq, CALENDAR_RECURRENCE_NONE);
		if (error != CALENDAR_ERROR_NONE) break;
	}
		break;
	case CalScheduleRepeat::UnitType::DAY:
	{
		error = calendar_record_set_int(getRecord(), _calendar_event.freq, CALENDAR_RECURRENCE_DAILY);
		if (error != CALENDAR_ERROR_NONE) break;
	}
		break;
	case CalScheduleRepeat::UnitType::WEEK:
	{
		error = calendar_record_set_int(getRecord(), _calendar_event.freq, CALENDAR_RECURRENCE_WEEKLY);
		if (error != CALENDAR_ERROR_NONE) break;
		std::string byday;

		if (repeat.unitInfo.weekly.selected[0] == true)
		{
			byday.append("SU");
		}
		if (repeat.unitInfo.weekly.selected[1] == true)
		{
			if (!byday.empty())
				byday.append(",");
			byday.append("MO");
		}
		if (repeat.unitInfo.weekly.selected[2] == true)
		{
			if (!byday.empty())
				byday.append(",");
			byday.append("TU");
		}
		if (repeat.unitInfo.weekly.selected[3] == true)
		{
			if (!byday.empty())
				byday.append(",");
			byday.append("WE");
		}
		if (repeat.unitInfo.weekly.selected[4] == true)
		{
			if (!byday.empty())
				byday.append(",");
			byday.append("TH");
		}
		if (repeat.unitInfo.weekly.selected[5] == true)
		{
			if (!byday.empty())
				byday.append(",");
			byday.append("FR");
		}
		if (repeat.unitInfo.weekly.selected[6] == true)
		{
			if (!byday.empty())
				byday.append(",");
			byday.append("SA");
		}
		error = calendar_record_set_str(getRecord(), _calendar_event.byday, byday.c_str());
	}
		break;
	case CalScheduleRepeat::UnitType::MONTH:
	{
		{
			// get start time
			calendar_time_s start;
			error = calendar_record_get_caltime(getRecord(), _calendar_event.start_time, &start);
			WPRET_M((error!=CALENDAR_ERROR_NONE), "fail");
			CalDateTime dtTime;
			if (start.type == CALENDAR_TIME_UTIME)
			{
				dtTime.set(start.time.utime);
			}
			else
			{
				dtTime.set(start.time.date.year,start.time.date.month,start.time.date.mday);
			}
			// TODO check
			__repeat.unitInfo.monthly.date.year = dtTime.getYear();
			__repeat.unitInfo.monthly.date.month = dtTime.getMonth();
			__repeat.unitInfo.monthly.date.mday = dtTime.getMday();
			__repeat.startDate.year = dtTime.getYear();
			__repeat.startDate.month = dtTime.getMonth();
			__repeat.startDate.mday = dtTime.getMday();
		}

		error = calendar_record_set_int(getRecord(), _calendar_event.freq, CALENDAR_RECURRENCE_MONTHLY);
		if (error != CALENDAR_ERROR_NONE) {WERROR("fail");break;}
		if (repeat.unitInfo.monthly.type == CalScheduleRepeat::MonthlyType::WEEKDAY)
		{
			CalDateTime start(__repeat.startDate.year, __repeat.startDate.month, __repeat.startDate.mday);
			int weekNum = 0;// TODO CALDATETIME: update this after CalDateTime class is ready to use CalLocaleManager::getInstance().getDayOfWeekInMonth(start.getUtimeFromTm());

			char dayofweek[WEEKDAY_BUFFER] = {0};
			snprintf(dayofweek, sizeof(dayofweek), "%d", weekNum);
			const char *wdayText = CalLocaleManager::getInstance().getWeekdayText(start.getUtimeFromTm());
			strncat(dayofweek, wdayText, WEEKDAY_BUFFER - 1);
			WDEBUG("dayofweek[%s]",dayofweek);
			error = calendar_record_set_str(getRecord(), _calendar_event.byday, dayofweek);
			if (error != CALENDAR_ERROR_NONE) break;
			// reset bymonthday
			error = calendar_record_set_str(getRecord(), _calendar_event.bymonthday, "");
			if (error != CALENDAR_ERROR_NONE) break;

		}
		else
		{
			char bymonthday[WEEKDAY_BUFFER];
			// TODO check local mday
			snprintf(bymonthday, sizeof(bymonthday), "%d", __repeat.unitInfo.monthly.date.mday);
			WDEBUG("bymonthday[%s]",bymonthday);
			// bymonthday
			error = calendar_record_set_str(getRecord(), _calendar_event.bymonthday, bymonthday);
			if (error != CALENDAR_ERROR_NONE) break;
			// reset byday !
			error = calendar_record_set_str(getRecord(), _calendar_event.byday, "");
			if (error != CALENDAR_ERROR_NONE) break;
		}
	}
		break;
	case CalScheduleRepeat::UnitType::YEAR:
	{
		{
			// get start time
			calendar_time_s start;
			error = calendar_record_get_caltime(getRecord(), _calendar_event.start_time, &start);
			WPRET_M((error!=CALENDAR_ERROR_NONE), "fail");
			CalDateTime dtTime;
			if (start.type == CALENDAR_TIME_UTIME)
			{
				dtTime.set(start.time.utime);
			}
			else
			{
				dtTime.set(start.time.date.year,start.time.date.month,start.time.date.mday);
			}
			// TODO check
			__repeat.unitInfo.monthly.date.year = dtTime.getYear();
			__repeat.unitInfo.monthly.date.month = dtTime.getMonth();
			__repeat.unitInfo.monthly.date.mday = dtTime.getMday();
			__repeat.startDate.year = dtTime.getYear();
			__repeat.startDate.month = dtTime.getMonth();
			__repeat.startDate.mday = dtTime.getMday();
		}

		calendar_record_set_int(getRecord(), _calendar_event.freq, CALENDAR_RECURRENCE_YEARLY);
		char bymonthday[WEEKDAY_BUFFER];
		// TODO check local mday
		snprintf(bymonthday, sizeof(bymonthday), "%d", __repeat.unitInfo.monthly.date.mday);
		WDEBUG("bymonthday[%s]",bymonthday);
		// bymonthday
		error = calendar_record_set_str(getRecord(), _calendar_event.bymonthday, bymonthday);
		if (error != CALENDAR_ERROR_NONE) break;
		// reset byday !
		error = calendar_record_set_str(getRecord(), _calendar_event.byday, "");
		if (error != CALENDAR_ERROR_NONE) break;
	}
		break;
	}

	// interval
	int interval = __repeat.unitInterval;
	if (interval <= 0)
		interval = 1;
	error = calendar_record_set_int(getRecord(), _calendar_event.interval, interval);
	if (error != CALENDAR_ERROR_NONE)
		WERROR("set fail");

	// TODO until..
	switch(repeat.untilType)
	{
	case CalScheduleRepeat::UntilType::DUE_DATE:
		error = calendar_record_set_int(getRecord(), _calendar_event.range_type, CALENDAR_RANGE_UNTIL);
		if (error != CALENDAR_ERROR_NONE) break;
		calendar_time_s untilTime;
		calendar_time_s startTime;
		error = calendar_record_get_caltime(getRecord(), _calendar_event.start_time, &startTime);
		if (error != CALENDAR_ERROR_NONE) break;
		if (startTime.type == CALENDAR_TIME_UTIME)
		{
			struct tm startTm;
			// TODO CALDATETIME: update this after CalDateTime class is ready to use
			//CalLocaleManager::getInstance().getTmFromUtime(getTimeZone(),startTime.time.utime, startTm);
			startTm.tm_year = repeat.untilInfo.date.year - 1900;
			startTm.tm_mon = repeat.untilInfo.date.month - 1;
			startTm.tm_mday = repeat.untilInfo.date.mday;
			untilTime.type = CALENDAR_TIME_UTIME;
			// TODO CALDATETIME: update this after CalDateTime class is ready to use
			//untilTime.time.utime = CalLocaleManager::getInstance().getUtimeFromTm(getTimeZone(),startTm);
		}
		else
		{
			untilTime.type = CALENDAR_TIME_LOCALTIME;
			untilTime.time.date.year = repeat.untilInfo.date.year;
			untilTime.time.date.month = repeat.untilInfo.date.month;
			untilTime.time.date.mday = repeat.untilInfo.date.mday;
			untilTime.time.date.hour = 0;
			untilTime.time.date.minute = 0;
			untilTime.time.date.second = 0;
			untilTime.time.date.is_leap_month = false;
		}
		error = calendar_record_set_caltime(getRecord(), _calendar_event.until_time, untilTime);
		break;
	case CalScheduleRepeat::UntilType::TIMES:
		error = calendar_record_set_int(getRecord(), _calendar_event.range_type, CALENDAR_RANGE_COUNT);
		if (error != CALENDAR_ERROR_NONE) break;
		error = calendar_record_set_int(getRecord(), _calendar_event.count, __repeat.untilInfo.times);
		if (error != CALENDAR_ERROR_NONE) break;
		break;
	case CalScheduleRepeat::UntilType::FOREVER:
		error = calendar_record_set_int(getRecord(), _calendar_event.range_type, CALENDAR_RANGE_NONE);
		if (error != CALENDAR_ERROR_NONE) break;
		break;
	}

	WPRET_M((error != CALENDAR_ERROR_NONE), "fail");

}

unsigned int CalOriginalSchedule::getIndexProperty(void) const
{
	if (__type == CalSchedule::ORIGINAL)
		return _calendar_event.id;
	WASSERT(0);
	return 0;
}

unsigned int CalOriginalSchedule::getStartProperty(void) const
{
	if (__type == CalSchedule::ORIGINAL)
		return _calendar_event.start_time;
	WASSERT(0);
	return 0;
}

unsigned int CalOriginalSchedule::getEndProperty(void) const
{
	if (__type == CalSchedule::ORIGINAL)
		return _calendar_event.end_time;
	WASSERT(0);
	return 0;
}

unsigned int CalOriginalSchedule::getSummaryProperty(void) const
{
	if (__type == CalSchedule::ORIGINAL)
		return _calendar_event.summary;
	WASSERT(0);
	return 0;
}

unsigned int CalOriginalSchedule::getLocationProperty(void) const
{
	if (__type == CalSchedule::ORIGINAL)
		return _calendar_event.location;
	WASSERT(0);
	return 0;
}

unsigned int CalOriginalSchedule::getDescriptionProperty(void) const
{
	if (__type == CalSchedule::ORIGINAL)
		return _calendar_event.description;
	WASSERT(0);
	return 0;
}

unsigned int CalOriginalSchedule::getBookIdProperty(void) const
{
	if (__type == CalSchedule::ORIGINAL)
		return _calendar_event.calendar_book_id;
	WASSERT(0);
	return 0;
}

unsigned int CalOriginalSchedule::getHasReminderProperty(void) const
{
	if (__type == CalSchedule::ORIGINAL)
		return _calendar_event.has_alarm;
	WASSERT(0);
	return 0;
}

unsigned int CalOriginalSchedule::getHasRepeatProperty(void) const
{
	return 0;
}

unsigned int CalOriginalSchedule::getStartTimezoneProperty(void) const
{
	if (__type == CalSchedule::ORIGINAL)
		return _calendar_event.start_tzid;
	WASSERT(0);
	return 0;
}

unsigned int CalOriginalSchedule::getEndTimezoneProperty(void) const
{
	if (__type == CalSchedule::ORIGINAL)
		return _calendar_event.end_tzid;
	WASSERT(0);
	return 0;
}

unsigned int CalOriginalSchedule::getSyncData1Property(void) const
{
	if (__type == CalSchedule::ORIGINAL)
		return _calendar_event.sync_data1;
	WASSERT(0);
	return 0;
}

unsigned int CalOriginalSchedule::getOriginalEventIdProperty() const
{
	if (__type == CalSchedule::ORIGINAL)
		return _calendar_event.original_event_id;
	WASSERT(0);
	return 0;
}

unsigned int CalOriginalSchedule::getRecurranceIdProperty() const
{
	if (__type == CalSchedule::ORIGINAL)
		return _calendar_event.recurrence_id;
	WASSERT(0);
	return 0;
}

unsigned int CalOriginalSchedule::getUidProperty() const
{
	if (__type == CalSchedule::ORIGINAL)
		return _calendar_event.uid;
	WASSERT(0);
	return 0;
}

unsigned int CalOriginalSchedule::getSyncData2Property() const
{
	if (__type == CalSchedule::ORIGINAL)
		return _calendar_event.sync_data2;
	WASSERT(0);
	return 0;
}

void CalOriginalSchedule::updateRepeat(void)
{
	// freq, range_type, until_time, count, interval,
	// byday, bymonthday, byyearday, byweekno, bymonth
	// wkst

	int error = 0;
	int value = 0;
	error = calendar_record_get_int(getRecord(), _calendar_event.freq, &value);
	WPRET_M((error!=CALENDAR_ERROR_NONE), "fail");
	switch(value)
	{
	default:
	case CALENDAR_RECURRENCE_NONE:
		__repeat.unitType = CalScheduleRepeat::UnitType::ONE_TIME;
		break;
	case CALENDAR_RECURRENCE_DAILY:
		__repeat.unitType = CalScheduleRepeat::UnitType::DAY;
		break;
	case CALENDAR_RECURRENCE_WEEKLY:
		{
			__repeat.unitType = CalScheduleRepeat::UnitType::WEEK;
			char *byday = NULL;
			error = calendar_record_get_str_p(getRecord(), _calendar_event.byday, &byday);
			WPRET_M((error!=CALENDAR_ERROR_NONE), "fail");
			if (strstr(byday, "SU") != NULL)
			{
				__repeat.unitInfo.weekly.selected[0] = true;
			}
			if (strstr(byday, "MO") != NULL)
			{
				__repeat.unitInfo.weekly.selected[1] = true;
			}
			if (strstr(byday, "TU") != NULL)
			{
				__repeat.unitInfo.weekly.selected[2] = true;
			}
			if (strstr(byday, "WE") != NULL)
			{
				__repeat.unitInfo.weekly.selected[3] = true;
			}
			if (strstr(byday, "TH") != NULL)
			{
				__repeat.unitInfo.weekly.selected[4] = true;
			}
			if (strstr(byday, "FR") != NULL)
			{
				__repeat.unitInfo.weekly.selected[5] = true;
			}
			if (strstr(byday, "SA") != NULL)
			{
				__repeat.unitInfo.weekly.selected[6] = true;
			}
		}
		break;
	case CALENDAR_RECURRENCE_MONTHLY:
		{
			__repeat.unitType = CalScheduleRepeat::UnitType::MONTH;
			char *byday = NULL;
			char *bymonthday = NULL;
			error = calendar_record_get_str_p(getRecord(), _calendar_event.byday, &byday);
			WPRET_M((error!=CALENDAR_ERROR_NONE), "fail");
			if (byday && strlen(byday) > 0)
			{
				// byday
				__repeat.unitInfo.monthly.type = CalScheduleRepeat::MonthlyType::WEEKDAY;
			}
			else
			{
				// bymonthday
				error = calendar_record_get_str(getRecord(), _calendar_event.bymonthday, &bymonthday);
				WPRET_M((error!=CALENDAR_ERROR_NONE), "fail");
				__repeat.unitInfo.monthly.type = CalScheduleRepeat::MonthlyType::MONTHDAY;
			}
			//  __repeat.unitInfo.monthly.date
			calendar_time_s start;
			error = calendar_record_get_caltime(getRecord(), _calendar_event.start_time, &start);
			WPRET_M((error!=CALENDAR_ERROR_NONE), "fail");
			CalDateTime dtTime;
			if (start.type == CALENDAR_TIME_UTIME)
			{
				dtTime.set(start.time.utime);
			}
			else
			{
				dtTime.set(start.time.date.year,start.time.date.month,start.time.date.mday);
			}
			__repeat.unitInfo.monthly.date.year = dtTime.getYear();
			__repeat.unitInfo.monthly.date.month = dtTime.getMonth();
			__repeat.unitInfo.monthly.date.mday = dtTime.getMday();
		}
		break;
	case CALENDAR_RECURRENCE_YEARLY:
		__repeat.unitType = CalScheduleRepeat::UnitType::YEAR;
		break;
	}

	error = calendar_record_get_int(getRecord(), _calendar_event.interval, &value);
	WPRET_M((error!=CALENDAR_ERROR_NONE), "fail");
	__repeat.unitInterval = value;

	error = calendar_record_get_int(getRecord(), _calendar_event.range_type, &value);
	WPRET_M((error!=CALENDAR_ERROR_NONE), "fail");

	switch(value)
	{
	case CALENDAR_RANGE_UNTIL:
		{
			__repeat.untilType = CalScheduleRepeat::UntilType::DUE_DATE;
			calendar_time_s untilTime;
			error = calendar_record_get_caltime(getRecord(), _calendar_event.until_time, &untilTime);
			WPRET_M((error!=CALENDAR_ERROR_NONE), "fail");
			int year = 0, month = 0, mday = 0;
			if (untilTime.type == CALENDAR_TIME_UTIME)
			{
				if (CALENDAR_RECORD_NO_UNTIL == untilTime.time.utime)
				{
					__repeat.untilType = CalScheduleRepeat::UntilType::FOREVER;
					break;
				}
				else
				{
					CalDateTime dateTime;
					dateTime.set(untilTime.time.utime);
					year = dateTime.getYear();
					month = dateTime.getMonth();
					mday = dateTime.getMday();
				}
			}
			else
			{
				year = untilTime.time.date.year;
				month = untilTime.time.date.month;
				mday = untilTime.time.date.mday;
			}
			__repeat.untilInfo.date.year = year;
			__repeat.untilInfo.date.month = month;
			__repeat.untilInfo.date.mday = mday;
		}
		break;
	case CALENDAR_RANGE_COUNT:
		{
			__repeat.untilType = CalScheduleRepeat::UntilType::TIMES;
			error = calendar_record_get_int(getRecord(), _calendar_event.count, &value);
			WPRET_M((error!=CALENDAR_ERROR_NONE), "fail");
			__repeat.untilInfo.times = value;
		}
		break;
	default:
	case CALENDAR_RANGE_NONE:
		__repeat.untilType = CalScheduleRepeat::UntilType::FOREVER;
		break;
	}

	do
	{
		calendar_time_s start;
		error = calendar_record_get_caltime(getRecord(), _calendar_event.start_time, &start);
		WPRET_M((error!=CALENDAR_ERROR_NONE), "fail");
		CalDateTime dtTime;
		if (start.type == CALENDAR_TIME_UTIME)
		{
			dtTime.set(start.time.utime);
		}
		else
		{
			dtTime.set(start.time.date.year,start.time.date.month,start.time.date.mday);
		}
		__repeat.startDate.year = dtTime.getYear();
		__repeat.startDate.month = dtTime.getMonth();
		__repeat.startDate.mday = dtTime.getMday();
	}while(0);
}

void CalOriginalSchedule::getReminder(const int nth, CalScheduleReminder& reminder)
{
	int error = 0;
	WPRET_M((getRecord()==NULL), "fail");
	calendar_record_h alarmHandle = NULL;
	error = calendar_record_get_child_record_at_p(getRecord(),_calendar_event.calendar_alarm, nth, &alarmHandle);
	WPRET_M((error!=CALENDAR_ERROR_NONE), "fail");
	__convertAlarmToReminder(alarmHandle, reminder);
}

void CalOriginalSchedule::setReminder(const int nth, const CalScheduleReminder& reminder)
{
	int error = 0;
	WPRET_M((getRecord()==NULL), "fail");
	calendar_record_h alarmHandle = NULL;
	error = calendar_record_get_child_record_at_p(getRecord(),_calendar_event.calendar_alarm, nth, &alarmHandle);
	WPRET_M((error!=CALENDAR_ERROR_NONE), "fail");
	__convertReminderToAlarm(reminder, alarmHandle);
}

int CalOriginalSchedule::addReminder(const CalScheduleReminder& reminder)
{
	int error = 0;
	unsigned int alarmCount = 0;
	error = calendar_record_get_child_record_count(getRecord(), _calendar_event.calendar_alarm, &alarmCount);
	WPRET_VM((error!=CALENDAR_ERROR_NONE), -1, "get child count fail");

	if (alarmCount >= CAL_SCHEDULE_REMINDERS_MAX)
		return -1;

	// check duplicate -> save time
	calendar_record_h alarmHandle;
	alarmHandle = __convertReminderToAlarm(reminder);
	WPRET_VM((NULL==alarmHandle), -1, "convert fail");

	error = calendar_record_add_child_record(getRecord(), _calendar_event.calendar_alarm, alarmHandle);
	if (error != CALENDAR_ERROR_NONE)
	{
		WERROR("add child fail");
		calendar_record_destroy(alarmHandle, true);
		return -1;
	}
	return 0;
}

void CalOriginalSchedule::removeReminder(const int nth)
{
	int error = 0;
	WPRET_M((getRecord()==NULL), "fail");
	calendar_record_h alarmHandle = NULL;
	error = calendar_record_get_child_record_at_p(getRecord(),_calendar_event.calendar_alarm, nth, &alarmHandle);
	WPRET_M((error!=CALENDAR_ERROR_NONE), "fail");

	error = calendar_record_remove_child_record(getRecord(),_calendar_event.calendar_alarm, alarmHandle);
	WPRET_M((error!=CALENDAR_ERROR_NONE), "fail");
	calendar_record_destroy(alarmHandle, true);
}

void CalOriginalSchedule::__convertAlarmToReminder(const calendar_record_h alarm, CalScheduleReminder& reminder)
{
	int error = 0;
	int tickUnit = CALENDAR_ALARM_NONE;
	//calendar_time_s alarmTime = {};
	int tick = 0;
	error = calendar_record_get_int(alarm, _calendar_alarm.tick, &tick);
	WPRET_M((error!=CALENDAR_ERROR_NONE), "fail");
	error = calendar_record_get_int(alarm, _calendar_alarm.tick_unit, &tickUnit);
	WPRET_M((error!=CALENDAR_ERROR_NONE), "fail");

	if (tick == 0) {
		reminder.unitValue = 0;
		reminder.unitType = CalScheduleReminder::UnitType::NONE;
		WDEBUG("[tick:%d][tickUnit:%d]",tick,reminder.unitType);
	}

	switch(tickUnit)
	{
	case CALENDAR_ALARM_TIME_UNIT_SPECIFIC:
		// TODO specipic
		reminder.unitType = CalScheduleReminder::UnitType::MIN;
		break;
	case CALENDAR_ALARM_TIME_UNIT_MINUTE:
		reminder.unitType = CalScheduleReminder::UnitType::MIN;
		break;
	case CALENDAR_ALARM_TIME_UNIT_HOUR:
		reminder.unitType = CalScheduleReminder::UnitType::HOUR;
		break;
	case CALENDAR_ALARM_TIME_UNIT_DAY:
		reminder.unitType = CalScheduleReminder::UnitType::DAY;
		break;
	case CALENDAR_ALARM_TIME_UNIT_WEEK:
		reminder.unitType = CalScheduleReminder::UnitType::WEEK;
		break;
	case CALENDAR_ALARM_NONE:
	default:
		reminder.unitType = CalScheduleReminder::UnitType::NONE;
		break;
	}
	reminder.unitValue = tick;
	WDEBUG("[tick:%d][tickUnit:%d]",tick,reminder.unitType);
}

void CalOriginalSchedule::__convertReminderToAlarm(const CalScheduleReminder& reminder, calendar_record_h alarm)
{
	int error = CALENDAR_ERROR_NONE;

	do
	{
		int tick = reminder.unitValue;
		WDEBUG("[tick:%d][tickUnit:%d]",tick,reminder.unitType);

		int tickUnit = 0;
		switch(reminder.unitType)
		{
		case CalScheduleReminder::UnitType::NONE:
			tickUnit = CALENDAR_ALARM_NONE;
			break;
		case CalScheduleReminder::UnitType::MIN:
			tickUnit = CALENDAR_ALARM_TIME_UNIT_MINUTE;
			break;
		case CalScheduleReminder::UnitType::HOUR:
			tickUnit = CALENDAR_ALARM_TIME_UNIT_HOUR;
			break;
		case CalScheduleReminder::UnitType::DAY:
			tickUnit = CALENDAR_ALARM_TIME_UNIT_DAY;
			break;
		case CalScheduleReminder::UnitType::WEEK:
			tickUnit = CALENDAR_ALARM_TIME_UNIT_WEEK;
			break;
		}
		error = calendar_record_set_int(alarm, _calendar_alarm.tick, tick);
		if (error != CALENDAR_ERROR_NONE) break;
		error = calendar_record_set_int(alarm, _calendar_alarm.tick_unit, tickUnit);
		if (error != CALENDAR_ERROR_NONE) break;

	} while(0);

	if (error != CALENDAR_ERROR_NONE)
		WERROR("reminder fail");
}

calendar_record_h CalOriginalSchedule::__convertReminderToAlarm(const CalScheduleReminder& reminder)
{
	calendar_record_h alarmHandle = NULL;
	int error = calendar_record_create(_calendar_alarm._uri, &alarmHandle);
	if (error != CALENDAR_ERROR_NONE)
		return NULL;
	__convertReminderToAlarm(reminder, alarmHandle);
	return alarmHandle;
}

void CalOriginalSchedule::initialize(const char* tzid, const CalDateTime& startTime)
{
	int error = 0;
	WPRET_M((getRecord()==NULL), "fail");
	do
	{
		CalDateTime bufTime = startTime;
		setStart(bufTime);
		bufTime.addHours(1);
		setEnd(bufTime);

		if (tzid)
		{
			error = calendar_record_set_str(getRecord(), _calendar_event.start_tzid, tzid);
			if (error != CALENDAR_ERROR_NONE) break;
			error = calendar_record_set_str(getRecord(), _calendar_event.end_tzid, tzid);
			if (error != CALENDAR_ERROR_NONE) break;
		}
		error = calendar_record_set_int(getRecord(), _calendar_event.busy_status, CALENDAR_EVENT_BUSY_STATUS_BUSY);
		if (error != CALENDAR_ERROR_NONE) break;
		error = calendar_record_set_int(getRecord(), _calendar_event.sensitivity, CALENDAR_SENSITIVITY_CONFIDENTIAL);
		if (error != CALENDAR_ERROR_NONE) break;
	} while(0);
	if (error != CALENDAR_ERROR_NONE)
		WERROR("set fail");
}

int CalOriginalSchedule::getInstanceCountBeforeDate(const CalDateTime& date) const
{
	calendar_time_s caltime = getCaltime(date);

	int event_id = 0;

	calendar_record_get_int(getRecord(), _calendar_event.id, &event_id);

	calendar_query_h query = NULL;
	calendar_filter_h filter = NULL;
	int instance_order = 0;

	if (date.isAllDay()) {
		int error = calendar_query_create(_calendar_instance_localtime_calendar_book._uri, &query);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_query_create has failed(%d)");

		error = calendar_filter_create(_calendar_instance_localtime_calendar_book._uri, &filter);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_filter_create has failed(%d)");

		error = calendar_filter_add_int(filter,_calendar_instance_localtime_calendar_book.event_id, CALENDAR_MATCH_EQUAL, event_id);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_filter_add_int has failed(%d)");

		error = calendar_filter_add_operator(filter, CALENDAR_FILTER_OPERATOR_AND);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_filter_add_operator has failed(%d)");

		error = calendar_filter_add_caltime(filter, _calendar_instance_localtime_calendar_book.start_time, CALENDAR_MATCH_LESS_THAN, caltime);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_filter_add_caltime has failed(%d)");

		error = calendar_query_set_filter(query, filter);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_query_set_filter has failed(%d)");

		error = calendar_db_get_count_with_query(query, &instance_order);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_db_get_count_with_query has failed(%d)");

		error = calendar_filter_destroy(filter);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_filter_destroy has failed(%d)");

		error = calendar_query_destroy(query);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_query_destroy has failed(%d)");
		// for exdate
		char *exdate = NULL;
		error = calendar_record_get_str_p(getRecord(), _calendar_event.exdate, &exdate);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_record_get_str_p has failed(%d)");

		if (exdate) {
			// 20141024,20141025
			gchar **ptr = NULL;
			gchar **split_str = NULL;

			split_str = g_strsplit(exdate,",", 0);
			for(ptr = split_str; *ptr; ptr++) {
				int y=0,m=0,d=0;
				char* buf = (char*)*ptr;
				sscanf(buf,"%4d%2d%2d", &y, &m, &d);
				CalDateTime tmp(y,m,d);
				if (tmp < date) {
					instance_order++;
				}
			}
			g_strfreev(split_str);
		}
	} else {

		int error = calendar_query_create(_calendar_instance_utime_calendar_book._uri, &query);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_query_create has failed(%d)");

		error = calendar_filter_create(_calendar_instance_utime_calendar_book._uri, &filter);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_filter_create has failed(%d)");

		error = calendar_filter_add_int(filter,_calendar_instance_utime_calendar_book.event_id, CALENDAR_MATCH_EQUAL, event_id);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_filter_add_int has failed(%d)");

		error = calendar_filter_add_operator(filter, CALENDAR_FILTER_OPERATOR_AND);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_filter_add_operator has failed(%d)");

		error = calendar_filter_add_caltime(filter, _calendar_instance_utime_calendar_book.start_time, CALENDAR_MATCH_LESS_THAN, caltime);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_filter_add_caltime has failed(%d)");

		error = calendar_query_set_filter(query, filter);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_query_set_filter has failed(%d)");

		error = calendar_db_get_count_with_query(query, &instance_order);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_db_get_count_with_query has failed(%d)");

		error = calendar_filter_destroy(filter);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_filter_destroy has failed(%d)");

		error = calendar_query_destroy(query);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_query_destroy has failed(%d)");
		// for exdate
		char *exdate = NULL;
		error = calendar_record_get_str_p(getRecord(), _calendar_event.exdate, &exdate);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_record_get_str_p has failed(%d)");

		if (exdate) {
			// 20141024T070000Z,20141025T070000Z
			gchar **ptr = NULL;
			gchar **split_str = NULL;

			split_str = g_strsplit(exdate,",", 0);
			for(ptr = split_str; *ptr; ptr++) {
				int y=0,m=0,d=0,hour=0,min=0;
				char* buf = (char*)*ptr;
				sscanf(buf,"%4d%2d%2dT%2d%2d", &y, &m, &d, &hour, &min);
				CalDateTime tmp(y,m,d,hour,min,0);
				if (tmp < date) {
					instance_order++;
				}
			}
			g_strfreev(split_str);
		}
	}

	return instance_order;
}

void CalOriginalSchedule::truncateRepeatToBeforeDate(const CalDateTime& date)
{
	int repeat_range_type = 0;
	int error = calendar_record_get_int(getRecord(), _calendar_event.range_type, &repeat_range_type);
	if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_record_get_int has failed(%d)");

	if (repeat_range_type == CALENDAR_RANGE_COUNT) {

		int error = calendar_record_set_int(getRecord(), _calendar_event.count, getInstanceCountBeforeDate(date));
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_record_set_int has failed(%d)");
	} else {

		// set rrule until
		int error = calendar_record_set_int(getRecord(), _calendar_event.range_type, CALENDAR_RANGE_UNTIL);
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_record_set_int has failed(%d)");

		CalDateTime untilTime(date);
		untilTime.addDays(-1);
		error = calendar_record_set_caltime(getRecord(), _calendar_event.until_time, CalSchedule::getCaltime(untilTime));
		if (error != CALENDAR_ERROR_NONE)
			WERROR("calendar_record_set_caltime has failed(%d)");
	}
}
