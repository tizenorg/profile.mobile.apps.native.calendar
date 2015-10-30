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

#include <glib.h>
#include "CalInstanceSchedule.h"
#include "CalCommon.h"

CalInstanceSchedule::CalInstanceSchedule(calendar_record_h record) :
	CalSchedule(record)
{
	char *view_uri = NULL;
	int error = CALENDAR_ERROR_NONE;
	error =  calendar_record_get_uri_p(record, &view_uri);
	if (error == CALENDAR_ERROR_NONE)
	{
		if(g_strcmp0(view_uri, _calendar_instance_utime_calendar_book_extended._uri) == 0)
		{
			__type = CalSchedule::INSTANCE_NORMAL;
		}
		else if(g_strcmp0(view_uri, _calendar_instance_localtime_calendar_book_extended._uri) == 0)
		{
			__type = CalSchedule::INSTANCE_ALLDAY;
		}
	}
	else
	{
		WASSERT(0);
	}

//	CalDateTime start;
//	CalDateTime end;
//
//	getStart(start);
//	getEnd(end);
//
//	WDEBUG("%d/%d/%d %d/%d/%d",
//		start.getYear(), start.getMonth(), start.getMday(),
//		end.getYear(), end.getMonth(), end.getMday());
}

CalInstanceSchedule::CalInstanceSchedule(calendar_record_h record, CalSchedule::Type type) :
	CalSchedule(record)
{
	__type = type;
}

CalInstanceSchedule::~CalInstanceSchedule()
{
}

CalInstanceSchedule::CalInstanceSchedule(const CalInstanceSchedule& obj) :
		CalSchedule(obj)
{
}

const CalInstanceSchedule& CalInstanceSchedule::operator=(const CalInstanceSchedule& obj)
{
	CalSchedule::operator=(obj);
	//if (this != &obj)
	//{
	//}
	return *this;
}

unsigned int CalInstanceSchedule::getIndexProperty(void) const
{
	if (__type == CalSchedule::INSTANCE_NORMAL)
		return _calendar_instance_utime_calendar_book_extended.event_id;
	else if (__type == CalSchedule::INSTANCE_ALLDAY)
		return _calendar_instance_localtime_calendar_book_extended.event_id;
	WASSERT(0);
	return 0;
}

unsigned int CalInstanceSchedule::getStartProperty(void) const
{
	if (__type == CalSchedule::INSTANCE_NORMAL)
		return _calendar_instance_utime_calendar_book_extended.start_time;
	else if (__type == CalSchedule::INSTANCE_ALLDAY)
		return _calendar_instance_localtime_calendar_book_extended.start_time;
	WASSERT(0);
	return 0;
}

unsigned int CalInstanceSchedule::getEndProperty(void) const
{
	if (__type == CalSchedule::INSTANCE_NORMAL)
		return _calendar_instance_utime_calendar_book_extended.end_time;
	else if (__type == CalSchedule::INSTANCE_ALLDAY)
		return _calendar_instance_localtime_calendar_book_extended.end_time;
	WASSERT(0);
	return 0;
}

unsigned int CalInstanceSchedule::getSummaryProperty(void) const
{
	if (__type == CalSchedule::INSTANCE_NORMAL)
		return _calendar_instance_utime_calendar_book_extended.summary;
	else if (__type == CalSchedule::INSTANCE_ALLDAY)
		return _calendar_instance_localtime_calendar_book_extended.summary;
	WASSERT(0);
	return 0;
}

unsigned int CalInstanceSchedule::getLocationProperty(void) const
{
	if (__type == CalSchedule::INSTANCE_NORMAL)
		return _calendar_instance_utime_calendar_book_extended.location;
	else if (__type == CalSchedule::INSTANCE_ALLDAY)
		return _calendar_instance_localtime_calendar_book_extended.location;
	WASSERT(0);
	return 0;
}

unsigned int CalInstanceSchedule::getDescriptionProperty(void) const
{
	if (__type == CalSchedule::INSTANCE_NORMAL)
		return _calendar_instance_utime_calendar_book_extended.description;
	else if (__type == CalSchedule::INSTANCE_ALLDAY)
		return _calendar_instance_localtime_calendar_book_extended.description;
	WASSERT(0);
	return 0;
}

unsigned int CalInstanceSchedule::getBookIdProperty(void) const
{
	if (__type == CalSchedule::INSTANCE_NORMAL)
		return _calendar_instance_utime_calendar_book_extended.calendar_book_id;
	else if (__type == CalSchedule::INSTANCE_ALLDAY)
		return _calendar_instance_localtime_calendar_book_extended.calendar_book_id;
	WASSERT(0);
	return 0;
}

