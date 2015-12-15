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

#ifndef _CAL_DATE_TIME_H_
#define _CAL_DATE_TIME_H_

#include <time.h>
#include <utils_i18n.h>

#include "WDefine.h"

class WAPP_ASSIST_EXPORT CalDateTime
{
public:
	typedef enum
	{
		INIT_TODAY = 0,
		INIT_LOWER_BOUND,
		INIT_UPPER_BOUND
	} InitialValue;

	CalDateTime();
	CalDateTime(InitialValue initialValue);
	CalDateTime(const long long int uTime, bool fixedDate = false);
	CalDateTime(const struct tm& dateTm, bool fixedDate = false);
	CalDateTime(int year, int month, int mday, int hour = 0, int min = 0, int sec = 0, bool fixedDate = false);
	CalDateTime(const char* stringParam);
	virtual ~CalDateTime();

	CalDateTime( const CalDateTime& );
	const CalDateTime& operator=( const CalDateTime& ) ;
	bool operator ==(const CalDateTime &) const;
	bool operator !=(const CalDateTime &) const;
	bool operator >(const CalDateTime &) const;
	bool operator >=(const CalDateTime &) const;
	bool operator <(const CalDateTime &) const;
	bool operator <=(const CalDateTime &) const;
	bool isSameDay (const CalDateTime &) const;

public:

	const char* getWeekdayText(const long long int utime);
	std::string dump(bool showTime = false) const;

	void createUCalendar();

	// set
	void setUCalendar(const struct tm &tm) const;
	void set(const int year, const int month, const int mday, const int hour = 0, const int minute = 0, const int second = 0);
	void set(const long long int utime);
	void set(const struct tm& dateTm);
	void setAllDay(const bool isAllDay);
	void setToMonthGridStart(int firstWeekday);
	void setToMonthGridStart(int firstWeekday, int year, int month);

	// get
	void getUCalendar(struct tm &tm) const;
	int getYear() const;
	int getMonth() const;
	int getMday() const;
	int getHour() const;
	int getMinute() const;
	int getSecond() const;

	void getTmTime(struct tm *dateTm) const;
	long long int getUtime() const;

	int getDayOfWeek();
	void getString(std::string& text) const;
	void getTimeString(std::string& text) const;
	void getDateString(std::string& text) const;
	bool isAllDay() const;
	bool isSameMonth(const CalDateTime &date);
	const char* getWeekdayText() const;
	int getWeekday() const;  // sun: 0 ~ sat: 6

	//
	void addSeconds(const long long int seconds, const bool setLimit = true);
	void addMinutes(const int minutes, const bool setLimit = true);
	void addHours(const int hours, const bool setLimit = true);
	void addDays(const int days, const bool setLimit = true);
	void addMonths(const int months, const bool setLimit = true);
	void addYears(const int years, const bool setLimit = true);

	void incrementDay();
	void decrementDay();
	void incrementMonth();
	void decrementMonth();

	void getStringParam(char buffer[]) const;
	int getDateCompareVal() const;

	const char* getUnixTimeString() const;
	const char* getString();
	const char* getMonthString() const;

	static int getDayDiff(CalDateTime __date1, CalDateTime __date2);
	static int compareMonth(const CalDateTime& date1, const CalDateTime& date2);
	static int compareMonth(int year1, int month1, int year2, int month2);
	static const char* getWeekdayText(int weekday);

private:
	void __createTm(const int year, const int month, const int day, const int hour, const int min, const int sec, struct tm *timeptr);
	void __getString(int df, int tf, std::string& text) const;
	void __setLimit();

	static void __getWeekStartDate(int firstWeekday, struct tm& date);
	static void __normalizeStructTm(struct tm& date);

	i18n_ucalendar_h __cal;
	long long int __utime;
	bool __fixedDate;
};

#endif /* _CAL_DATE_TIME_H_ */
