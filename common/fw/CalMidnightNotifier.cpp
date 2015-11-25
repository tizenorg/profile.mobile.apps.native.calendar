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
#include <app_manager.h>

#include "CalMidnightNotifier.h"
#include "CalCommon.h"
#include "CalDateTime.h"
#include "CalEventManager.h"

#define CAL_MIDNIGHT_NOTIFIER_OPERATION	"CalMidnightNotifier"

SINGLETON_INSTANCE(CalMidnightNotifier);

CalMidnightNotifier::CalMidnightNotifier() :
	type(NONE),
	__type(NONE),
	__onMidnight(nullptr),
	__onMidnightData(nullptr),
	__timeout(0),
	__alarmId(0),
	__date(0)
{
	WENTER();
}

CalMidnightNotifier::~CalMidnightNotifier()
{
	WENTER();

	__unsetTimeout();
	__unsetAlarm();
}

void CalMidnightNotifier::init()
{
}

int CalMidnightNotifier::__setTimeout(const CalDate& date)
{
	__unsetTimeout();

	CalDateTime dateTime(date.getYear(), date.getMonth(), date.getMday(), 0, 0, 0);
	CalDateTime now;

	int interval = dateTime.getUtimeFromTm() - now.getUtimeFromTm();
	if (interval < 0)
	{
		return -1;
	}

	__timeout = g_timeout_add_seconds(interval, CalMidnightNotifier::__onTimeout, this);

	return 0;
}

int CalMidnightNotifier::__setAlarm(const CalDate& date)
{
	__unsetAlarm();

	char* app_id = NULL;
	int error = app_manager_get_app_id(getpid(), &app_id);
	if (error != APP_MANAGER_ERROR_NONE)
	{
		WERROR("app_manager_get_app_id(getpid()) is failed.");
		return -1;
	}
	if (!app_id)
	{
		WERROR("app_manager_get_app_id(getpid()) is failed.");
		return -1;
	}

	app_control_h request = NULL;
	app_control_create(&request);
	app_control_set_operation(request, CAL_MIDNIGHT_NOTIFIER_OPERATION);
	app_control_set_app_id(request, app_id);
	free(app_id);

	CalDateTime dateTime(date.getYear(), date.getMonth(), date.getMday(), 0, 0, 0);
	struct tm tm = {0};
	dateTime.getTmFromUtime(&tm);

	int alarm_id = 0;

	// todo: alarm_schedule_at_date doesn't exist in GBS build
#ifndef GBS_BUILD
	int ret = alarm_schedule_at_date(request, &tm, 0, &alarm_id);
	if (ret < 0)
	{
		WERROR("alarm_schedule_at_date() is failed(%d)", ret);
		return -1;
	}
#endif

	app_control_destroy(request);

	WDEBUG("alarm id = %d", alarm_id);
	__alarmId = alarm_id;

	return 0;
}

int CalMidnightNotifier::__unsetTimeout()
{
	if (__timeout)
	{
		g_source_remove(__timeout);
		__timeout = 0;
	}

	return 0;
}

int CalMidnightNotifier::__unsetAlarm()
{
	if (__alarmId)
	{
		// todo: alarm_cancel doesn't exist in GBS build
#ifndef GBS_BUILD
		alarm_cancel(__alarmId);
#endif
		__alarmId = 0;
	}

	return 0;
}

int CalMidnightNotifier::turnOn(Type type)
{
	WENTER();

	turnOn(type, NULL, NULL);

	return 0;
}

int CalMidnightNotifier::turnOn(Type type, std::function<int (void*)> onMidnight, void* onMidnightData)
{
	WENTER();

	CalDate date;
	date.incrementDay();

	turnOn(type, date, onMidnight, onMidnightData);

	return 0;
}

int CalMidnightNotifier::turnOn(Type type, const CalDate& date, std::function<int (void*)> onMidnight, void* onMidnightData)
{
	WENTER();

	__type = type;
	__onMidnight = onMidnight;
	__onMidnightData = onMidnightData;
	__date = date.getMday();

	if (CalEventManager::getInstance().isDetached(this))
	{
		CalEventManager::getInstance().attach(this);
	}

	switch (type)
	{
		case TIMEOUT:
			return __setTimeout(date);
		case ALARM:
			return __setAlarm(date);
		default:
			WERROR("Invaild type : %d", type);
			return -1;
	}
}

int CalMidnightNotifier::turnOff()
{
	WENTER();

	CalEventManager::getInstance().detach(this);

	__unsetTimeout();
	__unsetAlarm();

	__type = NONE;
	__onMidnight = NULL;
	__onMidnightData = NULL;

	return 0;
}

bool CalMidnightNotifier::isOnMidnight(app_control_h request)
{
	WENTER();

	bool ret = false;

	char* operation = NULL;
	app_control_get_operation(request, &operation);

	if (operation)
	{
		if (!strcmp(operation, CAL_MIDNIGHT_NOTIFIER_OPERATION))
		{
			ret = true;
		}

		free(operation);
	}

	return ret;
}

void CalMidnightNotifier::onMidnight()
{
	WENTER();

	CalEvent event(CalEvent::TIME_CHANGED, CalEvent::LOCAL);
	CalEventManager::getInstance().notify(event);

	if (__onMidnight)
	{
		__onMidnight(__onMidnightData);
	}

	turnOn(__type, __onMidnight, __onMidnightData);
}

gboolean CalMidnightNotifier::__onTimeout(gpointer data)
{
	WENTER();

	CalMidnightNotifier* pCalMidnightNotifier = (CalMidnightNotifier*) data;

	pCalMidnightNotifier->__timeout = 0;
	pCalMidnightNotifier->onMidnight();

	return false;
}

void CalMidnightNotifier::onEvent(const CalEvent& event)
{
	if (event.type != CalEvent::TIME_CHANGED)
	{
		return;
	}

	WENTER();

	CalDate date;
	date.incrementDay();

	if (__date != date.getMday() && __onMidnight)
	{
		__onMidnight(__onMidnightData);
	}

	if (__timeout || __alarmId)
	{
		turnOn(__type, __onMidnight, __onMidnightData);
	}
}
