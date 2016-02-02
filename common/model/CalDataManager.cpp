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

#include "CalDataManager.h"
#include "CalCommon.h"
#include "CalEventManager.h"
#include "CalBookManager.h"
#include "CalSettingsManager.h"
#include "CalInstanceSchedule.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <app.h>

SINGLETON_INSTANCE(CalDataManager);

CalDataManager::CalDataManager()
{
	WENTER();
	__localVersion = 0;
	__version = 0;
}

CalDataManager::~CalDataManager()
{
	WENTER();
	calendar_db_remove_changed_cb(_calendar_event._uri, __onChanged, NULL);
	calendar_disconnect();
}

void CalDataManager::init()
{
	WENTER();
	int error = calendar_connect();
	WASSERT(error == CALENDAR_ERROR_NONE);
	error = calendar_db_get_current_version(&__version);
	WASSERT(error == CALENDAR_ERROR_NONE);
	__localVersion = __version;
	WDEBUG("version = %d:%d", __localVersion, __version);
	error = calendar_db_add_changed_cb(_calendar_event._uri, __onChanged, NULL);
	WASSERT(error == CALENDAR_ERROR_NONE);
}

int CalDataManager::insertSchedule(const CalSchedule& schedule, int* newId)
{
	WENTER();
	// check if it's an original schedule
	WASSERT(schedule.getType() == CalSchedule::ORIGINAL);

	const calendar_record_h record = schedule.getRecord();

	int id = 0;
	int error = calendar_db_insert_record(record, &id);
	if (error != CALENDAR_ERROR_NONE)
	{
		WERROR("calendar_db_insert_record failed : %d", error);
		return -1;
	}
	__notify(CalEvent::LOCAL);

	if (newId) {
		*newId = id;
	}
	WLEAVE();
	return 0;
}

void CalDataManager::__updateSchedule(CalOriginalSchedule& workingCopy)
{
	WENTER();

	const calendar_record_h record = workingCopy.getRecord();
	int error = calendar_db_update_record(record);
	if (error != CALENDAR_ERROR_NONE)
	{
		WERROR("calendar_db_update_record failed : %d", error);
		return;
	}
	__notify(CalEvent::LOCAL);
	WLEAVE();
}

std::shared_ptr<CalOriginalSchedule> CalDataManager::__getOriginalSchedule(const CalSchedule& instance)
{
	const int originalRecordId =
		instance.isException() ? instance.getOriginalEventId() : instance.getIndex();

	calendar_record_h originalRecord = NULL;
	int error = calendar_db_get_record(_calendar_event._uri, originalRecordId, &originalRecord);
	if (error != CALENDAR_ERROR_NONE)
	{
		WERROR("calendar_db_get_record(%d) failed : %d", originalRecordId, error);
		return nullptr;
	}

	return std::make_shared<CalOriginalSchedule>(originalRecord);
}

int CalDataManager::updateSchedule(OperationMode mode, const CalSchedule& inputSchedule, CalSchedule& workingCopy, int* newId)
{
	WENTER();
	WASSERT(workingCopy.getType() == CalSchedule::ORIGINAL);

	switch (mode)
	{
		case CalDataManager::ONLY_THIS:
			__updateOnlyThisInstance(inputSchedule, (CalOriginalSchedule&)workingCopy);
			break;
		case CalDataManager::THIS_AND_FUTURE:
			__updateThisAndFutureInstances(inputSchedule, (CalOriginalSchedule&)workingCopy, newId);
			break;
		case CalDataManager::ALL:
			__updateAllInstances(inputSchedule, (CalOriginalSchedule&)workingCopy);
			break;
		default:
			break;
	}

	__notify(CalEvent::LOCAL);

	WLEAVE();
	return 0;
}

void CalDataManager::__updateOnlyThisInstance(const CalSchedule& inputSchedule, CalOriginalSchedule& workingCopy)
{
	if (inputSchedule.isException())
	{
		WASSERT(workingCopy.isException());
		__updateSchedule(workingCopy);
	}
	else
	{
		workingCopy.setOriginalEventId(__getOriginalSchedule(inputSchedule)->getIndex());
		workingCopy.setRecurranceId(inputSchedule.getExceptionString());
		insertSchedule(workingCopy);
	}
}

