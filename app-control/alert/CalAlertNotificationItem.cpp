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

#include <sstream>

#include "CalAlertNotificationItem.h"
#include "CalOriginalSchedule.h"

CalAlertNotificationItem::CalAlertNotificationItem(int recordIndex):
	__isSnoozed(false),
	__alarmId(0),
	__recordIndex(recordIndex)
{
	WENTER();
	__getRecord(recordIndex);
}

CalAlertNotificationItem::~CalAlertNotificationItem()
{
	WENTER();
}

void CalAlertNotificationItem::setSnoozed(bool isSnoozed)
{
	WENTER();
	WDEBUG("setSnoozed to %s", isSnoozed ? "true" : "false");
	__isSnoozed = isSnoozed;
}

void CalAlertNotificationItem::setAlarmId(int alarmId)
{
	__alarmId = alarmId;
}

bool CalAlertNotificationItem::isSnoozed()
{
	WENTER();
	WDEBUG("isSnoozed %s", __isSnoozed ? "true" : "false");
	return __isSnoozed;
}

int CalAlertNotificationItem::getScheduleId()
{
	return __schedule->getIndex();
}

int CalAlertNotificationItem::getRecordIndex()
{
	return __recordIndex;
}

int CalAlertNotificationItem::getAlarmId()
{
	return __alarmId;
}

void CalAlertNotificationItem::__getRecord(int recordIndex)
{
	WENTER();
	if(recordIndex)
	{
		calendar_record_h record = NULL;
		int error = calendar_db_get_record(_calendar_event._uri, recordIndex, &record);
		if (error == CALENDAR_ERROR_NONE)
		{
			__schedule = std::make_shared<CalOriginalSchedule>(record);
		}
	}
}

const std::shared_ptr<CalSchedule> CalAlertNotificationItem::getSchedule()
{
	return __schedule;
}

char *CalAlertNotificationItem::getEventName()
{
	WENTER();
	const char *summary = __schedule->getSummary();
	const char *eventName = NULL;

	if (summary && *summary)
	{
		eventName = summary;
	}
	else
	{
		eventName = _L_("IDS_CLD_MBODY_MY_EVENT");
	}

	if (isSnoozed())
	{
		int newSize = strlen(eventName) + strlen(_L_("IDS_CLD_MBODY_PS_HSNOOZED_T_CALENDAR")) + 1;
		char *newName = (char*)calloc(1, newSize);
		snprintf(newName, newSize, _L_("IDS_CLD_MBODY_PS_HSNOOZED_T_CALENDAR"), eventName);
		return newName;
	}
	else
	{
		return strdup(eventName);
	}
}

char *CalAlertNotificationItem::getEventOriginalName()
{
	WENTER();
	const char *summary = __schedule->getSummary();
	return summary ? strdup(summary) : nullptr;
}

char *CalAlertNotificationItem::getStartTime()
{
	WENTER();
	CalDateTime startTime;
	__schedule->getStart(startTime);
	std::string startText;
	startTime.getTimeString(startText);

	return strdup(startText.c_str());
}

char *CalAlertNotificationItem::getEndTime()
{
	WENTER();
	CalDateTime endTime;
	__schedule->getEnd(endTime);
	std::string endText;
	endTime.getTimeString(endText);

	return strdup(endText.c_str());
}

char *CalAlertNotificationItem::getLocation()
{
	WENTER();
	const char *location = __schedule->getLocation();
	if (location && *location)
	{
		return strdup(location);
	}
	else
	{
		return strdup("");
	}
}
