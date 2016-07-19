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

#include <algorithm>
#include <app_control.h>
#include "CalAlertData.h"
#include "CalAlertType.h"
#include "CalOriginalSchedule.h"
#include "CalStatusBarManager.h"

CalAlertData::CalAlertData(app_control_h request) :
	__tick(0),
	__unit(0)
{
	WENTER();
	char *value = NULL;
	app_control_get_extra_data(request, CAL_APPCALSVC_PARAM_TICK, &value);
	if (value)
	{
		__tick = atoi(value);
		free(value);
		value = NULL;
	}
	app_control_get_extra_data(request, CAL_APPCALSVC_PARAM_UNIT, &value);
	if (value)
	{
		__unit = atoi(value);
		free(value);
		value = NULL;
	}

	// ids
	char **ids = NULL;
	int len = 0;
	app_control_get_extra_data_array(request, CAL_APPCALSVC_PARAM_IDS, &ids, &len);
	WDEBUG("len(%d)", len);
	if (len > 0)
	{
		// calendar-service
		for (int i = 0; i < len; i++)
		{
			int recordIndex = atoi(ids[i]);
			__addRecord(recordIndex, false);
			WDEBUG("alert index[%d]",recordIndex);
		}

		if (ids)
		{
			free(ids);
		}

		return;
	}

	if (ids)
	{
		free(ids);
	}

	app_control_get_extra_data(request, CAL_APPSVC_PARAM_COUNT, &value);
	if (value)
	{
		free(value);
		value = NULL;
	}

	WDEBUG("alert count[%d]",getCount());
	if (getCount() > 1)
	{
		char key[ID_LENGTH] = {0};
		char snoozedKey[ID_LENGTH] = {0};
		int index = 0;
		char *snoozed = NULL;
		bool isSnoozed = false;

		while (getCount() != index)
		{
			snprintf(key, sizeof(key), "%d", index);
			app_control_get_extra_data(request, key, &value);

			snprintf(snoozedKey, sizeof(snoozedKey), "%d.%s", index, CAL_APPALERT_PARAM_IS_SNOOZED);

			if(APP_CONTROL_ERROR_NONE == app_control_get_extra_data(request, snoozedKey, &snoozed))
			{
				isSnoozed = snoozed && !strcmp(snoozed, CAL_APPALERT_IS_SNOOZED);
				free(snoozed);
			}

			if (value)
			{
				int recordIndex = atoi(value);
				free(value);
				value = NULL;
				__addRecord(recordIndex, isSnoozed);
				WDEBUG("alert index[%d]",recordIndex);
			}
			index++;
		}
	}
	else
	{
		app_control_get_extra_data(request, CAL_APPCALSVC_PARAM_ID, &value);
		if (value)
		{
			int recordIndex = atoi(value);
			free(value);
			value = NULL;
			__addRecord(recordIndex, false);
			WDEBUG("alert index[%d]",recordIndex);
		}
		else
		{
			WERROR("get value fail");
		}
	}
	WLEAVE();
}

CalAlertData::CalAlertData():
	__tick(0),
	__unit(0)
{
	WENTER();

	CalStatusBarManager::getInstance().getAllStatusBar(__alerts);
	WLEAVE();
}

CalAlertData::~CalAlertData()
{
	WENTER();
}

CalAlertData::CalAlertData(const CalAlertData& obj)
{
	__tick = obj.__tick;
	__unit = obj.__unit;
	__alerts = obj.__alerts;
}

const CalAlertData& CalAlertData::operator=(const CalAlertData& obj)
{
	if (this != &obj)
	{
		__tick = obj.__tick;
		__unit = obj.__unit;
		__alerts = obj.__alerts;
	}
	return *this;
}

int CalAlertData::getTick(void)
{
	WENTER();
	return __tick;
}

int CalAlertData::getUnit(void)
{
	WENTER();
	return __unit;
}

int CalAlertData::getCount(void)
{
	return __alerts.size();
}

std::shared_ptr<CalAlertNotificationItem> CalAlertData::getAt(int nth)
{
	WENTER();
	if (nth >= getCount())
	{
		WERROR("invalid input[%d/%d]", nth, getCount());
		return nullptr;
	}
	return __alerts[nth];
}

bool CalAlertData::isSnoozed(int nth)
{
	return __alerts[nth]->isSnoozed();
}

void CalAlertData::remove(int nth)
{
	WENTER();
	if (nth >= getCount())
	{
		WERROR("invalid input[%d/%d]", nth, getCount());
		return;
	}
	__alerts.erase(__alerts.begin() + nth);
}

bool CalAlertData::removeById(int id)
{

	WENTER();
	for (auto alertNotiItem = __alerts.begin() ; alertNotiItem != __alerts.end(); ++alertNotiItem)
	{
		if ((*alertNotiItem)->getRecordIndex() == id)
		{
			__alerts.erase(alertNotiItem);

			WLEAVE();
			return true;
		}
	}
	WLEAVE();
	return false;
}

void CalAlertData::clear()
{
	WENTER();
	__alerts.clear();
}

void CalAlertData::add(const CalAlertData& obj)
{
	WENTER();
	__tick = obj.__tick;
	__unit = obj.__unit;

	for (auto item : obj.__alerts)
	{
		auto result = std::find_if(__alerts.begin(), __alerts.end(),
			[item](const std::shared_ptr<CalAlertNotificationItem> &it) -> bool
			{
				return item->getScheduleId() == it->getScheduleId();
			});

		if(__alerts.end() == result)
		{
			__alerts.insert(__alerts.begin(), item);
		}
	}
}

void CalAlertData::replace(const CalAlertData& obj)
{
	WENTER();
	__tick = obj.__tick;
	__unit = obj.__unit;
	__alerts = obj.__alerts;
}

void CalAlertData::__addRecord(int recordIndex, bool isSnoozed)
{
	calendar_record_h record = nullptr;
	int error = calendar_db_get_record(_calendar_event._uri, recordIndex, &record);
	if (error == CALENDAR_ERROR_NONE)
	{
		auto item = std::make_shared<CalAlertNotificationItem>(recordIndex);
		item->setSnoozed(isSnoozed);
		__alerts.push_back(item);

		calendar_record_destroy(record, true);
	}
	else
	{
		WERROR("Record %d isn't found in the calendar database", recordIndex);
	}
}
