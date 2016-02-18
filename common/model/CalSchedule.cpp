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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "CalSchedule.h"
#include "CalCommon.h"
#include "CalOriginalSchedule.h"
#include "CalSettingsManager.h"
#include "CalLocaleManager.h"

CalSchedule::CalSchedule()
{
	__type = INVALID;
	__trimedSummary = NULL;
	__trimedLocation = NULL;
	__record = NULL;
}

CalSchedule::CalSchedule(calendar_record_h record)
{
	__record = record;
	__trimedSummary = NULL;
	__trimedLocation = NULL;
	__type = INVALID;
}

CalSchedule::~CalSchedule()
{
	if (__record)
	{
		calendar_record_destroy(__record, true);
	}

	free(__trimedSummary);
	free(__trimedLocation);
}

CalSchedule::CalSchedule(const CalSchedule& obj)
{
	if (obj.__record)
	{
		calendar_record_h clone;
		calendar_record_clone(obj.__record, &clone);
		__record = clone;
	}
	__record = NULL;
	__type = obj.__type;

	__trimedSummary = NULL;
	__trimedLocation = NULL;
}

const CalSchedule& CalSchedule::operator=(const CalSchedule& obj)
{
	if (this != &obj)
	{
		if (obj.__record)
		{
			calendar_record_h clone;
			calendar_record_clone(obj.__record, &clone);
			__record = clone;
		}
		__type = obj.__type;

		__trimedSummary = NULL;
		__trimedLocation = NULL;
	}
	return *this;
}

CalSchedule::Type CalSchedule::getType() const
{
	return __type;
}

int CalSchedule::getIndex() const
{
	const unsigned int propertyId = getIndexProperty();
	int index = 0;
	int error = calendar_record_get_int(__record, propertyId, &index);
	WPRET_VM((error != CALENDAR_ERROR_NONE),-1,"get id fail");
	return index;
}

void CalSchedule::getFromToDisplayString(std::string& text) const
{
	CalDateTime startTime, endTime;
	getStart(startTime);
	getEnd(endTime);

	const char* myTimezone = __getTimeZone();
	std::string calendarTimezone;
	CalSettingsManager::getInstance().getCalendarTimeZone(calendarTimezone);

	std::string displayMyTz;
	std::string displayCalendarTz;
	CalLocaleManager::getInstance().getDisplayTextTimeZone(calendarTimezone, displayCalendarTz);
	if (myTimezone)
	{
		CalLocaleManager::getInstance().getDisplayTextTimeZone(myTimezone, displayMyTz);
	}

	if (startTime.isAllDay( )== true ||
			myTimezone == NULL ||
			g_str_has_prefix(myTimezone, TIMEZONE_ETC_GMT) ||
			displayCalendarTz.compare(displayMyTz) == 0)
	{
		__getFromToDisplayString(NULL, startTime, endTime, text);
	}
	else
	{
		text = displayCalendarTz + "<br/>";

		std::string stringTemp;
		__getFromToDisplayString(calendarTimezone.c_str(), startTime, endTime, stringTemp);

		text += stringTemp + "<br/>";
		text += displayMyTz + "<br/>";

		stringTemp.clear();
		__getFromToDisplayString(myTimezone, startTime, endTime, stringTemp);

		text += stringTemp;
	}
	return;
}

