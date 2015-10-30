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

#include <app_alarm.h>
#include <app_preference.h>
#include <device/display.h>
#include <device/power.h>
#include "CalAlertModel.h"
#include "CalCommon.h"
#include "CalStatusBarManager.h"
#include "CalAlertType.h"

#define DEFAULT_SNOOZE_TIME 5 // in minutes

CalAlertModel::CalAlertModel(std::shared_ptr<CalAlertData> alertData)
{
	WENTER();
	__alertData = alertData;
}

CalAlertModel::~CalAlertModel()
{
	WENTER();
}

void CalAlertModel::addAlertData(const std::shared_ptr<CalAlertData>& alertData)
{
	WENTER();
	__alertData->add(*alertData);
}

void CalAlertModel::relaceAlertData(const std::shared_ptr<CalAlertData>& alertData)
{
	WENTER();
	__alertData->replace(*alertData);
}

int CalAlertModel::getCount(void)
{
	WENTER();
	return __alertData->getCount();
}

std::shared_ptr<CalSchedule> CalAlertModel::getAt(int nth)
{
	WENTER();
	return __alertData->getAt(nth);
}

void CalAlertModel::snoozeAll(void)
{
	WENTER();
	int count =  __alertData->getCount();
	if (count <= 0)
	{
		WERROR("count[%d]",count);
		return ;
	}

	int recordIndex = 0;
	for (int i=0;i<count;i++)
	{
		std::shared_ptr<CalSchedule> schedule = __alertData->getAt(i);
		if(schedule)
		{
			recordIndex = schedule->getIndex();
			__snooze(recordIndex);
		}
	}
}

void CalAlertModel::snooze(int nth)
{
	WENTER();
	std::shared_ptr<CalSchedule> schedule = __alertData->getAt(nth);

	if (schedule == NULL)
	{
		WERROR("fail");
		return;
	}

	__snooze(schedule->getIndex());
}

void CalAlertModel::dismissAll(void)
{
	WENTER();
	int count = __alertData->getCount();
	if(count > 0)
	{
		__alertData->clear();
		CalStatusBarManager::getInstance().removeNotification();
	}
}

void CalAlertModel::dismiss(int nth)
{
	WENTER();
	std::shared_ptr<CalSchedule> schedule = __alertData->getAt(nth);
	if (schedule == NULL)
	{
		WERROR("fail");
		return;
	}
	int recordIndex = schedule->getIndex();
	CalStatusBarManager::getInstance().removeFromNotification(recordIndex);
}

void CalAlertModel::remove(int nth)
{
	WENTER();
	__alertData->remove(nth);
}

void CalAlertModel::turnOnLcd(void)
{
	int error = device_power_wakeup(false);
	if (error != DEVICE_ERROR_NONE)
	{
		WERROR("device_power_wakeup error %d", error);
	}
	// set display normal
	display_state_e state = DISPLAY_STATE_NORMAL;
	device_display_get_state(&state);
	if (state != DISPLAY_STATE_NORMAL)
	{
		device_display_change_state(DISPLAY_STATE_NORMAL);
	}
	WDEBUG("display state: %d -> %d", state, DISPLAY_STATE_NORMAL);
	// power
	WHIT();
	error = device_power_request_lock(POWER_LOCK_DISPLAY, 60*1000);
	if (error != DEVICE_ERROR_NONE)
	{
		WERROR("device_power_request_lock error %d", error);
	}
}

void CalAlertModel::turnOffLcd(void)
{
	display_state_e state = DISPLAY_STATE_NORMAL;
	device_display_get_state(&state);
	if (state != DISPLAY_STATE_SCREEN_DIM)
	{
		device_display_change_state(DISPLAY_STATE_SCREEN_DIM);
	}
	WDEBUG("display state: %d -> %d", state ,DISPLAY_STATE_SCREEN_DIM);
	int ret = device_power_release_lock(POWER_LOCK_DISPLAY);
	if (ret != 0)
	{
		WERROR("fail");
	}
}

bool CalAlertModel::isScreenOn(void)
{
	display_state_e state = DISPLAY_STATE_NORMAL;
	int ret = device_display_get_state(&state);
	if (ret != DEVICE_ERROR_NONE)
	{
		WERROR("device_display_get_state error %d", ret);
		return false;
	}
	WDEBUG("display state: %d", state);
	return (state == DISPLAY_STATE_NORMAL);
}

void CalAlertModel::setSnoozeMinute(const int min)
{
	WENTER();
	int error = PREFERENCE_ERROR_NONE;

	error = preference_set_int(CALENDAR_ALERT_NOTI_MIN_KEY, min);
	if (error != PREFERENCE_ERROR_NONE)
	{
		WERROR("preference_set_int is failed(%x)", error);
	}
}

int CalAlertModel::getSnoozeMinute(void)
{
	WENTER();
	int error = PREFERENCE_ERROR_NONE;
	int min;
	error = preference_get_int(CALENDAR_ALERT_NOTI_MIN_KEY, &min);
	if (error != PREFERENCE_ERROR_NONE)
	{
		min = DEFAULT_SNOOZE_TIME;
	}
	WDEBUG("%d", min);

	return min;
}

void CalAlertModel::__snooze(int recordIndex)
{
	WENTER();
	WDEBUG("index[%d]",recordIndex);

	const int indexBuffer = 8;

	app_control_h service = NULL;

	app_control_create(&service);
	app_control_set_app_id(service, CALENDAR_NOTI_PACKAGE);
	app_control_set_operation(service, APP_CONTROL_OPERATION_DEFAULT);
	char value[indexBuffer] = {0};
	snprintf(value, sizeof(value), "%s", CAL_APPSVC_PARAM_COUNT_SINGLE);
	app_control_add_extra_data(service, CAL_APPSVC_PARAM_COUNT, value);
	snprintf(value, sizeof(value), "%d", recordIndex);
	app_control_add_extra_data(service, CAL_APPSVC_PARAM_ZERO, value);
	app_control_add_extra_data(service, CAL_APPSVC_PARAM_REMINDER_ID, value);

	char* ids[CAL_APPCALSVC_PARAM_IDS_DEFAULT_ARRAY_LENGTH];
	ids[0] = (char*) value;
	app_control_add_extra_data_array(service, CAL_APPCALSVC_PARAM_IDS, (const char**) ids, CAL_APPCALSVC_PARAM_IDS_DEFAULT_ARRAY_LENGTH);
	WDEBUG("index[%s]", ids[0]);
	time_t now = time(NULL);
	int min = getSnoozeMinute();
	now += min * 60;

	struct tm tm_now = {0};
	localtime_r(&now, &tm_now);

	int alarm_id = 0;
	int error = ALARM_ERROR_NONE;
	error = alarm_schedule_once_after_delay(service, getSnoozeMinute() * 60, &alarm_id);
	if (error != ALARM_ERROR_NONE)
	{
		WERROR("fail");
	}
	WDEBUG("alarm_id[%d]", alarm_id);

	app_control_destroy(service);
}