unsigned int CalInstanceSchedule::getHasReminderProperty(void) const
{
	if (__type == CalSchedule::INSTANCE_NORMAL)
		return _calendar_instance_utime_calendar_book_extended.has_alarm;
	else if (__type == CalSchedule::INSTANCE_ALLDAY)
		return _calendar_instance_localtime_calendar_book_extended.has_alarm;
	WASSERT(0);
	return 0;
}

unsigned int CalInstanceSchedule::getHasRepeatProperty(void) const
{
	if (__type == CalSchedule::INSTANCE_NORMAL)
		return _calendar_instance_utime_calendar_book_extended.has_rrule;
	else if (__type == CalSchedule::INSTANCE_ALLDAY)
		return _calendar_instance_localtime_calendar_book_extended.has_rrule;
	WASSERT(0);
	return 0;
}

unsigned int CalInstanceSchedule::getStartTimezoneProperty(void) const
{
	return 0;
}

unsigned int CalInstanceSchedule::getEndTimezoneProperty(void) const
{
	return 0;
}

unsigned int CalInstanceSchedule::getUidProperty() const
{
	return 0;
}

unsigned int CalInstanceSchedule::getSyncData2Property() const
{
	if (__type == CalSchedule::INSTANCE_NORMAL)
		return _calendar_instance_utime_calendar_book_extended.sync_data2;
	else if (__type == CalSchedule::INSTANCE_ALLDAY)
		return _calendar_instance_localtime_calendar_book_extended.sync_data2;
	WASSERT(0);
	return 0;
}

unsigned int CalInstanceSchedule::getSyncData1Property(void) const
{
	if (__type == CalSchedule::INSTANCE_NORMAL)
		return _calendar_instance_utime_calendar_book_extended.sync_data1;
	else if (__type == CalSchedule::INSTANCE_ALLDAY)
		return _calendar_instance_localtime_calendar_book_extended.sync_data1;
	WASSERT(0);
	return 0;
}

unsigned int CalInstanceSchedule::getOriginalEventIdProperty() const
{
	if (__type == CalSchedule::INSTANCE_NORMAL)
		return _calendar_instance_utime_calendar_book_extended.original_event_id;
	else if (__type == CalSchedule::INSTANCE_ALLDAY)
		return _calendar_instance_localtime_calendar_book_extended.original_event_id;
	WASSERT(0);
	return 0;
}

unsigned int CalInstanceSchedule::getRecurranceIdProperty() const
{
	WASSERT(0);
	return 0;
}

calendar_query_h CalInstanceSchedule::createSimpleListQuery(const CalDate& base, int dir, bool allDay, calendar_filter_h additionalFilter)
{
	const char* uri = allDay ?
			_calendar_instance_localtime_calendar_book_extended._uri :
			_calendar_instance_utime_calendar_book_extended._uri;

	const unsigned int startProperty = allDay ?
			_calendar_instance_localtime_calendar_book_extended.start_time :
			_calendar_instance_utime_calendar_book_extended.start_time;
	int error = CALENDAR_ERROR_NONE;
	calendar_query_h query = NULL;
	calendar_filter_h filter = NULL;

	do {
		error = calendar_query_create(uri, &query);
		if (error != CALENDAR_ERROR_NONE) break;

		error = calendar_filter_create(uri, &filter);
		if (error != CALENDAR_ERROR_NONE) break;

		const calendar_time_s baseTime = getCaltime(allDay ?
				CalDateTime(base.getYear(), base.getMonth(), base.getMday()) :
				CalDateTime(base.getYear(), base.getMonth(), base.getMday(), 0, 0, 0));

		if (dir > 0) {
			error = calendar_filter_add_caltime(filter, startProperty, CALENDAR_MATCH_GREATER_THAN_OR_EQUAL, baseTime);
			if (error != CALENDAR_ERROR_NONE) break;
		} else {
			error = calendar_filter_add_caltime(filter, startProperty, CALENDAR_MATCH_LESS_THAN, baseTime);
			if (error != CALENDAR_ERROR_NONE) break;
		}
		if (additionalFilter) {
			error = calendar_filter_add_operator(filter, CALENDAR_FILTER_OPERATOR_AND);
 			if (error != CALENDAR_ERROR_NONE) break;
			calendar_filter_add_filter(filter, additionalFilter);
			calendar_filter_destroy(additionalFilter);
		}
	} while(0);


	if (error != CALENDAR_ERROR_NONE) {
		if (query) {
			calendar_query_destroy(query);
			query = NULL;
		}
		if (filter)
			calendar_filter_destroy(filter);
	} else {
		calendar_query_set_filter(query, filter);
		calendar_filter_destroy(filter);
		calendar_query_set_sort(query, startProperty, dir > 0);
	}

	return query;
}