void CalDataManager::__updateThisAndFutureInstances(const CalSchedule& inputSchedule, CalOriginalSchedule& workingCopy, int* newId)
{
	CalDateTime inputInstanceStartTime;
	inputSchedule.getStart(inputInstanceStartTime);
	{std::string str; inputInstanceStartTime.getString(str); WDEBUG("%s", str.c_str());}

	auto originalSchedule = __getOriginalSchedule(inputSchedule);

	if (originalSchedule->getInstanceCountBeforeDate(inputInstanceStartTime) == 0)
	{
		__updateAllInstances(inputSchedule, workingCopy);
		*newId = workingCopy.getIndex();
		return;
	}

	originalSchedule->truncateRepeatToBeforeDate(inputInstanceStartTime);
	__updateSchedule(*originalSchedule);

	__deleteExceptionRecordsOnAndAfter(*originalSchedule, inputInstanceStartTime);

	insertSchedule(workingCopy, newId);
}

void CalDataManager::__updateAllInstances(const CalSchedule& inputSchedule, CalOriginalSchedule& workingCopy)
{
	WENTER();

	__deleteAllExceptionRecords(workingCopy);

	const calendar_record_h record = workingCopy.getRecord();

	int error = calendar_db_update_record(record);
	if (error != CALENDAR_ERROR_NONE)
	{
		WERROR("calendar_db_update_record failed : %d", error);
		return;
	}

	WLEAVE();
}

int CalDataManager::deleteSchedule(const CalSchedule& inputSchedule)
{
	WENTER();
	int ret = __deleteSchedule(inputSchedule);
	if (ret == 0)
	{
		__notify(CalEvent::LOCAL);
		__notifyAlert();
	}
	WLEAVE();
	return ret;
}

int CalDataManager::deleteSchedule(OperationMode mode, const CalSchedule& inputSchedule)
{
	WENTER();
	switch (mode)
	{
		case CalDataManager::ONLY_THIS:
			__deleteOnlyThisInstance(inputSchedule);
			break;
		case CalDataManager::THIS_AND_FUTURE:
			__deleteThisAndFutureInstances(inputSchedule);
			break;
		case CalDataManager::ALL:
			__deleteAllInstances(inputSchedule);
			break;
		default:
			break;
	}
	WLEAVE();
	return 0;
}

int CalDataManager::__deleteSchedule(const CalSchedule& inputSchedule)
{
	int error = calendar_db_delete_record(_calendar_event._uri, inputSchedule.getIndex());
	if (error != CALENDAR_ERROR_NONE)
	{
		WERROR("calendar_db_delete(%d) failed : %d", inputSchedule.getIndex(), error);
		return -1;
	}

	return 0;
}

void CalDataManager::__deleteOnlyThisInstance(const CalSchedule& inputSchedule)
{
	if (inputSchedule.isException())
	{
		deleteSchedule(inputSchedule);
	}
	else
	{
		auto originalSchedule = __getOriginalSchedule(inputSchedule);
		calendar_record_h originalRecord = originalSchedule->getRecord();

		char* exdate = NULL;
		int error = calendar_record_get_str(originalRecord, _calendar_event.exdate, &exdate);
		c_warn_if(error != CALENDAR_ERROR_NONE, "calendar_record_get_str() has failed(%x)", error);

		char* new_exdate = NULL;
		if (exdate && *exdate)
		{
			new_exdate = g_strdup_printf("%s,%s", exdate, inputSchedule.getExceptionString());
		}
		else
		{
			new_exdate = g_strdup_printf("%s", inputSchedule.getExceptionString());
		}

		WDEBUG("%s", new_exdate);

		error = calendar_record_set_str(originalRecord, _calendar_event.exdate, new_exdate);
		c_warn_if(error != CALENDAR_ERROR_NONE, "calendar_record_set_str() has failed(%x)", error);

		__updateSchedule(*originalSchedule);
	}
}

void CalDataManager::__deleteThisAndFutureInstances(const CalSchedule& inputSchedule)
{
	CalDateTime inputInstanceStartTime;
	inputSchedule.getStart(inputInstanceStartTime);

	auto originalSchedule = __getOriginalSchedule(inputSchedule);
	if (originalSchedule->getInstanceCountBeforeDate(inputInstanceStartTime) == 0)
	{
		__deleteAllInstances(inputSchedule);
		return;
	}

	originalSchedule->truncateRepeatToBeforeDate(inputInstanceStartTime);
	__updateSchedule(*originalSchedule);
	__deleteExceptionRecordsOnAndAfter(*originalSchedule, inputInstanceStartTime);
}

void CalDataManager::__deleteAllInstances(const CalSchedule& inputSchedule)
{
	if (inputSchedule.isException())
	{
		deleteSchedule(*__getOriginalSchedule(inputSchedule));
	}
	else
	{
		deleteSchedule(inputSchedule);
	}
}

