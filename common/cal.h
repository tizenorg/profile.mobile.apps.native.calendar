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

#ifndef _C_CAL_H_
#define _C_CAL_H_

#include <glib.h>
#include <calendar.h>

#include "cal_log.h"
#include "cal_utils.h"

#ifndef API
#define API __attribute__ ((visibility("default")))
#endif

#if !defined(CALENDAR)
#define CALENDAR "calendar"
#endif

#if !defined(VENDOR)
#define VENDOR "org.tizen"
#endif

#if !defined(CALENDAR_PACKAGE)
#define CALENDAR_PACKAGE VENDOR "." CALENDAR
#endif

#define _CALENDAR_ALL_ACCOUNT_ID 0
#define _CALENDAR_LOCAL_ACCOUNT_ID -1

#endif /* _C_CAL_H_ */
