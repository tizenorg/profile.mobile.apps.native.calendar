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

#ifndef _CAL_COMMON_H_
#define _CAL_COMMON_H_

#include "CalDefines.h"
#include "CalUtils.h"
#include "CalDebugInternal.h"

// === ALL OPERATIONS ===
#define APP_CONTROL_DATA_ID "http://tizen.org/appcontrol/data/id"
#define APP_CONTROL_DATA_MENU_STATE "Menu State"

// === PICK OPERATION ===
#define APP_CONTROL_DATA_TYPE "http://tizen.org/appcontrol/data/type"
#define APP_CONTROL_DATA_SELECTION_MODE "http://tizen.org/appcontrol/data/selection_mode"
#define APP_CONTROL_DATA_SELECTION_MODE_MULTIPLE "multiple"
#define APP_CONTROL_DATA_TOTAL_COUNT "http://tizen.org/appcontrol/data/total_count"
#define APP_CONTROL_KEY_ID "id"
#define APP_CONTROL_KEY_VCS "vcs"
#define APP_CONTROL_KEY_SELECTED "http://tizen.org/appcontrol/data/selected"

// === ADD AND EDIT OPERATION ===
#define APP_CONTROL_DATA_CALENDAR_ALLDAY "http://tizen.org/appcontrol/data/calendar/all_day"
#define APP_CONTROL_DATA_CALENDAR_START_TIME "http://tizen.org/appcontrol/data/calendar/start_time"
#define APP_CONTROL_DATA_CALENDAR_END_TIME "http://tizen.org/appcontrol/data/calendar/end_time"
#define APP_CONTROL_DATA_LOCATION "http://tizen.org/appcontrol/data/location"
#define APP_CONTROL_DATA_TEXT "http://tizen.org/appcontrol/data/text"

// MIME
#define APP_CONTROL_MIME_CONTACT "application/vnd.tizen.contact"
#define APP_CONTROL_MIME_CALENDAR "application/vnd.tizen.calendar"
#define APP_CONTROL_MIME_TEXT_CALENDAR "text/calendar"

#define CAL_VCS_FILE_TEMPLATE "Calendar%d.vcs"
#define CAL_ICS_FILE_TEMPLATE "Calendar%d.ics"

#define CAL_APPSVC_PARAM_CALLER "caller"
#define CAL_APPSVC_PARAM_CALLER_CALENDAR "calendar"
#define CAL_APPSVC_PARAM_VIEW "view"
#define CAL_APPSVC_PARAM_VIEW_MAIN "main"
#define CAL_APPSVC_PARAM_VIEW_DETAIL "detail"
#define CAL_APPSVC_PARAM_DATE "date"
#define CAL_APPSVC_PARAM_INDEX "index"

#define PKG_3DPARTIES_QUICKPANEL "org.tizen.quickpanel"
#define PKG_3DPARTIES_BOOKMARK_ADD "bookmark-add-efl"
#define APP_CONTROL_BOOKMARK_ADD_KEY_URL "url"
#define PKG_3DPARTIES_MAPS "org.tizen.maps"
#define APP_CONTROL_MAPS_KEY_SEARCHBYNAME "searchByName"
#define PKG_3DPARTIES_CLOCK_SHORT "clock"
#define PKG_3DPARTIES_WORLDCLOCK "org.tizen.worldclock-efl"
#define APP_CONTROL_WORLDCLOCK_KEY_TZPATH "tzpath"
#define PKG_3DPARTIES_SETTING_RINGTONE "setting-ringtone-efl"
#define APP_CONTROL_SETTING_RINGTONE_KEY_RESULT "result"

#define CAL_ALERT_DISMISS 0
#define CAL_ALERT_SNOOZE 1
#define CAL_POPUP_ITEM_HEIGHT 120
#define CAL_EVENT_TITLE_MAX_CHAR_LIMIT 1000
#define CAL_WARNING_MAX_LENGTH 100

#define ID_LENGTH 16
#define CAL_NOTIFICATION_TITLE_LENGTH 16

#pragma GCC diagnostic ignored "-Wformat-extra-args"
#define _L_(str) dgettext(CALENDAR, str)
#define _L_S_(str)    str
#define _L_G_(str)   _L_(str)

/////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
#define TIME_CHECKER CalTimeChecker __timeChecker(__FILE__, __LINE__, __FUNCTION__)

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

class CalTimeChecker
{
public:
	CalTimeChecker(const char* file, int line, const char* function) :
		__file(file), __line(line), __function(function)
	{
		gettimeofday(&s, NULL);
	}
	virtual ~CalTimeChecker()
	{
		struct timeval e, diff;
		gettimeofday(&e, NULL);
		timersub(&e, &s, &diff);
		WDEBUG("TIME_CHECK( %s:%d:%s:%03ld.%06d )", __file, __line, __function, diff);
	}
private:
	const char* __file;
	int __line;
	const char* __function;
	struct timeval s;
};
#else
#define TIME_CHECKER
#endif

/////////////////////////////////////////////////////////////////////////////////

#endif