int CalDataManager::deleteSchedules(const std::list<std::shared_ptr<CalSchedule>>& inputSchedules)
{
	WENTER();
	for (auto it = inputSchedules.begin(); it != inputSchedules.end(); ++it)
	{
		__deleteSchedule(**it);
	}
	__notify(CalEvent::LOCAL);
	__notifyAlert();
	WLEAVE();
	return 0;
}

std::shared_ptr<CalSchedule> CalDataManager::getUpdatedWorkingCopy(OperationMode mode, const CalSchedule& inputSchedule)
{
	switch (mode)
	{
		case CalDataManager::ONLY_THIS:
		case CalDataManager::THIS_AND_FUTURE:
		{
			auto workingCopy = __getWorkingUpdatedCopy(inputSchedule);
			if (workingCopy)
			{
				return workingCopy;
			}
			return __getWorkingInstanceCopy(inputSchedule);
		}
		case CalDataManager::ALL:
			return __getWorkingOriginalCopy(inputSchedule);
	}

	return nullptr;
}

std::shared_ptr<CalSchedule> CalDataManager::getWorkingCopy(OperationMode mode, const CalSchedule& inputSchedule)
{
	switch (mode)
	{
		case CalDataManager::ONLY_THIS:
			return __getWorkingInstanceCopy(inputSchedule);
		case CalDataManager::THIS_AND_FUTURE:
		{
			auto workingCopy = __getWorkingInstanceCopy(inputSchedule);
			__setThisAndFutureRepeatCount(inputSchedule, *workingCopy);
			return workingCopy;
		}
		case CalDataManager::ALL:
			return __getWorkingOriginalCopy(inputSchedule);
	}

	return nullptr;
}

std::shared_ptr<CalSchedule> CalDataManager::getWorkingCopyForForward(OperationMode mode, const CalSchedule& inputSchedule)
{
	switch (mode)
	{
		case CalDataManager::ONLY_THIS:
		{
			auto workingCopy = __getWorkingInstanceCopy(inputSchedule);
			WASSERT(workingCopy);
			CalScheduleRepeat repeat;
			repeat.unitType = CalScheduleRepeat::UnitType::ONE_TIME;
			workingCopy->setRepeat(repeat);
			return workingCopy;
		}
		case CalDataManager::ALL:
			return __getWorkingOriginalCopy(inputSchedule);
		case CalDataManager::THIS_AND_FUTURE:
		default:
			break;
	}

	return nullptr;
}

std::shared_ptr<CalOriginalSchedule> CalDataManager::__getWorkingInstanceCopy(const CalSchedule& inputSchedule)
{
	const int originalRecordId = inputSchedule.getIndex();

	calendar_record_h originalRecord = NULL;
	int error = calendar_db_get_record(_calendar_event._uri, originalRecordId, &originalRecord);
	if (error != CALENDAR_ERROR_NONE)
	{
		WERROR("calendar_db_get_record(%d) failed : %d", originalRecordId, error);
		return NULL;
	}

	// Note:
	//	If inputSchedule is an exception instance, originalRecord is the exception record
	//  from the original table, which is precisely what we want to work with.
	auto workingCopy = std::make_shared<CalOriginalSchedule>(originalRecord);
	if (!workingCopy->isException() && workingCopy->hasRepeat() == true)
	{
		CalDateTime startTime, endTime;
		inputSchedule.getStart(startTime);
		inputSchedule.getEnd(endTime);
		workingCopy->setStart(startTime);
		workingCopy->setEnd(endTime);
	}

	return workingCopy;
}

std::shared_ptr<CalOriginalSchedule> CalDataManager::__getWorkingOriginalCopy(const CalSchedule& inputSchedule)
{
	auto workingCopy = __getOriginalSchedule(inputSchedule);

	if (!workingCopy->hasRepeat())
	{
		const char* myTimezone = workingCopy->getTimeZone();
		if (myTimezone == NULL || g_strcmp0(myTimezone, "Etc/GMT") == 0)
		{
			WHIT();
			std::string calendarTimezone;
			CalSettingsManager::getInstance().getCalendarTimeZone(calendarTimezone);
			// update tz
			workingCopy->setTimeZone(calendarTimezone.c_str());
		}
	}

	return workingCopy;
}

void CalDataManager::__setThisAndFutureRepeatCount(const CalSchedule& inputSchedule, CalOriginalSchedule& workingCopy)
{
	CalScheduleRepeat repeat = workingCopy.getRepeat();
	repeat.print();
	if (repeat.untilType != CalScheduleRepeat::UntilType::TIMES)
	{
		return;
	}

	CalDateTime inputInstanceStartTime;
	inputSchedule.getStart(inputInstanceStartTime);

	const auto originalSchedule = __getOriginalSchedule(inputSchedule);

	repeat.untilInfo.times -= originalSchedule->getInstanceCountBeforeDate(inputInstanceStartTime);
	repeat.print();

	workingCopy.setRepeat(repeat);
}

