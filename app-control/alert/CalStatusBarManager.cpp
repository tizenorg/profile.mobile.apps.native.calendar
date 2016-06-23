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

#include <app_preference.h>
#include <vector>
#include <sstream>

#include "CalStatusBarManager.h"
#include "CalCommon.h"
#include "CalSettingsManager.h"
#include "CalLocaleManager.h"
#include "CalAlertType.h"
#include "CalPath.h"

#define INDEX_BUFFER 8
#define APP_MAX_NOTI_COUNT   16
#define APP_MAX_NOTI_BUFFER  sizeof(CALENDAR_NOTI_PACKAGE) + 16

SINGLETON_INSTANCE(CalStatusBarManager);

CalStatusBarManager::CalStatusBarManager()
{
	WENTER();
}

CalStatusBarManager::~CalStatusBarManager()
{
	WENTER();
}

void CalStatusBarManager::pushNotification(std::shared_ptr<CalAlertData> &alertData, bool addSound)
{
	WENTER();
	notification_h notification = __getHandle();
	WDEBUG("Notification: %p", notification);
	app_control_h service = NULL;

	if (NULL == notification)
	{
		WDEBUG("Create new notification");
		notification = notification_create(NOTIFICATION_TYPE_NOTI);
		__setTag(notification);
		service = __createAppControl();
	}
	else
	{
		WDEBUG("Notification already on status bar");
		notification_get_launch_option(notification, NOTIFICATION_LAUNCH_OPTION_APP_CONTROL, (void*)&service);
		if (service)
		{
			__getNotificationData(alertData);
		}
		else
		{
			WDEBUG("There is no app-control");
		}
	}

	__setNotificationData(service, alertData);
	notification_set_launch_option(notification, NOTIFICATION_LAUNCH_OPTION_APP_CONTROL, service);

	if(addSound)
	{
		__addSound(notification);
		__addVib(notification);
	}

	__setupStatusBarNotification(notification, alertData);
	__postNotification(notification);

	notification_free(notification);
	app_control_destroy(service);
}

void CalStatusBarManager::pushActiveNotification(const std::shared_ptr<CalAlertData>& alertData, bool addSound)
{
	WENTER();
	int count = alertData->getCount();

	for (int i = 0; i < count; i++)
	{
		auto alertItem = alertData->getAt(i);
		auto schedule = alertItem->getSchedule();
		WPRET_M((schedule == NULL),"invalid param");
		__pushActiveNotification(schedule, addSound);
	}
}

void CalStatusBarManager::update(std::shared_ptr<CalAlertData>& alertData)
{
	WENTER();
	notification_h notification = __getHandle();

	if (notification != NULL)
	{
		if (alertData->getCount() > 0)
		{
			app_control_h service = NULL;
			notification_get_launch_option(notification, NOTIFICATION_LAUNCH_OPTION_APP_CONTROL, (void*)&service);
			if(service)
			{
				__setupStatusBarNotification(notification, alertData);
				__setNotificationData(service, alertData);
				notification_set_launch_option(notification, NOTIFICATION_LAUNCH_OPTION_APP_CONTROL, service);
				notification_update(notification);
				app_control_destroy(service);
			}

			notification_update(notification);
		}
		else
		{
			WDEBUG("No more events! Delete notification");
			notification_delete(notification);
			notification_free(notification);
		}
	}
}

void CalStatusBarManager::init()
{
}

void CalStatusBarManager::removeFromNotification(const int id)
{
	WENTER();
	notification_h notification = __getHandle();

	if (notification != NULL)
	{
		WDEBUG("Remove id[%d] from notification", id);

		app_control_h service = NULL;
		service = __createAppControl();

		std::shared_ptr<CalAlertData> alertData = std::make_shared<CalAlertData>();
		alertData->removeById(id);

				if (alertData->getCount() == 0)
				{
					WDEBUG("No more events! Delete notification");
					notification_delete(notification);
					app_control_destroy(service);
					return;
				}

				__setNotificationData(service, alertData);
				__setNotificationTitle(notification, alertData);

				notification_set_launch_option(notification, NOTIFICATION_LAUNCH_OPTION_APP_CONTROL, service);
				notification_update(notification);

		notification_update(notification);
		notification_free(notification);
	}
	WLEAVE();
}

