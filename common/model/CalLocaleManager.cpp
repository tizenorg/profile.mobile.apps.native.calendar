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

#include <calendar.h>
#include <sqlite3.h>
#include <system_settings.h>
#include <unordered_set>
#include "CalLocaleManager.h"
#include "CalCommon.h"
#include "CalEventManager.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <utils_i18n.h>
#ifdef __cplusplus
}
#endif

#define DATETIME_BUFFER 64

// sec <-> millisec
#define ms2sec(ms) ((long long int)ms / 1000)
#define sec2ms(s) ((i18n_udate)s * 1000.0)

#define CAL_LOCALE_MANAGER_WORLDCLOCK_DB "/usr/dbspace/.worldclock.db"

static const char* __dateformat[CalLocaleManager::DateFormat::DATEFORMAT_END] = {
  "EEMMMddyyyy", /* Thu Aug 23 2001 */
  "EE", /* Thu */
  "ddEE", /* 27 Thu */
  "MMMM", /* August */
  "yyyy", /* 2011 */
  "MMMMyyyy", /* August 2011 */
  "EEEEMMMdd", /* Thursday Aug 23 */
  "dMMyyyy", /* 23 08 2011 */
  "d", /* 27 */
  "MMMd", /* Aug 27 */
  "MMMMdyyyy", /* August 23 2011 */
  "MMMdyyyy", /* Aug 23 2011 */
  "EEMMMMdyyyy", /* Thu August 23 2001 */
  "EEEE", /* Thursday */
  "EEdMMMMyyyy", /* Thu August 23 2001 */
  "ww", /* week of year */
  "EEEEE", /* day of week "J" "T" "W" */
  "MMMyyyy", /* Aug 2011 */
  "MMM", /*Aug*/
  "EEEEddMMyyyy", /*Monday 08 23 2013*/
  "MMMMd", /* August 27 */
  "EEMMMd", /* Thur Aug 23 */
  "EEEEMMMMddyyyy", /* Thursday August 27 2013*/
  "EEMMMdd",
  "EEEEEE",
};

static const char* __timeformat[CalLocaleManager::TimeFormat::TIMEFORMAT_END] = {
  "hhmma", /* 07:58 AM */
  "hhmm", /* 07:58 */
  "hha", /* 07 AM */
  "a", /* AM */
  "hma", /* 7:58 AM */
  "HHmm", /* 19:58 */
  "HH", /* 19 */
  "Hm", /* 7:58 AM */
};

SINGLETON_INSTANCE(CalLocaleManager);

CalLocaleManager::CalLocaleManager()
{
	WENTER();

	__cal = NULL;
	__obj = NULL;
	__patternGenerator = NULL;
}

CalLocaleManager::~CalLocaleManager()
{
	WENTER();
	if (__cal)
		i18n_ucalendar_destroy(__cal);

	if (__patternGenerator)
		i18n_udatepg_destroy(__patternGenerator);

	for (auto it=__mapFormat.begin(); it!=__mapFormat.end(); ++it)
	{
		i18n_udate_format_h dateFormat= it->second;
		i18n_udate_destroy(dateFormat);
	}
	__mapFormat.clear();
}

void CalLocaleManager::init()
{
	WENTER();
	__cal = NULL;
	__patternGenerator = NULL;
	__obj = NULL;

	updateLocaleForEvasObj();
}

void CalLocaleManager::updateLocaleForEvasObj()
{
	WENTER();
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	WDEBUG("Language %s", locale);

	if (locale) {
		elm_language_set(locale);
		free(locale);
	}
	WLEAVE();
}

void CalLocaleManager::setEvasObjForRTL(Evas_Object* obj)
{
	WENTER();
	__obj = obj;
}

