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

#include <Ecore.h>
#include <Elementary.h>

#include "WWindow.h"
#include "CalNaviframe.h"

#include "CalApp.h"
#include "CalCommon.h"
#include "CalTheme.h"

#include "CalMainView.h"
#include "CalDetailView.h"

#include "CalEventManager.h"
#include "CalDataManager.h"
#include "CalBookManager.h"
#include "CalSettingsManager.h"
#include "CalMidnightNotifier.h"
#include "CalListModelFactory.h"
#include "CalEditField.h"
#include "CalLocaleManager.h"

CalApp::CalApp()
{
}

CalApp::~CalApp()
{
}

/**
 * Executes the pause action.
 */
void CalApp::onPause()
{
	WENTER();

	CalEvent event(CalEvent::APP_PAUSED, CalEvent::REMOTE);
	CalEventManager::getInstance().notify(event);

	CalEventManager::getInstance().suspend();
}

/**
 * Executes the resume action.
 */
void CalApp::onResume()
{
	WENTER();

	CalEventManager::getInstance().resume();

	CalEvent event(CalEvent::APP_RESUMED, CalEvent::REMOTE);
	CalEventManager::getInstance().notify(event);
}

/**
 * Executes the create action.
 *
 * @return	true if it succeeds, false if it fails.
 */
