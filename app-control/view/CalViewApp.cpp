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

#include "notification.h"

#include "WWindow.h"
#include "CalCommon.h"
#include "CalNaviframe.h"
#include "CalTheme.h"
#include "CalEventManager.h"
#include "CalDataManager.h"
#include "CalBookManager.h"
#include "CalSettingsManager.h"
#include "CalVcsView.h"
#include "CalOriginalSchedule.h"
#include "CalViewApp.h"
#include "CalLocaleManager.h"
#include "CalPath.h"

CalViewApp::CalViewApp() :
	__replyToRequest(false),
	__request(nullptr)
{
}

CalViewApp::~CalViewApp()
{
}

void CalViewApp::onPause()
{
	WENTER();

	CalEventManager::getInstance().suspend();
}

void CalViewApp::onResume()
{
	WENTER();

	CalEventManager::getInstance().resume();
}

bool CalViewApp::onCreate()
{
	bindtextdomain(CALENDAR, CalPath::getLocaleDir().c_str());

	CalEventManager::getInstance();
	CalDataManager::getInstance();
	CalBookManager::getInstance();
	CalSettingsManager::getInstance();

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

	elm_win_indicator_mode_set(getWindow()->getEvasObj(), ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(getWindow()->getEvasObj(), ELM_WIN_INDICATOR_TRANSPARENT);
	int rots[1] = {0};
	elm_win_wm_rotation_available_rotations_set(getWindow()->getEvasObj(), rots, 1);
	elm_win_wm_rotation_preferred_rotation_set(getWindow()->getEvasObj(), (const int)(rots[0]));

	CalLocaleManager::getInstance().setEvasObjForRTL(getWindow()->getEvasObj());

	CalTheme::initialize();

	return true;
}

void CalViewApp::onTerminate()
{
	CalTheme::finalize();

	if(!__replyToRequest)
	{
		__replyError();
	}
}

void CalViewApp::onAppControl(app_control_h request, bool firstLaunch)
{
	WENTER();

	WDEBUG("firstLaunch = %u", firstLaunch);

	int ret = app_control_clone(&__request, request);
	WPRET_M(ret != APP_CONTROL_ERROR_NONE, "Can't clone the request");

	__makeSchedule();

	WApp::onAppControl(request, firstLaunch);
	WLEAVE();
}

void CalViewApp::__makeSchedule()
{
	CalNaviframe* frame = (CalNaviframe*)getWindow()->getBaseUiObject();
	const char* filePath = __getFilePath();
	if (filePath)
	{
		WDEBUG("URI = %s", filePath);
		if (__isVtsFile(filePath))
		{
			notification_status_message_post(_L_("IDS_CLD_TPOP_VTS_FILES_NOT_SUPPORTED"));
		}

		std::list<std::shared_ptr<CalSchedule>> schedules;
		CalDataManager::getInstance().getSchedulesFromVcs(filePath, schedules);
		int count = schedules.size();
		if (count == 1)
		{
			std::shared_ptr<CalSchedule> inputSchedule = *(schedules.begin());
			frame->push(new CalDetailView(inputSchedule, CalDetailView::MENU_DISABLED, true));
		}
		else
		{
			frame->push(new CalVcsView(schedules));
		}

		__replyToRequest = true;
	}
	else
	{
		WDEBUG("MIME") ;
		char *value = NULL;
		int ret = app_control_get_extra_data(__request, APP_CONTROL_DATA_ID, &value);

		WDEBUG("get %s [%s]", APP_CONTROL_DATA_ID, value);
		if (APP_CONTROL_ERROR_NONE != ret || !value || !strlen(value))
		{
			free(value);

			__replyError();

			elm_exit();
			return;
		}

		int eventIndex = atoi(value);
		std::shared_ptr<CalSchedule> inputSchedule = CalDataManager::getInstance().getSchedule(eventIndex);
		frame->push(new CalDetailView(inputSchedule, CalDetailView::MENU_DISABLED));

		free(value);
		value = NULL;

		__replyToRequest = true;
	}
}

bool CalViewApp::__isVtsFile(const char* filePath)
{
	if (!filePath)
	{
		return false;
	}
	int len = strlen(filePath);
	if (len < 4)
		return false;
	if (strncasecmp(&filePath[len - 4], ".vts", 4) == 0)
		return true;
	return false;
}

const char* CalViewApp::__getFilePath()
{
	WENTER();
	static char* __internalFilePath = NULL;

	free(__internalFilePath);
	__internalFilePath = NULL;
	int ret = app_control_get_uri(__request, &__internalFilePath);
	if (ret != CALENDAR_ERROR_NONE || !__internalFilePath || !strlen(__internalFilePath))
	{
		WERROR("app_control_get_uri failed : %d", ret);
		free(__internalFilePath);
		__internalFilePath = NULL;
		return NULL;
	}

#define CAL_URI_FILE_FORMAT "file://"

	WDEBUG("vcs path = %s", __internalFilePath);
	if (!strstr(__internalFilePath, CAL_URI_FILE_FORMAT))
	{
		return __internalFilePath;
	}

	return __internalFilePath + strlen(CAL_URI_FILE_FORMAT);
}

std::shared_ptr<CalSchedule> CalViewApp::__getScheduleFromExtraData(app_control_h request)
{
	WENTER();
	char* value = NULL;
	app_control_get_extra_data(request, APP_CONTROL_DATA_ID, &value);
	if (value && strlen(value))
	{
		int scheduleId = atoi(value);
		free(value);
		value = NULL;

		calendar_record_h record = NULL;
		int error = calendar_db_get_record(_calendar_event._uri, scheduleId, &record);
		if (error != CALENDAR_ERROR_NONE)
		{
			WERROR("unable to find %d event", scheduleId);
			return nullptr;
		}

		return std::make_shared<CalOriginalSchedule>(record);
	}

	WERROR("Invalid argument");
	return nullptr;
}

void CalViewApp::__replyError()
{
	WENTER();
	app_control_h reply;
	app_control_create(&reply);
	app_control_reply_to_launch_request(reply, __request, APP_CONTROL_RESULT_FAILED);
	app_control_destroy(__request);
	app_control_destroy(reply);
}
