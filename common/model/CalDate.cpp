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

#include "CalDate.h"

#include "CalCommon.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "CalLocaleManager.h"

#define DATE_BUFFER 128

CalDate::CalDate(InitialValue initialValue)
{
	switch (initialValue)
	{
		case INIT_TODAY:
		{
			struct tm now_tm = {0};
			time_t now = 0;
			time(&now);
			CalDateTime dgsg((long long int)now);
			dgsg.getTmTime(&now_tm);
			break;
		}
		case INIT_LOWER_BOUND:
		{
			set(1902, 1, 1);
			break;
		}
		case INIT_UPPER_BOUND:
		{
			set(2037, 12, 31);
			break;
		}
		default:
		{
			WASSERT(0);
			break;
		}
	}
}

CalDate::CalDate(int year, int month, int mday)
{
	set(year, month, mday);
}

CalDate::CalDate( const CalDate& obj)
{
	__date = obj.__date;
}

CalDate::CalDate(const char* stringParam)
{
	time_t timeFromParam = (time_t)atoll(stringParam);
	memset(&__date, 0, sizeof(struct tm));
	tm *t = localtime(&timeFromParam);
	if (t != NULL)
	{
		__date = *t;
	}
}

CalDate::~CalDate()
{
}

const CalDate& CalDate::operator=( const CalDate& obj)
{
	if (this != &obj)
	{
		__date = obj.__date;
	}
	return *this;
}

const char* CalDate::getString() const
{
	static std::string dtString;
	// TODO CALDATETIME: update this after CalDateTime class is ready to use
	CalDateTime derg(getYear(), getMonth(), getMday());
	CalLocaleManager::getInstance().getDateText(CalLocaleManager::DATEFORMAT_1, derg, dtString);

	return dtString.c_str();
}

const char* CalDate::getUnixTimeString() const
{
	const int unixTimeBuffer = 24;
	static char buffer[unixTimeBuffer];
	memset(buffer, 0x0, sizeof(buffer));
	snprintf(buffer, unixTimeBuffer, "%ld", mktime((tm*)&__date));

	return buffer;
}

const char* CalDate::getMonthString() const
{
	static std::string dtString;
	// TODO CALDATETIME: update this after CalDateTime class is ready to use
	CalDateTime derg(getYear(), getMonth(), getMday());
	CalLocaleManager::getInstance().getDateText(CalLocaleManager::DATEFORMAT_6, derg, dtString);

	return dtString.c_str();
}

void CalDate::set(const struct tm* currentTm)
{
	__date = *currentTm;
	__date.tm_hour = 12;
	__date.tm_min = 0;
	__date.tm_sec = 0;
}

void CalDate::set(int year, int month, int mday)
{
	memset(&__date, 0, sizeof(struct tm));
	__date.tm_year = year - 1900;
	__date.tm_mon = month - 1;
	__date.tm_mday = mday;
	__normalizeStructTm(__date);
}

void CalDate::setToMonthGridStart(int firstWeekday)
{
	__date.tm_mday = 1;
	__normalizeStructTm(__date);
	__getWeekStartDate(firstWeekday, __date);
}

void CalDate::setToMonthGridStart(int firstWeekday, int year, int month)
{
	set(year, month, 1);
	setToMonthGridStart(firstWeekday);
}

std::string CalDate::dump(bool showTime) const
{
	WENTER();

	return "1";
}

void CalDate::incrementDay()
{
	__date.tm_mday++;
	__normalizeStructTm(__date);
}

void CalDate::decrementDay()
{
	__date.tm_mday--;
	__normalizeStructTm(__date);
}

void CalDate::incrementMonth()
{
	if (__date.tm_mon == 11)
	{
		__date.tm_mon = 0;
		__date.tm_year++;
	}
	else
	{
		__date.tm_mon++;
	}
	__normalizeMday();
}

void CalDate::__normalizeStructTm(struct tm& date)
{
	mktime(&date);
	date.tm_hour = 12;
	date.tm_min = 0;
	date.tm_sec = 0;
}

void CalDate::__normalizeMday()
{
	const int mon = __date.tm_mon;
	__normalizeStructTm(__date);
	if (__date.tm_mon != mon)
	{
		__date.tm_mday = 1;
		__normalizeStructTm(__date);
		decrementDay();
	}
}

