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

#include <device/display.h>
#include <device/power.h>
#include <efl_util.h>
#include "WWindow.h"
#include "CalNaviframe.h"
#include "CalAlertType.h"
#include "CalAlertApp.h"
#include "CalAlertView.h"
#include "CalNotificationsView.h"
#include "CalSettingsManager.h"
#include "CalStatusBarManager.h"
#include "CalCommon.h"
#include "CalEventManager.h"
#include "CalDataManager.h"
#include "CalBookManager.h"
#include "CalTheme.h"
#include "CalLocaleManager.h"
#include "CalPath.h"

#define EXIT_DELAY 2.0

CalAlertApp::CalAlertApp() :
	__alertView(nullptr),
	__notificationsView(nullptr),
	__timer(nullptr),
	__mode(CALALERT_NONE)
{
	WENTER();
}

CalAlertApp::~CalAlertApp()
{
	WENTER();
	if(__timer)
	{
		ecore_timer_del(__timer);
	}

	__timer = NULL;
}

void CalAlertApp::onPause()
{
	WENTER();
	CalEvent event(CalEvent::APP_PAUSED, CalEvent::REMOTE);
	CalEventManager::getInstance().notify(event);
}

void CalAlertApp::onResume()
{
	WENTER();
	CalEvent event(CalEvent::APP_RESUMED, CalEvent::REMOTE);
	CalEventManager::getInstance().notify(event);
}

bool CalAlertApp::onCreate()
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
			char* lang = NULL;

			app_event_get_language( eventInfo, &lang );
			WDEBUG("changed language=%s", lang);

			CalLocaleManager::getInstance().updateLocaleForEvasObj();

			CalEvent event(CalEvent::LANGUAGE_CHANGED, CalEvent::REMOTE);
			CalEventManager::getInstance().notify(event);

			free(lang);

		}, this );

	WLEAVE();
	return true;
}

void CalAlertApp::onTerminate()
{
	WENTER();
	CalTheme::finalize();
	WLEAVE();
}

void CalAlertApp::onAppControl(app_control_h request, bool firstLaunch)
{
	WENTER();
	__checkRequest(request);
	switch (__mode)
	{
	case CALALERT_VIEW:
		WDEBUG("Show Alert");
		__createWindowSafe(firstLaunch, true);
		__launchAlertView(__alertData);
		break;

	case CALALERT_ACTIVE_NOTIFICATION:
		WDEBUG("Show active notification");
		__setStatusBar(__alertData);
		break;

	case CALALERT_DISMISS:
		WDEBUG("Dismiss alert");
		__model->dismiss(0);
		break;

	case CALALERT_SNOOZE:
		WDEBUG("Snooze alert");
		__model->snooze(0);
		break;

	case CALALERT_SHOW_NOTIFICATION_LIST:
		WDEBUG("Show deffered alerts");
		__createWindowSafe(firstLaunch, false);
		__launchNotificationView(__alertData);
		break;

	case CALALERT_UPDATE_STATUSBAR:
		if (firstLaunch)
		{
			WDEBUG("exit");
			__exit();
			WLEAVE();
			return;
		}
		CalStatusBarManager::getInstance().checkDeletedEvent();
		break;

	case CALALERT_NONE:
	default:
		CalStatusBarManager::getInstance().checkDeletedEvent();
		break;
	}

	if(getWindow())
	{
		WApp::onAppControl(request, firstLaunch);
	}

	WLEAVE();
}
void CalAlertApp::__createWindowSafe(bool isFirstLaunch, bool isAlertPopup)
{
	if (isFirstLaunch)
	{
		__createWindow(isAlertPopup);
	}
	else
	{
		WWindow* win = getWindow();
		if (win == NULL)
		{
			__stopExit();
		}
		__createWindow(isAlertPopup);
	}
}
void CalAlertApp::__createWindow(bool isAlertPopup)
{
	if (isAlertPopup)
	{
		device_power_wakeup(false);
		device_display_change_state(DISPLAY_STATE_NORMAL);

		attachWindow(new WWindow("Calendar", ELM_WIN_NOTIFICATION));

		efl_util_set_notification_window_level(getWindow()->getEvasObj(), EFL_UTIL_NOTIFICATION_LEVEL_3);
	}
	else
	{
		attachWindow(new WWindow("Calendar", ELM_WIN_BASIC));
	}

	elm_win_indicator_mode_set(getWindow()->getEvasObj(), ELM_WIN_INDICATOR_SHOW);

	CalNaviframe* naviframe = new CalNaviframe();
	if (!isAlertPopup)
	{
		naviframe->setOnLastItemPop([] (bool* popOut) {
				*popOut = false;
				elm_exit();
			});
	}
	getWindow()->attachBaseUiObject(naviframe);

	int rots[1] = {0};
	elm_win_wm_rotation_available_rotations_set(getWindow()->getEvasObj(), rots, 1);
	elm_win_wm_rotation_preferred_rotation_set(getWindow()->getEvasObj(), (const int)(rots[0]));

	CalLocaleManager::getInstance().setEvasObjForRTL(getWindow()->getEvasObj());

	CalTheme::initialize();
}

