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


CalDateTime::CalDateTime()
{
	time_t current_time = 0;
	time(&current_time);
	__utime = current_time;
	__fixedDate = false;
}

CalDateTime::CalDateTime(const struct tm& dateTm)
{
	set(dateTm);
}

CalDateTime::CalDateTime(int year, int month, int mday, bool fixedDate)
{
	struct tm time = {};
	__createTm(year, month, mday, 0, 0, 0, &time);
	if (fixedDate)
	{
		__utime = mktime(&time);
	}
	else
	{
		// TODO CALDATETIME: update this after CalDateTime class is ready to use
		//__utime = CalLocaleManager::getInstance().getUtime(time);
		__utime = 0;
	}
	__fixedDate = fixedDate;
}

CalDateTime::CalDateTime(int year, int month, int mday, int hour, int min, int sec)
{
	struct tm time = {};
	__createTm(year, month, mday, hour, min, sec, &time);
	// TODO CALDATETIME: update this after CalDateTime class is ready to use
	//__utime = CalLocaleManager::getInstance().getUtime(time);
	__utime = 0;
	__fixedDate = false;
}

CalDateTime::~CalDateTime()
{
}

CalDateTime::CalDateTime(const CalDateTime& obj)
{
	__utime = obj.__utime;
	__fixedDate = obj.__fixedDate;
}

const CalDateTime& CalDateTime::operator=(const CalDateTime& obj)
{
	if (this != &obj)
	{
		__utime = obj.__utime;
		__fixedDate = obj.__fixedDate;
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
	long long int srcTime = getUtimeFromTm();
	long long int objTime = obj.getUtimeFromTm();
	return (srcTime > objTime);
}

bool CalDateTime::operator >=(const CalDateTime &obj) const
{
	long long int srcTime = getUtimeFromTm();
	long long int objTime = obj.getUtimeFromTm();
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
	return ((getYear() == obj.getYear()) && (getMonth() == obj.getMonth()) && (getMday() == obj.getMday()));
}

void CalDateTime::set(const int year, const int month, const int mday, bool fixedDate)
{
	struct tm time = {};
	__createTm(year, month, mday, 0, 0, 0, &time);
	if (fixedDate)
	{
		__utime = mktime(&time);
	}
	else
	{
		// TODO CALDATETIME: update this after CalDateTime class is ready to use
		//CalLocaleManager::getInstance().getTmFromUtime(NULL, __utime, time);
		__utime = 0;
	}
	__fixedDate = fixedDate;
}

void CalDateTime::set(const long long int utime)
{
	__utime = utime;
	__fixedDate = false;
}

void CalDateTime::set(const struct tm& dateTm)
{
	__fixedDate = false;
	struct tm tmp = dateTm;
	// TODO CALDATETIME: update this after CalDateTime class is ready to use
	//__utime = CalLocaleManager::getInstance().getUtime(tmp);
	__utime = 0;
}

int CalDateTime::getYear() const
{
	struct tm time = {};
	getTmFromUtime(&time);
	return time.tm_year + CAL_STRUCT_TM_YEAR_LOWER_SHIFT;
}

int CalDateTime::getMonth() const
{
	struct tm time = {};
	getTmFromUtime(&time);
	return time.tm_mon + CAL_STRUCT_TM_MONTH_ARRAY_LOWER_SHIFT;
}

int CalDateTime::getMday() const
{
	struct tm time = {};
	getTmFromUtime(&time);
	return time.tm_mday;
}

int CalDateTime::getHour() const
{
	struct tm time = {};
	getTmFromUtime(&time);
	WDEBUG("%lld %d:%d:%d", __utime, time.tm_hour, time.tm_min, time.tm_sec);
	return time.tm_hour;
}

int CalDateTime::getMinute() const
{
	if (__fixedDate == true)
	{
		return 0;
	}
	else
	{
		struct tm time = {};
		// TODO CALDATETIME: update this after CalDateTime class is ready to use
		//CalLocaleManager::getInstance().getTmFromUtime(NULL, __utime, time);
		WDEBUG("%lld %d:%d:%d", __utime, time.tm_hour, time.tm_min, time.tm_sec);
		return time.tm_min;
	}
}

int CalDateTime::getSecond() const
{
	if (__fixedDate == true)
	{
		return 0;
	}
	else
	{
		struct tm time = {};
		// TODO CALDATETIME: update this after CalDateTime class is ready to use
		//CalLocaleManager::getInstance().getTmFromUtime(NULL, __utime, time);
		WDEBUG("%lld %d:%d:%d", __utime, time.tm_hour, time.tm_min, time.tm_sec);
		return time.tm_sec;
	}
}

void CalDateTime::getTmFromUtime(struct tm *dateTm) const
{
	if (dateTm == NULL)
	{
		WERROR("invalid input");
		return;
	}
	if (__fixedDate == true)
	{
		time_t utime = (long int) __utime;
		struct tm *time = gmtime(&utime);
		memcpy(dateTm, time, sizeof(struct tm));
	}
	else
	{
		// TODO CALDATETIME: update this after CalDateTime class is ready to use
		//CalLocaleManager::getInstance().getTmFromUtime(NULL, __utime, *dateTm);
	}
}

long long int CalDateTime::getUtimeFromTm() const
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
	// TODO CALDATETIME: update this after CalDateTime class is ready to use
	//CalLocaleManager::getInstance().getTmFromUtime(NULL, (long long int)current_time, today);
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
	long long int utime = getUtimeFromTm();
	return CalLocaleManager::getInstance().getWeekdayText(utime);
}

