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

#ifndef _CAL_LOCALE_MANAGER_H_
#define _CAL_LOCALE_MANAGER_H_

#include <string>
#include <map>
#include <Elementary.h>

#include <utils_i18n.h>

#include "WDefine.h"
#include "CalSingleton.h"
#include "CalDateTime.h"
#include "CalDate.h"

class WAPP_ASSIST_EXPORT CalLocaleManager
{
SINGLETON_IDIOM(CalLocaleManager);

public:

	/**
	 * @brief The date format enum.
	 *
	 */
	enum DateFormat
	{
		DATEFORMAT_NONE = -1,
		DATEFORMAT_1 = 0,	//"EEMMMddyyyy" /* Thu Aug 23 2001 */
		DATEFORMAT_2,		//"EE" /* Thu */
		DATEFORMAT_3,		//"ddEE" /* 27 Thu */
		DATEFORMAT_4,		//"MMMM" /* August */
		DATEFORMAT_5,		//"yyyy" /* 2011 */
		DATEFORMAT_6,		//"MMMMyyyy" /* August 2011 */
		DATEFORMAT_7,		//"EEEEMMMdd" /* Thursday Aug 23 */
		DATEFORMAT_8,		//"dMMyyyy" /* 23 08 2011 */
		DATEFORMAT_9,		//"d" /* 27 */
		DATEFORMAT_10,		//"MMMd" /* Aug 27 */
		DATEFORMAT_11,		//"MMMMdyyyy" /* August 23 2011 */
		DATEFORMAT_12,		//"MMMdyyyy" /* Aug 23 2011 */
		DATEFORMAT_13,		//"EEMMMMdyyyy" /* Thu August 23 2001 */
		DATEFORMAT_14,		//"EEEE" /* Thursday */
		DATEFORMAT_15,		//"EEdMMMMyyyy" /* Thu August 23 2001 */
		DATEFORMAT_16,		//"ww" /* week of year */
		DATEFORMAT_17,		//"EEEEE" /* day of week "J" "T" "W" */
		DATEFORMAT_18,		//"MMMyyyy" /* Aug 2011 */
		DATEFORMAT_19,		//"MMM" /*Aug*/
		DATEFORMAT_20,		//"EEEEddMMyyyy" /*Monday 08 23 2013*/
		DATEFORMAT_21,		//"MMMMd" /* August 27 */
		DATEFORMAT_22,		//"EEMMMd" /* Thur Aug 23 */
		DATEFORMAT_23,		//"EEEEMMMMddyyyy" /* Thursday August 27 2013*/
		DATEFORMAT_24,		//"EEMMMdd"
		DATEFORMAT_25,		//"EEEEEE" /* day of week "Tu" "Su" */
		DATEFORMAT_END,
	};

	/**
	 * @brief The time format enum.
	 *
	 */
	enum TimeFormat
	{
		TIMEFORMAT_NONE = -1,
		TIMEFORMAT_1 = 0,	//"hhmma" /* 07:58 AM */
		TIMEFORMAT_2,		//"hhmm" /* 07:58 */
		TIMEFORMAT_3,		//"hha" /* 07 AM */
		TIMEFORMAT_4,		//"a" /* AM */
		TIMEFORMAT_5,		//"hma" /* 7:58 AM */
		TIMEFORMAT_6,		//"HHmm" /* 19:58 */
		TIMEFORMAT_7,		//"HH" /* 19 */
		TIMEFORMAT_8,		//"Hm" /* 7:58 AM */
		TIMEFORMAT_END,
	};

	/**
	 * @brief Check whether RTL is tured ON.
	 *
	 * @return true is RTL enabled, otherwise false.
	 *
	 */
	bool isRTL();

	/**
	 * @brief Update for EFL.
	 *
	 * @see elm_language_set().
	 *
	 */
	void updateLocaleForEvasObj();

	/**
	 * @brief Set EvasObj for RTL.
	 *
	 * @param obj    EvasObj
	 *
	 */
	void setEvasObjForRTL(Evas_Object* obj);

	/**
	 * @brief Set time zone.
	 *
	 * @param[in] timeZone     timeZone
	 *
	 */
	void setTimeZone(const std::string &timeZone);

	/**
	 * @brief Get time zone.
	 *
	 * @param[out] timeZone      time zone.
	 *
	 */
	void getTimeZone(std::string &timeZone);

	/**
	 * @brief Update region settings.
	 *
	 */
	void updateRegion(void);

	/**
	 * @brief Get date and time as text.
	 *
	 * @param[in]  dateFormat         data format, @see DateFormat
	 * @param[in]  timeFormat         time format, @see TimeFormat
	 * @param[in]  dateTime           datatime
	 * @param[out] text               text buffer
	 *
	 */
	void getDateTimeText(const DateFormat dateFormat, const TimeFormat timeFormat, const CalDateTime &dateTime, std::string &text);

	/**
	 * @brief Get date and time for time zone as text.
	 *
	 * @param[in]  timezone         time zone
	 * @param[in]  dateFormat       data format, @see DateFormat
	 * @param[in]  timeFormat       time format, @see TimeFormat
	 * @param[in]  dateTime         datatime
	 * @param[out] text             text buffer
	 *
	 */
	void getDateTimeText(const char* timezone, const DateFormat dateFormat, const TimeFormat timeFormat, const CalDateTime &dateTime, std::string &text);