std::shared_ptr<CalOriginalSchedule> CalDataManager::__getWorkingUpdatedCopy(const CalSchedule& inputSchedule)
{
	WENTER();

	if (inputSchedule.hasRepeat())
	{
		std::shared_ptr<CalOriginalSchedule> workingCopy = NULL;
		int originalRecordId = 0;
		char *recurrenceId = NULL;

		int error = 0;
		if (inputSchedule.isException() == true)
		{
			originalRecordId = inputSchedule.getOriginalEventId();
			// get recurrence_id
			recurrenceId = inputSchedule.getExceptionString();
		}
		else
		{
			originalRecordId = inputSchedule.getIndex();
			// make recurrence_id
			recurrenceId = inputSchedule.getExceptionString();
		}
		WDEBUG("recurrenceId[%s]",recurrenceId);
		calendar_query_h query = NULL;
		calendar_filter_h filter = NULL;
		const char* view_uri = _calendar_event._uri;
		calendar_list_h list = NULL;
		do
		{
			error = calendar_query_create(view_uri, &query);
			if (error != CALENDAR_ERROR_NONE)
			{
				break;
			}
			error = calendar_filter_create(view_uri, &filter);
			if (error != CALENDAR_ERROR_NONE)
			{
				break;
			}
			error = calendar_filter_add_int(filter, _calendar_event.original_event_id,CALENDAR_MATCH_EQUAL,originalRecordId);
			if (error != CALENDAR_ERROR_NONE)
			{
				break;
			}
			error = calendar_filter_add_operator(filter, CALENDAR_FILTER_OPERATOR_AND);
			if (error != CALENDAR_ERROR_NONE)
			{
				break;
			}
			error = calendar_filter_add_str(filter, _calendar_event.recurrence_id, CALENDAR_MATCH_CONTAINS,recurrenceId);
			if (error != CALENDAR_ERROR_NONE)
			{
				break;
			}
			error = calendar_query_set_filter(query,filter);
			if (error != CALENDAR_ERROR_NONE)
			{
				break;
			}
			error = calendar_db_get_records_with_query(query,0,0,&list);
		}
		while(0);
		if (error != CALENDAR_ERROR_NONE)
		{
			WERROR("fail : %d", error);
		}

		if (list != NULL)
		{
			int count = 0;
			calendar_list_first(list);
			calendar_record_h record2 = NULL;
			error = calendar_list_get_count(list, &count);
			if (error != CALENDAR_ERROR_NONE)
			{
				WERROR("fail : %d", error);
			}
			error = calendar_list_get_current_record_p(list, &record2);
			if (error != CALENDAR_ERROR_NONE)
			{
				WERROR("fail : %d", error);
			}
			if (count > 0 && record2 != NULL)
			{
				calendar_record_h clone = NULL;
				error = calendar_record_clone(record2, &clone);
				if (error != CALENDAR_ERROR_NONE)
				{
					WERROR("fail : %d", error);
				}
				workingCopy = std::make_shared<CalOriginalSchedule>(clone);
			}
			calendar_list_destroy(list,true);
		}
		calendar_filter_destroy(filter);
		calendar_query_destroy(query);
		g_free(recurrenceId);

		return workingCopy;
	}
	return NULL;
}

void CalDataManager::getSchedulesFromVcs(const char* filePath, std::list<std::shared_ptr<CalSchedule>>& schedules)
{
	WENTER();
	WASSERT(filePath && strlen(filePath) > 0);
	WDEBUG("file path = %s", filePath);

	struct stat st = {0};
	int ret = stat(filePath, &st);
	if (ret != 0 || st.st_size <= 0)
	{
		WERROR("stat(%s) failed : %s", filePath, strerror(errno));
		return;
	}

	FILE* file = fopen(filePath, "r");
	if (!file)
	{
		WERROR("fopen(%s) failed : %s", filePath, strerror(errno));
		return;
	}

	char* vcsData = (char*)g_malloc0(st.st_size);
	if(vcsData == NULL)
	{
		WERROR("g_malloc0(%d) failed : %s", st.st_size, strerror(errno));
		fclose(file);
		return;
	}

	ret = fread(vcsData, 1, st.st_size, file);
	if (ret <= 0)
	{
		WERROR("fread(%s) failed : %s", filePath, strerror(errno));
		g_free(vcsData);
		fclose(file);
		return;
	}
	fclose(file);

	calendar_list_h records = NULL;
	int error = calendar_vcalendar_parse_to_calendar(vcsData, &records);
	g_free(vcsData);
	if (error != CALENDAR_ERROR_NONE)
	{
		WERROR("calendar_vcalendar_parse_to_calendar failed : %d", error);
		return;
	}

	int count = 0;
	calendar_list_get_count(records, &count);
	calendar_list_first(records);
	for (int i = 0; i < count; i++)
	{
		calendar_record_h record = NULL;
		calendar_list_get_current_record_p(records, &record);
		WASSERT(record);
		char *view_uri = NULL;
		calendar_record_get_uri_p(record, &view_uri);
		WASSERT(view_uri);
		if (strcmp(view_uri, _calendar_todo._uri) == 0)
		{
			WERROR("Task cannot be supported");
			continue;
		}
		else if (strcmp(view_uri, _calendar_event._uri) == 0)
		{
			std::shared_ptr<CalOriginalSchedule> original(std::make_shared<CalOriginalSchedule>(record));
			schedules.push_back(original);
		}
		else
		{
			WDEBUG("%s", view_uri);
		}
		calendar_list_next(records);
	}
	calendar_list_destroy(records, false);
	WLEAVE();
}

