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

#ifndef _CAL_ALERT_TYPE_H_
#define _CAL_ALERT_TYPE_H_

#include "CalCommon.h"

/*
 * calendar-service : id, time, tick, unit, type
 *                   -> ids / id array
 * notification : caller, count, id, "0"
 */

#define CAL_APPSVC_PARAM_CALLER_NOTIFICATION "notification"
#define CAL_APPSVC_PARAM_REMINDER_ID "id"
#define CAL_APPSVC_PARAM_COUNT "count"
#define CAL_APPSVC_PARAM_COUNT_SINGLE "1"
#define CAL_APPSVC_PARAM_ZERO "0"

#define CAL_APPALERT_PARAM_ACTION "action"
#define CAL_APPALERT_ACTION_DISMISS "dismiss"
#define CAL_APPALERT_ACTION_SNOOZE "snooze"
#define CAL_APPALERT_ACTION_SHOW_NOTIFICATION_LIST "show"

#define CAL_APPALERT_PARAM_IS_SNOOZED "snoozed"
#define CAL_APPALERT_PARAM_ALARM_ID "alarmid"
#define CAL_APPALERT_IS_SNOOZED "1"

#define CAL_APPCALSVC_PARAM_UNIT "unit"
#define CAL_APPCALSVC_PARAM_TICK "tick"
#define CAL_APPCALSVC_PARAM_ID "id"
#define CAL_APPCALSVC_PARAM_IDS "ids"
#define CAL_APPCALSVC_PARAM_IDS_DEFAULT_ARRAY_LENGTH 1

#define CAL_INDICATOR_ICON "noti_calendar_white.png"
#define CAL_NOTIFICATION_ICON "noti_calendar_blue.png"
#define CAL_ACTIVENOTIFICATION_ICON "noti_calendar_02_while.png"
#define CALENDAR_ALERT_NOTI_MIN_KEY "alert_noti_min"

#endif
