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

CalDateTime::CalDateTime()
{
	time_t current_time = 0;
	time(&current_time);
	__utime = current_time;
	__allday = false;
	__mday = 0;
	__month = 0;
	__year = 0;
}

CalDateTime::CalDateTime(const struct tm& dateTm)
	: __year(0)
	, __month(0)
	, __mday(0)
{
	set(dateTm);
}

CalDateTime::CalDateTime(int year, int month, int mday) :
		__utime(0),__year(year),__month(month),__mday(mday),__allday(true)
{
}

CalDateTime::CalDateTime(int year, int month, int mday, int hour, int min, int sec)
{
	struct tm time;
	memset(&time, 0, sizeof(struct tm));
	time.tm_year = year - 1900;
	time.tm_mon = month - 1;
	time.tm_mday = mday;
	time.tm_hour = hour;
	time.tm_min = min;
	time.tm_sec = sec;
	__utime = CalLocaleManager::getInstance().getUtime(time);
	__allday = false;
	__mday = mday;
	__month = month;
	__year = year;
}

CalDateTime::~CalDateTime()
{
}

CalDateTime::CalDateTime(const CalDateTime& obj)
{
	__utime = obj.__utime;
	__year = obj.__year;
	__month = obj.__month;
	__mday = obj.__mday;
	__allday = obj.__allday;
}

const CalDateTime& CalDateTime::operator=(const CalDateTime& obj)
{
	if (this != &obj)
	{
		__date = obj.__date;
		__allday = obj.__allday;
	}
	return *this;
}

bool CalDateTime::operator==(const CalDateTime &obj) const
{
	return (__date.tm_year == obj.__date.tm_year && __date.tm_mon == obj.__date.tm_mon
			&& __date.tm_mday == obj.__date.tm_mday && __date.tm_hour == obj.__date.tm_hour
			&&__date.tm_min == obj.__date.tm_min && __date.tm_sec == obj.__date.tm_sec);
}

bool CalDateTime::operator !=(const CalDateTime &obj) const
{
	return !(*this == obj);
}

bool CalDateTime::operator >(const CalDateTime &obj) const
{
	return (mktime(&__date) > mktime(&obj.__date));
}