void CalDataManager::generateVcsFromSchedule(const CalSchedule& schedule, const char* filePath, bool isOriginalSchedule)
{
	WENTER();
	WDEBUG("filePath = %s", filePath);
	int error = CALENDAR_ERROR_NONE;
	calendar_record_h record = NULL;
	calendar_list_h list = NULL;
	char *vcs = NULL;
	int fd = -1;

	do
	{
		if (isOriginalSchedule)
		{
			error = calendar_record_clone(schedule.getRecord(), &record);
		}
		else
		{
			error = calendar_db_get_record(_calendar_event._uri, schedule.getIndex(), &record);
		}

		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}

		// check birthday book
		if (DEFAULT_BIRTHDAY_CALENDAR_BOOK_ID == schedule.getBookId())
		{
			std::string text;
			schedule.getDisplaySummary(-1, text);
			char* input = elm_entry_markup_to_utf8(text.c_str());
			error = calendar_record_set_str(record, _calendar_event.summary, input);
			free(input);
			if (error != CALENDAR_ERROR_NONE)
			{
				break;
			}
		}

		error = calendar_list_create(&list);
		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}

		error = calendar_list_add(list, record);
		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}

		error = calendar_vcalendar_make_from_records(list, &vcs);
		if (vcs == NULL || strlen(vcs) == 0)
		{
			WDEBUG("no data");
			error = CALENDAR_ERROR_NO_DATA;
		}
		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}

		fd = open(filePath, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
		if (fd < 0)
		{
			WDEBUG("open error: %s", strerror(errno));
			error = CALENDAR_ERROR_SYSTEM;
			break;
		}

		if (write(fd, vcs, strlen(vcs)) == -1)
		{
			WDEBUG("write error: %s", strerror(errno));
			error = CALENDAR_ERROR_SYSTEM;
			break;
		}

		close(fd);
		fd = -1;
	}
	while (0);

	if (record)
	{
		calendar_record_destroy(record, true);
	}
	if (list)
	{
		calendar_list_destroy(list, false);
	}

	free(vcs);
	if (fd >= 0)
	{
		close(fd);
	}

	if (error != CALENDAR_ERROR_NONE)
	{
		WASSERT_EX(0, "cannot make a vcs file from %d event : %d", schedule.getIndex(), error);
		return;
	}
}

char* CalDataManager::generateTextFromSchedule(const CalSchedule& schedule)
{
	std::string summaryMarkUp;
	std::string summary;
	schedule.getDisplaySummary(-1, summaryMarkUp);
	if (summaryMarkUp.empty())
	{
		summaryMarkUp = _L_("IDS_CLD_MBODY_MY_EVENT");
	}

	char *tmp = elm_entry_markup_to_utf8(summaryMarkUp.c_str());
	if (!tmp)
	{
		WERROR("elm_entry_markup_to_utf8(%s) failed : %s", summaryMarkUp.c_str(), strerror(errno));
		summary = "";
	}
	else
	{
		summary = tmp;
	}
	free(tmp);

	std::string fromToString;
	schedule.getFromToString(fromToString);
	std::string location;
	if (schedule.getLocation())
	{
		location = schedule.getLocation();
	}
	std::string description;
	if (schedule.getDescription())
	{
		description = schedule.getDescription();
	}

	if (!location.empty() && !description.empty())
	{
		return g_strdup_printf("%s\n%s\n\n%s\n\n%s", summary.c_str(), fromToString.c_str(), location.c_str(), description.c_str());
	}
	else if (location.empty() && description.empty())
	{
		return g_strdup_printf("%s\n%s", summary.c_str(), fromToString.c_str());
	}
	else if (location.empty())
	{
		return g_strdup_printf("%s\n%s\n\n%s", summary.c_str(), fromToString.c_str(), description.c_str());
	}
	else
	{
		return g_strdup_printf("%s\n%s\n\n%s", summary.c_str(), fromToString.c_str(), location.c_str());
	}
}