bool CalLocaleManager::isRTL()
{
	WENTER();
	Evas_BiDi_Direction bidi = EVAS_BIDI_DIRECTION_NATURAL;
	if(__obj)
	{
		const char *type = evas_object_type_get(__obj);
		if(type && strstr(type, "text"))
			bidi = evas_object_text_direction_get(__obj);
		else
		{
			char *locale = NULL;
			int value = system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
			if(value == SYSTEM_SETTINGS_ERROR_NONE && locale)
			{
				/*
				"ar" - Arabic
				"dv" - Divehi
				"fa" - Persian (Farsi)
				"ha" - Hausa
				"he" - Hebrew
				"iw" - Hebrew (old code)
				"ji" - Yiddish (old code)
				"ps" - Pashto, Pushto
				"ur" - Urdu
				"yi" - Yiddish
				*/

				std::string strLocale = locale;
				std::size_t found = strLocale.find("_");
				if(found != std::string::npos)
					strLocale = strLocale.substr(0, found);

				static std::unordered_set<std::string> locales = {"ar", "dv", "fa", "ha", "he", "iw", "ji", "ps", "ur", "yi"};
				std::unordered_set<std::string>::const_iterator got = locales.find(strLocale);
				if(got != locales.end())
					bidi = EVAS_BIDI_DIRECTION_RTL;

				free(locale);
			}
		}
	}
	WLEAVE();
	return (bidi == EVAS_BIDI_DIRECTION_RTL);
}

void CalLocaleManager::setTimeZone(const std::string &timeZone)
{
	WENTER();

	if (timeZone.empty())
	{
		WERROR("input timezone is empty");
		return;
	}

	if (timeZone.compare(__tzid)== 0)
	{
		return;
	}

	__tzid = timeZone;
	WDEBUG("timezone[%s]",__tzid.c_str());
	if (__cal)
		i18n_ucalendar_destroy (__cal);

	__cal = __getUcal(__tzid);

	// for PG
	updateRegion();
}

void CalLocaleManager::getTimeZone(std::string &timeZone)
{
	WENTER();
	WDEBUG("In CalLocaleManager::getTimeZone(): __tzid == %s", __tzid.c_str());
	timeZone = __tzid;
	WLEAVE();
}

void CalLocaleManager::updateRegion(void)
{
	WENTER();

	i18n_ulocale_set_default(getenv("LC_TIME"));
	std::string locale;
	__getLocale(locale);
	WDEBUG("%s/%s",__locale.c_str(), locale.c_str());

	__locale = locale;
	if (__patternGenerator)
		i18n_udatepg_destroy(__patternGenerator);

	i18n_udatepg_create(__locale.c_str(), &__patternGenerator);

	for (auto it=__mapFormat.begin(); it!=__mapFormat.end(); ++it)
	{
		i18n_udate_format_h dateFormat= it->second;
		i18n_udate_destroy(dateFormat);
	}
	__mapFormat.clear();

	__initWeekday();
}

void CalLocaleManager::getDateTimeText(const DateFormat df, const TimeFormat tf, const CalDateTime &dt, std::string &text)
{
	WENTER();

	const i18n_udate_format_h formatter = __getUDateFormat(df, tf);
	WASSERT(formatter);
	i18n_uchar formatted[DATETIME_BUFFER] = {0,};
	char formattedString[DATETIME_BUFFER * 2] = {0,};
	int32_t formattedCapacity;
	int32_t formattedLength;
	i18n_udate date;

	// get udate from CalDateTime
	long long int utime = dt.getUtimeFromTm();
	date = sec2ms(utime);

	formattedCapacity = (int32_t)(sizeof(formatted)/sizeof((formatted)[0]));
	i18n_udate_format_date(formatter, date, formatted, formattedCapacity, NULL, &formattedLength);

	i18n_ustring_copy_au_n(formattedString, formatted, 128);
	text = formattedString;
	WLEAVE();
}

void CalLocaleManager::getDateTimeText(const char* timezone, const DateFormat df, const TimeFormat tf, const CalDateTime &dt, std::string &text)
{
	WENTER();

	if (timezone == NULL || (timezone != NULL && __tzid.compare(timezone) == 0) )
	{
		getDateTimeText(df, tf, dt, text);
	}

	i18n_udate_format_h formatter = __getUDateFormat(timezone, df, tf);
	WASSERT(formatter);
	i18n_uchar formatted[DATETIME_BUFFER] = {0,};
	char formattedString[DATETIME_BUFFER * 2] = {0,};
	int32_t formattedCapacity;
	int32_t formattedLength;
	i18n_udate date;

	// get udate from CalDateTime
	long long int utime = dt.getUtimeFromTm();
	date = sec2ms(utime);

	formattedCapacity = (int32_t)(sizeof(formatted)/sizeof((formatted)[0]));
	i18n_udate_format_date(formatter, date, formatted, formattedCapacity, NULL, &formattedLength);

	i18n_ustring_copy_au_n(formattedString, formatted, DATETIME_BUFFER * 2);
	text = formattedString;

	i18n_udate_destroy(formatter);
	WLEAVE();
}

