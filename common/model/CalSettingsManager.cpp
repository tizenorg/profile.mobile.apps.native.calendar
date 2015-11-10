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
#include <app_preference.h>
#include "CalSettingsManager.h"
#include "CalEventManager.h"
#include "CalCommon.h"
#include "CalLocaleManager.h"


#ifndef CALENDAR_PREFERENCE_PATH
#define CALENDAR_PREFERENCE_PATH "db/private/" CAL_PKGNAME"/"
#endif

#define CAL_SETTING_FIRST_DAY_OF_WEEK_KEY           CALENDAR_PREFERENCE_PATH"first_day_of_week" 	// locales, sat, sun, mon
#define CAL_SETTING_LOCK_TIME_ZONE_KEY              CALENDAR_PREFERENCE_PATH"timezone_on_off"		// on, off
#define CAL_SETTING_TIME_ZONE_KEY                   CALENDAR_PREFERENCE_PATH"timezone_path"			// string
#define CAL_SETTING_SELECT_ALERT_TYPE_KEY           CALENDAR_PREFERENCE_PATH"select_alert_type"		// alert, status bar, off
#define CAL_SETTING_NOTIFICATION_SOUND_KEY          CALENDAR_PREFERENCE_PATH"notification_sound"	// string
#define CAL_SETTING_LAST_USED_CALENDAR_KEY          CALENDAR_PREFERENCE_PATH"last_used_calendar"	// int

/*
 * default value
 * First day of week : locale
 * Lock time zone : On ? -> off !!
 * Select alert type : Status bar notification
 */

#define CAL_SETTING_FIRST_DAY_OF_WEEK_DEFAULT		LOCALES
#define CAL_SETTING_LOCK_TIME_ZONE_DEFAULT			false
#define CAL_SETTING_TIME_ZONE_DEFAULT 				NULL
#define CAL_SETTING_SELECT_ALERT_TYPE_DEFAULT 		STATUS_BAR_NOTIFICATION
#define CAL_SETTING_LAST_USED_CALENDAR_DEFAULT		1

SINGLETON_INSTANCE(CalSettingsManager);

CalSettingsManager::CalSettingsManager()
{
	WENTER();

	__isHour24 = NULL;
}

CalSettingsManager::~CalSettingsManager()
{
	WENTER();

	system_settings_unset_changed_cb(SYSTEM_SETTINGS_KEY_LOCALE_TIMEFORMAT_24HOUR);
	system_settings_unset_changed_cb(SYSTEM_SETTINGS_KEY_LOCALE_TIMEZONE);
	system_settings_unset_changed_cb(SYSTEM_SETTINGS_KEY_TIME_CHANGED);
}

