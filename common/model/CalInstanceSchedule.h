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
	virtual ~CalInstanceSchedule();

	CalInstanceSchedule(const CalInstanceSchedule&);
	const CalInstanceSchedule& operator=(const CalInstanceSchedule&);

protected:
	virtual unsigned int getIndexProperty(void) const;
	virtual unsigned int getStartProperty(void) const;
	virtual unsigned int getEndProperty(void) const;
	virtual unsigned int getSummaryProperty(void) const;
	virtual unsigned int getLocationProperty(void) const;
	virtual unsigned int getDescriptionProperty(void) const;
	virtual unsigned int getBookIdProperty(void) const;
	virtual unsigned int getHasReminderProperty(void) const;
	virtual unsigned int getHasRepeatProperty(void) const;
	virtual unsigned int getStartTimezoneProperty(void) const;
	virtual unsigned int getEndTimezoneProperty(void) const;
	virtual unsigned int getSyncData1Property(void) const;
	virtual unsigned int getOriginalEventIdProperty() const;
	virtual unsigned int getRecurranceIdProperty() const;
	virtual unsigned int getUidProperty() const;
	virtual unsigned int getSyncData2Property() const;

protected:
	static calendar_query_h createSimpleListQuery(const CalDateTime& base, int dir, bool allDay, calendar_filter_h additionalFilter);
	static calendar_query_h createRangeListQuery(const CalDateTime& from, const CalDateTime& to, bool allDay, calendar_filter_h additionalFilter);
	static calendar_query_h createComplexListQuery(const CalDateTime& base, int dir, bool allDay, calendar_filter_h additionalFilter);
	static calendar_filter_h createListFilter(bool allDay,
			bool visibleCalendarsOnly,
			bool editableOnly,
			const char* keyword);
friend class CalSimpleListProvider;
friend class CalComplexListProvider;
friend class CalListModelFactory;
};

#endif
