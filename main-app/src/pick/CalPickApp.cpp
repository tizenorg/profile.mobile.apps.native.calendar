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
#include "CalMidnightNotifier.h"
#include "CalLocaleManager.h"

CalPickApp::CalPickApp() :
	__resultType(CalPickView::RESULT_TYPE_ERROR),
	__mode(CalPickApp::SELECTION_MODE_SINGLE),
	__mimeType(CalPickApp::MIME_VCS),
	__maxLimit(1),
	__isDone(false),
	__request(NULL)
{
}

CalPickApp::~CalPickApp()
{
}

void CalPickApp::onAppControl(app_control_h request, bool firstLaunch)
{
	char *value = NULL;

	int ret = app_control_get_extra_data(__request, APP_CONTROL_DATA_SELECTION_MODE, &value);
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
	WDEBUG("Resutl type: %s", value);

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

	// TODO
	//CalNaviframe* frame = (CalNaviframe*)getWindow()->getBaseUiObject();
	CalPickView* view = new CalPickView(__maxLimit, __resultType);

	view->setSelectCb([this](const std::list<std::shared_ptr<CalSchedule>>& schedules) {
		__processResult(schedules);
	});
	// TODO
	//frame->push(view);
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
	// TODO
	//char *resPath = app_get_shared_data_path();
		char *resPath = NULL;
		char *pathFormat = NULL;
		if(__mimeType == CalPickApp::MIME_ICS)
		{
			pathFormat = g_strdup_printf("%s%s", resPath, CAL_ICS_FILE_TEMPLATE);
		}
		else
		{
			pathFormat = g_strdup_printf("%s%s", resPath, CAL_VCS_FILE_TEMPLATE);
		}

		free(resPath);
		if(NULL != pathFormat)
		{
			for (auto it : schedules)
			{
				filePath = g_strdup_printf(pathFormat, it->getIndex());
				CalDataManager::getInstance().generateVcsFromSchedule(*it, filePath);
				resultArray[resultIndex] = filePath;
				WDEBUG("resultArray[%d] = %s", resultIndex, resultArray[resultIndex]);
				resultIndex++;
			}
			g_free(pathFormat);
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
	app_control_destroy(reply);

	for (int i = 0; i < resultIndex; i++)
	{
		free(resultArray[i]);
	}

	free(resultArray);

	__isDone = true;
	WLEAVE();
}