calendar_query_h CalInstanceSchedule::createRangeListQuery(const CalDate& from, const CalDate& to, bool allDay, calendar_filter_h additionalFilter)
{
	const char* uri = allDay ?
			_calendar_instance_localtime_calendar_book_extended._uri :
			_calendar_instance_utime_calendar_book_extended._uri;

	const unsigned int startProperty = allDay ?
			_calendar_instance_localtime_calendar_book_extended.start_time :
			_calendar_instance_utime_calendar_book_extended.start_time;

	const unsigned int endProperty = allDay ?
			_calendar_instance_localtime_calendar_book_extended.end_time :
			_calendar_instance_utime_calendar_book_extended.end_time;
	int error = CALENDAR_ERROR_NONE;
	calendar_query_h query = NULL;
	calendar_filter_h filter = NULL;
	do {
		error = calendar_query_create(uri, &query);
		if (error != CALENDAR_ERROR_NONE) break;

		error = calendar_filter_create(uri, &filter);
		if (error != CALENDAR_ERROR_NONE) break;

		const calendar_time_s fromTime = getCaltime(allDay ?
				CalDateTime(from.getYear(), from.getMonth(), from.getMday()) :
				CalDateTime(from.getYear(), from.getMonth(), from.getMday(), 0, 0, 0));

		const calendar_time_s toTime = getCaltime(allDay ?
				CalDateTime(to.getYear(), to.getMonth(), to.getMday()) :
				CalDateTime(to.getYear(), to.getMonth(), to.getMday(), 0, 0, 0));

		error = calendar_filter_add_caltime(filter, endProperty, CALENDAR_MATCH_GREATER_THAN, fromTime);
		if (error != CALENDAR_ERROR_NONE) break;
		error = calendar_filter_add_operator(filter, CALENDAR_FILTER_OPERATOR_AND);
		if (error != CALENDAR_ERROR_NONE) break;
		error = calendar_filter_add_caltime(filter, startProperty, CALENDAR_MATCH_LESS_THAN_OR_EQUAL, toTime);
		if (error != CALENDAR_ERROR_NONE) break;

		if (additionalFilter) {
			error = calendar_filter_add_operator(filter, CALENDAR_FILTER_OPERATOR_AND);
			if (error != CALENDAR_ERROR_NONE) break;
			calendar_filter_add_filter(filter, additionalFilter);
			calendar_filter_destroy(additionalFilter);
		}
	}while (0);


	if (error != CALENDAR_ERROR_NONE) {
		if (query) {
			calendar_query_destroy(query);
			query = NULL;
		}
		if (filter)
			calendar_filter_destroy(filter);
	} else {
		calendar_query_set_filter(query, filter);
		calendar_filter_destroy(filter);
		calendar_query_set_sort(query, startProperty, true);
	}

	return query;
}

