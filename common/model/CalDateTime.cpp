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

#include <stdio.h>
#include <string.h>
#include "CalLocaleManager.h"
#include "CalSettingsManager.h"
#include "CalCommon.h"
#include "CalDateTime.h"

#define CAL_STRUCT_TM_YEAR_LOWER_SHIFT 1900
#define CAL_STRUCT_TM_MONTH_ARRAY_LOWER_SHIFT 1
#define CAL_DATETIME_YEAR_UPPER_BOUND 2037
#define CAL_DATETIME_MONTH_UPPER_BOUND 12
#define CAL_DATETIME_DAY_UPPER_BOUND 31
#define CAL_DATETIME_YEAR_LOWER_BOUND 1902
#define CAL_DATETIME_MONTH_LOWER_BOUND 1
#define CAL_DATETIME_DAY_LOWER_BOUND 1

#define CAL_TIME_MINUTES_IN_HOUR   60
#define CAL_TIME_SECONDS_IN_MINUTE 60
#define CAL_TIME_HOURS_IN_DAY 24
#define CAL_TIME_SECONDS_IN_HOUR   (CAL_TIME_MINUTES_IN_HOUR * CAL_TIME_SECONDS_IN_MINUTE)

// sec <-> millisec
#define ms2sec(ms) ((long long int)ms / 1000)
#define sec2ms(s) ((i18n_udate)s * 1000.0)

CalDateTime::CalDateTime(): __cal(NULL), __fixedDate(false)
{
	createUCalendar();
	time_t current_time = 0;
	time(&current_time);
	__utime = current_time;
}

CalDateTime::CalDateTime(const long long int uTime, bool fixedDate): __cal(NULL), __utime(uTime), __fixedDate(fixedDate)
{
	createUCalendar();
}

CalDateTime::CalDateTime(const struct tm& dateTm, bool fixedDate): __cal(NULL), __fixedDate(fixedDate) //Difference between current and 1970 years
{
	createUCalendar();
	set(dateTm);
}

CalDateTime::CalDateTime(int year, int month, int mday, int hour, int min, int sec, bool fixedDate): __cal(NULL), __fixedDate(fixedDate)
{
	WENTER();
	createUCalendar();
	struct tm time = {};
	__createTm(year, month, mday, hour, min, sec, &time);
	if (__fixedDate)
	{
		__utime = mktime(&time);
	}
	else
	{
		set(time);
	}
	WLEAVE();
}

CalDateTime::~CalDateTime()
{
	WENTER();
	//TODO OBLIGATORY!!!
//	if (__cal)
//		i18n_ucalendar_destroy(__cal);
	WLEAVE();
}

CalDateTime::CalDateTime(const CalDateTime& obj)
{
	__utime = obj.__utime;
	__fixedDate = obj.__fixedDate;
	__cal = obj.__cal;
}

const CalDateTime& CalDateTime::operator=(const CalDateTime& obj)
{
	if (this != &obj)
	{
		__utime = obj.__utime;
		__fixedDate = obj.__fixedDate;
		__cal = obj.__cal;
	}
	return *this;
}

bool CalDateTime::operator==(const CalDateTime &obj) const
{
	//TODO: Investigate presence
	if (__fixedDate != obj.__fixedDate)
	{
		return false;
	}
	return ((getYear() == obj.getYear()) && (getMonth() == obj.getMonth()) && (getMday() == obj.getMday()));
}

bool CalDateTime::operator !=(const CalDateTime &obj) const
{
	return !(*this == obj);
}

bool CalDateTime::operator >(const CalDateTime &obj) const
{
	long long int objTime = obj.getUtime();
	return (__utime > objTime);
}

bool CalDateTime::operator >=(const CalDateTime &obj) const
{
	long long int srcTime = getUtime();
	long long int objTime = obj.getUtime();
	return (srcTime >= objTime);
}

bool CalDateTime::operator <(const CalDateTime &obj) const
{
	return !(*this >= obj);
}

bool CalDateTime::operator <=(const CalDateTime &obj) const
{
	return !(*this > obj);
}

bool CalDateTime::isSameDay(const CalDateTime &obj) const
{
	return ((getYear() == obj.getYear()) && (getMonth() == obj.getMonth()) && (getMday() == obj.getMday()) &&
			(getHour() == obj.getHour()) && (getMinute() == obj.getMinute()) && (getSecond() == obj.getSecond()));
}

const char* CalDateTime::getWeekdayText(const long long int utime)
{
	static char weeks[7][3] = {"SU", "MO", "TU", "WE", "TH", "FR", "SA"};

	i18n_ucalendar_set_milliseconds(__cal, sec2ms(utime));
	int wday = 0;
	i18n_ucalendar_get(__cal, I18N_UCALENDAR_DAY_OF_WEEK, &wday);
	return weeks[wday - 1];
}

