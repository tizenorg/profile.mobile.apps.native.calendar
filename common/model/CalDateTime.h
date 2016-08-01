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
#include "WDefine.h"

#define TIMEZONE_ETC_GMT "Etc/GMT"

/**
 * @brief The CalDateTime class conveniently represents data+time data.
 *
 */
class WAPP_ASSIST_EXPORT CalDateTime
{
public:
	CalDateTime();
	CalDateTime(const struct tm &dateTm);
	CalDateTime(int year, int month, int mday);
	CalDateTime(int year, int month, int mday, int hour, int min, int sec);
	CalDateTime(const CalDateTime &base, const char *timeZone);
	CalDateTime(const CalDateTime &obj);
	const CalDateTime& operator=(const CalDateTime& obj);

	//Operator owerlaoads
	bool operator ==(const CalDateTime &obj) const;
	bool operator !=(const CalDateTime &obj) const;
	bool operator >(const CalDateTime &obj) const;
	bool operator >=(const CalDateTime &obj) const;
	bool operator <(const CalDateTime &obj) const;
	bool operator <=(const CalDateTime &obj) const;

	/**
	 * @brief Compare two datatimes for equivalence.
	 *
	 * @param[in] obj    second datetime to compare
	 *
	 * @return true if date and time of both objercts are equivalent, otherwise false.
	 *
	 */
	bool isSameDay(const CalDateTime &obj) const;

	/**
	 * @brief Set date and time by year, month and month day.
	 *
	 * @param[in] year    year
	 * @param[in] month   month
	 * @param[in] mday    month day
	 *
	 */
	void set(const int year, const int month, const int mday);

	/**
	 * @brief Set date and time by UTIME.
	 *
	 * @param[in] utime    UTIME value
	 *
	 */
	void set(const long long int utime);

	/**
	 * @brief Set date and time by UTIME.
	 *
	 * @param[in] dateTm    TM structure
	 *
	 */
	void set(const struct tm& dateTm);

	/**
	 * @brief Set AllDay flag
	 *
	 * @param[in] isAllDay    AllDay flag
	 *
	 */
	void setAllDay(const bool isAllDay);

	/**
	 * @brief Get year.
	 *
	 * @param[in] timeZone    time zone
	 *
	 * @return year value.
	 */
	int getYear(const char *timeZone = nullptr) const;

	/**
	 * @brief Get month.
	 *
	 * @param[in] timeZone    time zone
	 *
	 * @return month value.
	 */
	int getMonth(const char *timeZone = nullptr) const;

	/**
	 * @brief Get month day.
	 *
	 * @param[in] timeZone    time zone
	 *
	 * @return manth day value.
	 */
	int getMday(const char *timeZone = nullptr) const;

	/**
	 * @brief Get hours.
	 *
	 * @param[in] timeZone    time zone
	 *
	 * @return hours value.
	 */
	int getHour(const char *timeZone = nullptr) const;

	/**
	 * @brief Get minutes.
	 *
	 * @param[in] timeZone    time zone
	 *
	 * @return minutes value.
	 */
	int getMinute(const char *timeZone = nullptr) const;

	/**
	 * @brief Get seconds.
	 *
	 * @param[in] timeZone    time zone
	 *
	 * @return seconds value.
	 */
	int getSecond(const char *timeZone = nullptr) const;

	/**
	 * @brief Get TM structure by UTIME value in object.
	 *
	 * @param[out] dateTm    TM structure
	 */
	void getTmFromUtime(struct tm* dateTm) const;

	/**
	 * @brief Get UTIME value from TM structure in object.
	 *
	 * @return UTIME value.
	 *
	 */
	long long int getUtimeFromTm() const;

	/**
	 * @brief Get date and time as text.
	 *
	 * @param[out] text    text buffer
	 *
	 */
	void getString(std::string& text) const;

	/**
	 * @brief Get time as text.
	 *
	 * @param[out] text    text buffer
	 *
	 */
	void getTimeString(std::string& text) const;

	/**
	 * @brief Get date as text.
	 *
	 * @param[out] text    text buffer
	 *
	 */
	void getDateString(std::string& text) const;

	/**
	 * @brief Check whether isAllDay.
	 *
	 * @return true if is allDay, false otherwise.
	 *
	 */
	bool isAllDay() const;

	/**
	 * @brief Get weekday as text.
	 *
	 * @return the name of weekday.
	 *
	 */
	const char* getWeekdayText() const;

	/**
	 * @brief Get weekday.
	 *
	 * @return the number of weekday.
	 *         (sun: 0 ~ sat: 6)
	 *
	 */
	int getWeekday() const;

	/**
	 * @brief Add seconds to date/time.
	 *
	 * @param[in] seconds    seconds to add
	 * @param[in] setLimit   if true, it will be limited to upper bound
	 *
	 */
	void addSeconds(const long long int seconds, const bool setLimit = true);

	/**
	 * @brief Add hours to date/time.
	 *
	 * @param[in] hours      hours to add
	 * @param[in] setLimit   if true, final date will be limited to upper bound
	 *
	 */
	void addHours(const int hours, const bool setLimit = true);

	/**
	 * @brief Add days to date.
	 *
	 * @param[in] days       days to add
	 * @param[in] setLimit   if true, final date will be limited to upper bound
	 *
	 */
	void addDays(const int days, const bool setLimit = true);

	/**
	 * @brief Add month to date.
	 *
	 * @param[in] months     months to add
	 * @param[in] setLimit   if true, final date will be limited to upper bound
	 *
	 */
	void addMonths(const int months, const bool setLimit = true);

	/**
	 * @brief Add years to date.
	 *
	 * @param[in] years      years to add
	 * @param[in] setLimit   if true, final date will be limited to upper bound
	 *
	 */
	void addYears(const int years, const bool setLimit = true);

	/**
	 * @brief Get date/time compare value.
	 *
	 * @return compare value.
	 *
	 */
	int getDateCompareVal() const;

private:
	void __getString(int df, int tf, std::string& text) const;
	void __setLimit();

	long long int __utime;
	int __year;
	int __month;
	int __mday;
	bool __allday;
};

#endif /* _CAL_DATE_TIME_H_ */
