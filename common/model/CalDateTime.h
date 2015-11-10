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

class WAPP_ASSIST_EXPORT CalDateTime
{
public:
	CalDateTime();
	CalDateTime(const struct tm& dateTm);
	CalDateTime(int year, int month, int mday);
	CalDateTime(int year, int month, int mday, int hour, int min, int sec);
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
	// set
	void set(const int year, const int month, const int mday);
	void set(const long long int utime);
	void set(const struct tm& dateTm, const char* timezone = NULL);
	void setAllDay(const bool isAllDay);

	// get
	int getYear(const char* timezone = NULL) const;
	int getMonth(const char* timezone = NULL) const;
	int getMday(const char* timezone = NULL) const;
	int getHour(const char* timezone = NULL) const;
	int getMinute(const char* timezone = NULL) const;
	int getSecond(const char* timezone = NULL) const;
	void getTm(struct tm* dateTm) const;
	void getTm(const char* timezone, struct tm* dateTm) const;
	long long int getUtime(const char* timezone = NULL) const;
	void getString(std::string& text) const;
	void getTimeString(std::string& text) const;
	void getDateString(std::string& text) const;
	void getString(const char* timezone, std::string& text) const;
	void getTimeString(const char* timezone, std::string& text) const;
	void getDateString(const char* timezone, std::string& text) const;
	bool isAllDay() const;
	const char* getWeekdayText(const char* timezone = NULL) const;
	int getWeekday(const char* timezone = NULL) const;  // sun: 0 ~ sat: 6

	//
	void addSeconds(const long long int seconds, const bool setLimit = true);
	void addHours(const int hours, const bool setLimit = true);
	void addDays(const int days, const bool setLimit = true);
	void addMonths(const int months, const bool setLimit = true);
	void addYears(const int years, const bool setLimit = true);

public:
	void getStringParam(char buffer[]) const;
	int getDateCompareVal() const;

private:
	void __getString(const char* timezone, int df, int tf, std::string& text) const;
	void __setLimit();

private:
	long long int __utime;
	int __year;
	int __month;
	int __mday;
	bool __allday;
};

#endif /* _CAL_DATE_TIME_H_ */