void CalSettingsManager::init()
{
	WENTER();

	bool check = false;
	if (PREFERENCE_ERROR_NONE == preference_is_existing(CAL_SETTING_FIRST_DAY_OF_WEEK_KEY, &check))
	{
		if (!check)
		{
			setFirstDayOfWeek(CAL_SETTING_FIRST_DAY_OF_WEEK_DEFAULT);
		}
	}
	else
	{
		WERROR ("Failing of existence of key %s", CAL_SETTING_FIRST_DAY_OF_WEEK_KEY);
	}

	if (PREFERENCE_ERROR_NONE
			== preference_is_existing(CAL_SETTING_TIME_ZONE_KEY, &check))
	{
		if (!check)
		{
			setLockTimeZone(CAL_SETTING_TIME_ZONE_DEFAULT);
		}
	}
	else
	{
		WERROR("Failing of existence of key %s", CAL_SETTING_TIME_ZONE_KEY);
	}

	if (PREFERENCE_ERROR_NONE
			== preference_is_existing(CAL_SETTING_SELECT_ALERT_TYPE_KEY, &check))
	{
		if (!check)
		{
			setAlertType(ALERT);
		}
	}
	else
	{
		WERROR("Failing of existence of key %s",
				CAL_SETTING_SELECT_ALERT_TYPE_KEY);
	}

	if (PREFERENCE_ERROR_NONE
			== preference_is_existing(CAL_SETTING_TIME_ZONE_KEY, &check))
	{
		if (!check)
		{
			setLockTimeZone(CAL_SETTING_TIME_ZONE_DEFAULT);
		}
	}
	else
	{
		WERROR("Failing of existence of key %s", CAL_SETTING_TIME_ZONE_KEY);
	}

	if (PREFERENCE_ERROR_NONE
			== preference_is_existing(CAL_SETTING_LAST_USED_CALENDAR_KEY, &check))
	{
		if (!check)
		{
			setLastUsedCalendar(CAL_SETTING_LAST_USED_CALENDAR_DEFAULT);
		}
	}
	else
	{
		WERROR("Failing of existence of key %s",
				CAL_SETTING_LAST_USED_CALENDAR_KEY);
	}

	__alertSound = std::string();
	__setChangedCb(CAL_SETTING_FIRST_DAY_OF_WEEK_KEY, CalEvent::FIRST_DAY_OF_WEEK_CHANGED);
	__setChangedCb(CAL_SETTING_LOCK_TIME_ZONE_KEY, CalEvent::SETTING_CHANGED);
	__setChangedCb(CAL_SETTING_TIME_ZONE_KEY, CalEvent::TIME_CHANGED);

	__setHour24();
	__setLocaleTimeZone();

	system_settings_set_changed_cb(SYSTEM_SETTINGS_KEY_LOCALE_TIMEFORMAT_24HOUR, __systemSettingsChangeCb, NULL);
	system_settings_set_changed_cb(SYSTEM_SETTINGS_KEY_LOCALE_TIMEZONE, __systemSettingsChangeCb, NULL);
	system_settings_set_changed_cb(SYSTEM_SETTINGS_KEY_TIME_CHANGED, __systemSettingsChangeCb, NULL);
}

void CalSettingsManager::__changedCb(const char *key, void *userData)
{
	WENTER();

	CalEvent::Type* type = (CalEvent::Type*)&userData;
	if (*type == CalEvent::TIME_CHANGED)
	{
		bool isLockTimeZone = CalSettingsManager::getInstance().getLockTimeZone();
		if (isLockTimeZone == false)
		{
			return;
		}
	}

	CalSettingsManager::getInstance().__notify(*type, CalEvent::LOCAL);
}

void CalSettingsManager::__setChangedCb(const char* key, CalEvent::Type type)
{
	WENTER();
	preference_set_changed_cb(key, __changedCb, (void*)type);
}

void CalSettingsManager::__notify(CalEvent::Type type, CalEvent::Source source)
{
	WENTER();
	WDEBUG("type=%d, source=%d", type, source);
	CalEvent event(type, source);
	CalEventManager::getInstance().notify(event);
}

void CalSettingsManager::__setHour24(void)
{
	// Indicates whether the 24-hour clock is used. If the value is false, the 12-hour clock is used
	int error = system_settings_get_value_bool(SYSTEM_SETTINGS_KEY_LOCALE_TIMEFORMAT_24HOUR, &__isHour24);
	if(error != SYSTEM_SETTINGS_ERROR_NONE)
	{
		WERROR("get SYSTEM_SETTINGS_KEY_LOCALE_TIMEFORMAT_24HOUR [%d]", error);
	}

	WDEBUG("%d",__isHour24);
}

void CalSettingsManager::__setLocaleTimeZone(void)
{
	bool isLockTimeZone = getLockTimeZone();
	std::string timeZone;
	std::string localeTimeZone;
	if (isLockTimeZone == false)
	{
		getDeviceTimeZone(timeZone);
	}
	else
	{
		 getLockTimeZone(timeZone);
	}
	CalLocaleManager::getInstance().getTimeZone(localeTimeZone);

	if (timeZone.compare(localeTimeZone) != 0)
	{
		CalLocaleManager::getInstance().setTimeZone(timeZone);
	}
}