void CalLocaleManager::getDateText(const DateFormat dateFormat, const CalDate &date, std::string &text)
{
	WENTER();

	if (dateFormat == DATEFORMAT_NONE || dateFormat == DATEFORMAT_END)
	{
		WERROR("invalid input");
		return;
	}

	const i18n_udate_format_h formatter = __getUDateFormat(dateFormat, TIMEFORMAT_NONE);

	i18n_uchar formatted[DATETIME_BUFFER] = {0,};
	char formattedString[DATETIME_BUFFER * 2] = {0,};
	int32_t formattedLength;
	i18n_udate udate;

	// get udate from CalDate
	{
		struct tm time;
		memset(&time, 0, sizeof(struct tm));
		time.tm_year = date.getYear() - 1900;
		time.tm_mon = date.getMonth() - 1;
		time.tm_mday = date.getMday();
		time.tm_hour = 0;
		time.tm_min = 0;
		time.tm_sec = 0;
		long long int utime = getUtimeFromTm(NULL, time);
		udate = sec2ms(utime);
	}
	WENTER();

	i18n_udate_format_date(formatter, udate, formatted, DATETIME_BUFFER, NULL, &formattedLength);

	i18n_ustring_copy_au_n(formattedString, formatted, DATETIME_BUFFER * 2);
	text = formattedString;

	WLEAVE();
}

i18n_ucalendar_h CalLocaleManager::__getUcal(const std::string &tzid)
{
	WENTER();
	int status = I18N_ERROR_NONE;
	i18n_uchar utf16_timezone[DATETIME_BUFFER] = {0};

	i18n_ustring_copy_ua_n(utf16_timezone, tzid.c_str(), DATETIME_BUFFER);

	i18n_ucalendar_h cal;
	const char* locale;
	i18n_ulocale_get_default(&locale);
	status = i18n_ucalendar_create(utf16_timezone, -1, locale, I18N_UCALENDAR_GREGORIAN , &cal);
	if (status)
	{
		WERROR("i18n_ucalendar_create got an error : %d", status);
		return NULL;
	}

	return cal;
}

void CalLocaleManager::__getLocale(std::string &localeStr)
{
	WENTER();
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_COUNTRY, &locale);
	if(locale == NULL) {
		WERROR("system_settings_get_value_string() failed : region format");
		localeStr = "en_US";
		return ;
	}

	char *str = NULL;
	int count = 0;
	gchar **ptr = NULL;
	gchar **split_str = NULL;

	split_str = g_strsplit(locale,".", 0);

	for(ptr = split_str; *ptr; ptr++)
	{
		count++;
	}

	if(count == 2)
	{
		if(!strcmp(split_str[1], "UTF-8"))
		{
			str = g_strdup_printf("%s", split_str[0]);
			free(locale);
			WDEBUG("dest_locale:%s", str);
			g_strfreev(split_str);
			localeStr = str;
			free(str);
			return ;
		}
		else
		{
			g_strfreev(split_str);
			localeStr = locale;
			free(locale);
			return ;
		}
	}
	else
	{
		g_strfreev(split_str);
		localeStr = locale;
		free(locale);
		return ;
	}
	WLEAVE();
}

