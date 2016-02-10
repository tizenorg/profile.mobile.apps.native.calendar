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

#ifndef _C_CAL_DEFINES_H_
#define _C_CAL_DEFINES_H_

#define XSTR(x) #x
#define STR(x) XSTR(x)

#define CAL_DIR			STR(CAL_PREFIX)

#ifdef PACKAGE_NAME
#define CAL_PKGNAME		PACKAGE_NAME
#else
#define CAL_PKGNAME		"org.tizen.calendar"
#endif

#define CALENDAR_NOTI_PACKAGE CAL_PKGNAME ".alert"

//TODO: Should be used in CalPath. Defined only in GBS env.
#define CAL_RES_DIR		CAL_DIR "/res/"

#define CAL_IMAGE_DIR		"images/"
#define CAL_EDJE_DIR		"edje/"
#define CAL_THEME_DIR		"theme/"
#define CAL_LOCALE_DIR		"locale/"
#define CAL_EDJE			"master.edj"
#define CAL_THEME			"theme.edj"

#endif /* _C_CAL_DEFINES_H_ */