void CalAlertApp::__checkRequest(const app_control_h request)
{
	WENTER();
	char *caller = NULL;
	app_control_get_extra_data(request, CAL_APPSVC_PARAM_CALLER, &caller);

	if (caller != NULL)
	{
		WDEBUG("caller [%s]", caller);
		if(!strcmp(caller, CAL_APPSVC_PARAM_CALLER_NOTIFICATION))
		{
			char *action = NULL;
			app_control_get_extra_data(request, CAL_APPALERT_PARAM_ACTION, &action);

			if (action != NULL)
			{
				WDEBUG("action [%s]", action);
				if (!strcmp(action, CAL_APPALERT_ACTION_DISMISS))
				{
					__mode = CALALERT_DISMISS;
				}
				else if (!strcmp(action, CAL_APPALERT_ACTION_SNOOZE))
				{
					__mode = CALALERT_SNOOZE;
				}
				else if (!strcmp(action, CAL_APPALERT_ACTION_SHOW_NOTIFICATION_LIST))
				{
					__mode = CALALERT_SHOW_NOTIFICATION_LIST;
				}
				else
				{
					WERROR("Wrong action");
				}
				free(action);
			}

			__alertData = std::make_shared<CalAlertData>();
			__model = std::make_shared<CalAlertModel>(__alertData);
			return;
		}
		else if(!strcmp(caller, CAL_APPSVC_PARAM_CALLER_CALENDAR))
		{
			__mode = CALALERT_UPDATE_STATUSBAR;
		}
		free(caller);
	}
	else
	{
		__alertData = std::make_shared<CalAlertData>(request);
		if (CalAlertModel::isDeviceLocked())
		{
			__mode = CALALERT_VIEW;
		}
		else
		{
			__mode = CALALERT_ACTIVE_NOTIFICATION;
		}
	}
}

void CalAlertApp::__launchAlertView(std::shared_ptr<CalAlertData> alertData)
{
	WENTER();
	CalNaviframe* frame = (CalNaviframe*)getWindow()->getBaseUiObject();
	if (__alertView == NULL)
	{
		__alertView = new CalAlertView(alertData);
		frame->push(__alertView);
	}
	else
	{
		__alertView->replaceAlertData(alertData);
	}
}

void CalAlertApp::__launchNotificationView(std::shared_ptr<CalAlertData> alertData)
{
	WENTER();
	CalNaviframe* frame = (CalNaviframe*)getWindow()->getBaseUiObject();
	if (__notificationsView == NULL)
	{
		__notificationsView = new CalNotificationsView(alertData);
		frame->push(__notificationsView);
	}
	else
	{
		WHIT();
		__notificationsView->replaceAlertData(alertData);
	}

	WDEBUG("alertData [%p]", __alertData.get());
}

void CalAlertApp::__setStatusBar(std::shared_ptr<CalAlertData> alertData)
{
	WENTER();
	CalStatusBarManager::getInstance().pushActiveNotification(alertData, true);
	CalStatusBarManager::getInstance().pushNotification(alertData, false);
}

void CalAlertApp::__exit()
{
	if (__timer)
	{
		return;
	}

	__timer = ecore_timer_add(EXIT_DELAY, [](void* data) {
		WDEBUG("exit");
		ui_app_exit();
		return ECORE_CALLBACK_CANCEL;
	}, NULL);
}

void CalAlertApp::__stopExit()
{
	if(__timer)
	{
		ecore_timer_del(__timer);
	}

	__timer = NULL;
}