const i18n_udate_format_h CalLocaleManager::__getUDateFormat(const DateFormat df, const TimeFormat tf)
{
	WENTER();
	if (__patternGenerator == NULL)
	{
		WASSERT(__patternGenerator);
	}
	char buf[1024];
	const char* dateFormat = NULL;
	const char* timeFormat = NULL;

	if (df > DATEFORMAT_NONE && df < DATEFORMAT_END)
		dateFormat = __dateformat[df];
	else
		dateFormat = "";

	if (tf > TIMEFORMAT_NONE && tf < TIMEFORMAT_END)
		timeFormat = __timeformat[tf];
	else
		timeFormat = "";

	snprintf(buf,sizeof(buf),"%s%s",dateFormat,timeFormat);

	std::string formatString = buf;
	auto it = __mapFormat.find(formatString);
	if (it == __mapFormat.end())
	{
		i18n_uchar custom_format[DATETIME_BUFFER]={0};
		int32_t bestPatternLength, bestPatternCapacity;
		i18n_udate_format_h formatter;
		i18n_uchar bestPattern[DATETIME_BUFFER] = {0};
		i18n_uchar utf16_timezone[DATETIME_BUFFER] = {0};
		i18n_ustring_copy_ua_n(custom_format, formatString.c_str(), formatString.length());

		bestPatternCapacity = (int32_t)(sizeof(bestPattern)/sizeof((bestPattern)[0]));
		i18n_udatepg_get_best_pattern(__patternGenerator, custom_format, i18n_ustring_get_length(custom_format), bestPattern,   bestPatternCapacity, &bestPatternLength);

		i18n_ustring_copy_ua_n(utf16_timezone, __tzid.c_str(), DATETIME_BUFFER);

		i18n_udate_create(I18N_UDATE_PATTERN , I18N_UDATE_PATTERN , __locale.c_str(), utf16_timezone, -1, bestPattern, -1, &formatter);

		__mapFormat.insert(std::pair<std::string,i18n_udate_format_h>(formatString, formatter));

		return formatter;
	}
	else
	{
		return it->second;
	}
}

i18n_udate_format_h CalLocaleManager::__getUDateFormat(const char* timezone, const DateFormat df, const TimeFormat tf)
{
	WENTER();
	if (__patternGenerator == NULL)
	{
		WASSERT(__patternGenerator);
	}
	char buf[1024];
	const char* dateFormat = NULL;
	const char* timeFormat = NULL;

	if (df > DATEFORMAT_NONE && df < DATEFORMAT_END)
		dateFormat = __dateformat[df];
	else
		dateFormat = "";

	if (tf > TIMEFORMAT_NONE && tf < TIMEFORMAT_END)
		timeFormat = __timeformat[tf];
	else
		timeFormat = "";

	snprintf(buf,sizeof(buf),"%s%s",dateFormat,timeFormat);

	std::string formatString = buf;
	i18n_uchar custom_format[DATETIME_BUFFER]={0};
	int32_t bestPatternLength, bestPatternCapacity;
	i18n_udate_format_h formatter;
	i18n_uchar bestPattern[DATETIME_BUFFER] = {0};
	i18n_uchar utf16_timezone[DATETIME_BUFFER] = {0};

	i18n_ustring_copy_ua_n(custom_format, formatString.c_str(), formatString.length());

	bestPatternCapacity = (int32_t)(sizeof(bestPattern)/sizeof((bestPattern)[0]));
	i18n_udatepg_get_best_pattern(__patternGenerator, custom_format, i18n_ustring_get_length(custom_format), bestPattern,   bestPatternCapacity, &bestPatternLength);

	i18n_ustring_copy_ua_n(utf16_timezone, timezone, DATETIME_BUFFER);
	i18n_udate_create(I18N_UDATE_PATTERN , I18N_UDATE_PATTERN , __locale.c_str(), utf16_timezone, -1, bestPattern, -1, &formatter);

	return formatter;
}

long long int CalLocaleManager::getUtime(struct tm &tm)
{
	WENTER();
	__setUCalendar(__cal, &tm);
	i18n_udate date;
	i18n_ucalendar_get_milliseconds(__cal, &date);
	return ms2sec(date);
}

long long int CalLocaleManager::getUtimeFromTm(const char *timezone, const struct tm &tm)
{
	WENTER();

	if (timezone == NULL || !strcmp(timezone, __tzid.c_str()))
	{
		__setUCalendar(__cal, &tm);
		i18n_udate date;
		i18n_ucalendar_get_milliseconds(__cal, &date);
		return ms2sec(date);
	}
	else
	{
		i18n_ucalendar_h ucal = __getUcal(timezone);
		__setUCalendar(ucal, &tm);

		i18n_udate date;
		i18n_ucalendar_get_milliseconds(ucal, &date);
		i18n_ucalendar_destroy(ucal);
		return ms2sec(date);
	}
}

