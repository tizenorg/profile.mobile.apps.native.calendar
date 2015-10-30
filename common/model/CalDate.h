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

#ifndef _CAL_DATE_H_
#define _CAL_DATE_H_

#include <time.h>

#include <string>

#include "WDefine.h"

class WAPP_ASSIST_EXPORT CalDate
{
public:
	typedef enum {
		INIT_TODAY = 0,
		INIT_LOWER_BOUND,
		INIT_UPPER_BOUND
	} InitialValue;
public:
	CalDate(InitialValue initialValue = INIT_TODAY);
	CalDate(int year, int month, int mday);
	virtual ~CalDate();
	CalDate(const CalDate&);
	CalDate(const char* stringParam);
	const CalDate& operator=(const CalDate&);
public:
	int getYear() const {return __date.tm_year + 1900;}
	int getMonth() const {return __date.tm_mon + 1;}
	int getMday() const {return __date.tm_mday;}
	const char* getString() const;
	const char* getUnixTimeString() const;
	const char* getMonthString() const;
	void getDateString(std::string& text) const;
	void set(int year, int month, int mday);
	void set(const struct tm* currentTm);
	void setToMonthGridStart(int firstWeekday);
	void setToMonthGridStart(int firstWeekday, int year, int month);
public:
	std::string dump(bool showTime = false) const;
	int getDateCompareVal() const;
public:
	void incrementDay();
	void decrementDay();
	void incrementMonth();
	void decrementMonth();
	void incrementYear();
	void decrementYear();
	void addDays(int days);
	void setMday(int mday);
	bool isYearLowerLimit();
	bool isYearUpperLimit();
	bool isMonthLowerLimit();
	bool isMonthUpperLimit();
	bool isDayLowerLimit();
	bool isDayUpperLimit();
	bool isSameMonth(const CalDate& date);
public:
	static const char* getWeekdayText(int weekday);
	static const char* getWeekdayShortText(int weekday);
	static int getDayDiff(const CalDate& date1, const CalDate& date2);
	static int compareMonth(const CalDate& date1, const CalDate& date2);
	static int compareMonth(int year1, int month1, int year2, int month2);
private:
	void __normalizeMday();
private:
	static void __getWeekStartDate(int firstWeekday, struct tm& date);
	static void __normalizeStructTm(struct tm& date);
private:
	struct tm __date;
friend bool operator<(const CalDate& date1, const CalDate& date2);
friend bool operator>(const CalDate& date1, const CalDate& date2);
friend bool operator<=(const CalDate& date1, const CalDate& date2);
friend bool operator>=(const CalDate& date1, const CalDate& date2);
friend bool operator==(const CalDate& date1, const CalDate& date2);
friend bool operator!=(const CalDate& date1, const CalDate& date2);
};

WAPP_ASSIST_EXPORT bool operator<(const CalDate& date1, const CalDate& date2);
WAPP_ASSIST_EXPORT bool operator>(const CalDate& date1, const CalDate& date2);
WAPP_ASSIST_EXPORT bool operator<=(const CalDate& date1, const CalDate& date2);
WAPP_ASSIST_EXPORT bool operator>=(const CalDate& date1, const CalDate& date2);
WAPP_ASSIST_EXPORT bool operator==(const CalDate& date1, const CalDate& date2);
WAPP_ASSIST_EXPORT bool operator!=(const CalDate& date1, const CalDate& date2);

#endif