calendar_query_h CalInstanceSchedule::createComplexListQuery(const CalDate& base, int dir, bool allDay, calendar_filter_h additionalFilter)
{
	const char* uri = allDay ?
			_calendar_instance_localtime_calendar_book_extended._uri :
			_calendar_instance_utime_calendar_book_extended._uri;

	const unsigned int startProperty = allDay ?
			_calendar_instance_localtime_calendar_book_extended.start_time :
			_calendar_instance_utime_calendar_book_extended.start_time;

	const unsigned int endProperty = allDay ?
			_calendar_instance_localtime_calendar_book_extended.end_time :
			_calendar_instance_utime_calendar_book_extended.end_time;
	int error = CALENDAR_ERROR_NONE;
	calendar_query_h query = NULL;
	calendar_filter_h filter = NULL;
	do {
		error = calendar_query_create(uri, &query);
		if (error != CALENDAR_ERROR_NONE) break;

		error = calendar_filter_create(uri, &filter);
		if (error != CALENDAR_ERROR_NONE) break;

		CalDateTime baseTime = allDay ?
				CalDateTime(base.getYear(), base.getMonth(), base.getMday()) :
				CalDateTime(base.getYear(), base.getMonth(), base.getMday(), 0, 0, 0);

		WDEBUG("base(%s), dir(%d), allDay(%d)", base.dump().c_str(), dir, allDay);

		if (dir > 0) {
			if (allDay) {
				calendar_time_s low = getCaltime(CalDateTime(1901,12,30));
	//			calendar_filter_add_caltime(filter, endProperty, CALENDAR_MATCH_GREATER_THAN, getCaltime(baseTime));
				baseTime.addDays(1);
				error = calendar_filter_add_caltime(filter, endProperty, CALENDAR_MATCH_GREATER_THAN_OR_EQUAL, getCaltime(baseTime));
				if (error != CALENDAR_ERROR_NONE) break;

				error = calendar_filter_add_operator(filter, CALENDAR_FILTER_OPERATOR_AND);
				if (error != CALENDAR_ERROR_NONE) break;

				error = calendar_filter_add_caltime(filter, startProperty, CALENDAR_MATCH_GREATER_THAN, low);
				if (error != CALENDAR_ERROR_NONE) break;
			} else {
				calendar_time_s low = getCaltime(CalDateTime(1901,12,30,0,0,0));
				// Note: the equality is included for events which both start time and end time is 00:00 of base date.
				error = calendar_filter_add_caltime(filter, endProperty, CALENDAR_MATCH_GREATER_THAN_OR_EQUAL, getCaltime(baseTime));
				if (error != CALENDAR_ERROR_NONE) break;

				error = calendar_filter_add_operator(filter, CALENDAR_FILTER_OPERATOR_AND);
				if (error != CALENDAR_ERROR_NONE) break;

				error = calendar_filter_add_caltime(filter, startProperty, CALENDAR_MATCH_GREATER_THAN, low);
				if (error != CALENDAR_ERROR_NONE) break;
			}
		} else {
			error = calendar_filter_add_caltime(filter, startProperty, CALENDAR_MATCH_LESS_THAN, getCaltime(baseTime));
			if (error != CALENDAR_ERROR_NONE) break;

			error = calendar_filter_add_operator(filter, CALENDAR_FILTER_OPERATOR_AND);
			if (error != CALENDAR_ERROR_NONE) break;

			if (allDay) {
				calendar_time_s high = getCaltime(CalDateTime(2037,1,2));
				error = calendar_filter_add_caltime(filter, endProperty, CALENDAR_MATCH_LESS_THAN, high);
				if (error != CALENDAR_ERROR_NONE) break;
			} else {
				calendar_time_s high = getCaltime(CalDateTime(2037,1,2,0,0,0));
				error = calendar_filter_add_caltime(filter, endProperty, CALENDAR_MATCH_LESS_THAN, high);
				if (error != CALENDAR_ERROR_NONE) break;
			}
		}

		if (additionalFilter) {
			error = calendar_filter_add_operator(filter, CALENDAR_FILTER_OPERATOR_AND);
			if (error != CALENDAR_ERROR_NONE) break;

			error = calendar_filter_add_filter(filter, additionalFilter);
			if (error != CALENDAR_ERROR_NONE) break;

			error = calendar_filter_destroy(additionalFilter);
			if (error != CALENDAR_ERROR_NONE) break;
		}
	}while(0);

	if (error != CALENDAR_ERROR_NONE) {
		if (query) {
			calendar_query_destroy(query);
			query = NULL;
		}
		if (filter)
			calendar_filter_destroy(filter);
	} else {
		calendar_query_set_filter(query, filter);
		calendar_filter_destroy(filter);
		calendar_query_set_sort(query, dir > 0 ? startProperty : endProperty, dir > 0);
	}

	return query;
}

static calendar_filter_h __createWritableOnlyFilter(const char* filter_uri, unsigned int property_id);
static calendar_filter_h __createKeywordFilterForAllday(const char* keyword);
static calendar_filter_h __createKeywordFilterForNormal(const char* keyword);

