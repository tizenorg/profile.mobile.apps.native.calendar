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

#ifdef TIZEN_2_4
#define CAL_LOCALE_MANAGER_WORLDCLOCK_DB "/opt/dbspace/.worldclock.db"
#elif TIZEN_3_0
#define CAL_LOCALE_MANAGER_WORLDCLOCK_DB "/usr/dbspace/.worldclock.db"
#else
#define CAL_LOCALE_MANAGER_WORLDCLOCK_DB "/opt/dbspace/.worldclock.db"
#endif

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
	updateRegion();
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
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	WDEBUG("Language %s", locale);

	if (locale) {
		elm_language_set(locale);
		free(locale);
	}
}

void CalLocaleManager::setEvasObjForRTL(Evas_Object* obj)
{
	__obj = obj;
}

bool CalLocaleManager::isRTL()
{
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

	return (bidi == EVAS_BIDI_DIRECTION_RTL);
}

void CalLocaleManager::updateRegion()
{
	WENTER();
	WDEBUG("UPDATE---------------------------------------------------------------------------------------------------------------");

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

void CalLocaleManager::getDateTimeText(const DateFormat df, const TimeFormat tf, const CalDateTime& dt, std::string& text)
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
	long long int utime = 0;// TODO CALDATETIME: Get this stuff from updated CalDateTime class: dt.getUtimeDate();
	date = sec2ms(utime);

	formattedCapacity = (int32_t)(sizeof(formatted)/sizeof((formatted)[0]));
	i18n_udate_format_date(formatter, date, formatted, formattedCapacity, NULL, &formattedLength);

	i18n_ustring_copy_au_n(formattedString, formatted, 128);
	text = formattedString;
	WLEAVE();
}

void CalLocaleManager::getDateText(const DateFormat dateFormat, const CalDateTime& date, std::string& text)
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

	CalDateTime time(date.getYear() - 1900, date.getMonth() - 1, date.getMday());
	long long int utime = 0; // TODO CALDATETIME: Get this stuff from updated CalDateTime class: time.getUtimeDate();
	udate = sec2ms(utime);

	WENTER();

	i18n_udate_format_date(formatter, udate, formatted, DATETIME_BUFFER, NULL, &formattedLength);

	i18n_ustring_copy_au_n(formattedString, formatted, DATETIME_BUFFER * 2);
	text = formattedString;

	WLEAVE();
}

void CalLocaleManager::__getLocale(std::string &localeStr)
{
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
}

const i18n_udate_format_h CalLocaleManager::__getUDateFormat(const DateFormat df, const TimeFormat tf)
{
	WENTER();
	WDEBUG("UPDATE---------------------------------------------------------------------------------------------------------------");
	if (__patternGenerator == NULL)
	{
		updateRegion();
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
		i18n_ustring_copy_ua_n(custom_format, formatString.c_str(), formatString.length());

		bestPatternCapacity = (int32_t)(sizeof(bestPattern)/sizeof((bestPattern)[0]));
		i18n_udatepg_get_best_pattern(__patternGenerator, custom_format, i18n_ustring_get_length(custom_format), bestPattern,   bestPatternCapacity, &bestPatternLength);

		i18n_udate_create(I18N_UDATE_PATTERN , I18N_UDATE_PATTERN , __locale.c_str(), nullptr, -1, bestPattern, -1, &formatter);

		__mapFormat.insert(std::pair<std::string,i18n_udate_format_h>(formatString, formatter));

		return formatter;
	}
	else
	{
		return it->second;
	}
}

const char* CalLocaleManager::getWeekdayText(const long long int utime)
{
	static char weeks[7][3] = {"SU", "MO", "TU", "WE", "TH", "FR", "SA"};

	i18n_ucalendar_set_milliseconds(__cal, sec2ms(utime));
	int wday = 0;
	i18n_ucalendar_get(__cal, I18N_UCALENDAR_DAY_OF_WEEK, &wday);
	return weeks[wday - 1];
}

const char* CalLocaleManager::getWeekdayText(int weekday)
{
	return __weekdayText[weekday].c_str();
}

const char* CalLocaleManager::getWeekdayShortText(int weekday)
{
	return __weekdayShortText[weekday].c_str();
}

void CalLocaleManager::__initWeekday()
{
	// get Sunday
	CalDateTime date(2013,12,1);

	for(int i=0; i < 7; i++)
	{
		getDateText(CalLocaleManager::DATEFORMAT_2, date, __weekdayText[i]);
		getDateText(CalLocaleManager::DATEFORMAT_17, date, __weekdayShortText[i]);
		// TODO CALDATETIME: Get this stuff from updated CalDateTime class: date.incrementDay();
	}
}
