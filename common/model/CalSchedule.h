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

/**
 * @class CalSchedule
 * @brief The %CalSchedule class is a base class of event.
 */
class WAPP_ASSIST_EXPORT CalSchedule
{
public:

	/**
	 * @brief Schedule object type
	 */
	enum Type
	{
		INVALID = -1,    /**  Bad type                                            */
		ORIGINAL = 0,    /** _calendar_event                                      */
		INSTANCE_NORMAL, /** _calendar_instance_utime_calendar_book_extended      */
		INSTANCE_ALLDAY, /** _calendar_instance_localtime_calendar_book_extended  */
	};

	/**
	 * @brief Get object type
	 *
	 * @return object type, @see Type.
	 *
	 */
	Type getType() const;

	/**
	 * @brief Get event ID.
	 *
	 * @return event ID.
	 *
	 */
	int getIndex() const;

	/**
	 * @brief Get start and end time as text.
	 *
	 * @param[out] text        text buffer
	 *
	 */
	void getFromToDisplayString(std::string& text) const;

	/**
	 * @brief Get start and end time as text.
	 *
	 * @param[out] text        text buffer
	 *
	 */
	void getFromToString(std::string& text) const;

	/**
	 * @brief Get start and end time as text.
	 *
	 * @param[in]  date        date
	 * @param[out] text        text buffer
	 */
	void getFromToString(const CalDate& date, std::string& text) const;

	/**
	 * @brief Get event title.
	 *
	 * @param[in]  stringLimit   buffer size
	 * @param[out] text          text buffer
	 *
	 */
	void getDisplaySummary(const int stringLimit, std::string& text) const;

	/**
	 * @brief Get event title as one line.
	 *
	 * @param[in]  stringLimit   buffer size
	 * @param[out] text          text buffer
	 *
	 */
	void getDisplayTrimedSummary(const int stringLimit, std::string& text);

	/**
	 * @brief Get event title.
	 *
	 * @return event title.
	 *
	 */
	const char* getSummary() const;

	/**
	 * @brief Get event location as text.
	 *
	 * @param[in]  stringLimit   buffer size
	 * @param[out] text          text buffer
	 *
	 */
	void getDisplayLocation(const int stringLimit, std::string& text) const;

	/**
	 * @brief Get event location as one line.
	 *
	 * @param[in]  stringLimit   buffer size
	 * @param[out] text          text buffer
	 *
	 */
	void getDisplayTrimedLocation(const int stringLimit, std::string& text);

	/**
	 * @brief Get event location.
	 *
	 * @return event location.
	 *
	 */
	const char* getLocation() const;

	/**
	 * @brief Get event calendar ID.
	 *
	 * @return calendar ID.
	 *
	 */
	int getBookId() const;

	/**
	 * @brief Get event start time.
	 *
	 * @param[out] startTime       event start time
	 *
	 */
	void getStart(CalDateTime& startTime) const;

	/**
	 * @brief Get event end time.
	 *
	 * @param[out]  endTime       event end time
	 * @param[in]   isAdjustive
	 *
	 */
	void getEnd(CalDateTime& endTime, bool isAdjustive = false) const;

	/**
	 * @brief Get end time for complex list model.
	 *
	 * @param[out]  endTime        end time for complex list model
	 *
	 */
	void getEndForComplexList(CalDateTime& endTime) const;

	/**
	 * @brief Get event description.
	 *
	 * @param[out] text     event description
	 *
	 */
	void getDisplayDescription(std::string& text) const;

	/**
	 * @brief Get event description.
	 *
	 * @return event description.
	 *
	 */
	const char* getDescription() const;

	/**
	 * @brief Get event time zone.
	 *
	 * @return event time zone.
	 *
	 */
	const char* getTimeZone() const;

	/**
	 * @brief Get event UID.
	 *
	 * @return event UID.
	 *
	 */
	const char* getUid() const;

	/**
	 * @brief Set event title.
	 *
	 * @param summary      event title
	 *
	 */
	void setSummary(const char* summary);

	/**
	 * @brief Set event location.
	 *
	 * @param location           event location
	 *
	 */
	void setLocation(const char* location);

	/**
	 * @brief Set start time.
	 *
	 * @param startTime      event start time
	 *
	 */
	virtual void setStart(const CalDateTime& startTime);

	/**
	 * @brief Set end time.
	 *
	 * @param endTime      event end time
	 *
	 */
	virtual void setEnd(const CalDateTime& endTime);

	/**
	 * @brief Set event description.
	 *
	 * @param description       event description
	 *
	 */
	void setDescription(const char* description);

