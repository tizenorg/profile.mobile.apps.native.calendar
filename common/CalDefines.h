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

#define CAL_DIR      	 STR(CAL_PREFIX)
#define CAL_PKGNAME      "org.tizen.calendar"
#define CALENDAR_NOTI_PACKAGE CAL_PKGNAME ".alert"
#define CAL_RES_DIR      CAL_DIR "/res"
#define CAL_IMAGE_DIR    CAL_RES_DIR "/images/"
#define CAL_EDJE_DIR     CAL_RES_DIR "/edje"
#define CAL_EDJE         CAL_EDJE_DIR "/master.edj"
#define CAL_THEME_EDJE   CAL_EDJE_DIR "/theme/theme.edj"
#define CAL_IMAGE_EDJE   CAL_EDJE_DIR "/image.edj"
#define CAL_LOCALE_DIR   CAL_RES_DIR "/locale"

#endif /* _C_CAL_DEFINES_H_ */