	/**
	 * @brief Get date as text.
	 *
	 * @param[in]  dateFormat       data format, @see DateFormat
	 * @param[in]  dateTime         datatime
	 * @param[out] text             text buffer
	 *
	 */
	void getDateText(const DateFormat dateFormat, const CalDate &date, std::string &text);

	/**
	 * @brief Get UTIME from TM structure.
	 *
	 * @param[in] tm       TM structure
	 *
	 * @return UTIME value.
	 *
	 */
	long long int getUtime(struct tm &tm);

	/**
	 * @brief Get UTIME from TM structure for time zone.
	 *
	 * @param[in] timezone        time zone
	 * @param[in] tm              TM structure
	 *
	 * @return UTIME value.
	 *
	 */
	long long int getUtimeFromTm(const char *timezone, const struct tm &tm);

	/**
	 * @brief Get TM structure from UTIME.
	 *
	 * @param[in]  timezone      time zone
	 * @param[in]  utime         UTIME
	 * @param[out] tm            TM structure
	 *
	 */
	void getTmFromUtime(const char *timezone, const long long int utime, struct tm &tm);

	/**
	 * @brief Get week day number.
	 *
	 * @param[in]  timezone      time zone
	 * @param[in]  utime         UTIME
	 *
	 * @return week day number.
	 *
	 */
	int getDayOfWeekInMonth(const char *timezone, const long long int utime);

	/**
	 * @brief Get week day name.
	 *
	 * @param[in]  timezone      time zone
	 * @param[in]  utime         UTIME
	 *
	 * @return week day name.
	 *
	 */
	const char* getWeekdayText(const char *timezone, const long long int utime);

	/**
	 * @brief Get week day name by number.
	 *
	 * @param[in]  weekday      week day numbe
	 *
	 * @return week day name.
	 *
	 */
	const char* getWeekdayText(int weekday);

	/**
	 * @brief Get week day short name by number.
	 *
	 * @param[in]  weekday      week day numbe
	 *
	 * @return week day short name.
	 *
	 */
	const char* getWeekdayShortText(int weekday);

	/**
	 * @brief Get day of week number.
	 *
	 * @param[in]  timezone      time zone
	 * @param[in]  utime         UTIME
	 *
	 * @return day of week number, 0(sun) - 6(sat).
	 *
	 */
	int getWeekday(const char *timezone, const long long int utime);

	/**
	 * @brief Add hours to time.
	 *
	 * @param[in]      delta     hours delta
	 * @param[in,out]  utime     UTIME
	 *
	 */
	void updateTmHour(const int delta, struct tm &tm);

	/**
	 * @brief Add days to time.
	 *
	 * @param[in]      delta     hours delta
	 * @param[in,out]  utime     UTIME
	 *
	 */
	void updateTmDay(const int delta, struct tm &tm);

	/**
	 * @brief Add months to time.
	 *
	 * @param[in]      delta     hours delta
	 * @param[in,out]  utime     UTIME
	 *
	 */
	void updateTmMonth(const int delta, struct tm &tm);

	/**
	 * @brief Add years to time.
	 *
	 * @param[in]      delta     hours delta
	 * @param[in,out]  utime     UTIME
	 *
	 */
	void updateTmYear(const int delta, struct tm &tm);


	/**
	 * @brief Get days difference dabetween two times.
	 *
	 * @param[in] date1    first time to compare
	 * @param[in] date2    second time to compare
	 *
	 * @return time difference, @see difftime();
	 *
	 */
	int getDayDiff(const struct tm &date1, const struct tm &date2);

	/**
	 * @brief Get display text of time zone.
	 *
	 * @param[in]   timezone      time zone
	 * @param[out]  displayText   text buffer
	 */
	void getDisplayTextTimeZone(const std::string &timeZone, std::string &displayText);

	/**
	 * @brief Get number of the first day of week for current locale.
	 *
	 * @return day number, 0(sun) - 6(sat).
	 *
	 */
	int getLocaleFirstDayOfWeek();

	std::string __tzid;
	i18n_ucalendar_h __cal;

	std::string __locale;
	i18n_udatepg_h __patternGenerator;
	std::map<std::string,i18n_udate_format_h> __mapFormat;

protected:
	CalLocaleManager();
	virtual ~CalLocaleManager();

private:
	WDISABLE_COPY_AND_ASSIGN(CalLocaleManager);

	i18n_ucalendar_h __getUcal(const std::string &tzid); // __cal_locale_manager_get_ucal
	void __getLocale(std::string &localeStr);

	// __mapFormat
	const i18n_udate_format_h __getUDateFormat(const DateFormat df, const TimeFormat tf);
	i18n_udate_format_h __getUDateFormat(const char* timezone, const DateFormat df, const TimeFormat tf);

	// for UCalendar
	void __setUCalendar(i18n_ucalendar_h calendar, const struct tm *tm);
	void __getUCalendar(i18n_ucalendar_h calendar, struct tm *tm);
	char* __getTzOffset(i18n_ucalendar_h cal);
	char* __getTzName(i18n_ucalendar_h cal, const char *language, const std::string &timeZone);

	// for weekday
	void __initWeekday();

	int __getTimezoneOffsetFormCalDb(const std::string &standardName);

private:
	std::string __weekdayText[7];
	std::string __weekdayShortText[7];
	Evas_Object* __obj;
};

#endif