	/**
	 * @brief Set event time zone.
	 *
	 * @param timezone       event time zone
	 *
	 */
	void setTimeZone(const char* timezone);

	/**
	 * @brief Set event Calendar ID.
	 *
	 * @param bookId       Calendar ID
	 *
	 */
	void setBookId(const int bookId);

	/**
	 * @brief Set event UID
	 *
	 * @param UID           event UID.
	 *
	 */
	void setUid(const char* uid);

	/**
	 * @brief Check whether event has remainder reminders.
	 *
	 * @return true if has, otherwise false.
	 *
	 */
	virtual const bool hasReminder() const;

	/**
	 * @brief Get event reminders count.
	 *
	 * @return event reminders count
	 *
	 */
	virtual const int getRemindersCount() const;

	/**
	 * @brief Get event reminder by index.
	 *
	 * @param[in]  nth        reminder index
	 * @param[out] reminder   reminder
	 *
	 */
	virtual void getReminder(const int nth, CalScheduleReminder& reminder);

	/**
	 * @brief Set reminder to event by index.
	 *
	 * @param[in]  nth        reminder index
	 * @param[in]  reminder   reminder
	 */
	virtual void setReminder(const int nth, const CalScheduleReminder& reminder);

	/**
	 * @brief Add reminder to event.
	 *
	 * @param reminder        reminder to add
	 *
	 * @return operation result.
	 *
	 */
	virtual int addReminder(const CalScheduleReminder& reminder);

	/**
	 * @brief Remove reminder by index.
	 *
	 * @param[in]  nth        reminder index
	 */
	virtual void removeReminder(const int nth);

	/**
	 * @brief Check whether event has repeat rules.
	 *
	 * @return true if has, otherwise false.
	 *
	 */
	virtual const bool hasRepeat() const;

	/**
	 * @brief Get event repeat rule.
	 *
	 * @return event repeat rule.
	 *
	 */
	virtual const CalScheduleRepeat& getRepeat() const;

	/**
	 * @brief Set repeat rule to event.
	 *
	 * @param repeat       event repeat rule
	 *
	 */
	virtual void setRepeat(const CalScheduleRepeat& repeat);

	/**
	 * @brief Initialization.
	 *
	 * @param[in] tzid        time zone
	 * @param[in] startTime   event start time
	 *
	 */
	virtual void initialize(const char* tzid, const CalDateTime& startTime);

	/**
	 * @brief Check whether this instance is exception.
	 *
	 * @return true if exception, otherwise false.
	 *
	 */
	bool isException() const { return getOriginalEventId() >= 0; }

	/**
	 * @brief Check whether event is all day.
	 *
	 * @return true if all day, otherwise false.
	 *
	 */
	bool isAllDay() const;

	/**
	 * @brief Create new event instance.
	 *
	 * @param isAllDay         all day event flag.
	 *
	 * @return event instance.
	 *
	 */
	static std::shared_ptr<CalSchedule> makeDefaultSchedule(bool isAllDay = false);

	/**
	 * @brief Create new event instance.
	 *
	 * @param[in] date       start date
	 *
	 * @return event instance.
	 *
	 */
	static std::shared_ptr<CalSchedule> makeDefaultSchedule(const CalDate& date);

	/**
	 * @brief Create new event instance.
	 *
	 * @param[in] startTime       start date/time
	 * @param[in] endTime         end date/time
	 *
	 * @return event instance.
	 *
	 */
	static std::shared_ptr<CalSchedule> makeDefaultSchedule(const CalDateTime& startTime, const CalDateTime& endTime);

	/**
	 * @brief Create new event instance.
	 *
	 * @param[in] startTime       start date/time
	 *
	 * @return event instance.
	 *
	 */
	static std::shared_ptr<CalSchedule> makeDefaultSchedule(const CalDateTime& startTime);

protected:
	CalSchedule();
	CalSchedule(calendar_record_h record);
	virtual ~CalSchedule();

	CalSchedule(const CalSchedule&);
	const CalSchedule& operator=(const CalSchedule&);

	calendar_record_h getRecord() const;
	static calendar_time_s getCaltime(const CalDateTime& startTime);
	int getOriginalEventId() const;
	void setOriginalEventId(int originalEventId);
	void setRecurranceId(const char* recurranceId);
	char* getRecurranceId();
	void checkInsertForSync() const;
	void checkUpdateForSync() const;

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
	virtual char* getExceptionString() const;

	calendar_record_h __record;
	Type __type;

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

	char* __trimedSummary;
	char* __trimedLocation;

	friend class CalDataManager;
};

#endif /* _CAL_SCHEDULE_H_ */
