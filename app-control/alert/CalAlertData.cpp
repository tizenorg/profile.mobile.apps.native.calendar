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
	__unit(0),
	__count(0)
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

	const int indexBuffer = 8;

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
			__addRecord(recordIndex);
			WDEBUG("alert index[%d]",recordIndex);
		}

		if (ids)
		{
			free(ids);
		}

		__count = __schedules.size();
		return;
	}

	if (ids)
	{
		free(ids);
	}

	app_control_get_extra_data(request, CAL_APPSVC_PARAM_COUNT, &value);
	if (value)
	{
		__count = atoi(value);
		free(value);
		value = NULL;
	}

	WDEBUG("alert count[%d]",__count);
	if (__count > 1)
	{
		char key[indexBuffer] = {0};
		int index = 0;
		while (__count != index)
		{
			snprintf(key, sizeof(key), "%d", index++);
			app_control_get_extra_data(request, key, &value);
			if (value)
			{
				int recordIndex = atoi(value);
				free(value);
				value = NULL;
				__addRecord(recordIndex);
				WDEBUG("alert index[%d]",recordIndex);
			}
		}
	}
	else
	{
		__count = 1;
		app_control_get_extra_data(request, CAL_APPCALSVC_PARAM_ID, &value);
		if (value)
		{
			int recordIndex = atoi(value);
			free(value);
			value = NULL;
			__addRecord(recordIndex);
			WDEBUG("alert index[%d]",recordIndex);
		}
		else
		{
			WERROR("get value fail");
		}
	}
	WLEAVE();
}

CalAlertData::CalAlertData()
{
	WENTER();
	__tick = 0;
	__unit = 0;
	__count = 0;

	std::vector<int> missed;
	CalStatusBarManager::getInstance().getAllStatusBar(missed);
	for (auto it = missed.begin() ; it != missed.end(); ++it)
	{
		int recordIndex = *it;
		__addRecord(recordIndex);
		WDEBUG("alert index[%d]",recordIndex);
	}
	__count = __schedules.size();

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
	__count = obj.__count;
	__schedules = obj.__schedules;
}

const CalAlertData& CalAlertData::operator=(const CalAlertData& obj)
{
	if (this != &obj)
	{
		__tick = obj.__tick;
		__unit = obj.__unit;
		__count = obj.__count;
		__schedules = obj.__schedules;
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
	WENTER();
	return __count;
}

std::shared_ptr<CalSchedule> CalAlertData::getAt(int nth)
{
	WENTER();
	if (nth >= __count)
	{
		WERROR("invalid input[%d/%d]",nth,__count);
		return nullptr;
	}
	std::shared_ptr<CalSchedule> schedule = __schedules[nth];
	return schedule;
}

void CalAlertData::remove(int nth)
{
	WENTER();
	if (nth >= __count)
	{
		WERROR("invalid input[%d/%d]",nth,__count);
		return;
	}
	__schedules.erase(__schedules.begin()+nth);
	__count--;
}

void CalAlertData::removeById(int id)
{
	WENTER();
	for (auto schedule = __schedules.begin(); schedule != __schedules.end();)
	{
		if ((*schedule)->getIndex() == id)
		{
			schedule = __schedules.erase(schedule);
		}
		else
		{
			++schedule;
		}
	}
	__count = __schedules.size();
	WLEAVE();
}

void CalAlertData::clear()
{
	WENTER();
	__schedules.clear();
	__count = __schedules.size();
}

void CalAlertData::add(const CalAlertData& obj)
{
	WENTER();
	__tick = obj.__tick;
	__unit = obj.__unit;
	__count = __count + obj.__count;

	for (auto it = obj.__schedules.begin(); it != obj.__schedules.end(); ++it)
	{
		__schedules.push_back(*it);
	}
}

void CalAlertData::replace(const CalAlertData& obj)
{
	WENTER();
	__tick = obj.__tick;
	__unit = obj.__unit;
	__count = obj.__count;
	__schedules = obj.__schedules;
}

void CalAlertData::__addRecord(int recordIndex)
{
	calendar_record_h record = NULL;
	int error = calendar_db_get_record(_calendar_event._uri, recordIndex, &record);
	if (error == CALENDAR_ERROR_NONE)
	{
		std::shared_ptr<CalSchedule> original(std::make_shared<CalOriginalSchedule>(record));
		__schedules.push_back(original);
	}
}