void CalStatusBarManager::removeFromNotification(std::vector<int> &idsToRemove)
{
	WENTER();
	notification_h notification = __getHandle();

	if (notification != NULL)
	{
		WDEBUG("Remove %d IDs from notification", idsToRemove.size());

		app_control_h service = NULL;
		service = __createAppControl();

		std::shared_ptr<CalAlertData> alertData = std::make_shared<CalAlertData>();
		for (auto id : idsToRemove)
		{
			alertData->removeById(id);
		}

		if (alertData->getCount() == 0)
		{
			notification_delete(notification);
			return;
		}
		__setNotificationData(service, alertData);
		__setNotificationTitle(notification, alertData);

		if (alertData->getCount() == 0)
		{
			notification_delete(notification);
			app_control_destroy(service);
			return;
		}
		__setNotificationData(service, alertData);
		__setNotificationTitle(notification, alertData);

		notification_set_launch_option(notification, NOTIFICATION_LAUNCH_OPTION_APP_CONTROL, (void *)service);
		notification_update(notification);

		app_control_destroy(service);
		notification_free(notification);
	}
	WLEAVE();
}

void CalStatusBarManager::checkDeletedEvent()
{
	WENTER();
	std::vector<int> missed;
	for (auto it = missed.begin() ; it != missed.end(); ++it)
	{
		calendar_record_h record = NULL;
		int recordIndex = *it;
		int error = calendar_db_get_record(_calendar_event._uri, recordIndex, &record);
		if (error != CALENDAR_ERROR_NONE)
		{
			WDEBUG("remove[%d]",recordIndex);
			removeFromNotification(recordIndex);
		}

		if (record)
		{
			calendar_record_destroy(record, true);
		}
	}
	WLEAVE();
}

void CalStatusBarManager::__addVib(notification_h notification)
{
	WENTER();
	notification_set_vibration(notification, NOTIFICATION_VIBRATION_TYPE_DEFAULT, NULL);
}

void CalStatusBarManager::__addSound(notification_h notification)
{
	WENTER();
	std::string sound;
	__getSound(sound);
	if (!sound.empty())
	{
		WDEBUG("sound:%s",sound.c_str());
		if (!strcmp(sound.c_str(), CAL_SETTING_NOTIFICATION_SOUND_DEFAULT))
		{
			char *value = NULL;
			int result = system_settings_get_value_string(SYSTEM_SETTINGS_KEY_SOUND_NOTIFICATION, &value);
			if (result == SYSTEM_SETTINGS_ERROR_NONE && value)
			{
				WDEBUG("get default ringtone(%s)", value);
				notification_set_sound(notification, NOTIFICATION_SOUND_TYPE_USER_DATA, value);
			}
			else
			{
				WERROR("fail");
				notification_set_sound(notification, NOTIFICATION_SOUND_TYPE_USER_DATA, NULL);
			}

			free(value);
		}
		else
		{
			notification_set_sound(notification, NOTIFICATION_SOUND_TYPE_USER_DATA, sound.c_str());
		}
	}
	else
	{
		WERROR("sound is null");
		notification_set_sound(notification, NOTIFICATION_SOUND_TYPE_NONE, NULL);
	}
}

void CalStatusBarManager::__getSound(std::string& sound)
{
	WENTER();
	const char* alertSound = CalSettingsManager::getInstance().getAlertSound();
	if (alertSound)
	{
		sound = alertSound;
	}
}

void CalStatusBarManager::__setupStatusBarNotification(notification_h notification, const std::shared_ptr<CalAlertData> &alertData)
{
	WENTER();

	__setNotificationTitle(notification, alertData);

	notification_set_image(notification,
		NOTIFICATION_IMAGE_TYPE_ICON,
		CalPath::getPath(CAL_NOTIFICATION_ICON, CalPath::DirType::SHARED_RESOURCE).c_str());

	notification_set_image(notification,
		NOTIFICATION_IMAGE_TYPE_ICON_FOR_INDICATOR,
		CalPath::getPath(CAL_INDICATOR_ICON, CalPath::DirType::SHARED_RESOURCE).c_str());

	notification_set_property(notification, NOTIFICATION_PROP_DISABLE_AUTO_DELETE);

	WLEAVE();
}

