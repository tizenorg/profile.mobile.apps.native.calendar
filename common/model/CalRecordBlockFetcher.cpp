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

#include "CalRecordBlockFetcher.h"

static bool __integrityCheck(calendar_record_h record);

CalRecordBlockFetcher::CalRecordBlockFetcher(calendar_query_h query, bool fetchAllAtOnce) :
	__query(query),
	__fetchAllAtOnce(fetchAllAtOnce)
{
	__currentBlock = NULL;
	__nextBlock = NULL;
	__offset = 0;
	__noMoreToFetch = false;
	__noMoreToGive = false;
}

CalRecordBlockFetcher::~CalRecordBlockFetcher()
{
	calendar_query_destroy(__query);
}

void CalRecordBlockFetcher::prefetch(bool fillBothBuffers)
{
	if (__noMoreToFetch)
		return;

	TIME_CHECKER;

	if (__currentBlock == NULL)
		__fetch(__currentBlock);

	if (fillBothBuffers && __nextBlock == NULL)
		__fetch(__nextBlock);
}

calendar_record_h CalRecordBlockFetcher::getNext(bool autoFetch)
{
RETRY:
	if (__noMoreToGive)
		return NULL;

	calendar_record_h record = NULL;

	if (__currentBlock == NULL) {
		if (!autoFetch) {
			WDEBUG("Ran out records in buffered blocks and auto-fetch is not requested - prefetch is necessary.");
			return NULL;
		}
		WDEBUG("Defaulting to auto-fetch for the first batch of records - explicit prefetch is recommended.");
		__fetch(__currentBlock);
	}

	int error = calendar_list_get_current_record_p(__currentBlock, &record);

	if (error) {

		calendar_list_destroy(__currentBlock, false);
		__currentBlock = NULL;

		if (__nextBlock != NULL) {
			__currentBlock = __nextBlock;
			__nextBlock = NULL;
		} else {
			if (!autoFetch) {
				WDEBUG("Ran out records in buffered blocks and auto-fetch is not requested - prefetch is necessary.");
				return NULL;
			}
			WDEBUG("Defaulting to auto-fetch because next batch wasn't ready - explicit prefetch is recommended.");
			__fetch(__currentBlock);
			if (__currentBlock == NULL) {
				__noMoreToGive = true;
				return NULL;
			}
		}

		WASSERT(__currentBlock != NULL);
		WASSERT(__nextBlock == NULL);

		error = calendar_list_get_current_record_p(__currentBlock, &record);
		if (error) // must be empty list
			return NULL;
	}

	WASSERT(record);
	calendar_list_remove(__currentBlock, record);

	if (!__integrityCheck(record)) {
		WASSERT(0);
		calendar_record_destroy(record, true);
		goto RETRY;
	}

	return record;
}

bool CalRecordBlockFetcher::eof()
{
	return __noMoreToGive;
}

#define BLOCK_SIZE 10

void CalRecordBlockFetcher::__fetch(calendar_list_h& block)
{
	if (__noMoreToFetch) {
		block = NULL;
		return;
	}

	int error = calendar_db_get_records_with_query(__query, __offset, __fetchAllAtOnce ? 0 : BLOCK_SIZE, &block);
	WPRET_M(error != CALENDAR_ERROR_NONE, "calendar_db_get_records_with_query has failed(%d)", error);

	int retreivedCount;
	calendar_list_get_count(block, &retreivedCount);
	WDEBUG("retreivedCount(%d)", retreivedCount);
	__offset += retreivedCount;
	if (retreivedCount < BLOCK_SIZE)
		__noMoreToFetch = true;
	WDEBUG("%d", retreivedCount);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool __integrityCheck(calendar_record_h record)
{
	char *view_uri = NULL;
	int error = calendar_record_get_uri_p(record, &view_uri);
	if (error != CALENDAR_ERROR_NONE) {
		WERROR("calendar_record_get_uri_p failed(%d)!", error);
		return false;
	}

	int startTimeProperty, endTimeProperty;

	if(g_strcmp0(view_uri, _calendar_event._uri) == 0) {

		startTimeProperty = _calendar_event.start_time;
		endTimeProperty = _calendar_event.end_time;

	} else if (g_strcmp0(view_uri, _calendar_instance_utime_calendar_book_extended._uri) == 0) {

		startTimeProperty = _calendar_instance_utime_calendar_book_extended.start_time;
		endTimeProperty = _calendar_instance_utime_calendar_book_extended.end_time;

	} else if (g_strcmp0(view_uri, _calendar_instance_localtime_calendar_book_extended._uri) == 0) {

		startTimeProperty = _calendar_instance_localtime_calendar_book_extended.start_time;
		endTimeProperty = _calendar_instance_localtime_calendar_book_extended.end_time;

	} else {

		WASSERT(0);
		return false;

	}

	calendar_time_s start;
	error = calendar_record_get_caltime(record, startTimeProperty, &start);
	if (error != CALENDAR_ERROR_NONE) {

		WERROR("Get start time failed(%d)!", error);
		return false;

	}

	calendar_time_s end;
	error = calendar_record_get_caltime(record, endTimeProperty, &end);
	if (error != CALENDAR_ERROR_NONE) {

		WERROR("Get end time failed(%d)!", error);
		return false;

	}

	switch (start.type) {

	case CALENDAR_TIME_UTIME:
		{
			WASSERT(end.type == CALENDAR_TIME_UTIME);

			if (start.time.utime > end.time.utime) {
				WERROR("End time is before start time! (%lld, %lld)", start.time.utime, end.time.utime);
				return false;
			}

			break;
		}

	case CALENDAR_TIME_LOCALTIME:
		{
			WASSERT(end.type == CALENDAR_TIME_LOCALTIME);

			const int startVal = (start.time.date.year << 9) | (start.time.date.month << 5) | start.time.date.mday;
			const int endVal = (end.time.date.year << 9) | (end.time.date.month << 5) | end.time.date.mday;

			if (startVal > endVal) {
				WERROR("End date is before start date! (%d/%d/%d, %d/%d/%d)",
						start.time.date.year, start.time.date.month, start.time.date.mday,
						end.time.date.year, end.time.date.month, end.time.date.mday);
				return false;
			}

			break;
		}

	default:
		{
			WASSERT(0);
			return false;
		}
	}

	return true;
}
