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

#include "WWindow.h"
#include "CalCommon.h"
#include "CalNaviframe.h"
#include "CalTheme.h"
#include "CalEventManager.h"
#include "CalDataManager.h"
#include "CalBookManager.h"
#include "CalSettingsManager.h"
#include "CalEditApp.h"
#include "CalMidnightNotifier.h"
#include "CalLocaleManager.h"

CalEditApp::CalEditApp() :
	__replyToRequest(false),
	__schedule(nullptr),
	__request(nullptr),
	__editMode(CalEditView::CREATE)
{
}

CalEditApp::~CalEditApp()
{
}

void CalEditApp::onPause()
{
	WENTER();

	CalEventManager::getInstance().suspend();
}

void CalEditApp::onResume()
{
	WENTER();

	CalEventManager::getInstance().resume();
}

bool CalEditApp::onCreate()
{
	WENTER();

	WDEBUG("CAL_LOCALE_DIR %s", CAL_LOCALE_DIR);
	bindtextdomain(CALENDAR, CAL_LOCALE_DIR);
	CalEventManager::getInstance();
	CalDataManager::getInstance();
	CalBookManager::getInstance();
	CalSettingsManager::getInstance();
	CalMidnightNotifier::getInstance().turnOn(CalMidnightNotifier::TIMEOUT);

	__regionFormatChangedHandler.addEventHandler(APP_EVENT_REGION_FORMAT_CHANGED, [](app_event_info_h eventInfo, void* userData) {
			char* region = NULL;

			app_event_get_region_format( eventInfo, &region );
			WDEBUG("changed region=%s", region);

			CalSettingsManager::getInstance().updateRegion();
			CalEvent event(CalEvent::SETTING_CHANGED, CalEvent::REMOTE);
			CalEventManager::getInstance().notify(event);

			free(region);

		}, this );
	__languageChangedHandler.addEventHandler(APP_EVENT_LANGUAGE_CHANGED, [](app_event_info_h eventInfo, void* userData) {
			char* lang = NULL;

			app_event_get_language( eventInfo, &lang );
			WDEBUG("changed language=%s", lang);

			CalLocaleManager::getInstance().updateLocaleForEvasObj();

			CalEvent event(CalEvent::LANGUAGE_CHANGED, CalEvent::REMOTE);
			CalEventManager::getInstance().notify(event);

			free(lang);

		}, this );

	attachWindow(new WWindow("Calendar", ELM_WIN_BASIC));
	CalNaviframe* naviframe = new CalNaviframe();
	naviframe->setOnLastItemPop([] (bool* popOut) {
			*popOut = true;
			elm_exit();
		});
	getWindow()->attachBaseUiObject(naviframe);
	evas_object_hide(getWindow()->getEvasObj());
	elm_win_indicator_mode_set(getWindow()->getEvasObj(), ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(getWindow()->getEvasObj(), ELM_WIN_INDICATOR_TRANSPARENT);
	int rots[1] = {0};
	elm_win_wm_rotation_available_rotations_set(getWindow()->getEvasObj(), rots, 1);
	elm_win_wm_rotation_preferred_rotation_set(getWindow()->getEvasObj(), (const int)(rots[0]));

	CalLocaleManager::getInstance().setEvasObjForRTL(getWindow()->getEvasObj());

	CalTheme::initialize();

	return true;
}

void CalEditApp::onTerminate()
{
	CalTheme::finalize();

	if(!__replyToRequest)
	{
		__replyError();
	}
}

void CalEditApp::onAppControl(app_control_h request, bool firstLaunch)
{
	WENTER();

	WDEBUG("firstLaunch = %u", firstLaunch);

	__request = request;

	__makeScheduleFromExtraData();
	WDEBUG(" __editMode: %d", __editMode);

	CalNaviframe* frame = (CalNaviframe*)getWindow()->getBaseUiObject();
	CalEditView* view = new CalEditView(__schedule, __editMode,
		[this](int newId)
		{
			app_control_h reply;
			app_control_create(&reply);

			char id[ID_LENGTH] = {0};
			snprintf(id, sizeof(id), "%d", newId);
			app_control_add_extra_data(reply, APP_CONTROL_DATA_ID, id);
			app_control_reply_to_launch_request(reply, __request, APP_CONTROL_RESULT_SUCCEEDED);
			app_control_destroy(__request);
			app_control_destroy(reply);

			__replyToRequest = true;
		}, false);

	frame->push(view);

	WApp::onAppControl(request, firstLaunch);
	WLEAVE();
}

void CalEditApp::__makeScheduleForEditMode()
{
	char *value = NULL;
	int ret = app_control_get_extra_data(__request, APP_CONTROL_DATA_ID, &value);
	if (APP_CONTROL_ERROR_NONE != ret || !value || !strlen(value))
	{
		if(!__replyToRequest)
		{
			__replyError();
		}

		free(value);
		elm_exit();
	}

	int eventIndex = atoi(value);
	__schedule = CalDataManager::getInstance().getSchedule(eventIndex);

	free(value);

	if(__schedule)
	{
		value = NULL;
		app_control_get_extra_data(__request, APP_CONTROL_DATA_CALENDAR_START_TIME, &value);
		if (value)
		{
			struct tm startTm = {0};
			strptime(value, "%Y-%m-%d %H:%M:%S", &startTm);
			free(value);

			__schedule->setStart(CalDateTime(startTm));
		}

		value = NULL;
		app_control_get_extra_data(__request, APP_CONTROL_DATA_CALENDAR_END_TIME, &value);
		if (value)
		{
			struct tm endTm = {0};
			strptime(value, "%Y-%m-%d %H:%M:%S", &endTm);
			free(value);

			__schedule->setEnd(CalDateTime(endTm));
		}

		value = NULL;
		app_control_get_extra_data(__request, APP_CONTROL_DATA_CALENDAR_ALLDAY, &value);
		if (value)
		{
			if (!strcasecmp(value, "true") || !strcasecmp(value, "1"))
			{
				CalDateTime startTime, endTime;
				__schedule->getStart(startTime);
				__schedule->getEnd(endTime);

				startTime = CalDateTime(startTime.getYear(), startTime.getMonth(), startTime.getMday(), 0, 0, 0);
				__schedule->setStart(startTime);

				endTime = CalDateTime(endTime.getYear(), endTime.getMonth(), endTime.getMday(), 0, 0, 0);
				endTime.addDays(1);
				__schedule->setEnd(endTime);
			}

			free(value);
		}

		__addExtraParams();
	}
}

void CalEditApp::__makeScheduleForCreateMode()
{
	char *value = NULL;
	bool isAllDay = false;
	app_control_get_extra_data(__request, APP_CONTROL_DATA_CALENDAR_ALLDAY, &value);
	WDEBUG("all_day = %s", value);
	if (value)
	{
		if (!strcasecmp(value, "true") || !strcasecmp(value, "1"))
		{
			isAllDay = true;
		}

		free(value);
	}

	value = NULL;
	app_control_get_extra_data(__request, APP_CONTROL_DATA_CALENDAR_START_TIME, &value);
	WDEBUG("%s", value);
	if (value)
	{
		WDEBUG("start_time = %s", value);
		struct tm startTm = {0};
		strptime(value, "%Y-%m-%d %H:%M:%S", &startTm);
		free(value);
		value = NULL;
		app_control_get_extra_data(__request, APP_CONTROL_DATA_CALENDAR_END_TIME, &value);
		if (value)
		{
			WDEBUG("end_time = %s", value);
			struct tm endTm = {0};
			strptime(value, "%Y-%m-%d %H:%M:%S", &endTm);
			free(value);
			value = NULL;

			CalDateTime startDateTime(startTm);
			CalDateTime endDateTime(endTm);
			if(isAllDay && endDateTime.isSameDay(startDateTime))
			{
				// reset hours & minutes, if any
				CalDateTime dateTemp = CalDateTime(startDateTime.getYear(), startDateTime.getMonth(), startDateTime.getMday(), 0, 0, 0);
				startDateTime = dateTemp;
				endDateTime = dateTemp;
			}
			__schedule =  CalSchedule::makeDefaultSchedule(startDateTime, endDateTime);
		}
		else
		{
			CalDateTime startDateTime(startTm);
			startDateTime.setAllDay(isAllDay);
			__schedule =  CalSchedule::makeDefaultSchedule(startDateTime);
		}
	}
	else
	{
		__schedule = CalSchedule::makeDefaultSchedule(isAllDay);
	}

	__addExtraParams();

	free(value);
}

void CalEditApp::__makeScheduleFromExtraData()
{
	char *value = NULL;
	int ret = app_control_get_operation(__request, &value);
	if (APP_CONTROL_ERROR_NONE == ret && value && !strcmp(value, APP_CONTROL_OPERATION_EDIT))
	{
		__editMode = CalEditView::EDIT_EXTERNAL;
	}

	free(value);

	WDEBUG("mode = %d", __editMode);
	if(__editMode == CalEditView::EDIT_EXTERNAL)
	{
		__makeScheduleForEditMode();
	}
	else
	{
		__makeScheduleForCreateMode();
	}
}

void CalEditApp::__replyError()
{
	app_control_h reply;
	app_control_create(&reply);
	app_control_reply_to_launch_request(reply, __request, APP_CONTROL_RESULT_FAILED);
	app_control_destroy(__request);
	app_control_destroy(reply);
}

void CalEditApp::__addExtraParams()
{
	char *value = NULL;
	app_control_get_extra_data(__request, APP_CONTROL_DATA_TITLE, &value);
	WDEBUG("title = %s", value);
	if (value && __schedule)
	{
		__schedule->setSummary(value);
	}

	free(value);
	value = NULL;
	app_control_get_extra_data(__request, APP_CONTROL_DATA_TEXT, &value);
	WDEBUG("description = %s", value);
	if (value && __schedule)
	{
		__schedule->setDescription(value);
	}

	free(value);
	value = NULL;
	app_control_get_extra_data(__request, APP_CONTROL_DATA_LOCATION, &value);
	WDEBUG("location = %s", value);
	if (value && __schedule)
	{
		__schedule->setLocation(value);
	}

	free(value);
}