void CalSchedule::getFromToString(std::string& text) const
{
	CalDateTime startTime, endTime;
	getStart(startTime);
	getEnd(endTime);

	const char* myTimezone = getTimeZone();
	std::string calendarTimezone;
	CalSettingsManager::getInstance().getCalendarTimeZone(calendarTimezone);

	if (startTime.isAllDay( )== true ||
			myTimezone == NULL ||
			g_strcmp0(myTimezone, TIMEZONE_ETC_GMT) == 0 ||
			calendarTimezone.compare(myTimezone) == 0)
	{
		__getFromToString(NULL, startTime, endTime, text);
	}
	else
	{
		std::string stringTemp;
		CalLocaleManager::getInstance().getDisplayTextTimeZone(calendarTimezone, stringTemp);

		text = stringTemp + "\r\n";

		stringTemp.clear();
		__getFromToString(calendarTimezone.c_str(), startTime, endTime, stringTemp);

		text = stringTemp + "\r\n";

		stringTemp.clear();
		CalLocaleManager::getInstance().getDisplayTextTimeZone(myTimezone, stringTemp);

		text = stringTemp + "\r\n";

		stringTemp.clear();
		__getFromToString(myTimezone, startTime, endTime, stringTemp);

		text = stringTemp + "\r\n";
	}
}

void CalSchedule::getFromToString(const CalDate& date, std::string& text) const
{
	CalDateTime startTime, endTime;
	getStart(startTime);
	getEnd(endTime);

	std::string startText, endText;
	startTime.getString(startText);
	endTime.getString(endText);

	text = startText + " - " + endText;
}

void CalSchedule::__getFromToString(const char* timezone, const CalDateTime& start, const CalDateTime& end, std::string& text) const
{
	std::string startText, endText, endNoDataText;
	start.getString(startText);
	end.getString(endText);
	end.getTimeString(endNoDataText);

	bool isSameDay = (start.getYear() == end.getYear()) &&
			(start.getMonth() == end.getMonth()) &&
			(start.getMday() == end.getMday());

	if (start.isAllDay() == true)
	{
		if (isSameDay == true)
		{
			text = startText;
		}
		else
		{
			text = startText + " - \r\n" + endText;
		}
	}
	else
	{
		if (isSameDay == true)
		{
			text = startText + " - " + endNoDataText;
		}
		else
		{
			text = startText + " - \r\n" + endText;
		}
	}
}

void CalSchedule::__getFromToDisplayString(const char* timezone, const CalDateTime& start, const CalDateTime& end, std::string& text) const
{
	std::string bufString;
	__getFromToString(timezone, start, end, bufString);
	char *markupStr = elm_entry_utf8_to_markup(bufString.c_str());
	if (!markupStr)
	{
		WERROR("elm_entry_utf8_to_markup(%s) failed : %s", bufString.c_str(), strerror(errno));
		return;
	}
	text = markupStr;
	free(markupStr);
}

void CalSchedule::__trimLeft(std::string& str) const
{
	if (str.empty())
		return;
	str.erase(0,str.find_first_not_of(" \t\v\n"));
}

void CalSchedule::__trimRight(std::string& str) const
{
	if (str.empty())
		return;
	std::size_t found = str.find_last_not_of(" \t\v\n");
	if (found != std::string::npos)
		str.erase(str.find_last_not_of(" \t\v\n")+1);
	else
		str.clear();
}

void CalSchedule::__trimNewLine(char* str) const
{
	if (str == NULL)
		return;

	while (*str != '\0')
	{
		if(*str == '\n' || *str == '\r')
			*str = ' ';
		str++;
	}
}