void CalDate::decrementMonth()
{
	if (__date.tm_mon == 0)
	{
		__date.tm_mon = 11;
		__date.tm_year--;
	}
	else
	{
		__date.tm_mon--;
	}
	__normalizeMday();
}

void CalDate::incrementYear()
{
	__date.tm_year++;
}

void CalDate::decrementYear()
{
	__date.tm_year--;
}

void CalDate::addDays(int days)
{
	__date.tm_mday += days;
	__normalizeStructTm(__date);
}

void CalDate::__getWeekStartDate(int firstWeekday, struct tm& date)
{
	int diff = date.tm_wday - firstWeekday;
	if (diff < 0)
		diff += 7;
	date.tm_mday -= diff;
	__normalizeStructTm(date);
}

void CalDate::setMday(int mday)
{
	const int originalMon = __date.tm_mon;
	__date.tm_mday = mday;
	__normalizeStructTm(__date);
	if (__date.tm_mon != originalMon)
	{
		__date.tm_mday = 0;
		__normalizeStructTm(__date);
	}
	// TODO CALDATETIME: update this after CalDateTime class is ready to use
//	WASSERT(__date.tm_mon == originalMon);
}

bool CalDate::isYearLowerLimit()
{
	return __date.tm_year + 1900 == 1902;
}
bool CalDate::isYearUpperLimit()
{
	return __date.tm_year + 1900 == 2037;
}

bool CalDate::isMonthLowerLimit()
{
	return isYearLowerLimit() && __date.tm_mon == 0;
}
bool CalDate::isMonthUpperLimit()
{
	return isYearLowerLimit() && __date.tm_mon == 11;
}
bool CalDate::isDayLowerLimit()
{
	return isMonthLowerLimit() && __date.tm_mday == 1;
}
bool CalDate::isDayUpperLimit()
{
	return isMonthUpperLimit() && __date.tm_mday == 31;
}

bool CalDate::isSameMonth(const CalDate& date)
{
	return getYear() == date.getYear() && getMonth() == date.getMonth();
}

const char* CalDate::getWeekdayText(int weekday)
{
	return CalLocaleManager::getInstance().getWeekdayText(weekday);
}

const char* CalDate::getWeekdayShortText(int weekday)
{
	return CalLocaleManager::getInstance().getWeekdayShortText(weekday);
}

int CalDate::getDayDiff(const CalDate& date1, const CalDate& date2)
{
	return CalDateTime::getDayDiff(date1.__date, date2.__date);
}

int CalDate::compareMonth(const CalDate& date1, const CalDate& date2)
{
	return compareMonth(
		date1.__date.tm_year, date1.__date.tm_mon,
		date2.__date.tm_year, date2.__date.tm_mon);
}

int CalDate::compareMonth(int year1, int month1, int year2, int month2)
{
	const int monval1 = (year1 << 4) | month1;
	const int monval2 = (year2 << 4) | month2;
	return monval1 - monval2;
}

int CalDate::getDateCompareVal() const
{
	return (__date.tm_year << 9) | (__date.tm_mon << 5) | __date.tm_mday;
}

static int __getDateCompareVal(const struct tm& date)
{
	return (date.tm_year << 9) | (date.tm_mon << 5) | date.tm_mday;
}

bool operator<(const CalDate& date1, const CalDate& date2)
{
	return __getDateCompareVal(date1.__date) < __getDateCompareVal(date2.__date);
}

bool operator>(const CalDate& date1, const CalDate& date2)
{
	return __getDateCompareVal(date1.__date) > __getDateCompareVal(date2.__date);
}

bool operator<=(const CalDate& date1, const CalDate& date2)
{
	return __getDateCompareVal(date1.__date) <= __getDateCompareVal(date2.__date);
}

bool operator>=(const CalDate& date1, const CalDate& date2)
{
	return __getDateCompareVal(date1.__date) >= __getDateCompareVal(date2.__date);
}

bool operator==(const CalDate& date1, const CalDate& date2)
{
	return __getDateCompareVal(date1.__date) == __getDateCompareVal(date2.__date);
}

bool operator!=(const CalDate& date1, const CalDate& date2)
{
	return __getDateCompareVal(date1.__date) != __getDateCompareVal(date2.__date);
}
