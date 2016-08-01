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

/**
 * @brief The CalDate class
 */
class WAPP_ASSIST_EXPORT CalDate
{
public:

	/**
	 * @brief Initial value define the earliest past/now/the farthest future.
	 */
	typedef enum {
		INIT_TODAY = 0,
		INIT_LOWER_BOUND,
		INIT_UPPER_BOUND
	} InitialValue;


	CalDate(InitialValue initialValue = INIT_TODAY);
	CalDate(int year, int month, int mday);
	CalDate(const CalDate&);
	CalDate(const char* stringParam);
	const CalDate& operator=(const CalDate&);

	/**
	 * @brief Get date year.
	 *
	 * @return year value.
	 *
	 */
	int getYear() const {return __date.tm_year + 1900;}

	/**
	 * @brief Get date month.
	 *
	 * @return month value.
	 *
	 */
	int getMonth() const {return __date.tm_mon + 1;}

	/**
	 * @brief Get date month day.
	 *
	 * @return month day value.
	 *
	 */
	int getMday() const {return __date.tm_mday;}

	/**
	 * @brief Get date as text.
	 *
	 * @return date text.
	 *
	 */
	const char* getString() const;

	/**
	 * @brief Get date as text in *nix format.
	 *
	 * @return date text.
	 *
	 */
	const char* getUnixTimeString() const;

	/**
	 * @brief Get date month as text.
	 *
	 * @return date month text.
	 *
	 */
	const char* getMonthString() const;

	/**
	 * @brief Get date as text.
	 *
	 * @param[out] text    text buffer
	 *
	 */
	void getDateString(std::string& text) const;

	/**
	 * @brief Set date.
	 *
	 * @param[in] year    date year
	 * @param[in] month   date month
	 * @param[in] mday    date month day
	 *
	 */
	void set(int year, int month, int mday);

	/**
	 * @brief Set date.
	 *
	 * @param[in] current time
	 *
	 */
	void set(const struct tm* currentTm);

	/**
	 * @brief Set date to month grid start.
	 *
	 * @param first weekday
	 *
	 */
	void setToMonthGridStart(int firstWeekday);

	/**
	 * @brief Set date to month grid start by month and year.
	 *
	 * @param firstWeekday    first weekday
	 * @param year            year
	 * @param month           month
	 *
	 */
	void setToMonthGridStart(int firstWeekday, int year, int month);

	/**
	 * @brief Dump date to string.
	 *
	 * @param[in] showTime    whether show the time
	 *
	 * @return string with date and time.
	 *
	 */
	std::string dump(bool showTime = false) const;

	/**
	 * @brief Get date compare value.
	 *
	 * @return compare value.
	 *
	 */
	int getDateCompareVal() const;

	/**
	 * @brief Increment day
	 *
	 */
	void incrementDay();

	/**
	 * @brief Decrement day
	 *
	 */
	void decrementDay();

	/**
	 * @brief Increment month
	 *
	 */
	void incrementMonth();

	/**
	 * @brief Decrement month
	 *
	 */
	void decrementMonth();

	/**
	 * @brief Increment year
	 *
	 */
	void incrementYear();

	/**
	 * @brief Decrement year
	 *
	 */
	void decrementYear();

	/**
	 * @brief Add days to day
	 *
	 * @param[in] days    days count to add
	 *
	 */
	void addDays(int days);

	/**
	 * @brief Set month day to date.
	 *
	 * @param mday	month day
	 *
	 */
	void setMday(int mday);

	/**
	 * @brief Check whether the date is the earliest past (lower bound).
	 *
	 * @return true if is lower, otherwise false.
	 *
	 */
	bool isYearLowerLimit();

	/**
	 * @brief Check whether the date is the farthest future (upper bound).
	 *
	 * @return true if is upper, otherwise false.
	 *
	 */
	bool isYearUpperLimit();

	/**
	 * @brief Check whether the month is a first.
	 *
	 * @return true if first, otherwise false.
	 */
	bool isMonthLowerLimit();

	/**
	 * @brief Check whether the month is a last.
	 *
	 * @return true if last, otherwise false.
	 *
	 */
	bool isMonthUpperLimit();

	/**
	 * @brief Check whether the day is a first.
	 *
	 * @return true if first, otherwise false.
	 */
	bool isDayLowerLimit();

	/**
	 * @brief Check whether the day is a last.
	 *
	 * @return true if last, otherwise false.
	 */
	bool isDayUpperLimit();

	/**
	 * @brief Compare are two date have the same month.
	 *
	 * @param date    second date to compare
	 *
	 * @return true if the same, otherwise false.
	 *
	 */
	bool isSameMonth(const CalDate& date);

	/**
	 * @brief Get weekday as text.
	 *
	 * @param weekday    weekday number
	 *
	 * @return the name of weekday.
	 *
	 */
	static const char* getWeekdayText(int weekday);

	/**
	 * @brief Get weekday short label.
	 *
	 * @param weekday    weekday number
	 *
	 * @return short name of weekday.
	 */
	static const char* getWeekdayShortText(int weekday);

	/**
	 * @brief Get time difference between two dates.
	 *
	 * @param[in] date1    first date to compare
	 * @param[in] date2    second date to compare
	 *
	 * @return time difference, @see difftime();
	 *
	 */
	static int getDayDiff(const CalDate& date1, const CalDate& date2);

	/**
	 * @brief Compare year and month of two dates.
	 *
	 * @param[in] date1    first date to compare
	 * @param[in] date2    second date to compare
	 *
	 * @return true if year and month of both objects are equal, otherwise false.
	 *
	 */
	static int compareMonth(const CalDate& date1, const CalDate& date2);

	/**
	 * @brief Compare year and month of two dates.
	 *
	 * @param[in] year1    year of a first date
	 * @param[in] month1   month of a first date
	 * @param[in] year2    year of a second date
	 * @param[in] month2   month of a second date
	 *
	 * @return true if year and month of both objrets are equal, otherwise false.
	 *
	 */
	static int compareMonth(int year1, int month1, int year2, int month2);

private:
	void __normalizeMday();

	static void __getWeekStartDate(int firstWeekday, struct tm& date);
	static void __normalizeStructTm(struct tm& date);

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