void CalSchedule::__checkSummary(const int stringLimit, int len, std::string& text) const
{
	const int summaryBuffer = 128;
	std::string utfStr = text;

	int bookId = getBookId();
	if (DEFAULT_BIRTHDAY_CALENDAR_BOOK_ID == bookId)
	{
		// TODO
		char *syncData1 = NULL;
		const unsigned int syncData1propertyId = getSyncData1Property();
		int error = calendar_record_get_str_p(__record, syncData1propertyId, &syncData1);
		WPRET_M((error != CALENDAR_ERROR_NONE),"get_str fail");

		if (!strcmp(syncData1, "anniversary"))
		{
			char buffer[summaryBuffer]={0};
			snprintf(buffer, sizeof( buffer)-1, _L_("IDS_CLD_MBODY_PSS_ANNIVERSARY"), utfStr.c_str());
			utfStr.clear();
			utfStr.append(buffer);
		}
		else if (!strcmp(syncData1, "birthday"))
		{
			char buffer[summaryBuffer]={0};
			snprintf(buffer, sizeof( buffer)-1, _L_("IDS_CLD_BODY_PSS_BIRTHDAY_ABB"), utfStr.c_str());
			utfStr.clear();
			utfStr.append(buffer);
		}
		else if (!strcmp(syncData1, "other"))
		{
			utfStr.append(". ");
			utfStr.append(_L_("IDS_CLD_BODY_OTHER_M_EVENT_TYPE"));
		}
		else
		{
			utfStr.append(". ");
			utfStr.append(syncData1);
		}

	}
	// TODO multi byte string
	if (stringLimit > 0 && len > stringLimit)
		utfStr.resize(stringLimit);

	char *markupStr = elm_entry_utf8_to_markup(utfStr.c_str());
	if (!markupStr)
	{
		WERROR("elm_entry_utf8_to_markup(%s) failed : %s", utfStr.c_str(), strerror(errno));
		return;
	}
	text = markupStr;
	free(markupStr);
}

void CalSchedule::getDisplaySummary(const int stringLimit, std::string& text) const
{
	const char *summary = getSummary();

	if (summary == NULL || strlen(summary) == 0)
	{
		text = _L_("IDS_CLD_MBODY_MY_EVENT");
		return ;
	}

	text = summary;
	__checkSummary(stringLimit, (int)strlen(summary), text);
}

void CalSchedule::getDisplayTrimedSummary(const int stringLimit, std::string& text)
{
	const char* summary = getSummary();

	if (summary == NULL || strlen(summary) == 0)
	{
		text = _L_("IDS_CLD_MBODY_MY_EVENT");
		return ;
	}

	if(__trimedSummary == NULL)
	{
		__trimedSummary = strdup(summary);
		if (!__trimedSummary)
		{
			WERROR("strdup(%s) failed : %s", summary, strerror(errno));
			return;
		}
		__trimNewLine(__trimedSummary);
	}

	text = __trimedSummary;
	__checkSummary(stringLimit, (int)strlen(summary), text);
}

const char* CalSchedule::getSummary() const
{
	const unsigned int propertyId = getSummaryProperty();
	char *tmp = NULL;
	int error = calendar_record_get_str_p(__record, propertyId, &tmp);
	WPRET_VM((error != CALENDAR_ERROR_NONE),NULL,"get_str fail");
	return tmp;
}

void CalSchedule::__checkLocation(const int stringLimit, int len, std::string& text) const
{
	std::string str = text;

	__trimLeft(str);
	__trimRight(str);

	if (str.empty())
		return ;

	// TODO multi byte string
	if (stringLimit > 0 && len > stringLimit)
		str.resize(stringLimit);

	char *markupStr = elm_entry_utf8_to_markup(str.c_str());
	if (!markupStr)
	{
		WERROR("elm_entry_utf8_to_markup(%s) failed : %s", str.c_str(), strerror(errno));
		return;
	}
	text = markupStr;
	free(markupStr);
}

void CalSchedule::getDisplayLocation(const int stringLimit, std::string& text) const
{
	const char* location = getLocation();

	if (location == NULL)
	{
		text.clear();
		return ;
	}

	text = location;
	__checkLocation(stringLimit, (int)strlen(location), text);

}

void CalSchedule::getDisplayTrimedLocation(const int stringLimit, std::string& text)
{
	const char* location = getLocation();

	if (location)
	{
		if(__trimedLocation == NULL)
		{
			__trimedLocation = strdup(location);
			if (!__trimedLocation)
			{
				WERROR("strdup(%s) failed : %s", location, strerror(errno));
				return;
			}
			__trimNewLine(__trimedLocation);
		}
		text = __trimedLocation;
		__checkLocation(stringLimit, (int)strlen(location), text);
	}
	else
	{
		text.clear();
		WERROR("Location could not be taken from getLocation() function");
	}
}