void CalLocaleManager::getTmFromUtime(const char *timezone, const long long int utime, struct tm &tm)
{
	WENTER();
	if (timezone == NULL || !strcmp(timezone, __tzid.c_str()))
	{
		i18n_ucalendar_set_milliseconds(__cal, sec2ms(utime));
		__getUCalendar(__cal, &tm);
	}
	else
	{
		i18n_ucalendar_h ucal = __getUcal(timezone);
		i18n_ucalendar_set_milliseconds(ucal, sec2ms(utime));
		__getUCalendar(ucal, &tm);
		i18n_ucalendar_destroy(ucal);
	}
}

int CalLocaleManager::getDayOfWeekInMonth(const char *timezone, const long long int utime)
{
	WENTER();
	if (timezone == NULL || !strcmp(timezone, __tzid.c_str()))
	{
		i18n_ucalendar_set_milliseconds(__cal, sec2ms(utime));
		int32_t result = 0;
		i18n_ucalendar_get(__cal, I18N_UCALENDAR_DAY_OF_WEEK_IN_MONTH, &result);
		return (int)result;
	}
	else
	{
		int dayOfWeek = 0;
		i18n_ucalendar_h ucal = __getUcal(timezone);
		i18n_ucalendar_set_milliseconds(ucal, sec2ms(utime));
		i18n_ucalendar_get(ucal, I18N_UCALENDAR_DAY_OF_WEEK_IN_MONTH, &dayOfWeek);
		i18n_ucalendar_destroy(ucal);
		return dayOfWeek;
	}
}

const char* CalLocaleManager::getWeekdayText(const char *timezone, const long long int utime)
{
	WENTER();
	static char weeks[7][3] = {"SU", "MO", "TU", "WE", "TH", "FR", "SA"};
	if (timezone == NULL || !strcmp(timezone, __tzid.c_str()))
	{
		i18n_ucalendar_set_milliseconds(__cal, sec2ms(utime));
		int wday = 0;
		i18n_ucalendar_get(__cal, I18N_UCALENDAR_DAY_OF_WEEK, &wday);
		return weeks[wday - 1];
	}
	else
	{
		i18n_ucalendar_h ucal = __getUcal(timezone);
		i18n_ucalendar_set_milliseconds(ucal, sec2ms(utime));
		int wday = 0;
		i18n_ucalendar_get(ucal, I18N_UCALENDAR_DAY_OF_WEEK, &wday);
		i18n_ucalendar_destroy(ucal);
		return weeks[wday - 1];
	}
}

const char* CalLocaleManager::getWeekdayText(int weekday)
{
	WENTER();
	return __weekdayText[weekday].c_str();
}

const char* CalLocaleManager::getWeekdayShortText(int weekday)
{
	WENTER();
	return __weekdayShortText[weekday].c_str();
}

int CalLocaleManager::getWeekday(const char *timezone, const long long int utime)
{
	WENTER();
	if (timezone == NULL || !strcmp(timezone, __tzid.c_str()))
	{
		i18n_ucalendar_set_milliseconds(__cal, sec2ms(utime));
		int wday = 0;
		i18n_ucalendar_get(__cal, I18N_UCALENDAR_DAY_OF_WEEK, &wday);
		return (wday-1);
	}
	else
	{
		i18n_ucalendar_h ucal = __getUcal(timezone);
		i18n_ucalendar_set_milliseconds(ucal, sec2ms(utime));
		int wday = 0;
		i18n_ucalendar_get(ucal, I18N_UCALENDAR_DAY_OF_WEEK, &wday);
		i18n_ucalendar_destroy(ucal);
		return (wday-1);
	}
}

void CalLocaleManager::updateTmHour(const int delta, struct tm &tm)
{
	WENTER();
	if (!delta)
		return;

	__setUCalendar(__cal, &tm);
	i18n_ucalendar_add(__cal, I18N_UCALENDAR_HOUR, delta);
	__getUCalendar(__cal, &tm);
}

void CalLocaleManager::updateTmDay(const int delta, struct tm &tm)
{
	WENTER();
	if (!delta)
		return;

	__setUCalendar(__cal, &tm);
	i18n_ucalendar_add(__cal, I18N_UCALENDAR_DAY_OF_YEAR, delta);
	__getUCalendar(__cal, &tm);
}

