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

#ifndef _CAL_SCHEDULE_H_
#define _CAL_SCHEDULE_H_

#include <list>
#include <calendar.h>
#include <string>
#include <memory>
#include "CalDate.h"
#include "CalDateTime.h"
#include "CalScheduleReminder.h"
#include "CalScheduleRepeat.h"

#define CAL_SCHEDULE_REMINDERS_MAX 5
class WAPP_ASSIST_EXPORT CalSchedule
{
public:
	enum Type
	{
		INVALID = -1,
		ORIGINAL = 0, // _calendar_event
		INSTANCE_NORMAL, // _calendar_instance_utime_calendar_book_extended
		INSTANCE_ALLDAY, // _calendar_instance_localtime_calendar_book_extended
	};
protected:
	CalSchedule();
	CalSchedule(calendar_record_h record);
	virtual ~CalSchedule();

	CalSchedule(const CalSchedule&);
	const CalSchedule& operator=(const CalSchedule&);

public:
	// get
	Type getType() const;
	int getIndex() const;
	void getFromToDisplayString(std::string& text) const;  // for detail marupStr ..
	void getFromToString(std::string& text) const;
	void getFromToString(const CalDate& date, std::string& text) const;  // for list

	void getDisplaySummary(const int stringLimit, std::string& text) const; //text	markupStr (elm_entry_utf8_to_markup)
	void getDisplayTrimedSummary(const int stringLimit, std::string& text);
	const char* getSummary() const;
	void getDisplayLocation(const int stringLimit, std::string& text) const; //text	markupStr (elm_entry_utf8_to_markup)
	void getDisplayTrimedLocation(const int stringLimit, std::string& text);
	const char* getLocation() const;
	int getBookId() const;
	void getStart(CalDateTime& startTime) const;
	void getEnd(CalDateTime& endTime, bool isAdjustive = false) const;
	void getEndForComplexList(CalDateTime& endTime) const;
	void getDisplayDescription(std::string& text) const; //text	markupStr (elm_entry_utf8_to_markup)
	const char* getDescription() const;
	const char* getTimeZone() const;
	const char* getUid() const;

	// set
	void setSummary(const char* summary);
	void setLocation(const char* location);
	virtual void setStart(const CalDateTime& startTime);
	virtual void setEnd(const CalDateTime& endTime);
	void setDescription(const char* description);
	void setTimeZone(const char* timezone);
	void setBookId(const int bookId);
	void setUid(const char* uid);

	// reminders
	virtual const bool hasReminder() const;
	virtual const int getRemindersCount() const;
	virtual void getReminder(const int nth, CalScheduleReminder& reminder);
	virtual void setReminder(const int nth, const CalScheduleReminder& reminder);
	virtual int addReminder(const CalScheduleReminder& reminder);
	virtual void removeReminder(const int nth);

	// repeat
	virtual const bool hasRepeat() const;
	virtual const CalScheduleRepeat& getRepeat() const;
	virtual void setRepeat(const CalScheduleRepeat& repeat);

	virtual void initialize(const char* tzid, const CalDateTime& startTime);

	bool isException() const {return getOriginalEventId() >= 0;}

	// make
	static std::shared_ptr<CalSchedule> makeDefaultSchedule(bool isAllDay = false);
	static std::shared_ptr<CalSchedule> makeDefaultSchedule(const CalDate& date);
	static std::shared_ptr<CalSchedule> makeDefaultSchedule(const CalDateTime& startTime, const CalDateTime& endTime);
	static std::shared_ptr<CalSchedule> makeDefaultSchedule(const CalDateTime& startTime);

protected:
	calendar_record_h getRecord() const;
	static calendar_time_s getCaltime(const CalDateTime& startTime);
	int getOriginalEventId() const;
	void setOriginalEventId(int originalEventId);
	void setRecurranceId(const char* recurranceId);
	char* getRecurranceId();
	void checkInsertForSync() const;
	void checkUpdateForSync() const;

protected:
	virtual unsigned int getIndexProperty(void) const  = 0;
	virtual unsigned int getStartProperty(void) const = 0;
	virtual unsigned int getEndProperty(void) const = 0;
	virtual unsigned int getSummaryProperty(void) const = 0;
	virtual unsigned int getLocationProperty(void) const = 0;
	virtual unsigned int getDescriptionProperty(void) const = 0;
	virtual unsigned int getBookIdProperty(void) const = 0;
	virtual unsigned int getHasReminderProperty(void) const = 0;
	virtual unsigned int getHasRepeatProperty(void) const = 0;
	virtual unsigned int getStartTimezoneProperty(void) const = 0;
	virtual unsigned int getEndTimezoneProperty(void) const = 0;
	virtual unsigned int getSyncData1Property(void) const = 0;
	virtual unsigned int getOriginalEventIdProperty() const = 0;
	virtual unsigned int getRecurranceIdProperty() const = 0;
	virtual unsigned int getUidProperty() const = 0;
	virtual unsigned int getSyncData2Property() const = 0;

protected:
	virtual char* getExceptionString() const;

private:
	static std::shared_ptr<CalSchedule> __createDefaultSchedule();
	void __getFromToString(const char* timezone, const CalDateTime& start, const CalDateTime& end, std::string& text) const;
	void __getFromToDisplayString(const char* timezone, const CalDateTime& start, const CalDateTime& end, std::string& text) const;
	void __trimLeft(std::string& str) const;
	void __trimRight(std::string& str) const;
	void __trimNewLine(char* str) const;
	void __checkSummary(const int stringLimit, int len, std::string& text) const;
	void __checkLocation(const int stringLimit, int len, std::string& text) const;
	const char* __getTimeZone() const;
	void __getStart(CalDateTime& startTime) const;
	void __getEnd(CalDateTime& endTime, bool isAdjustive = false) const;
	void __removeDuplicateReminder(void) const;
protected:
	calendar_record_h __record;
	Type __type;

private:
	char* __trimedSummary;
	char* __trimedLocation;

friend class CalDataManager;
};

#endif /* _CAL_SCHEDULE_H_ */