const char* CalSchedule::getLocation() const
{
	const unsigned int propertyId = getLocationProperty();
	char *tmp = NULL;
	int error = calendar_record_get_str_p(__record, propertyId, &tmp);
	WPRET_VM((error != CALENDAR_ERROR_NONE),NULL,"get_str fail");
	return tmp;
}

int CalSchedule::getBookId() const
{
	const unsigned int propertyId = getBookIdProperty();
	int index = 0;
	int error = calendar_record_get_int(__record, propertyId, &index);
	WPRET_VM((error != CALENDAR_ERROR_NONE),-1,"get id fail");
	return index;
}

void CalSchedule::getStart(CalDateTime& startTime) const
{
	__getStart(startTime);
}

static int __getDateCompareVal(const calendar_time_s& caltime)
{
	WASSERT(caltime.type == CALENDAR_TIME_LOCALTIME);
	return (caltime.time.date.year << 9) | (caltime.time.date.month << 5) | caltime.time.date.mday;
}

void CalSchedule::getEnd(CalDateTime& endTime, bool isAdjustive) const
{
	__getEnd(endTime, isAdjustive);
}

void CalSchedule::getEndForComplexList(CalDateTime& endTime) const
{
	__getEnd(endTime, false);
}

void CalSchedule::getDisplayDescription(std::string& text) const
{
	const unsigned int propertyId = getDescriptionProperty();
	char *tmp = NULL;
	int error = calendar_record_get_str_p(__record, propertyId, &tmp);
	WPRET_M((error != CALENDAR_ERROR_NONE),"get_str fail");
	if (tmp == NULL)
	{
		text.clear();
		return ;
	}
	std::string utfStr = tmp;
	char *markupStr = elm_entry_utf8_to_markup(utfStr.c_str());
	if (!markupStr)
	{
		WERROR("elm_entry_utf8_to_markup(%s) failed : %s", utfStr.c_str(), strerror(errno));
		return;
	}
	text = markupStr;
	free(markupStr);
}

const char* CalSchedule::getDescription() const
{
	const unsigned int propertyId = getDescriptionProperty();
	char *tmp = NULL;
	int error = calendar_record_get_str_p(__record, propertyId, &tmp);
	WPRET_VM((error != CALENDAR_ERROR_NONE),NULL,"get_str fail");
	return tmp;
}

const char* CalSchedule::getTimeZone() const
{
	const unsigned int propertyId = getStartTimezoneProperty();
	if (propertyId == 0)
		return NULL;
	char *tmp = NULL;
	int error = calendar_record_get_str_p(__record, propertyId, &tmp);
	WPRET_VM((error != CALENDAR_ERROR_NONE),NULL,"get_str fail");

	// for EAS !
	static std::string __timeZone;
	std::string buf;
	if(tmp)
		buf = tmp;

	CalLocaleManager::getInstance().getEasTimeZone(buf,__timeZone);
	return __timeZone.c_str();
}

const char* CalSchedule::getUid() const
{
	const unsigned int propertyId = getUidProperty();
	if (propertyId == 0)
		return NULL;
	char *tmp = NULL;
	int error = calendar_record_get_str_p(__record, propertyId, &tmp);
	WPRET_VM((error != CALENDAR_ERROR_NONE),NULL,"get_str fail");
	return tmp;
}

const bool CalSchedule::hasReminder() const
{
	const unsigned int propertyId = getHasReminderProperty();
	int count = 0;
	if (propertyId == 0)
	{
		WASSERT(0);
		return false;
	}
	else
	{
		int error = calendar_record_get_int(__record, propertyId, &count);
		WPRET_VM((error != CALENDAR_ERROR_NONE),false,"get id fail");
	}
	if (count == 0)
		return false;
	return true;
}

const int CalSchedule::getRemindersCount() const
{
	bool reminder = hasReminder();
	return reminder==true ? 1:0;
}