bool CalApp::onCreate()
{
	__regionFormatChangedHandler.addEventHandler( APP_EVENT_REGION_FORMAT_CHANGED, [](app_event_info_h eventInfo, void* userData ){
			char* region = NULL;

			app_event_get_region_format( eventInfo, &region );
			WDEBUG("changed region=%s", region);

			CalSettingsManager::getInstance().updateRegion();
			CalEvent event(CalEvent::SETTING_CHANGED, CalEvent::REMOTE);
			CalEventManager::getInstance().notify(event);

			free(region);

	}, this );
	__languageChangedHandler.addEventHandler( APP_EVENT_LANGUAGE_CHANGED, [](app_event_info_h eventInfo, void* userData ){
			char* lang = NULL;

			app_event_get_language( eventInfo, &lang );
			WDEBUG("changed language=%s", lang);

			CalLocaleManager::getInstance().updateLocaleForEvasObj();

			CalEvent event(CalEvent::LANGUAGE_CHANGED, CalEvent::REMOTE);
			CalEventManager::getInstance().notify(event);

			free(lang);

	}, this );

	WDEBUG("CAL_LOCALE_DIR %s", CAL_LOCALE_DIR);
	bindtextdomain(CALENDAR, CAL_LOCALE_DIR);
	CalEventManager::getInstance();
	CalDataManager::getInstance();
	CalBookManager::getInstance();
	CalSettingsManager::getInstance();
	CalMidnightNotifier::getInstance().turnOn(CalMidnightNotifier::TIMEOUT);
	CalListModelFactory::getInstance().prepare();

	attachWindow(new WWindow("Calendar", ELM_WIN_BASIC));
	getWindow()->attachBaseUiObject(new CalNaviframe);
	elm_win_indicator_mode_set(getWindow()->getEvasObj(), ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(getWindow()->getEvasObj(), ELM_WIN_INDICATOR_TRANSPARENT);
	int rots[1] = {0};
	elm_win_wm_rotation_available_rotations_set(getWindow()->getEvasObj(), rots, 1);
	elm_win_wm_rotation_preferred_rotation_set(getWindow()->getEvasObj(), (const int)(rots[0]));

	CalLocaleManager::getInstance().setEvasObjForRTL(getWindow()->getEvasObj());

	CalTheme::initialize();

	return true;
}

/**
 * Executes the terminate action.
 */
void CalApp::onTerminate()
{
	__languageChangedHandler.removeEventHandler();
	__regionFormatChangedHandler.removeEventHandler();

	CalTheme::finalize();
}

/**
 * Executes the application control action.
 *
 * @param	request	   	The request.
 * @param	firstLaunch	true to first launch.
 */
void CalApp::onAppControl(app_control_h request, bool firstLaunch)
{
	WENTER();

	CalNaviframe& naviframe = *(CalNaviframe*)getWindow()->getBaseUiObject();

	char* viewParam = NULL;
	app_control_get_extra_data(request, CAL_APPSVC_PARAM_VIEW, &viewParam);

	if (viewParam)
	{
		if (!strcmp(viewParam, CAL_APPSVC_PARAM_VIEW_MAIN))
		{
			__showMain(request, firstLaunch);
		}
		else if (!strcmp(viewParam, CAL_APPSVC_PARAM_VIEW_DETAIL))
		{
			__showDetail(request);
		}
		else
		{
			WASSERT(0);
		}

		free(viewParam);
	}
	else
	{
		if(firstLaunch)
		{
			char* dateStringParam = NULL;
			app_control_get_extra_data(request, CAL_APPSVC_PARAM_DATE, &dateStringParam);

			if(dateStringParam)
			{
				CalDate date(dateStringParam);
				free(dateStringParam);

				naviframe.push(new CalMainView(date));
			}
			else
			{
				naviframe.push(new CalMainView(CalDate()));
			}
		}
		else
		{
			CalEventManager::getInstance().resume();

			CalEvent event(CalEvent::APP_RESUMED, CalEvent::REMOTE);
			CalEventManager::getInstance().notify(event);
		}
	}

	WApp::onAppControl(request, firstLaunch);
	WLEAVE();
}

/**
 * Shows the main.
 *
 * @param	request	   	The request.
 * @param	firstLaunch	true to first launch.
 */
void CalApp::__showMain(app_control_h request, bool firstLaunch)
{
	WDEBUG("firstLaunch = %u", firstLaunch);
	CalNaviframe& naviframe = *(CalNaviframe*)getWindow()->getBaseUiObject();

	CalMainView* mainView = NULL;
	if(!firstLaunch)
	{
		WView* topView = naviframe.getTopView();
		if(!topView || !strcmp(topView->getClassName(), "CalMainView"))
		{
			mainView = (CalMainView*)topView;
		}
		else
		{
			WDEBUG("All stacked views will be destroyed");
			naviframe.destroyAllViews();
			CalEventManager::getInstance().clear();
			firstLaunch = true;
		}
	}

	char* dateStringParam = NULL;
	app_control_get_extra_data(request, CAL_APPSVC_PARAM_DATE, &dateStringParam);

	if(dateStringParam)
	{
		CalDate date(dateStringParam);
		WDEBUG("%s", date.dump().c_str());
		free(dateStringParam);

		if (firstLaunch)
		{
			mainView = new CalMainView(date);
			naviframe.push(mainView);
			mainView->focus(date);
		}
		else
		{
			if(!mainView)
			{
				WERROR("mainView is null");
				elm_exit();
			}

			mainView->focus(date);
		}
	}
	else if(firstLaunch)
	{
		naviframe.push(new CalMainView(CalDate()));
	}
}

/**
 * Shows the detail.
 *
 * @param	request	The request.
 */
void CalApp::__showDetail(app_control_h request)
{
	char* eventIdParam = NULL;
	app_control_get_extra_data(request, CAL_APPSVC_PARAM_INDEX, &eventIdParam);
	WASSERT(eventIdParam);

	char* eventDateParam = NULL;
	app_control_get_extra_data(request, CAL_APPSVC_PARAM_DATE, &eventDateParam);
	std::shared_ptr<CalSchedule> schedule;
	if(eventDateParam)
	{
		time_t timeFromParam = (time_t)atoll(eventDateParam);
		CalDateTime eventDate(*localtime(&timeFromParam));
		schedule = CalDataManager::getInstance().getInstanceSchedule(atoi(eventIdParam), eventDate);
	}
	else
	{
		schedule = CalDataManager::getInstance().getSchedule(atoi(eventIdParam));
	}

	free(eventIdParam);
	free(eventDateParam);

	CalNaviframe& naviframe = *(CalNaviframe*)getWindow()->getBaseUiObject();
	naviframe.destroyAllViews();
	CalEventManager::getInstance().clear();

	CalDate date;
	naviframe.push(new CalMainView(date));
	naviframe.push(new CalDetailView(schedule));
}
