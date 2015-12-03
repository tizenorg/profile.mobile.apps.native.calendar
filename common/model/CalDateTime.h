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
	typedef enum {
		INIT_TODAY = 0,
		INIT_LOWER_BOUND,
		INIT_UPPER_BOUND
	} InitialValue;

	CalDateTime(InitialValue initialValue = INIT_TODAY);
	CalDateTime(const struct tm& dateTm);
	CalDateTime(int year, int month, int mday);
	CalDateTime(int year, int month, int mday, int hour, int min, int sec);
	virtual ~CalDateTime();

	CalDateTime(const CalDateTime&);
	const CalDateTime& operator =(const CalDateTime&);

	//* Comparators
	bool operator ==(const CalDateTime &) const;
	bool operator !=(const CalDateTime &) const;
	bool operator >(const CalDateTime &) const;
	bool operator >=(const CalDateTime &) const;
	bool operator <(const CalDateTime &) const;
	bool operator <=(const CalDateTime &) const;
	bool isSameDay(const CalDateTime &) const;
	bool isSameMonth(const CalDateTime &) const;

	//* Setters
	void set(const int year, const int month, const int mday);
	void set(const struct tm& dateTm);
	void setAllDay(const bool isAllDay);
	void setMday(int mday);

	//* Getters
	int getYear() const { return __date.tm_year + 1900; }
	int getMonth() const { return __date.tm_mon + 1; }
	int getMday() const { return __date.tm_mday; }
	int getHour() const { return __date.tm_hour; }
	int getMinute() const { return __date.tm_min; }
	int getSecond() const { return __date.tm_sec; }

	void getString(std::string& text) const;
	void getTimeString(std::string& text) const;
	void getDateString(std::string& text) const;
	bool isAllDay() const;
	const char* getWeekdayText() const;
	int getWeekday() const;  // sun: 0 ~ sat: 6
	int getDateCompareVal() const;
	std::string dump(bool showTime = false) const;

	//* Modifiers
	void addSeconds(const long long int seconds, const bool setLimit = true);
	void addHours(const int hours, const bool setLimit = true);
	void addDays(const int days, const bool setLimit = true);
	void addMonths(const int months, const bool setLimit = true);
	void addYears(const int years, const bool setLimit = true);

	void incrementDay();
	void decrementDay();
	void incrementMonth();
	void decrementMonth();
	void incrementYear();
	void decrementYear();

private:
	void __getString(int df, int tf, std::string& text) const;
	void __setLimit();
	void __normalizeDateTm();
	void __normalizeMday();

	struct tm __date;
	bool __allday;
};

#endif /* _CAL_DATE_TIME_H_ */