int CalDateTime::getWeekday() const
{
	long long int utime = getUtimeFromTm();
	// TODO CALDATETIME: update this after CalDateTime class is ready to use
	//return CalLocaleManager::getInstance().getWeekday(NULL, utime);
	return 0;
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
		//TODO investigate requirement
		// TODO CALDATETIME: update this after CalDateTime class is ready to use
		//CalLocaleManager::getInstance().getTmFromUtime(NULL, __utime, time);
		__utime = mktime(&time);
	}
	else
	{
		//TODO investigate requirement, prefer to delete
		struct tm *date = NULL;
		time_t utime = (long int) __utime;
		date = gmtime(&utime);
		// TODO CALDATETIME: update this after CalDateTime class is ready to use
		//CalLocaleManager::getInstance().getTmFromUtime(NULL, __utime, time);
		__utime = 0;
		time.tm_year = date->tm_year - CAL_STRUCT_TM_YEAR_LOWER_SHIFT;
		time.tm_mon = date->tm_mon - CAL_STRUCT_TM_MONTH_ARRAY_LOWER_SHIFT;
		time.tm_mday = date->tm_mday;
		// TODO CALDATETIME: update this after CalDateTime class is ready to use
		//__utime = CalLocaleManager::getInstance().getUtime(time);
		__utime = 0;
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

void CalDateTime::addHours(const int hours, const bool setLimit)
{
	addSeconds(hours * CAL_TIME_SECONDS_IN_HOUR);
}

void CalDateTime::addDays(const int days, const bool setLimit)
{
	__utime += days * CAL_TIME_HOURS_IN_DAY * CAL_TIME_SECONDS_IN_HOUR;

	if (setLimit == true)
		__setLimit();
}

void CalDateTime::addMonths(const int months, const bool setLimit)
{
	struct tm time = {};
	// TODO CALDATETIME: update this after CalDateTime class is ready to use
	//CalLocaleManager::getInstance().getTmFromUtime(NULL, __utime, time);
	//CalLocaleManager::getInstance().updateTmMonth(months, time);
	//__utime = CalLocaleManager::getInstance().getUtime(time);
	__utime = 0;

	if (setLimit == true)
		__setLimit();
}

void CalDateTime::addYears(const int years, const bool setLimit)
{
	struct tm time = {};
	// TODO CALDATETIME: update this after CalDateTime class is ready to use
	//CalLocaleManager::getInstance().getTmFromUtime(NULL, __utime, time);
	//CalLocaleManager::getInstance().updateTmYear(years, time);
	//__utime = CalLocaleManager::getInstance().getUtime(time);
	__utime = 0;

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
			__createTm(CAL_DATETIME_YEAR_LOWER_BOUND, CAL_DATETIME_MONTH_LOWER_BOUND, CAL_DATETIME_DAY_LOWER_BOUND,
					12, 0, 0,
					&time);
			// TODO CALDATETIME: update this after CalDateTime class is ready to use
			//__utime = CalLocaleManager::getInstance().getUtime(time);
			__utime = 0;
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
			__createTm(CAL_DATETIME_YEAR_UPPER_BOUND, CAL_DATETIME_MONTH_UPPER_BOUND, CAL_DATETIME_DAY_UPPER_BOUND,
					12, 0, 0,
					&time);
			// TODO CALDATETIME: update this after CalDateTime class is ready to use
			//__utime = CalLocaleManager::getInstance().getUtime(time);
			__utime = 0;
		}
	}
}

void CalDateTime::getStringParam(char buffer[]) const
{
	sprintf(buffer, "%.04d-%.02d-%.02d %.02d:%.02d:%.02d ",
			getYear(), getMonth(), getMday(),
			getHour(), getMinute(), getSecond());
}

int CalDateTime::getDateCompareVal() const
{
	return (getYear() << 9) | (getMonth() << 5) | getMday();
}

void CalDateTime::__createTm(const int year, const int month, const int day, const int hour, const int min, const int sec, tm *timeptr)
{
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
}