void CalDateTime::createUCalendar()
{
	int status = I18N_ERROR_NONE;

	if (__cal)
	{
		WDEBUG("Ucalendar is not NULL");
		return;
	}

	const char* locale;
	i18n_ulocale_get_default(&locale);
	status = i18n_ucalendar_create(0, -1, locale, I18N_UCALENDAR_GREGORIAN , &__cal);
	if (status != I18N_ERROR_NONE)
	{
		WERROR("i18n_ucalendar_create got an error : %d", status);
	}

	WDEBUG("Created Ucalendar");
}

void CalDateTime::getUCalendar(struct tm &tm) const
{
	WENTER();

	if (!__cal)
	{
		WDEBUG("Ucalendar is NULL");
		return;
	}

	i18n_ucalendar_get(__cal, I18N_UCALENDAR_SECOND, &tm.tm_sec);
	i18n_ucalendar_get(__cal, I18N_UCALENDAR_MINUTE, &tm.tm_min);
	i18n_ucalendar_get(__cal, I18N_UCALENDAR_HOUR_OF_DAY, &tm.tm_hour);
	i18n_ucalendar_get(__cal, I18N_UCALENDAR_DATE , &tm.tm_mday);
	i18n_ucalendar_get(__cal, I18N_UCALENDAR_MONTH, &tm.tm_mon);
	i18n_ucalendar_get(__cal, I18N_UCALENDAR_YEAR, &tm.tm_year);
	tm.tm_year -= 1900;
	i18n_ucalendar_get(__cal, I18N_UCALENDAR_DAY_OF_WEEK, &tm.tm_wday);
	tm.tm_wday -= 1;
	i18n_ucalendar_get(__cal, I18N_UCALENDAR_DAY_OF_YEAR, &tm.tm_yday);
	tm.tm_yday -= 1;
	i18n_ucalendar_get(__cal, I18N_UCALENDAR_DST_OFFSET, &tm.tm_isdst);
	if(tm.tm_isdst)
		tm.tm_isdst = 1;
	else
		tm.tm_isdst = 0;
	WLEAVE();
}

