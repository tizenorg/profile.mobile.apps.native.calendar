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

#ifndef _C_CAL_LOG_H_
#define _C_CAL_LOG_H_

#define CAL_DEBUGGING

//#define CAL_TIMECHECK
//#define CAL_USE_OWN_TIME

#define LOG_TAG "CALENDAR"
#include <dlog.h>

#define INFO(fmt, arg...) SLOGI(fmt, ##arg)
#define ERR(fmt, arg...) SLOGE(fmt, ##arg)
#define DBG(fmt, arg...) SLOGD(fmt, ##arg)
#define SEC_INFO(fmt, arg...) SECURE_LOGI(fmt, ##arg)
#define SEC_ERR(fmt, arg...) SECURE_LOGE(fmt, ##arg)
#define SEC_DBG(fmt, arg...) SECURE_LOGD(fmt, ##arg)

#ifdef CAL_DEBUGGING

#ifdef CAL_USE_OWN_TIME
#define CAL_FN_START CAL_TIME_CHECK_START
#define CAL_FN_END CAL_TIME_CHECK_END
#else//CAL_USE_OWN_TIME
#ifdef CAL_TIMECHECK
#define CAL_FN_START DBG("START")
#define CAL_FN_END DBG("END")
#else//CAL_TIMECHECK
#define CAL_FN_START DBG("")
#define CAL_FN_END
#endif//CAL_TIMECHECK
#endif//CAL_USE_OWN_TIME

#define c_warn_if(expr, fmt, arg...) do { \
	if (expr) { \
		ERR("(%s) -> " fmt, #expr, ##arg); \
	} \
} while (0)
#define c_ret_if(expr) do { \
	if (expr) { \
		DBG("(%s) -> %s() return", #expr, __FUNCTION__); \
		return; \
	} \
} while (0)
#define c_retv_if(expr, val) do { \
	if (expr) { \
		DBG("(%s) -> %s() return", #expr, __FUNCTION__); \
		return (val); \
	} \
} while (0)
#define c_retm_if(expr, fmt, arg...) do { \
	if (expr) { \
		ERR(fmt, ##arg); \
		DBG("(%s) -> %s() return", #expr, __FUNCTION__); \
		return; \
	} \
} while (0)
#define c_retvm_if(expr, val, fmt, arg...) do { \
	if (expr) { \
		ERR(fmt, ##arg); \
		DBG("(%s) -> %s() return", #expr, __FUNCTION__); \
		return (val); \
	} \
} while (0)
#else //CAL_DEBUGGING
#define CAL_FN_START
#define CAL_FN_END
#define DBG(...)

#define c_warn_if(expr, fmt, arg...) do { \
	if (expr) { \
		ERR(fmt, ##arg); \
	} \
} while (0)
#define c_ret_if(expr) do { \
	if (expr) { \
		return; \
	} \
} while (0)
#define c_retv_if(expr, val) do { \
	if (expr) { \
		return (val); \
	} \
} while (0)
#define c_retm_if(expr, fmt, arg...) do { \
	if (expr) { \
		ERR(fmt, ##arg); \
		return; \
	} \
} while (0)
#define c_retvm_if(expr, val, fmt, arg...) do { \
	if (expr) { \
		ERR(fmt, ##arg); \
		return (val); \
	} \
} while (0)

#endif //CAL_DEBUGGING

#ifdef CAL_TIMECHECK
double cal_correction, cal_startT;
double cal_set_start_time(void);
double cal_exec_time(double start, double correction);
double cal_init_time(double correction);
#define CAL_TIME_CHECK_START   \
	double cal_startT = 0, cal_correction = 0; \
	cal_correction = cal_init_time(cal_correction);\
	cal_startT = cal_set_start_time();
#define CAL_TIME_CHECK_END DBG("time = %f ms\n", cal_exec_time(cal_startT, cal_correction));
#else //CAL_TIMECHECK
#define CAL_TIME_CHECK_START
#define CAL_TIME_CHECK_END
#endif //CAL_TIMECHECK

#define CAL_LAUNCH_IN(fmt, arg...)  LOG(LOG_DEBUG, "LAUNCH", "[calendar:Application:%s:IN]" fmt, __FUNCTION__, ##arg)
#define CAL_LAUNCH_OUT(fmt, arg...) LOG(LOG_DEBUG, "LAUNCH", "[calendar:Application:%s:OUT]" fmt, __FUNCTION__, ##arg)

#define CAL_LAUNCH_LOG_DETAIL

#ifdef CAL_LAUNCH_LOG_DETAIL
	#define CAL_LAUNCH_IN_DETAIL(fmt, arg...)  LOG(LOG_DEBUG, "LAUNCH", "[calendar:Application:%s:IN]" fmt, __FUNCTION__, ##arg)
	#define CAL_LAUNCH_OUT_DETAIL(fmt, arg...) LOG(LOG_DEBUG, "LAUNCH", "[calendar:Application:%s:OUT]" fmt, __FUNCTION__, ##arg)
	#define CAL_LAUNCH_IN_DETAIL_CUSTOM(fn, fmt, arg...)  LOG(LOG_DEBUG, "LAUNCH", "[calendar:Application:%s:IN]" fmt, fn, ##arg)
	#define CAL_LAUNCH_OUT_DETAIL_CUSTOM(fn, fmt, arg...) LOG(LOG_DEBUG, "LAUNCH", "[calendar:Application:%s:OUT]" fmt, fn, ##arg)
#else
	#define CAL_LAUNCH_IN_DETAIL(fmt, arg...)
	#define CAL_LAUNCH_OUT_DETAIL(fmt, arg...)
	#define CAL_LAUNCH_IN_DETAIL_CUSTOM(fn, fmt, arg...)
	#define CAL_LAUNCH_OUT_DETAIL_CUSTOM(fn, fmt, arg...)
#endif

#endif /* _C_CAL_LOG_H_ */
