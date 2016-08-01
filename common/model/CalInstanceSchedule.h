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

#ifndef _CAL_INSTANCE_SCHEDULE_H_
#define _CAL_INSTANCE_SCHEDULE_H_

#include <calendar.h>
#include "CalSchedule.h"

class WAPP_ASSIST_EXPORT CalInstanceSchedule: public CalSchedule
{
public:
	CalInstanceSchedule(calendar_record_h record);
	CalInstanceSchedule(calendar_record_h record, CalSchedule::Type type);
	CalInstanceSchedule(const CalInstanceSchedule&);
	const CalInstanceSchedule& operator=(const CalInstanceSchedule&);

protected:

	/**
	 * @brief Get property index of DB record ID of the event.
	 *
	 * @return property index.
	 *
	 */
	virtual unsigned int getIndexProperty(void) const;

	/**
	 * @brief Get property index of start time of the event.
	 *
	 * @return property index.
	 *
	 */
	virtual unsigned int getStartProperty(void) const;

	/**
	 * @brief Get property index of end time of the event.
	 *
	 * @return property index.
	 *
	 */
	virtual unsigned int getEndProperty(void) const;

	/**
	 * @brief Get property index of title of the event.
	 *
	 * @return property index.
	 *
	 */
	virtual unsigned int getSummaryProperty(void) const;

	/**
	 * @brief Get property index of location of the event.
	 *
	 * @return property index.
	 *
	 */
	virtual unsigned int getLocationProperty(void) const;

	/**
	 * @brief Get property index of description of the event.
	 *
	 * @return property index.
	 *
	 */
	virtual unsigned int getDescriptionProperty(void) const;

	/**
	 * @brief Get property index of book ID of the event.
	 *
	 * @return property index.
	 *
	 */
	virtual unsigned int getBookIdProperty(void) const;

	/**
	 * @brief Get property index whether or not the event has an alarm list.
	 *
	 * @return property index.
	 *
	 */
	virtual unsigned int getHasReminderProperty(void) const;

	/**
	 * @brief Get property index whether or not the event has recurrence rule.
	 *
	 * @return property index.
	 *
	 */
	virtual unsigned int getHasRepeatProperty(void) const;

	/**
	 * @brief Get property index of start time timezone of the event.
	 *
	 * @return property index.
	 *
	 */
	virtual unsigned int getStartTimezoneProperty(void) const;

	/**
	 * @brief Get property index of end time timezone of the event.
	 *
	 * @return property index.
	 *
	 */
	virtual unsigned int getEndTimezoneProperty(void) const;

	/**
	 * @brief Get property index of sync data #1 of the event.
	 *
	 * @return property index.
	 *
	 */
	virtual unsigned int getSyncData1Property(void) const;

	/**
	 * @brief Get property index of ID of the original event if the event is an exception.
	 *
	 * @return property index.
	 *
	 */
	virtual unsigned int getOriginalEventIdProperty() const;

	/**
	 * @brief Get property index of recurrance ID, @see RFC #2445, 4.8.4.4 Recurrence ID.
	 *
	 * @return property index.
	 *
	 */
	virtual unsigned int getRecurranceIdProperty() const;

	/**
	 * @brief Get property index of unique ID of the event.
	 *
	 * @return property index.
	 *
	 */
	virtual unsigned int getUidProperty() const;

	/**
	 * @brief Get property index of sync data #2 of the event.
	 *
	 * @return property index.
	 *
	 */
	virtual unsigned int getSyncData2Property() const;

	/**
	 * @brief Create calendar-service query for simple list model, @see CalSimpleListProvider.
	 *
	 * @param base                focus date
	 * @param dir                 direction of fetching
	 * @param allDay              fetch only allDay or not
	 * @param additionalFilter    additional filter
	 *
	 * @return calendar-service query.
	 *
	 */
	static calendar_query_h createSimpleListQuery(const CalDate& base, int dir, bool allDay, calendar_filter_h additionalFilter);

	/**
	 * @brief Create calendar-service query for range list model, @see CalSimpleListProvider.
	 *
	 * @param from                  from date
	 * @param to                    to date
	 * @param allDay                fetch only allDay or not
	 * @param additionalFilter      additional filter
	 *
	 * @return calendar-service query.
	 *
	 */
	static calendar_query_h createRangeListQuery(const CalDate& from, const CalDate& to, bool allDay, calendar_filter_h additionalFilter);

	/**
	 * @brief Create calendar-service query for complex list model, @see CalComplexListProvider.
	 *
	 * @param base                focus date
	 * @param dir                 direction of fetching
	 * @param allDay              fetch only allDay or not
	 * @param additionalFilter    additional filter
	 *
	 * @return calendar-service query.
	 *
	 */
	static calendar_query_h createComplexListQuery(const CalDate& base, int dir, bool allDay, calendar_filter_h additionalFilter);

	/**
	 * @brief Create calendar-service filter.
	 *
	 * @param allDay                  fetch only allDay or not
	 * @param visibleCalendarsOnly    fetch only visible or not
	 * @param editableOnly            fetch only editable or not
	 * @param keyword                 match keyword
	 *
	 * @return calendar-service filter.
	 */
	static calendar_filter_h createListFilter(bool allDay, bool visibleCalendarsOnly, bool editableOnly, const char* keyword);

	friend class CalSimpleListProvider;
	friend class CalComplexListProvider;
	friend class CalListModelFactory;
};

#endif