void CalSettingsManager::getDeviceTimeZone(std::string& timeZone)
{
	WENTER();
	const int timeZoneBuffer = 256;
	char buf[timeZoneBuffer] = {0};

	ssize_t len = readlink("/opt/etc/localtime", buf, sizeof(buf) - 1);
	if (len == -1)
	{
		return;
	}

	buf[len] = '\0';

	// ex) /usr/share/zoneinfo/Asia/Seoul
	timeZone = buf;
	timeZone.erase(0, 20);
	WDEBUG("deviceTimeZone[%s]",timeZone.c_str());
	return ;
}

void CalSettingsManager::__notifyTimeChanged(void)
{
	CalEvent event(CalEvent::TIME_CHANGED, CalEvent::REMOTE);
	CalEventManager::getInstance().notify(event);

	return;
}

int CalSettingsManager::getFirstDayOfWeek(void)
{
	int value = CAL_SETTING_FIRST_DAY_OF_WEEK_DEFAULT;
	int error = preference_get_int(CAL_SETTING_FIRST_DAY_OF_WEEK_KEY, &value);
	if (error != PREFERENCE_ERROR_NONE)
	{
		WERROR("preference_get_int is failed(%x)", error);
		return CAL_SETTING_FIRST_DAY_OF_WEEK_DEFAULT;
	}

	return value;
}

bool CalSettingsManager::getLockTimeZone(void)
{
	int value = 0;
	int error = 0;

	error = preference_get_int(CAL_SETTING_LOCK_TIME_ZONE_KEY, &value);
	if (error != PREFERENCE_ERROR_NONE)
	{
		WERROR("preference_get_int is failed (%x)", error);
		return CAL_SETTING_LOCK_TIME_ZONE_DEFAULT;
	}
	else
	{
		return (value != 0) ? true:false;
	}
}

void CalSettingsManager::getLockTimeZone(std::string& timezone)
{
	char *value = NULL;
	bool isLockTimeZone = getLockTimeZone();

	int error = preference_get_string(CAL_SETTING_TIME_ZONE_KEY, &value);
	if (error != PREFERENCE_ERROR_NONE || value == NULL || strlen(value) == 0 || isLockTimeZone == false)
	{
		WHIT();
		std::string deviceTz;
		getDeviceTimeZone(deviceTz);
		setLockTimeZone(deviceTz);
		timezone = deviceTz;
	}
	else
	{
		timezone = value;
	}

	free(value);
}

CalSettingsManager::AlertType CalSettingsManager::getAlertType(void)
{
	int value = 0;
	int error = 0;

	error = preference_get_int(CAL_SETTING_SELECT_ALERT_TYPE_KEY, &value);
	if (error != PREFERENCE_ERROR_NONE)
	{
		WERROR("preference_get_int is failed(%x)", error);
		return CAL_SETTING_SELECT_ALERT_TYPE_DEFAULT;
	}

	return (CalSettingsManager::AlertType)value;
}

const char* CalSettingsManager::getAlertSound(void)
{
	char *value = NULL;

	__alertSound = std::string();
	int error = preference_get_string(CAL_SETTING_NOTIFICATION_SOUND_KEY, &value);
	if (error != PREFERENCE_ERROR_NONE || value == NULL || strlen(value) == 0)
	{
		error = system_settings_get_value_string(SYSTEM_SETTINGS_KEY_SOUND_NOTIFICATION, &value);
		WDEBUG("get default ringtone(%s)", value);
		if (error != SYSTEM_SETTINGS_ERROR_NONE)
		{
			WERROR("CalSettingsManager::getAlertSound fail");
			__alertSound = CAL_SETTING_NOTIFICATION_SILENT;
		}
		else
			__alertSound = value;
	}
	else
	{
		__alertSound = value;
	}

	free(value);

	return __alertSound.c_str();
}