calendar_filter_h CalInstanceSchedule::createListFilter(bool allDay, bool visibleCalendarsOnly, bool editableOnly, const char* keyword)
{
	WDEBUG("%d %d %d %s", allDay, visibleCalendarsOnly, editableOnly, keyword ? keyword : "(null)");

	const char* uri = allDay ?
			_calendar_instance_localtime_calendar_book_extended._uri :
			_calendar_instance_utime_calendar_book_extended._uri;

	calendar_filter_h filter = NULL;
	calendar_filter_h subfilter = NULL;
	int error = CALENDAR_ERROR_NONE;
	do {
		error = calendar_filter_create(uri, &filter);
		if (error != CALENDAR_ERROR_NONE) break;

		bool first = true;

		if (visibleCalendarsOnly) {
			error = calendar_filter_add_int(filter, allDay ?
					_calendar_instance_localtime_calendar_book_extended.calendar_book_visibility :
					_calendar_instance_utime_calendar_book_extended.calendar_book_visibility,
					CALENDAR_MATCH_EQUAL, 1);
			if (error != CALENDAR_ERROR_NONE) break;
			first = false;
		}

		if (editableOnly) {
			subfilter = __createWritableOnlyFilter(uri, allDay ?
					_calendar_instance_localtime_calendar_book_extended.calendar_book_id :
					_calendar_instance_utime_calendar_book_extended.calendar_book_id);
			WASSERT(subfilter);

			if (first) {
				first = false;
			} else {
				error = calendar_filter_add_operator(filter, CALENDAR_FILTER_OPERATOR_AND);
				if (error != CALENDAR_ERROR_NONE) break;
			}
			calendar_filter_add_filter(filter, subfilter);
			calendar_filter_destroy(subfilter);
		}

		if (keyword && strlen(keyword) > 0) {
			WDEBUG("%s", keyword);
			subfilter = allDay ?
					__createKeywordFilterForAllday(keyword) :
					__createKeywordFilterForNormal(keyword);
			WASSERT(subfilter);

			if (first) {
				first = false;
			} else {
				error = calendar_filter_add_operator(filter, CALENDAR_FILTER_OPERATOR_AND);
				if (error != CALENDAR_ERROR_NONE) break;
			}
			calendar_filter_add_filter(filter, subfilter);
			calendar_filter_destroy(subfilter);
		}
	}while(0);

	if (error != CALENDAR_ERROR_NONE) {
		if (filter) {
			calendar_filter_destroy(filter);
			filter = NULL;
		}
		if (subfilter)
			calendar_filter_destroy(subfilter);
	}

	return filter;
}

static calendar_filter_h __createWritableOnlyFilter(const char* filter_uri, unsigned int property_id)
{
	calendar_query_h calendar_book_query = NULL;
	calendar_filter_h calendar_book_filter = NULL;
	calendar_list_h calendar_book_list = NULL;
	calendar_record_h calendar_book = NULL;

	int error = calendar_query_create(_calendar_book._uri, &calendar_book_query);
	if (error != CALENDAR_ERROR_NONE)
		return NULL;

	error = calendar_filter_create(_calendar_book._uri, &calendar_book_filter);

	error = calendar_filter_add_int(calendar_book_filter, _calendar_book.mode, CALENDAR_MATCH_EQUAL, CALENDAR_BOOK_MODE_NONE);
	c_warn_if(error != CALENDAR_ERROR_NONE, "calendar_filter_add_int() is failed(%x)", error);

	error = calendar_query_set_filter(calendar_book_query, calendar_book_filter);
	c_warn_if(error != CALENDAR_ERROR_NONE, "calendar_query_add_filter() is failed(%x)", error);

	error = calendar_db_get_records_with_query(calendar_book_query, 0, 0, &calendar_book_list);
	c_warn_if(error != CALENDAR_ERROR_NONE, "calendar_db_get_records_with_query() is failed(%x)", error);

	error = calendar_list_first(calendar_book_list);
	c_warn_if(error != CALENDAR_ERROR_NONE, "calendar_list_first() is failed(%x)", error);

	error = calendar_list_get_current_record_p(calendar_book_list, &calendar_book);
	c_warn_if(error != CALENDAR_ERROR_NONE, "calendar_list_get_current_record_p() is failed(%x)", error);

	int calendar_book_id = 0;
	int previous_book_id = 0;

	calendar_filter_h writable_filter = NULL;

	error = calendar_filter_create(filter_uri, &writable_filter);
	c_warn_if(error != CALENDAR_ERROR_NONE, "calendar_filter_create() is failed(%x)", error);

	while (calendar_book) {
		if (calendar_book_id) {
			error = calendar_filter_add_operator(writable_filter, CALENDAR_FILTER_OPERATOR_OR);
			c_warn_if(error != CALENDAR_ERROR_NONE, "calendar_filter_add_operator() is failed(%x)", error);
		}
		previous_book_id = calendar_book_id;

		error = calendar_record_get_int(calendar_book, _calendar_book.id, &calendar_book_id);
		c_warn_if(error != CALENDAR_ERROR_NONE, "calendar_record_get_int() is failed(%x)", error);

		if (calendar_book_id != DEFAULT_BIRTHDAY_CALENDAR_BOOK_ID) {
			error = calendar_filter_add_int(writable_filter, property_id, CALENDAR_MATCH_EQUAL, calendar_book_id);
			c_warn_if(error != CALENDAR_ERROR_NONE, "calendar_filter_add_int() is failed(%x)", error);
		}
		else {
			calendar_book_id = previous_book_id;
		}

		error = calendar_list_next(calendar_book_list);
		if (error != CALENDAR_ERROR_NONE)
			break;

		error = calendar_list_get_current_record_p(calendar_book_list, &calendar_book);
		c_warn_if(error != CALENDAR_ERROR_NONE, "calendar_list_get_current_record_p() is failed(%x)", error);
	}

	calendar_list_destroy(calendar_book_list, true);
	calendar_filter_destroy(calendar_book_filter);
	calendar_query_destroy(calendar_book_query);

	return writable_filter;
}