void CalStatusBarManager::__pushActiveNotification(const std::shared_ptr<CalSchedule>& schedule, bool addSound)
{
	WENTER();
	const char *event_name = schedule->getSummary();
	notification_h notification = notification_create(NOTIFICATION_TYPE_NOTI);

	if (event_name && *event_name)
	{
		CalDateTime startTime;
		std::string timeBuff;
		std::stringstream title;

		schedule->getStart(startTime);
		startTime.getTimeString(timeBuff);
		title << timeBuff;

		const char * location = schedule->getLocation();
		if (location)
		{
			if (strlen(location))
			{
				title << ", ";
				title << location;
			}
		}
		notification_set_text(notification, NOTIFICATION_TEXT_TYPE_INFO_1, event_name, NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
		notification_set_text(notification, NOTIFICATION_TEXT_TYPE_TITLE, title.str().c_str(), NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
	}
	else
	{
		notification_set_text(notification, NOTIFICATION_TEXT_TYPE_INFO_1, _L_("IDS_CLD_MBODY_MY_EVENT"), NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
	}

	if (addSound)
	{
		__addSound(notification);
		__addVib(notification);
	}

	notification_set_layout(notification, NOTIFICATION_LY_NOTI_EVENT_SINGLE);

	notification_set_image(notification,
		NOTIFICATION_IMAGE_TYPE_ICON,
		CalPath::getPath(CAL_ACTIVENOTIFICATION_ICON, CalPath::DirType::SHARED_RESOURCE).c_str());

	notification_add_button(notification, NOTIFICATION_BUTTON_1);
	notification_set_text(notification, NOTIFICATION_TEXT_TYPE_BUTTON_1, _L_("IDS_CLD_BUTTON_DISMISS"), NULL, NOTIFICATION_VARIABLE_TYPE_NONE);

	notification_add_button(notification, NOTIFICATION_BUTTON_2);
	notification_set_text(notification, NOTIFICATION_TEXT_TYPE_BUTTON_2, _L_("IDS_CLD_BUTTON_SNOOZE"), NULL, NOTIFICATION_VARIABLE_TYPE_NONE);

	__setNotificationEventCallback(notification, NOTIFICATION_EVENT_TYPE_CLICK_ON_BUTTON_1, schedule, CAL_ALERT_DISMISS);
	__setNotificationEventCallback(notification, NOTIFICATION_EVENT_TYPE_CLICK_ON_BUTTON_2, schedule, CAL_ALERT_SNOOZE);

	notification_set_display_applist(notification, NOTIFICATION_DISPLAY_APP_ACTIVE);

	__postNotification(notification);

	notification_free(notification);

	WLEAVE();
}

void CalStatusBarManager::__postNotification(notification_h notification)
{
	WENTER();
	notification_set_time(notification, time(NULL));

	int noti_err = notification_post(notification);
	if (noti_err != NOTIFICATION_ERROR_NONE)
	{
		WERROR("Notification post fail [%d]", noti_err);
	}
}

app_control_h CalStatusBarManager::__createAppControl()
{
	app_control_h service = NULL;
	app_control_create(&service);
	app_control_set_app_id(service, CALENDAR_NOTI_PACKAGE);
	app_control_add_extra_data(service, CAL_APPALERT_PARAM_ACTION, CAL_APPALERT_ACTION_SHOW_NOTIFICATION_LIST);
	app_control_add_extra_data(service, CAL_APPSVC_PARAM_CALLER, CAL_APPSVC_PARAM_CALLER_NOTIFICATION);
	return service;
}

void CalStatusBarManager::removeNotification()
{
	WENTER();

	notification_h notification = __getHandle();
	WDEBUG("Notification: %p", notification);

	if (notification)
	{
		notification_delete(notification);
		notification_free(notification);
	}
}

void CalStatusBarManager::getAllStatusBar(std::vector<std::shared_ptr<CalAlertNotificationItem> > &items)
{
	WENTER();
	notification_h notification = __getHandle();
	if (!notification)
	{
		WWARNING("No status-bar notification");
		return;
	}

	app_control_h service = NULL;

	int err = notification_get_launch_option(notification, NOTIFICATION_LAUNCH_OPTION_APP_CONTROL, (void *)&service);
	if (service && err == NOTIFICATION_ERROR_NONE)
	{
		WDEBUG("service: %p", service);
		char **idsStr = NULL;
		int alertCount = 0;
		char key[ID_LENGTH] = {0};
		char alarmIdKey[ID_LENGTH] = {0};
		char *alarmId = NULL;
		char *isSnoozed = NULL;
		int recordIndex = 0;
		app_control_get_extra_data_array(service, CAL_APPCALSVC_PARAM_IDS, &idsStr, &alertCount);

		if (idsStr && alertCount > 0)
		{
			for (int i = 0; i < alertCount; i++)
			{
				recordIndex = atoi(idsStr[i]);
				WDEBUG("recordIndex: %d", recordIndex);

				snprintf(key, sizeof(key), "%d.%s", recordIndex, CAL_APPALERT_PARAM_IS_SNOOZED);
				app_control_get_extra_data(service, key, &isSnoozed);

				auto item = std::make_shared<CalAlertNotificationItem>(recordIndex);
				if(isSnoozed && !strcmp(isSnoozed, CAL_APPALERT_IS_SNOOZED))
				{
					WDEBUG("Snoozed!!!");
					item->setSnoozed(true);
					free(isSnoozed);
					isSnoozed = NULL;

					snprintf(alarmIdKey, sizeof(alarmIdKey), "%d.%s", recordIndex, CAL_APPALERT_PARAM_ALARM_ID);

					app_control_get_extra_data(service, alarmIdKey, &alarmId);
					item->setAlarmId(atoi(alarmId));
					WDEBUG("Alarm ID: %d", item->getAlarmId());
				}

				items.insert(items.begin(), item);
				free(idsStr[i]);
			}
			free(idsStr);
		}
		else
		{
			WERROR("Get notification data fail");
			app_control_destroy(service);
			notification_free(notification);
			return;
		}
		app_control_destroy(service);
	}
	WDEBUG("Items count is: %d", items.size());
	notification_free(notification);
}

void CalStatusBarManager::__setNotificationEventCallback(notification_h notification, notification_event_type_e type_button, std::shared_ptr<CalSchedule> schedule, int action)
{
	WENTER();
	WDEBUG("Notification: %p", notification);
	int recordIndex = schedule->getIndex();
	char recordIndexText[INDEX_BUFFER] = {0};
	snprintf(recordIndexText, sizeof(recordIndexText), "%d", recordIndex);

	do
	{
		app_control_h app_control = NULL;
		if (APP_CONTROL_ERROR_NONE != app_control_create(&app_control))
		{
			WERROR("Failed to create app_control");
			break;
		}

		app_control_set_app_id(app_control, CALENDAR_NOTI_PACKAGE);

		if (CAL_ALERT_DISMISS == action)
		{
			WDEBUG("Set app control for DISMISS");
			app_control_add_extra_data(app_control, CAL_APPALERT_PARAM_ACTION, CAL_APPALERT_ACTION_DISMISS);
		}
		else if (CAL_ALERT_SNOOZE == action)
		{
			WDEBUG("Set app control for SNOOZE");
			app_control_add_extra_data(app_control, CAL_APPALERT_PARAM_ACTION, CAL_APPALERT_ACTION_SNOOZE);
		}
		else
		{
			WERROR("Wrong action");
			app_control_destroy(app_control);
			break;
		}

		app_control_add_extra_data(app_control, CAL_APPSVC_PARAM_REMINDER_ID, recordIndexText);
		app_control_add_extra_data(app_control, CAL_APPSVC_PARAM_CALLER, CAL_APPSVC_PARAM_CALLER_NOTIFICATION);

		app_control_set_operation(app_control, APP_CONTROL_OPERATION_DEFAULT);
		notification_set_event_handler(notification, type_button, app_control);
		app_control_destroy(app_control);
	} while (0);

	WLEAVE();
}


notification_h CalStatusBarManager::__getHandle()
{
	notification_h notification = NULL;
	notification = notification_load_by_tag(CALENDAR_NOTI_PACKAGE);
	WDEBUG("Notification: %p", notification);
	if(NULL == notification)
	{
		int err = get_last_result();
		WDEBUG("Load notification by tag fail: '%s', %d (%s)", CALENDAR_NOTI_PACKAGE, err, get_error_message(err));
	}
	return notification;
}

void CalStatusBarManager::__setTag(notification_h notification)
{
	WENTER();
	int ret = notification_set_tag(notification, CALENDAR_NOTI_PACKAGE);
	WDEBUG("set tag %s, notification: %p", CALENDAR_NOTI_PACKAGE, notification);

	if (ret != NOTIFICATION_ERROR_NONE)
	{
		WERROR("set tag option fail: %d", ret);
	}
}

void CalStatusBarManager::__getNotificationData(std::shared_ptr<CalAlertData> &alertData)
{
	CalAlertData oldAlertData;
	alertData->add(oldAlertData);
}

bool CalStatusBarManager::__setNotificationData(app_control_h service, std::shared_ptr<CalAlertData>& alertData)
{
	WENTER();
	int alertCount = alertData->getCount();
	if (alertCount > 0)
	{
		char snoozedKey[ID_LENGTH] = {0};
		char alarmIdKey[ID_LENGTH] = {0};
		char alarmId[ID_LENGTH] = {0};
		char **ids = g_new0(char*, alertCount);

		for (int i = 0; i < alertCount; ++i)
		{
			auto alertItem = alertData->getAt(i);
			int id = alertItem->getScheduleId();
			char* strId = g_strdup_printf("%d", id);
			ids[i] = strId;

			if(alertItem->isSnoozed())
			{
				snprintf(snoozedKey, sizeof(snoozedKey), "%d.%s", id, CAL_APPALERT_PARAM_IS_SNOOZED);
				app_control_add_extra_data(service, snoozedKey, CAL_APPALERT_IS_SNOOZED);

				snprintf(alarmIdKey, sizeof(alarmIdKey), "%d.%s", id, CAL_APPALERT_PARAM_ALARM_ID);
				snprintf(alarmId, sizeof(alarmId), "%d", alertItem->getAlarmId());
				app_control_add_extra_data(service, alarmIdKey, alarmId);
			}
		}

		app_control_add_extra_data_array(service, CAL_APPCALSVC_PARAM_IDS, (const char**)ids, alertCount);
		for (int i = 0; i < alertCount; i++)
		{
			g_free(ids[i]);
		}
		g_free(ids);
	}

	return true;
}

void CalStatusBarManager::__setNotificationTitle(notification_h notification, const std::shared_ptr<CalAlertData> &alertData)
{
	WENTER();
	std::stringstream title;
	std::stringstream subTitle;

	int count = alertData->getCount();
	WDEBUG("Alerts count: %d", count);
	if (count > 1)
	{
		WDEBUG("Multiple event");
		for (int i = 0; i < count; i++)
		{
			auto alertItem = alertData->getAt(i);
			char *eventName = alertItem->getEventOriginalName();
			WDEBUG("Event name: %s", eventName);
			if (eventName && *eventName)
			{
				subTitle << eventName;
				free(eventName);
			}
			else
			{
				subTitle << _L_("IDS_CLD_MBODY_MY_EVENT");
			}

			if (i < count - 1)
			{
				subTitle << ", ";
			}
		}
		title << count << " events";

		WDEBUG("Title: %s", title.str().c_str());
		WDEBUG("Sub-title: %s", subTitle.str().c_str());

		notification_set_text(notification, NOTIFICATION_TEXT_TYPE_TITLE, title.str().c_str(), NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
		notification_set_text(notification, NOTIFICATION_TEXT_TYPE_CONTENT, subTitle.str().c_str(), NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
	}
	else if (count == 1)
	{
		WDEBUG("Single event");
		auto alertItem = alertData->getAt(0);
		char *eventName = alertItem->getEventOriginalName();
		char title[CAL_EVENT_TITLE_MAX_CHAR_LIMIT];
		if (alertData->isSnoozed(0)) {
			snprintf(title, sizeof(title), _L_("IDS_CLD_MBODY_PS_HSNOOZED_T_CALENDAR"), (eventName && *eventName) ? eventName : _L_("IDS_CLD_MBODY_MY_EVENT"));
		} else {
			snprintf(title, sizeof(title), "%s", (eventName && *eventName) ? eventName : _L_("IDS_CLD_MBODY_MY_EVENT"));
		}
		if (eventName && *eventName) {
			free(eventName);
		}
		notification_set_text(notification, NOTIFICATION_TEXT_TYPE_TITLE, title, NULL, NOTIFICATION_VARIABLE_TYPE_NONE);

		char* startTime = alertItem->getStartTime();
		subTitle << startTime;

		char * location = alertItem->getLocation();
		if (*location)
		{
			subTitle << ", ";
			subTitle << location;
		}
		free(location);
		free(startTime);

		notification_set_text(notification, NOTIFICATION_TEXT_TYPE_CONTENT, subTitle.str().c_str(), NULL, NOTIFICATION_VARIABLE_TYPE_NONE);

		WDEBUG("Title: %s", title);
		WDEBUG("Sub-title: %s", subTitle.str().c_str());
	}
	else
	{
		WERROR("No events!");
	}
}