void CalLocaleManager::updateTmMonth(const int delta, struct tm &tm)
{
	WENTER();
	if (!delta)
		return;

	__setUCalendar(__cal, &tm);
	i18n_ucalendar_add(__cal, I18N_UCALENDAR_MONTH, delta);
	__getUCalendar(__cal, &tm);
}

void CalLocaleManager::updateTmYear(const int delta, struct tm &tm)
{
	WENTER();
	if (!delta)
		return;

	__setUCalendar(__cal, &tm);
	i18n_ucalendar_add(__cal, I18N_UCALENDAR_YEAR, delta);
	__getUCalendar(__cal, &tm);
}

int CalLocaleManager::getDayDiff(const struct tm &date1, const struct tm &date2)
{
	WENTER();
	WASSERT(date1.tm_hour == 12);
	WASSERT(date1.tm_min == 0);
	WASSERT(date1.tm_sec == 0);

	WASSERT(date2.tm_hour == 12);
	WASSERT(date2.tm_min == 0);
	WASSERT(date2.tm_sec == 0);

	return difftime(mktime((tm*)&date1), mktime((tm*)&date2)) / (60 * 60 * 24);
}

void CalLocaleManager::__setUCalendar(i18n_ucalendar_h calendar, const struct tm *tm)
{
	WENTER();
	int status = I18N_ERROR_NONE;

	status = i18n_ucalendar_set_date_time(calendar, tm->tm_year+1900, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

	if(status != I18N_ERROR_NONE)
	{
		WDEBUG("i18n_ucalendar_set_date_time() is fail. error:%d", status);
		WDEBUG("tm->tm_year:%d, tm->tm_mon:%d, tm->tm_mday:%d, tm->tm_hour:%d, tm->tm_min:%d,  tm->tm_sec:%d ", tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	}
}

void CalLocaleManager::__getUCalendar(i18n_ucalendar_h calendar, struct tm *tm)
{
	WENTER();
	i18n_ucalendar_get(calendar, I18N_UCALENDAR_SECOND, &tm->tm_sec);
	i18n_ucalendar_get(calendar, I18N_UCALENDAR_MINUTE, &tm->tm_min);
	i18n_ucalendar_get(calendar, I18N_UCALENDAR_HOUR_OF_DAY, &tm->tm_hour);
	i18n_ucalendar_get(calendar, I18N_UCALENDAR_DATE , &tm->tm_mday);
	i18n_ucalendar_get(calendar, I18N_UCALENDAR_MONTH, &tm->tm_mon);
	i18n_ucalendar_get(calendar, I18N_UCALENDAR_YEAR, &tm->tm_year);
	tm->tm_year -= 1900;
	i18n_ucalendar_get(calendar, I18N_UCALENDAR_DAY_OF_WEEK, &tm->tm_wday);
	tm->tm_wday -= 1;
	i18n_ucalendar_get(calendar, I18N_UCALENDAR_DAY_OF_YEAR, &tm->tm_yday);
	tm->tm_yday -= 1;
	i18n_ucalendar_get(calendar, I18N_UCALENDAR_DST_OFFSET, &tm->tm_isdst);
	if(tm->tm_isdst)
		tm->tm_isdst = 1;
	else
		tm->tm_isdst = 0;

}

char* CalLocaleManager::__getTzOffset(i18n_ucalendar_h cal)
{
	WENTER();
	int32_t zoneOffset, dstOffset;
	i18n_ucalendar_get(cal, I18N_UCALENDAR_ZONE_OFFSET, &zoneOffset);
	i18n_ucalendar_get(cal, I18N_UCALENDAR_DST_OFFSET, &dstOffset);
	int offset =(zoneOffset + dstOffset)/1000;

	int gmtoffset_hour = offset/3600;
	int gmtoffset_min = (offset%3600)/60;
	gmtoffset_min *= gmtoffset_min < 0 ? -1 : 1;

	return g_strdup_printf("GMT %+d:%02d", gmtoffset_hour, gmtoffset_min);
}

char* CalLocaleManager::__getTzName(i18n_ucalendar_h cal, const char *language, const std::string &timeZone)
{
	WENTER();
	int status = I18N_ERROR_NONE;
	i18n_uchar tz_standard_name[DATETIME_BUFFER * 2] = {0};
	char* tz_standard_name_str = NULL;
	i18n_ubool isIn = false;

	status = i18n_ucalendar_is_in_daylight_time(cal, &isIn);

	int32_t buf_size_needed=0;

	if(isIn)
		status = i18n_ucalendar_get_timezone_displayname(cal, I18N_UCALENDAR_DST,  language, tz_standard_name, 128, &buf_size_needed);
	else
		status = i18n_ucalendar_get_timezone_displayname(cal, I18N_UCALENDAR_STANDARD ,  language, tz_standard_name, 128, &buf_size_needed);
	if (status) {
		WERROR("i18n_ucalendar_get_timezone_displayname got an error : %s, %d", tz_standard_name, status);
		return NULL;
	}

	int len = 0;
	int len_str = 0;
	int len_utf8 = 0;

	len = i18n_ustring_get_length(tz_standard_name);
	len_str = sizeof(char) * 4 * (len + 1);
	tz_standard_name_str = (char*)calloc(1, len_str);
	if (!tz_standard_name_str) {
		WERROR("timezone string is null");
		return NULL;
	}

	i18n_error_code_e error = I18N_ERROR_NONE;
	i18n_ustring_to_UTF8(tz_standard_name_str, len_str, &len_utf8, tz_standard_name, i18n_ustring_get_length(tz_standard_name), &error);

	return tz_standard_name_str;
}

void CalLocaleManager::__initWeekday()
{
	WENTER();
	// get Sunday
	CalDate date(2013,12,1);

	for(int i=0; i < 7; i++)
	{
		getDateText(CalLocaleManager::DATEFORMAT_2, date, __weekdayText[i]);
		getDateText(CalLocaleManager::DATEFORMAT_17, date, __weekdayShortText[i]);
		date.incrementDay();
	}
}

void CalLocaleManager::getDisplayTextTimeZone(const std::string &timeZone, std::string &displayText)
{
	WENTER();
	char *language = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &language);

	int status = I18N_ERROR_NONE;
	i18n_uchar utf16_timezone[DATETIME_BUFFER] = {0};
	i18n_ustring_copy_ua_n(utf16_timezone,  timeZone.c_str(), DATETIME_BUFFER);

	i18n_ucalendar_h cal;
	status = i18n_ucalendar_create(utf16_timezone, -1, language, I18N_UCALENDAR_GREGORIAN , &cal);
	if (status) {
		WERROR("i18n_ucalendar_create got an error : %d", status);
		return ;
	}

	char *tz_name = NULL;
	char *tz_offset = NULL;

	tz_name = __getTzName(cal, language, displayText);
	tz_offset = __getTzOffset(cal);

	g_free(language);

	i18n_ucalendar_destroy(cal);

	char *tz_text = NULL;

	tz_text = g_strdup_printf("(%s) %s", tz_offset, tz_name);

	g_free(tz_name);
	g_free(tz_offset);

	displayText = tz_text;

	g_free(tz_text);
	WLEAVE();
}

int CalLocaleManager::getLocaleFirstDayOfWeek()
{
	WENTER();
	i18n_uchar utf16_timezone[DATETIME_BUFFER] = {0};

	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_COUNTRY, &locale);

	if (CAL_STRLEN(locale)) {
		i18n_ulocale_set_default(locale);
		free(locale);
	}

	i18n_ustring_copy_ua(utf16_timezone, __tzid.c_str());

	i18n_ucalendar_h cal;

	const char *loc;
	i18n_ulocale_get_default(&loc);
	i18n_ucalendar_create(utf16_timezone, i18n_ustring_get_length(utf16_timezone), loc, I18N_UCALENDAR_TRADITIONAL , &cal);
	c_retvm_if(!cal, -1, "cal is null");

	int first_day_of_week;
	i18n_ucalendar_get_attribute(cal, I18N_UCALENDAR_FIRST_DAY_OF_WEEK, &first_day_of_week);

	i18n_ucalendar_destroy(cal);

/*
	UCAL_SUNDAY = 1,
	UCAL_MONDAY,
	UCAL_TUESDAY,
	UCAL_WEDNESDAY,
	UCAL_THURSDAY,
	UCAL_FRIDAY,
	UCAL_SATURDAY,
*/
	WLEAVE();
	return first_day_of_week -1;
}
