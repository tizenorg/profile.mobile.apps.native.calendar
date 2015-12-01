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

#ifndef _WDEBUG_H_
#define _WDEBUG_H_

#include <dlog.h>
#include <string.h>
#include <assert.h>

#define CALENDAR "calendar"
#ifndef LOGD
#define LOGD(fmt, ...) dlog_print(DLOG_DEBUG, CALENDAR, fmt)
#endif
#ifndef LOGE
#define LOGE(fmt, ...) dlog_print(DLOG_ERROR, CALENDAR, fmt)
#endif
#ifndef LOGW
#define LOGW(fmt, ...) dlog_print(DLOG_WARN, CALENDAR, fmt)
#endif
#ifndef LOGI
#define LOGI(fmt, ...) dlog_print(DLOG_INFO, CALENDAR, fmt)
#endif

#define WCOLOR_RED "\033[0;31m"
#define WCOLOR_GREEN "\033[0;32m"
#define WCOLOR_BROWN "\033[0;33m"
#define WCOLOR_BLUE "\033[0;34m"
#define WCOLOR_PURPLE "\033[0;35m"
#define WCOLOR_CYAN "\033[0;36m"
#define WCOLOR_LIGHTBLUE "\033[0;37m"
#define WCOLOR_END "\033[0;m"

// normal log
#define WDEBUG(fmt, args...) LOGD(fmt, ##args)
#define WDEBUG_(fmt, args...) LOGD(WCOLOR_RED fmt WCOLOR_END, ##args) // temporal red-colored text for debugging
#define WVERBOSE(fmt, args...) LOGD(WCOLOR_BROWN fmt WCOLOR_END, ##args)
#define WINFO(fmt, args...) LOGI(WCOLOR_GREEN fmt WCOLOR_END, ##args)
#define WWARNING(fmt, args...) LOGW(WCOLOR_PURPLE fmt WCOLOR_END, ##args)
#define WERROR(fmt, args...) LOGE(WCOLOR_RED "* Critical * " fmt WCOLOR_END, ##args)

#define WHIT() LOGD(WCOLOR_BLUE ">>>HIT<<<" WCOLOR_END)
#define WHITM(fmt, args... ) LOGD(WCOLOR_BLUE ">>>HIT<<<" fmt WCOLOR_END, ##args)

#define WENTER() LOGD(WCOLOR_GREEN "BEGIN >>>>" WCOLOR_END)
#define WLEAVE() LOGD(WCOLOR_GREEN "END <<<<" WCOLOR_END)

// secure log
#define WSDEBUG(fmt, args...) SECURE_LOGD(fmt, ##args)
#define WSVERBOSE(fmt, args...) SECURE_LOGD(WCOLOR_BROWN fmt WCOLOR_END, ##args)
#define WSINFO(fmt, args...) SECURE_LOGI(WCOLOR_GREEN fmt WCOLOR_END, ##args)
#define WSERROR(fmt, args...) SECURE_LOGE(WCOLOR_RED "* Critical * " fmt WCOLOR_END, ##args)

#define WPRET_VM(expr, val, fmt, arg...) do { \
					if (expr) { \
						WERROR(fmt, ##arg); \
						return (val); \
						} \
					} while (0)

#define WPRET_M(expr, fmt, arg...) do { \
					if (expr) { \
					WERROR(fmt, ##arg); \
					return; \
					} \
				} while (0)

#define WASSERT(expr) do { \
		if(!(expr)) \
		{\
 			WERROR( "WASSERT!" );\
			assert((expr)); \
		}\
	}while(0);

#define WASSERT_EX(expr, fmt, args... ) do { \
		if(!(expr)) \
		{\
 			WERROR( "WASSERT! " fmt, ##args);\
			assert((expr)); \
		}\
	}while(0);



#endif /* _WDEBUG_H_ */
