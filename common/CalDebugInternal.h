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

#ifndef _CAL_DEBUG_INTERNAL_H_
#define _CAL_DEBUG_INTERNAL_H_

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#ifndef TIZEN_WIDGET
#define TIZEN_WIDGET ""
#endif

#define LOG_TAG "CALENDAR" TIZEN_WIDGET

#include "WDebugBase.h"

#define ERR WERROR
#define c_warn_if(expr, fmt, arg...) do { \
	if (expr) { \
		WERROR(fmt, ##arg); \
	} \
} while (0)

#define c_retvm_if WPRET_VM
#define c_retv_if(expr, val) WPRET_VM(expr, val, "(%s) expression", #expr)
#define c_retm_if(expr, fmt, arg...) WPRET_M(expr, "(%s) expression", #expr)

#endif /* _CAL_DEBUG_INTERNAL_H_ */