bool CalDateTime::operator >=(const CalDateTime &obj) const
{
	return (mktime(&__date) >= mktime(&obj.__date));
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

void CalDateTime::set(const int year, const int month, const int mday)
{
	__year = year;
	__month = month;
	__mday = mday;
	__allday = true;
}

void CalDateTime::set(const struct tm& dateTm)
{
	__allday = false;
	struct tm tmp = dateTm;
	__utime = CalLocaleManager::getInstance().getUtime(tmp);
}

void CalDateTime::getString(std::string& text) const
{
	CalLocaleManager::TimeFormat tf = CalLocaleManager::TIMEFORMAT_1;
	CalLocaleManager::DateFormat df = CalLocaleManager::DATEFORMAT_1;

	int todayyear = 0;

	struct tm today;
	time_t current_time = 0;
	time(&current_time);
	CalLocaleManager::getInstance().getTmFromUtime(NULL, (long long int)current_time, today);
	todayyear = today.tm_year + 1900;

	bool isSameYear = false;
	if (todayyear == getYear())
		isSameYear = true;

	if (CalSettingsManager::getInstance().isHour24() == false)
		tf = CalLocaleManager::TIMEFORMAT_1;
	else
		tf = CalLocaleManager::TIMEFORMAT_6;
	if (isSameYear == true && __allday == false)
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
	if (__allday == true)
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
	if (__allday)
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
	return __allday;
}

const char* CalDateTime::getWeekdayText() const
{
	long long int utime = getUtimeFromTm();
	return CalLocaleManager::getInstance().getWeekdayText(utime);
}

int CalDateTime::getWeekday() const
{
	long long int utime = getUtimeFromTm();

	return CalLocaleManager::getInstance().getWeekday(NULL, utime);
}

void CalDateTime::setAllDay(const bool isAllDay)
{
	if (isAllDay == true)
	{
		if (__allday == true)
			return ;
		__allday = true;
		struct tm time;
		CalLocaleManager::getInstance().getTmFromUtime(NULL, __utime, time);
		__year = time.tm_year +1900;
		__month = time.tm_mon +1;
		__mday = time.tm_mday;
	}
	else
	{
		if (__allday == false)
			return;

		__allday = false;
		struct tm time;
		CalLocaleManager::getInstance().getTmFromUtime(NULL, __utime, time);
		time.tm_year = __year - 1900;
		time.tm_mon = __month - 1;
		time.tm_mday = __mday;
		__utime = CalLocaleManager::getInstance().getUtime(time);
	}
	return;
}

void CalDateTime::addSeconds(const long long int seconds, const bool setLimit)
{
	if (__allday == true)
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
	if (__allday == true)
	{
		WERROR("invalid");
		return;
	}

	__utime += hours * 60 * 60;

	if (setLimit == true)
		__setLimit();
}

void CalDateTime::addDays(const int days, const bool setLimit)
{
	if (__allday == true)
	{
		long long int utime = getUtimeFromTm();
		struct tm time;
		CalLocaleManager::getInstance().getTmFromUtime(NULL, utime, time);
		CalLocaleManager::getInstance().updateTmDay(days, time);
		__year = time.tm_year +1900;
		__month = time.tm_mon +1;
		__mday = time.tm_mday;

		if (setLimit == true)
			__setLimit();
		return;
	}

	__utime += days * 24 * 60 * 60;

	if (setLimit == true)
		__setLimit();
}

void CalDateTime::addMonths(const int months, const bool setLimit)
{
	if (__allday == true)
	{
		long long int utime = getUtimeFromTm();
		struct tm time;
		CalLocaleManager::getInstance().getTmFromUtime(NULL, utime, time);
		CalLocaleManager::getInstance().updateTmMonth(months, time);
		__year = time.tm_year +1900;
		__month = time.tm_mon +1;
		__mday = time.tm_mday;

		if (setLimit == true)
			__setLimit();
		return;
	}
	struct tm time;
	CalLocaleManager::getInstance().getTmFromUtime(NULL, __utime, time);
	CalLocaleManager::getInstance().updateTmMonth(months, time);
	__utime = CalLocaleManager::getInstance().getUtime(time);

	if (setLimit == true)
		__setLimit();
}

void CalDateTime::addYears(const int years, const bool setLimit)
{
	if (__allday == true)
	{
		long long int utime = getUtimeFromTm();
		struct tm time;
		CalLocaleManager::getInstance().getTmFromUtime(NULL, utime, time);
		CalLocaleManager::getInstance().updateTmYear(years, time);
		__year = time.tm_year +1900;
		__month = time.tm_mon +1;
		__mday = time.tm_mday;

		if (setLimit == true)
			__setLimit();
		return;
	}
	struct tm time;
	CalLocaleManager::getInstance().getTmFromUtime(NULL, __utime, time);
	CalLocaleManager::getInstance().updateTmYear(years, time);
	__utime = CalLocaleManager::getInstance().getUtime(time);

	if (setLimit == true)
		__setLimit();
}

void CalDateTime::__setLimit()
{
	// 1902 ~ 2036
	if (getYear() < 1902)
	{
		if (__allday == true)
		{
			set(1902,1,1);
		}
		else
		{
			struct tm time;
			memset(&time, 0, sizeof(struct tm));
			time.tm_year = 1902 - 1900;
			time.tm_mon = 1 - 1;
			time.tm_mday = 1;
			time.tm_hour = 12;
			time.tm_min = 0;
			time.tm_sec = 0;
			__utime = CalLocaleManager::getInstance().getUtime(time);
		}
	}
	else if (getYear() > 2037)
	{
		if (__allday == true)
		{
			set(2037,12,31);
		}
		else
		{
			struct tm time;
			memset(&time, 0, sizeof(struct tm));
			time.tm_year = 2037 - 1900;
			time.tm_mon = 12 - 1;
			time.tm_mday = 31;
			time.tm_hour = 12;
			time.tm_min = 0;
			time.tm_sec = 0;
			__utime = CalLocaleManager::getInstance().getUtime(time);
		}
	}
}

int CalDateTime::getDateCompareVal() const
{
	return (__date.tm_year << 9) | (__date.tm_mon << 5) | __date.tm_mday;
}

std::string CalDate::dump(bool showTime) const
{
	char buffer[DATE_BUFFER];
	if (showTime)
	{
		snprintf(buffer, DATE_BUFFER, "{%.04d/%.02d/%.02d (%s) %.02d:%.02d:%.02d}",
			getYear(), getMonth(), getMday(), getWeekdayText(__date.tm_wday),
			__date.tm_hour, __date.tm_min, __date.tm_sec);
	}
	else
	{
		snprintf(buffer, DATE_BUFFER, "{%.04d/%.02d/%.02d (%s)}",
			getYear(), getMonth(), getMday(), getWeekdayText(__date.tm_wday));
	}

	return std::string(buffer);
}