const bool CalSchedule::hasRepeat() const
{
	const unsigned int propertyId = getHasRepeatProperty();
	int count = 0;
	if (propertyId == 0)
	{
		const CalScheduleRepeat repeat = getRepeat();
		return (repeat.unitType == CalScheduleRepeat::UnitType::ONE_TIME) ? false : true;
	}
	else
	{
		int error = calendar_record_get_int(__record, propertyId, &count);
		WPRET_VM((error != CALENDAR_ERROR_NONE),false,"get id fail");
	}
	if (count == 0)
		return false;
	return true;
}

const CalScheduleRepeat& CalSchedule::getRepeat() const
{
	static CalScheduleRepeat repeat;
	return repeat;
}

void CalSchedule::setSummary(const char* summary)
{
	const unsigned int propertyId = getSummaryProperty();
	int error = calendar_record_set_str(__record, propertyId, summary);
	WPRET_M((error != CALENDAR_ERROR_NONE),"set_str fail");
}

void CalSchedule::setLocation(const char* location)
{
	const unsigned int propertyId = getLocationProperty();
	int error = calendar_record_set_str(__record, propertyId, location);
	WPRET_M((error != CALENDAR_ERROR_NONE),"set_str fail");
}

void CalSchedule::setStart(const CalDateTime& startTime)
{
	const unsigned int propertyId = getStartProperty();
	int error = calendar_record_set_caltime(__record, propertyId, getCaltime(startTime));
	WPRET_M((error != CALENDAR_ERROR_NONE),"set_caltime fail");
}

void CalSchedule::setEnd(const CalDateTime& endTime)
{
	const unsigned int propertyId = getEndProperty();
	int error = 0;
	if (endTime.isAllDay()) {
		CalDateTime adjustedEndTime = endTime;
		adjustedEndTime.addDays(1, false);
		error = calendar_record_set_caltime(__record, propertyId, getCaltime(adjustedEndTime));
	} else {
		error = calendar_record_set_caltime(__record, propertyId, getCaltime(endTime));
	}
	WPRET_M((error != CALENDAR_ERROR_NONE),"set_caltime fail");
}

void CalSchedule::setDescription(const char* description)
{
	std::string str;
	if (description) {
		str = description;
		__trimLeft(str);
		__trimRight(str);
	}

	const unsigned int propertyId = getDescriptionProperty();
	int error = calendar_record_set_str(__record, propertyId, str.c_str());
	WPRET_M((error != CALENDAR_ERROR_NONE),"set_str fail");
}

void CalSchedule::setTimeZone(const char* timezone)
{
	const unsigned int propertyId = getStartTimezoneProperty();
	int error = calendar_record_set_str(__record, propertyId, timezone);
	WPRET_M((error != CALENDAR_ERROR_NONE),"set_str fail");

	const unsigned int propertyId2 = getEndTimezoneProperty();
	error = calendar_record_set_str(__record, propertyId2, timezone);
	WPRET_M((error != CALENDAR_ERROR_NONE),"set_str fail");
}

void CalSchedule::setBookId(const int bookId)
{
	const unsigned int propertyId = getBookIdProperty();
	int error = calendar_record_set_int(__record, propertyId, bookId);
	WPRET_M((error != CALENDAR_ERROR_NONE),"set id fail");
}

void CalSchedule::setUid(const char* uid)
{
	const unsigned int propertyId = getUidProperty();
	int error = calendar_record_set_str(__record, propertyId, uid);
	WPRET_M((error != CALENDAR_ERROR_NONE),"set_str fail");
}

void CalSchedule::setRepeat(const CalScheduleRepeat& repeat)
{
	WERROR("Do not use this api for type[%d]", __type);
	return;
}

calendar_record_h CalSchedule::getRecord() const
{
	return __record;
}

void CalSchedule::getReminder(const int nth, CalScheduleReminder& reminder)
{
	WERROR("Do not use this api for type[%d]", __type);
	return;
}