std::shared_ptr<CalSchedule> CalDataManager::getSchedule(const int eventIndex)
{
	WENTER();

	calendar_record_h record = NULL;
	int error = calendar_db_get_record(_calendar_event._uri, eventIndex, &record);
	if (error != CALENDAR_ERROR_NONE)
	{
		WERROR("calendar_db_get_record(%d) failed : %d", eventIndex, error);
		return nullptr;
	}

	std::shared_ptr<CalOriginalSchedule> workingCopy(std::make_shared<CalOriginalSchedule>(record));

	WLEAVE();
	return workingCopy;
}

std::shared_ptr<CalSchedule> CalDataManager::getInstanceSchedule(const int eventIndex, const CalDateTime& dateTime)
{
	calendar_record_h record = NULL;
	int error = calendar_db_get_record(_calendar_event._uri, eventIndex, &record);
	if (error != CALENDAR_ERROR_NONE)
	{
		WERROR("calendar_db_get_record(%d) failed : %d", eventIndex, error);
		return nullptr;
	}

	calendar_time_s recordStart;
	error = calendar_record_get_caltime(record, _calendar_event.start_time, &recordStart);
	if (error != CALENDAR_ERROR_NONE)
	{
		calendar_record_destroy(record, true);
		WERROR("calendar_record_get_caltime(%d) failed : %d", eventIndex, error);
		return nullptr;
	}
	calendar_record_destroy(record, true);

	calendar_list_h list = NULL;
	if (recordStart.type == CALENDAR_TIME_UTIME)  // normal instance
	{
		CalDateTime tempDateTime(dateTime.getYear(), dateTime.getMonth(), dateTime.getMday(), 0, 0, 0);
		calendar_time_s start = CalSchedule::getCaltime(tempDateTime);
		tempDateTime.addDays(1);
		calendar_time_s end = CalSchedule::getCaltime(tempDateTime);
		calendar_query_h query = NULL;
		calendar_query_create(_calendar_instance_utime_calendar_book_extended._uri, &query);
		calendar_filter_h filter = NULL;
		calendar_filter_create(_calendar_instance_utime_calendar_book_extended._uri, &filter);
		calendar_filter_add_int(filter,_calendar_instance_utime_calendar_book_extended.event_id, CALENDAR_MATCH_EQUAL, eventIndex);
		calendar_filter_add_operator(filter, CALENDAR_FILTER_OPERATOR_AND);
		calendar_filter_add_caltime(filter, _calendar_instance_utime_calendar_book_extended.start_time, CALENDAR_MATCH_GREATER_THAN_OR_EQUAL, start);
		calendar_filter_add_operator(filter, CALENDAR_FILTER_OPERATOR_AND);
		calendar_filter_add_caltime(filter, _calendar_instance_utime_calendar_book_extended.start_time, CALENDAR_MATCH_LESS_THAN, end);
		calendar_query_set_filter(query, filter);
		calendar_db_get_records_with_query(query, 0, 0, &list);
		calendar_filter_destroy(filter);
		calendar_query_destroy(query);
	}
	else  // allday instance
	{
		CalDateTime tempDateTime(dateTime.getYear(), dateTime.getMonth(), dateTime.getMday());
		calendar_time_s start = CalSchedule::getCaltime(tempDateTime);
		calendar_query_h query = NULL;
		calendar_query_create(_calendar_instance_localtime_calendar_book_extended._uri, &query);
		calendar_filter_h filter = NULL;
		calendar_filter_create(_calendar_instance_localtime_calendar_book_extended._uri, &filter);
		calendar_filter_add_int(filter,_calendar_instance_localtime_calendar_book_extended.event_id, CALENDAR_MATCH_EQUAL, eventIndex);
		calendar_filter_add_operator(filter, CALENDAR_FILTER_OPERATOR_AND);
		calendar_filter_add_caltime(filter, _calendar_instance_localtime_calendar_book_extended.start_time, CALENDAR_MATCH_EQUAL, start);
		calendar_query_set_filter(query, filter);
		calendar_db_get_records_with_query(query, 0, 0, &list);
		calendar_filter_destroy(filter);
		calendar_query_destroy(query);
	}

	int count = 0;
	calendar_list_get_count(list, &count);
	if (count <= 0)
	{
		calendar_list_destroy(list, true);
		WERROR("There is no event for %d", eventIndex);
		return nullptr;
	}

	calendar_list_first(list);
	record = NULL;
	calendar_list_get_current_record_p(list, &record);
	WASSERT(record);
	calendar_list_destroy(list, false);

	WLEAVE();
	return std::make_shared<CalInstanceSchedule>(record);
}