int CalSettingsManager::getLastUsedCalendar(void)
{
	int value = 0;
	int error = 0;

	error = preference_get_int(CAL_SETTING_LAST_USED_CALENDAR_KEY, &value);
	if (error != PREFERENCE_ERROR_NONE)
	{
		WERROR("preference_get_int is failed(%x)", error);
		return CAL_SETTING_LAST_USED_CALENDAR_DEFAULT;
	}

	return value;
}

void CalSettingsManager::setFirstDayOfWeek(const FirstDayOfWeekType type)
{
	int error = preference_set_int(CAL_SETTING_FIRST_DAY_OF_WEEK_KEY, type);
	if (error != PREFERENCE_ERROR_NONE)
	{
		WERROR("preference_set_int is failed(%x)", error);
	}
}

void CalSettingsManager::setLockTimeZone(const bool isOn)
{
	int error = 0;
	int value = isOn?1:0;

	error = preference_set_int(CAL_SETTING_LOCK_TIME_ZONE_KEY, value);
	if (error != PREFERENCE_ERROR_NONE)
	{
		WERROR("preference_set_int is failed(%x)", error);
	}

	if (isOn)
	{
		char *value = NULL;
		int error = preference_get_string(CAL_SETTING_TIME_ZONE_KEY, &value);
		if (error != PREFERENCE_ERROR_NONE || value == NULL || strlen(value) == 0)
		{
			// this is first set
			std::string deviceTz;
			getDeviceTimeZone(deviceTz);
			setLockTimeZone(deviceTz);
		}

		free(value);
	}

	__setLocaleTimeZone();

	CalEvent event(CalEvent::TIME_CHANGED, CalEvent::REMOTE);
	CalEventManager::getInstance().notify(event);
}

void CalSettingsManager::setLockTimeZone(const std::string& timezone)
{
	int error = 0;

	error = preference_set_string(CAL_SETTING_TIME_ZONE_KEY, timezone.c_str());
	if (error != PREFERENCE_ERROR_NONE)
		WERROR("preference_set_string is failed(%x)", error);

	__setLocaleTimeZone();
}

void CalSettingsManager::setAlertType(const AlertType type)
{
	int error = 0;

	error = preference_set_int(CAL_SETTING_SELECT_ALERT_TYPE_KEY, (int)type);
	if (error != PREFERENCE_ERROR_NONE)
		WERROR("preference_set_int is failed(%x)", error);
}

void CalSettingsManager::setAlertSound(const char* sound)
{
	int error = 0;

	error = preference_set_string(CAL_SETTING_NOTIFICATION_SOUND_KEY, sound);
	if (error != PREFERENCE_ERROR_NONE)
		WERROR("preference_set_string is failed(%x)", error);
}

void CalSettingsManager::setLastUsedCalendar(const int bookId)
{
	int error = 0;

	error = preference_set_int(CAL_SETTING_LAST_USED_CALENDAR_KEY, (int)bookId);
	if (error != PREFERENCE_ERROR_NONE)
		WERROR("preference_set_int is failed(%x)", error);
}

bool CalSettingsManager::isHour24(void)
{
	return __isHour24;
}

void CalSettingsManager::setHour24(void)
{
	__setHour24();
}

void CalSettingsManager::getCalendarTimeZone(std::string& timezone)
{
	CalLocaleManager::getInstance().getTimeZone(timezone);
}

void CalSettingsManager::updateRegion(void)
{
	CalSettingsManager::getInstance().setHour24();
	CalLocaleManager::getInstance().updateRegion();
}

void CalSettingsManager::__systemSettingsChangeCb(system_settings_key_e key, void *user_data)
{
	WHIT();
	CalSettingsManager::getInstance().updateRegion();
	CalEvent event(CalEvent::SETTING_CHANGED, CalEvent::REMOTE);
	CalEventManager::getInstance().notify(event);
}