void CalSchedule::setReminder(const int nth, const CalScheduleReminder& reminder)
{
	WERROR("Do not use this api for type[%d]", __type);
	return;
}

int CalSchedule::addReminder(const CalScheduleReminder& reminder)
{
	WERROR("Do not use this api for type[%d]", __type);
	return 0;
}

void CalSchedule::removeReminder(const int nth)
{
	WERROR("Do not use this api for type[%d]", __type);
	return;
}

void CalSchedule::initialize(const char* tzid, const CalDateTime& startTime)
{
	WERROR("Do not use this api for type[%d]", __type);
	return;
}

bool CalSchedule::isAllDay() const
{
	CalDateTime eventStartDate, eventEndDate;
	__getStart(eventStartDate);
	__getEnd(eventEndDate);
	return eventStartDate.isAllDay() && eventEndDate.isAllDay();
}

std::shared_ptr<CalSchedule> CalSchedule::makeDefaultSchedule(bool isAllDay)
{
	WENTER();
	std::shared_ptr<CalSchedule> schedule = __createDefaultSchedule();

	// set default time
	CalDateTime startTime;
	struct tm startTm = {0};
	startTime.getTmFromUtime(&startTm);
	startTm.tm_min = 0;
	startTm.tm_sec = 0;
	startTime.set(startTm);
	startTime.addHours(1);
	startTime.setAllDay(isAllDay);

	schedule->initialize(NULL, startTime);

	return schedule;
}

std::shared_ptr<CalSchedule> CalSchedule::makeDefaultSchedule(const CalDate& date)
{
	WENTER();
	std::shared_ptr<CalSchedule> schedule = __createDefaultSchedule();

#if 0
	// start hour is always based on device time
	std::string deviceTimeZone;
	CalSettingsManager::getInstance().getDeviceTimeZone(deviceTimeZone);

	// set default time
	CalDateTime startTime;
	struct tm startTm;
	startTime.getTm(deviceTimeZone.c_str(), &startTm);
	startTm.tm_year = date.getYear() - 1900;
	startTm.tm_mon = date.getMonth() - 1;
	startTm.tm_mday = date.getMday();
	startTm.tm_min = 0;
	startTm.tm_sec = 0;
	startTime.set(startTm);
	startTime.addHours(1);

	std::string timeZone;
	CalLocaleManager::getInstance().getTimeZone(timeZone);

	schedule->initialize(timeZone.c_str(), startTime);
#else
	// set default time
	CalDateTime startTime;
	struct tm startTm = {0};
	startTime.getTmFromUtime(&startTm);
	bool isToday = false;
	if (startTm.tm_year == (date.getYear() - 1900) &&
			startTm.tm_mon == (date.getMonth() - 1) &&
			startTm.tm_mday == date.getMday() ) {
		isToday = true;
	}
	startTm.tm_year = date.getYear() - 1900;
	startTm.tm_mon = date.getMonth() - 1;
	startTm.tm_mday = date.getMday();
	startTm.tm_min = 0;
	startTm.tm_sec = 0;
	if (isToday == false) {
		// If not Today, 08:00.
		startTm.tm_hour = 8;
		startTime.set(startTm);
	} else {
		// If Today, current hour + 1
		startTime.set(startTm);
		startTime.addHours(1);
	}

	schedule->initialize(NULL, startTime);
#endif

	return schedule;
}

std::shared_ptr<CalSchedule> CalSchedule::makeDefaultSchedule(const CalDateTime& startTime, const CalDateTime& endTime)
{
	WENTER();
	std::shared_ptr<CalSchedule> schedule = __createDefaultSchedule();
	schedule->initialize(NULL, startTime);
	schedule->setEnd(endTime);
	return schedule;
}