bool CalDataManager::isOverlapped(const CalSchedule& schedule)
{
	WENTER();

	CalDateTime startTime, endTime;
	calendar_time_s start, end;
	schedule.getStart(startTime);
	schedule.getEnd(endTime);
	start = CalSchedule::getCaltime(startTime);
	end = CalSchedule::getCaltime(endTime);
	calendar_query_h query1 = NULL;
	calendar_filter_h filter = NULL;
	int error = 0;
	do
	{
		calendar_time_s caltimeStart, caltimeEnd;
		if (start.type == CALENDAR_TIME_LOCALTIME)
		{
			caltimeStart.type = CALENDAR_TIME_UTIME;
			caltimeStart.time.utime = startTime.getUtimeFromTm();
			caltimeEnd.type = CALENDAR_TIME_UTIME;
			caltimeEnd.time.utime= endTime.getUtimeFromTm();
		}
		else
		{
			caltimeStart = start;
			caltimeEnd = end;
		}
		unsigned int start_time_property_id = _calendar_instance_utime_calendar_book.start_time;
		unsigned int end_time_property_id = _calendar_instance_utime_calendar_book.end_time;
		const char* view_uri = _calendar_instance_utime_calendar_book._uri;
		error = calendar_filter_create(view_uri, &filter);
		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}
		error = calendar_filter_add_caltime(filter, end_time_property_id, CALENDAR_MATCH_GREATER_THAN, caltimeStart);
		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}
		error = calendar_filter_add_operator(filter, CALENDAR_FILTER_OPERATOR_AND);
		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}
		error = calendar_filter_add_caltime(filter, start_time_property_id, CALENDAR_MATCH_LESS_THAN_OR_EQUAL, caltimeEnd);
		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}
		error = calendar_query_create(view_uri, &query1);
		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}
		error = calendar_query_set_filter(query1, filter);
	}
	while(0);

	if (filter)
	{
		calendar_filter_destroy(filter);
		filter = NULL;
	}

	if (error != CALENDAR_ERROR_NONE)
	{
		if (query1)
		{
			calendar_query_destroy(query1);
		}
		WERROR("fail(%x)",error);
		return false;
	}

	// instance allday
	calendar_query_h query2 = NULL;

	do
	{
		calendar_time_s caltimeStart, caltimeEnd;
		if (start.type == CALENDAR_TIME_UTIME)
		{
			caltimeStart.type = CALENDAR_TIME_LOCALTIME;
			caltimeStart.time.date.year = startTime.getYear();
			caltimeStart.time.date.month = startTime.getMonth();
			caltimeStart.time.date.mday = startTime.getMday();
			caltimeEnd.type = CALENDAR_TIME_LOCALTIME;
			caltimeEnd.time.date.year = endTime.getYear();
			caltimeEnd.time.date.month = endTime.getMonth();
			caltimeEnd.time.date.mday = endTime.getMday();
		}
		else
		{
			caltimeStart = start;
			caltimeEnd = end;
		}
		unsigned int start_time_property_id = _calendar_instance_localtime_calendar_book.start_time;
		unsigned int end_time_property_id = _calendar_instance_localtime_calendar_book.end_time;
		const char* view_uri = _calendar_instance_localtime_calendar_book._uri;
		error = calendar_filter_create(view_uri, &filter);
		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}
		error = calendar_filter_add_caltime(filter, end_time_property_id, CALENDAR_MATCH_GREATER_THAN, caltimeStart);
		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}
		error = calendar_filter_add_operator(filter, CALENDAR_FILTER_OPERATOR_AND);
		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}
		error = calendar_filter_add_caltime(filter, start_time_property_id, CALENDAR_MATCH_LESS_THAN_OR_EQUAL, caltimeEnd);
		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}
		error = calendar_query_create(view_uri, &query2);
		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}
		error = calendar_query_set_filter(query2, filter);
	}
	while(0);

	if (filter)
	{
		calendar_filter_destroy(filter);
	}
	if (error != CALENDAR_ERROR_NONE)
	{
		if (query2)
		{
			calendar_query_destroy(query2);
		}
		WERROR("fail(%x)",error);
		if (query1)
		{
			calendar_query_destroy(query1);
		}
		return false;
	}

	calendar_list_h list = NULL;
	int count1 = 0;
	int count2 = 0;

	do
	{
		error = calendar_db_get_records_with_query(query1, 0, 0, &list);
		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}
		error = calendar_list_get_count(list, &count1);
		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}
		calendar_list_destroy(list, true);
		list = NULL;
		calendar_query_destroy(query1);
		query1 = NULL;

		error = calendar_db_get_records_with_query(query2, 0, 0, &list);
		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}
		error = calendar_list_get_count(list, &count2);
		if (error != CALENDAR_ERROR_NONE)
		{
			break;
		}
		calendar_list_destroy(list, true);
		list = NULL;
		calendar_query_destroy(query2);
		query2 = NULL;
	}
	while(0);

	if (error != CALENDAR_ERROR_NONE)
	{
		if (query1)
		{
			calendar_query_destroy(query1);
		}
		if (query2)
		{
			calendar_query_destroy(query2);
		}
		if (list)
		{
			calendar_list_destroy(list, true);
		}
		WERROR("fail(%x)",error);
		return false;
	}

	if (0 < count1 || 0 < count2)
	{
		return true;
	}

	return false;
}

