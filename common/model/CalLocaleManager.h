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

class WAPP_ASSIST_EXPORT CalLocaleManager
{
SINGLETON_IDIOM(CalLocaleManager);
protected:
	CalLocaleManager();
	virtual ~CalLocaleManager();
private:
	WDISABLE_COPY_AND_ASSIGN(CalLocaleManager);

public:
	bool isRTL();
	void updateLocaleForEvasObj();
	void setEvasObjForRTL(Evas_Object* obj);

	void updateRegion(void);

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
	void getDateTimeText(const DateFormat dateFormat, const TimeFormat timeFormat, const CalDateTime& dateTime, std::string& text);
	void getDateText(const DateFormat dateFormat, const CalDateTime& date, std::string& text);

	const char* getWeekdayText(int weekday);
	const char* getWeekdayShortText(int weekday);
	int getLocaleFirstDayOfWeek();

private:
	void __getLocale(std::string &localeStr);

	// __mapFormat
	const i18n_udate_format_h __getUDateFormat(const DateFormat df, const TimeFormat tf);

	// for weekday
	void __initWeekday();

public:
	std::string __locale;
	i18n_udatepg_h __patternGenerator;
	std::map<std::string,i18n_udate_format_h> __mapFormat;
private:
	std::string __weekdayText[7];
	std::string __weekdayShortText[7];
	Evas_Object* __obj;
};

#endif