static calendar_filter_h __createKeywordFilterForAllday(const char* keyword)
{
	calendar_filter_h sub_filter = NULL;
	int error = CALENDAR_ERROR_NONE;
	do {
		error = calendar_filter_create(_calendar_instance_localtime_calendar_book_extended._uri, &sub_filter);
		if (error != CALENDAR_ERROR_NONE) break;

		error = calendar_filter_add_str(sub_filter, _calendar_instance_localtime_calendar_book_extended.summary, CALENDAR_MATCH_CONTAINS, keyword);
		if (error != CALENDAR_ERROR_NONE) break;

		error = calendar_filter_add_operator(sub_filter, CALENDAR_FILTER_OPERATOR_OR);
		if (error != CALENDAR_ERROR_NONE) break;

		error = calendar_filter_add_str(sub_filter, _calendar_instance_localtime_calendar_book_extended.description, CALENDAR_MATCH_CONTAINS, keyword);
		if (error != CALENDAR_ERROR_NONE) break;

		error = calendar_filter_add_operator(sub_filter, CALENDAR_FILTER_OPERATOR_OR);
		if (error != CALENDAR_ERROR_NONE) break;

		error = calendar_filter_add_str(sub_filter, _calendar_instance_localtime_calendar_book_extended.location, CALENDAR_MATCH_CONTAINS, keyword);
		if (error != CALENDAR_ERROR_NONE) break;
	} while(0);

	if (error != CALENDAR_ERROR_NONE) {
		if (sub_filter) {
			calendar_filter_destroy(sub_filter);
			sub_filter = NULL;
		}
	}

	return sub_filter;
}

static calendar_filter_h __createKeywordFilterForNormal(const char* keyword)
{
	calendar_filter_h sub_filter = NULL;
	int error = calendar_filter_create(_calendar_instance_utime_calendar_book_extended._uri, &sub_filter);
	c_warn_if(error != CALENDAR_ERROR_NONE, "calendar_filter_create() is failed(%x)", error);

	error = calendar_filter_add_str(sub_filter, _calendar_instance_utime_calendar_book_extended.summary, CALENDAR_MATCH_CONTAINS, keyword);
	c_warn_if(error != CALENDAR_ERROR_NONE, "calendar_filter_add_str() is failed(%x)", error);

	error = calendar_filter_add_operator(sub_filter, CALENDAR_FILTER_OPERATOR_OR);
	c_warn_if(error != CALENDAR_ERROR_NONE, "calendar_filter_add_operator() is failed(%x)", error);

	error = calendar_filter_add_str(sub_filter, _calendar_instance_utime_calendar_book_extended.description, CALENDAR_MATCH_CONTAINS, keyword);
	c_warn_if(error != CALENDAR_ERROR_NONE, "calendar_filter_add_str() is failed(%x)", error);

	error = calendar_filter_add_operator(sub_filter, CALENDAR_FILTER_OPERATOR_OR);
	c_warn_if(error != CALENDAR_ERROR_NONE, "calendar_filter_add_operator() is failed(%x)", error);

	error = calendar_filter_add_str(sub_filter, _calendar_instance_utime_calendar_book_extended.location, CALENDAR_MATCH_CONTAINS, keyword);
	c_warn_if(error != CALENDAR_ERROR_NONE, "calendar_filter_add_str() is failed(%x)", error);

	return sub_filter;
}