std::shared_ptr<CalSchedule> CalSchedule::makeDefaultSchedule(const CalDateTime& startTime)
{
	WENTER();
	std::shared_ptr<CalSchedule> schedule = __createDefaultSchedule();
	schedule->initialize(NULL, startTime);
	return schedule;
}

char* CalSchedule::getExceptionString() const
{
	CalDateTime start;
	getStart(start);

	if (getType() == CalSchedule::INSTANCE_ALLDAY)
		return g_strdup_printf("%04d%02d%02dT000000",
				start.getYear(), start.getMonth(), start.getMday());
	else
		return g_strdup_printf("%04d%02d%02dT%02d%02d%02dZ",
				start.getYear(TIMEZONE_ETC_GMT), start.getMonth(TIMEZONE_ETC_GMT), start.getMday(TIMEZONE_ETC_GMT),
				start.getHour(TIMEZONE_ETC_GMT), start.getMinute(TIMEZONE_ETC_GMT), start.getSecond(TIMEZONE_ETC_GMT));
}

std::shared_ptr<CalSchedule> CalSchedule::__createDefaultSchedule()
{
	calendar_record_h record;
	calendar_record_create(_calendar_event._uri, &record);
	std::shared_ptr<CalSchedule> schedule(std::make_shared<CalOriginalSchedule>(record));
	std::string tz;
	CalSettingsManager::getInstance().getCalendarTimeZone(tz);
	schedule->setTimeZone(tz.c_str());

	return schedule;
}

calendar_time_s CalSchedule::getCaltime(const CalDateTime& datetime)
{
	calendar_time_s caltime;
	if (datetime.isAllDay()) {
		caltime.type = CALENDAR_TIME_LOCALTIME;
		caltime.time.date.year = datetime.getYear();
		caltime.time.date.month = datetime.getMonth();
		caltime.time.date.mday = datetime.getMday();
		caltime.time.date.hour = 0;
		caltime.time.date.minute = 0;
		caltime.time.date.second = 0;
		caltime.time.date.is_leap_month = false;
	} else {
		caltime.type = CALENDAR_TIME_UTIME;
		caltime.time.utime = datetime.getUtimeFromTm();
	}
	return caltime;
}

int CalSchedule::getOriginalEventId() const
{
	int originalEventId = 0;
	calendar_record_get_int(getRecord(), getOriginalEventIdProperty(), &originalEventId);
	return originalEventId;
}

void CalSchedule::setOriginalEventId(int originalEventId)
{
	int error = calendar_record_set_int(getRecord(), getOriginalEventIdProperty(), originalEventId);
	WPRET_M(error != CALENDAR_ERROR_NONE, "calendar_record_set_int has failed(%d)", error);
}

void CalSchedule::setRecurranceId(const char* recurranceId)
{
	int error = calendar_record_set_str(getRecord(), getRecurranceIdProperty(), recurranceId);
	WPRET_M(error != CALENDAR_ERROR_NONE, "calendar_record_set_str has failed(%d)", error);
}

char* CalSchedule::getRecurranceId()
{
	char *value = NULL;
	int error = calendar_record_get_str(getRecord(), getRecurranceIdProperty(), &value);
	if (error != CALENDAR_ERROR_NONE)
		WERROR("calendar_record_get_str has failed(%d)");
	return value;
}

const char* CalSchedule::__getTimeZone() const
{
	const unsigned int propertyId = getStartTimezoneProperty();
	if (propertyId == 0)
		return NULL;
	char *tmp = NULL;
	int error = calendar_record_get_str_p(__record, propertyId, &tmp);
	WPRET_VM((error != CALENDAR_ERROR_NONE),NULL,"get_str fail");
	return tmp;
}

void CalSchedule::__getStart(CalDateTime& startTime) const
{
	calendar_time_s start;
	int error = calendar_record_get_caltime(__record, getStartProperty(), &start);
	WPRET_M((error != CALENDAR_ERROR_NONE),"get_caltime fail");

	if (start.type == CALENDAR_TIME_UTIME)
		startTime.set(start.time.utime);
	else
		startTime.set(start.time.date.year,start.time.date.month,start.time.date.mday);
}