void CalDataManager::__notify(CalEvent::Source source)
{
	if (source == CalEvent::LOCAL)
	{
		int error = calendar_db_get_last_change_version(&__localVersion);
		if (error != CALENDAR_ERROR_NONE)
		{
			WASSERT_EX(0, "calendar_db_get_last_change_version failed : %d", error);
			WERROR("calendar_db_get_last_change_version failed : %d", error);
			return;
		}
		WDEBUG("version = %d:%d", __localVersion, __version);
	}
	CalEvent event(CalEvent::DB_CHANGED, source);
	CalEventManager::getInstance().notify(event);
}

void CalDataManager::__notifyAlert(void)
{
	WENTER();
	app_control_h service = NULL;

	app_control_create(&service);
	app_control_set_app_id(service, CALENDAR_NOTI_PACKAGE);
	app_control_set_operation(service, APP_CONTROL_OPERATION_DEFAULT);
	app_control_add_extra_data(service, CAL_APPSVC_PARAM_CALLER, CAL_APPSVC_PARAM_CALLER_CALENDAR);

	int ret = app_control_send_launch_request(service, NULL, NULL);
	if( ret != APP_CONTROL_ERROR_NONE )
	{
		WERROR("app_control ret error code= %d", ret);
	}

	app_control_destroy(service);
}

void CalDataManager::__onChanged(const char* uri, void* userData)
{
	WENTER();
	CalDataManager& self = CalDataManager::getInstance();
	calendar_list_h changedList = NULL;
	int error = calendar_db_get_changes_by_version(_calendar_event._uri, -1, self.__localVersion, &changedList, &self.__version);
	//int error = calendar_db_get_current_version(&self.__version);
	calendar_list_destroy(changedList, true);
	if (error != CALENDAR_ERROR_NONE)
	{
		WASSERT_EX(0, "calendar_db_get_changes_by_version failed : %d", error);
		WERROR("calendar_db_get_changes_by_version failed : %d", error);
		return;
	}

	WDEBUG("version = %d:%d", self.__localVersion, self.__version);
	if (self.__localVersion == self.__version)
	{
		WDEBUG("already updated");
		return;
	}
	self.__notify(CalEvent::REMOTE);
}

void CalDataManager::__deleteExceptionRecordsOnAndAfter(const CalOriginalSchedule& originalSchedule, const CalDateTime& inputInstanceStartTime)
{
	calendar_record_h originalRecord = originalSchedule.getRecord();
	if(originalRecord)
	{
		char* exdate = NULL;
		int error = calendar_record_get_str(originalRecord, _calendar_event.exdate, &exdate);
		if (error == CALENDAR_ERROR_NONE && exdate && *exdate)
		{
			// 20150124,20150125
			std::string targetString(exdate);
			std::istringstream iss(targetString);
			std::string strEach, strNew;

			char delim = ',';
			int year, month, day;
			std::getline(iss, strEach, delim);
			{
				sscanf(strEach.c_str(),"%4d%2d%2d", &year, &month, &day);

				CalDateTime testDate(year, month, day);
				if(testDate < inputInstanceStartTime)
				{
					if(!strNew.empty())
						strNew += ",";

					strNew += strEach;
				}
			}

			calendar_record_set_str(originalRecord, _calendar_event.exdate, strNew.c_str());
		}
	}
}

void CalDataManager::__deleteAllExceptionRecords(const CalOriginalSchedule& originalSchedule)
{
	calendar_record_h originalRecord = originalSchedule.getRecord();
	if(originalRecord)
	{
		calendar_record_set_str(originalRecord, _calendar_event.exdate, NULL);
	}
}
