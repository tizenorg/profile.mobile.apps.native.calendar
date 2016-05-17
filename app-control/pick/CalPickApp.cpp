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
#include "CalPickApp.h"
#include "CalLocaleManager.h"
#include "CalPath.h"

CalPickApp::CalPickApp() :
	__resultType(CalPickView::RESULT_TYPE_ERROR),
	__mode(CalPickApp::SELECTION_MODE_SINGLE),
	__mimeType(CalPickApp::MIME_VCS),
	__maxLimit(1),
	__isDone(false),
	__request(nullptr)
{
}

CalPickApp::~CalPickApp()
{
}

void CalPickApp::onPause()
{
	WENTER();

	CalEventManager::getInstance().suspend();
}

void CalPickApp::onResume()
{
	WENTER();

	CalEventManager::getInstance().resume();
}

bool CalPickApp::onCreate()
{
	WENTER();

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
			CalLocaleManager::getInstance().updateLocaleForEvasObj();
			CalEvent event(CalEvent::LANGUAGE_CHANGED, CalEvent::REMOTE);
			CalEventManager::getInstance().notify(event);
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

void CalPickApp::onTerminate()
{
	WENTER();
	CalTheme::finalize();

	if (__isDone)
		return;

	app_control_h reply;
	app_control_create(&reply);
	app_control_reply_to_launch_request(reply, __request, APP_CONTROL_RESULT_FAILED);
	app_control_destroy(__request);
	__request = nullptr;
	app_control_destroy(reply);
	WLEAVE();
}

void CalPickApp::onAppControl(app_control_h request, bool firstLaunch)
{
	WENTER();

	char *value = NULL;
	int ret = app_control_clone(&__request, request);
	WPRET_M(ret != APP_CONTROL_ERROR_NONE, "Can't clone the request");

	ret = app_control_get_extra_data(__request, APP_CONTROL_DATA_SELECTION_MODE, &value);
	if(APP_CONTROL_ERROR_NONE == ret && value && !strcasecmp(APP_CONTROL_DATA_SELECTION_MODE_MULTIPLE, value))
	{
		__mode = CalPickApp::SELECTION_MODE_MULTIPLE;
		__maxLimit = MULTI_SELECT_MAX;
	}

	free(value);
	value = NULL;

	ret = app_control_get_extra_data(__request, APP_CONTROL_DATA_TOTAL_COUNT, &value);
	if (APP_CONTROL_ERROR_NONE == ret && value)
	{
		__maxLimit = atoi(value);
	}

	free(value);
	value = NULL;

	ret = app_control_get_extra_data(__request, APP_CONTROL_DATA_TYPE, &value);
	WDEBUG("Result type: %s", value);

	if (APP_CONTROL_ERROR_NONE == ret && value)
	{
		if (!strcasecmp(APP_CONTROL_KEY_VCS, value))
		{
			__resultType = CalPickView::RESULT_TYPE_VCS;
		}
		else if (!strcasecmp(APP_CONTROL_KEY_ID, value))
		{
			__resultType = CalPickView::RESULT_TYPE_ID;
		}
		else
		{
			app_control_h reply;
			app_control_create(&reply);
			app_control_reply_to_launch_request(reply, __request, APP_CONTROL_RESULT_FAILED);
			app_control_destroy(__request);
			__request = nullptr;
			app_control_destroy(reply);
			WERROR("Wrong app control data type!!!");
			elm_exit();
		}
		free(value);
	}

	ret = app_control_get_mime(__request, &value);
	if (APP_CONTROL_ERROR_NONE == ret && value && !strcasecmp(APP_CONTROL_MIME_TEXT_CALENDAR, value))
	{
		__mimeType = CalPickApp::MIME_ICS;
	}

	CalNaviframe* frame = (CalNaviframe*)getWindow()->getBaseUiObject();
	CalPickView* view = new CalPickView(__maxLimit, __resultType);

	view->setBackButtonVisibility(false);

	view->setDoneCb([this](const std::list<std::shared_ptr<CalSchedule>>& schedules) {
		__processResult(schedules);
	});

	frame->push(view);

	WLEAVE();
}

void CalPickApp::__processResult(const std::list<std::shared_ptr<CalSchedule>>& schedules)
{
	WENTER();

	if (__mode != CalPickApp::SELECTION_MODE_MULTIPLE && __mode != CalPickApp::SELECTION_MODE_SINGLE)
	{
		WERROR("Unsupported mode : %d", __mode);
		return;
	}

	if(schedules.size() <= 0)
	{
		WERROR("No events selected");
		elm_exit();
	}

	app_control_h reply;
	app_control_create(&reply);
	int resultIndex = 0;
	char** resultArray = g_new0(char*, schedules.size());

	char* filePath = NULL;

	if (__resultType == CalPickView::RESULT_TYPE_VCS)
	{
		std::string fileTemplate = __mimeType == CalPickApp::MIME_ICS ? CAL_ICS_FILE_TEMPLATE : CAL_VCS_FILE_TEMPLATE;
		std::string pathFormat = CalPath::getPath(fileTemplate, CalPath::DATA);

		for (auto it : schedules)
		{
			filePath = g_strdup_printf(pathFormat.c_str(), it->getIndex());
			CalDataManager::getInstance().generateVcsFromSchedule(*it, filePath);
			resultArray[resultIndex] = filePath;
			WDEBUG("resultArray[%d] = %s", resultIndex, resultArray[resultIndex]);
			resultIndex++;
		}
	}
	else if (__resultType == CalPickView::RESULT_TYPE_ID)
	{
		for (auto it : schedules)
		{
			resultArray[resultIndex] = g_strdup_printf("%d", it->getIndex());
			WDEBUG("resultArray[%d] = %s", resultIndex, resultArray[resultIndex]);
			resultIndex++;
		}
	}

	app_control_add_extra_data(reply, APP_CONTROL_DATA_TYPE, APP_CONTROL_KEY_ID);
	app_control_add_extra_data_array(reply, APP_CONTROL_KEY_SELECTED, (const char**) resultArray, resultIndex);
	app_control_reply_to_launch_request(reply, __request, APP_CONTROL_RESULT_SUCCEEDED);
	app_control_destroy(__request);
	__request = nullptr;
	app_control_destroy(reply);

	for (int i = 0; i < resultIndex; i++)
	{
		g_free(resultArray[i]);
	}

	g_free(resultArray);

	__isDone = true;
	WLEAVE();
 }