void CalDateTime::setUCalendar(const struct tm &tm) const
{
	WENTER();

	int status = I18N_ERROR_NONE;

	status = i18n_ucalendar_set_date_time(__cal, tm.tm_year + 1900, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	if(status != I18N_ERROR_NONE)
	{
		WDEBUG("i18n_ucalendar_set_date_time() is fail. error:%d", status);
		WDEBUG("tm.m_year:%d, tm.tm_mon:%d, tm.tm_mday:%d, tm.tm_hour:%d, tm.tm_min:%d,  tm.tm_sec:%d ", tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	}
	WLEAVE();
}

void CalDateTime::set(const int year, const int month, const int mday, const int hour, const int minute, const int second)
{
	if (__fixedDate)
	{
		WDEBUG("Cannot set values to object with fixed date");
		return;
	};

	struct tm time = {};
	__createTm(year, month, mday, hour, minute, second, &time);
	__utime = mktime(&time);
}

void CalDateTime::set(const long long int utime)
{
	if (__fixedDate)
	{
		WDEBUG("Cannot set values to object with fixed date");
		return;
	};

	__utime = utime;
}

void CalDateTime::set(const struct tm& dateTm) //Must be difference between current year and 1970
{
	if (__fixedDate)
	{
		WDEBUG("Cannot set values to object with fixed date");
		return;
	};

	setUCalendar(dateTm);
	i18n_udate date;
	i18n_ucalendar_get_milliseconds(__cal, &date);
	__utime = ms2sec(date);
}

int CalDateTime::getYear() const
{
	struct tm time = {};
	getTmTime(&time);
	return time.tm_year + CAL_STRUCT_TM_YEAR_LOWER_SHIFT;
}

int CalDateTime::getMonth() const
{
	struct tm time = {};
	getTmTime(&time);
	return time.tm_mon + CAL_STRUCT_TM_MONTH_ARRAY_LOWER_SHIFT;
}

int CalDateTime::getMday() const
{
	struct tm time = {};
	getTmTime(&time);
	return time.tm_mday;
}

int CalDateTime::getHour() const
{
	struct tm time = {};
	getTmTime(&time);
	return time.tm_hour;
}

int CalDateTime::getMinute() const
{
	if (__fixedDate)
	{
		return 0;
	}
	else
	{
		struct tm time = {};
		getTmTime(&time);
		return time.tm_min;
	}
}

int CalDateTime::getSecond() const
{
	if (__fixedDate)
	{
		return 0;
	}
	else
	{
		struct tm time = {};
		getTmTime(&time);
		return time.tm_sec;
	}
}

void CalDateTime::getTmTime(struct tm *dateTm) const
{
	if (!dateTm)
	{
		WERROR("invalid input");
		return;
	}
	if (__fixedDate)
	{
		time_t utime = (long int) __utime;
		struct tm *time = gmtime(&utime);
		memcpy(dateTm, time, sizeof(struct tm));
	}
	else
	{
		i18n_ucalendar_set_milliseconds(__cal, sec2ms(__utime));
		getUCalendar(*dateTm);
	}
}

long long int CalDateTime::getUtime() const
{
	return __utime;
}

void CalDateTime::getString(std::string& text) const
{
	CalLocaleManager::TimeFormat tf = CalLocaleManager::TIMEFORMAT_1;
	CalLocaleManager::DateFormat df = CalLocaleManager::DATEFORMAT_1;

	int todayyear = 0;

	struct tm today;
	time_t current_time = 0;
	time(&current_time);
	getTmTime(&today);
	todayyear = today.tm_year + CAL_STRUCT_TM_YEAR_LOWER_SHIFT;

	bool isSameYear = false;
	if (todayyear == getYear())
		isSameYear = true;

	if (CalSettingsManager::getInstance().isHour24() == false)
		tf = CalLocaleManager::TIMEFORMAT_1;
	else
		tf = CalLocaleManager::TIMEFORMAT_6;
	if (isSameYear == true && __fixedDate == false)
		df = CalLocaleManager::DATEFORMAT_24;
	__getString(df, tf, text);
}

void CalDateTime::getTimeString(std::string& text) const
{
	CalLocaleManager::TimeFormat tf = CalLocaleManager::TIMEFORMAT_2;
	if (CalSettingsManager::getInstance().isHour24() == false)
		tf = CalLocaleManager::TIMEFORMAT_1;
	else
		tf = CalLocaleManager::TIMEFORMAT_6;
	text.clear();
	if (__fixedDate == true)
	{
		return;
	}
	__getString(CalLocaleManager::DATEFORMAT_NONE, tf, text);
}

void CalDateTime::getDateString(std::string& text) const
{
	__getString(CalLocaleManager::DATEFORMAT_1, CalLocaleManager::TimeFormat::TIMEFORMAT_NONE, text);
}

void CalDateTime::__getString(int df, int tf, std::string& text) const
{
	if (__fixedDate)
	{
		CalLocaleManager::getInstance().getDateTimeText(
				(CalLocaleManager::DateFormat)df,
				CalLocaleManager::TimeFormat::TIMEFORMAT_NONE, *this, text);
	}
	else
	{
		CalLocaleManager::getInstance().getDateTimeText(
				(CalLocaleManager::DateFormat)df,
				(CalLocaleManager::TimeFormat)tf, *this, text);
	}
}

bool CalDateTime::isAllDay() const
{
	return __fixedDate;
}

const char* CalDateTime::getWeekdayText() const
{
	long long int utime = getUtime();
	return CalLocaleManager::getInstance().getWeekdayText(utime);
}

int CalDateTime::getWeekday() const
{
	i18n_ucalendar_set_milliseconds(__cal, sec2ms(__utime));
	int wday = 0;
	i18n_ucalendar_get(__cal, I18N_UCALENDAR_DAY_OF_WEEK, &wday);
	return (wday-1);
}

void CalDateTime::setAllDay(const bool isAllDay)
{
	if (__fixedDate == isAllDay)
	{
		return;
	}
	struct tm time = {};
	if (isAllDay == true)
	{
		__utime = mktime(&time);
	}
	else
	{
		//TODO investigate requirement, prefer to delete
		struct tm *date = NULL;
		time_t utime = (long int) __utime;
		date = gmtime(&utime);
		getTmTime(&time);
		time.tm_year = date->tm_year - CAL_STRUCT_TM_YEAR_LOWER_SHIFT;
		time.tm_mon = date->tm_mon - CAL_STRUCT_TM_MONTH_ARRAY_LOWER_SHIFT;
		time.tm_mday = date->tm_mday;
		set(time);
	}
	__fixedDate = isAllDay;
	return;
}

void CalDateTime::addSeconds(const long long int seconds, const bool setLimit)
{
	if (__fixedDate == true)
	{
		WERROR("invalid");
		return;
	}
	__utime += seconds;

	if (setLimit == true)
		__setLimit();
}

void CalDateTime::addMinutes(const int minutes, const bool setLimit)
{
	if (__fixedDate == true)
	{
		WERROR("invalid");
		return;
	}
	addSeconds(minutes * CAL_TIME_SECONDS_IN_MINUTE, setLimit);
}

void CalDateTime::addHours(const int hours, const bool setLimit)
{
	if (__fixedDate == true)
	{
		WERROR("invalid");
		return;
	}
	addSeconds(hours * CAL_TIME_SECONDS_IN_HOUR, setLimit);
}

void CalDateTime::addDays(const int days, const bool setLimit)
{
	WDEBUG("addDays: %lld", __utime);

	if (__fixedDate == true)
	{
		WDEBUG("invalid");
		return;
	}

	__utime += days * CAL_TIME_HOURS_IN_DAY * CAL_TIME_SECONDS_IN_HOUR;

	if (setLimit == true)
		__setLimit();

	WDEBUG("addDays: %lld", __utime);
}

void CalDateTime::addMonths(const int months, const bool setLimit)
{
	struct tm time = {};

	getTmTime(&time);
	setUCalendar(time);
	i18n_ucalendar_add(__cal, I18N_UCALENDAR_MONTH, months);
	getUCalendar(time);
	set(time);

	if (setLimit == true)
		__setLimit();
}

void CalDateTime::addYears(const int years, const bool setLimit)
{
	i18n_udate utime = 0;
	i18n_ucalendar_set_milliseconds(__cal, sec2ms(__utime));
	i18n_ucalendar_add(__cal, I18N_UCALENDAR_YEAR, years);
	i18n_ucalendar_get_milliseconds(__cal, &utime);
	__utime = ms2sec(utime);

	if (setLimit == true)
		__setLimit();
}

void CalDateTime::__setLimit()
{
	/* CAL_DATETIME_YEAR_LOWER_BOUND <= date  < CAL_DATETIME_YEAR_UPPER_BOUND */
	if (getYear() < CAL_DATETIME_YEAR_LOWER_BOUND)
	{
		if (__fixedDate == true)
		{
			set(CAL_DATETIME_YEAR_LOWER_BOUND, CAL_DATETIME_MONTH_LOWER_BOUND, CAL_DATETIME_DAY_LOWER_BOUND);
		}
		else
		{
			struct tm time = {};
			__createTm(CAL_DATETIME_YEAR_LOWER_BOUND, CAL_DATETIME_MONTH_LOWER_BOUND, CAL_DATETIME_DAY_LOWER_BOUND, 12, 0, 0, &time);
			set(time);
		}
	}
	else if (getYear() > CAL_DATETIME_YEAR_UPPER_BOUND)
	{
		if (__fixedDate == true)
		{
			set(CAL_DATETIME_YEAR_UPPER_BOUND, CAL_DATETIME_MONTH_UPPER_BOUND, CAL_DATETIME_DAY_UPPER_BOUND);
		}
		else
		{
			struct tm time;
			__createTm(CAL_DATETIME_YEAR_UPPER_BOUND, CAL_DATETIME_MONTH_UPPER_BOUND, CAL_DATETIME_DAY_UPPER_BOUND, 12, 0, 0, &time);
			set(time);
		}
	}
}

int CalDateTime::getDayOfWeek()
{
	i18n_ucalendar_set_milliseconds(__cal, sec2ms(__utime));
	int32_t result = 0;
	i18n_ucalendar_get(__cal, I18N_UCALENDAR_DAY_OF_WEEK_IN_MONTH, &result);
	return (int)result;
}

void CalDateTime::getStringParam(char buffer[]) const
{
	sprintf(buffer, "%.04d-%.02d-%.02d %.02d:%.02d:%.02d ",
			getYear(), getMonth(), getMday(),
			getHour(), getMinute(), getSecond());
}

int CalDateTime::getDayDiff(CalDateTime date1, CalDateTime date2)
{
	struct tm time1, time2;
	date1.getTmTime(&time1);
	date2.getTmTime(&time2);

	time1.tm_hour = time2.tm_hour = 0;
	time1.tm_min = time2.tm_min = 0;
	time1.tm_sec = time2.tm_sec = 0;

	return difftime(mktime(&time1), mktime(&time2)) / (60 * 60 * 24);
}

int CalDateTime::getDateCompareVal() const
{
	return (getYear() << 9) | (getMonth() << 5) | getMday();
}

void CalDateTime::__createTm(const int year, const int month, const int day, const int hour, const int min, const int sec, tm *timeptr)
{
	WENTER();
	memset(timeptr, 0, sizeof(struct tm));
	if (timeptr)
	{
		timeptr->tm_year = year - CAL_STRUCT_TM_YEAR_LOWER_SHIFT;
		timeptr->tm_mon = month - CAL_STRUCT_TM_MONTH_ARRAY_LOWER_SHIFT;
		timeptr->tm_mday = day;
		timeptr->tm_hour = hour;
		timeptr->tm_min = min;
		timeptr->tm_sec = sec;
	}
	WLEAVE();
}