void CalSchedule::__getEnd(CalDateTime& endTime, bool isAdjustive) const
{
	int error = 0;
	calendar_time_s end;
	error = calendar_record_get_caltime(__record, getEndProperty(), &end);
	WPRET_M((error != CALENDAR_ERROR_NONE),"get_caltime fail");

	if (end.type == CALENDAR_TIME_UTIME) {
		if (isAdjustive)
			endTime.set(end.time.utime - 1); // For 1/1 00:00 ~ 1/2 00:00, adjustive end time is 1/1 23:59
		else
			endTime.set(end.time.utime);
	} else {
		calendar_time_s start;
		error = calendar_record_get_caltime(__record, getStartProperty(), &start);
		WPRET_M((error != CALENDAR_ERROR_NONE),"get_caltime fail");

		WDEBUG("%d/%d/%d", start.time.date.year, start.time.date.month, start.time.date.mday);
		WDEBUG("%d/%d/%d", end.time.date.year, end.time.date.month, end.time.date.mday);
		WASSERT(__getDateCompareVal(start) <= __getDateCompareVal(end));

		endTime.set(end.time.date.year, end.time.date.month, end.time.date.mday);
		if (__getDateCompareVal(start) < __getDateCompareVal(end))
			endTime.addDays(-1);
	}

}

void CalSchedule::__removeDuplicateReminder(void) const
{
	const calendar_record_h record = getRecord();
	int error = 0;
	unsigned int i = 0, j = 0;
	unsigned int alarmCount = 0;
	error = calendar_record_get_child_record_count(record, _calendar_event.calendar_alarm, &alarmCount);
	if (alarmCount <= 1)
		return;
	for (i=0;i<(alarmCount-1);i++)
	{
		calendar_record_h reminder = NULL;
		int tick = 0, tickUnit = 0;
		error = calendar_record_get_child_record_at_p(record,_calendar_event.calendar_alarm, i, &reminder);
		if ((error!=CALENDAR_ERROR_NONE)) {
			WERROR("fail");
			continue;
		}
		error = calendar_record_get_int(reminder, _calendar_alarm.tick, &tick);
		if (error != CALENDAR_ERROR_NONE)
		{
			WERROR("fail");
		}
		error = calendar_record_get_int(reminder, _calendar_alarm.tick_unit, &tickUnit);
		if (error != CALENDAR_ERROR_NONE)
		{
			WERROR("fail");
		}
		for (j = i+1;j<alarmCount;j++)
		{
			calendar_record_h reminder2 = NULL;
			int tick2 = 0, tickUnit2 = 0;
			error = calendar_record_get_child_record_at_p(record,_calendar_event.calendar_alarm, j, &reminder2);
			if ((error!=CALENDAR_ERROR_NONE)) {
				WERROR("fail");
				continue;
			}
			error = calendar_record_get_int(reminder2, _calendar_alarm.tick, &tick2);
			if (error != CALENDAR_ERROR_NONE)
			{
				WERROR("fail");
			}
			error = calendar_record_get_int(reminder2, _calendar_alarm.tick_unit, &tickUnit2);
			if (error != CALENDAR_ERROR_NONE)
			{
				WERROR("fail");
			}
			if (tick == tick2 && tickUnit == tickUnit2)
			{
				error = calendar_record_remove_child_record(record,_calendar_event.calendar_alarm, reminder2);
				if ((error!=CALENDAR_ERROR_NONE)) {
					WERROR("fail");
					continue;
				}
				WDEBUG("remove duplicate reminder(%d --)",alarmCount);
				alarmCount--;
				error = calendar_record_destroy(reminder2, true);
				if (error != CALENDAR_ERROR_NONE)
				{
					WERROR("fail");
				}
				break;
			}
		}
	}
}
