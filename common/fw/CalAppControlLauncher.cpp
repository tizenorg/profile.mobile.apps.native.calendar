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

#include "CalAppControlLauncher.h"
#include "CalCommon.h"

CalAppControlLauncher* CalAppControlLauncher::__instance = NULL;

CalAppControlLauncher& CalAppControlLauncher::getInstance()
{
	if(__instance == NULL)
	{
		__instance = new CalAppControlLauncher();
	}

	return *__instance;
}

void CalAppControlLauncher::destroy()
{
	delete __instance;
	__instance = NULL;
}

CalAppControlLauncher::CalAppControlLauncher()
{
	__appControl = NULL;
	__timer = NULL;
}

CalAppControlLauncher::~CalAppControlLauncher()
{
	__clearAll();
}

bool CalAppControlLauncher::sendLaunchRequest(app_control_h request, app_control_reply_cb callback, void *userData, Evas_Object* objToBlock, double lockTime)
{
	WENTER();
	__clearAll();

	__appControl = request;

	if (objToBlock)
	{
		__evasObjBlocked = WEvasObject_getWeakPtr(objToBlock);
		auto p = __evasObjBlocked.lock();

		evas_object_freeze_events_set(p->getEvasObj(), EINA_TRUE);

		__timer = ecore_timer_add(lockTime, __timeOutCb, this);
		evas_object_event_callback_add(p->getEvasObj(), EVAS_CALLBACK_DEL, __evasObjDelCb, this);
	}

	int ret = app_control_send_launch_request(request, callback, userData);
	if(ret != APP_CONTROL_ERROR_NONE)
	{
		WERROR("app_control return error code= %d", ret);
		app_control_destroy(__appControl);
		__appControl = NULL;
		return false;
	}

	return true;
}

void CalAppControlLauncher::terminate()
{
	__clearAll();
}

void CalAppControlLauncher::unblockUi()
{
	if(auto safeObj = __evasObjBlocked.lock())
	{
		Evas_Object* obj = safeObj->getEvasObj();
		evas_object_freeze_events_set(obj, EINA_FALSE);
		evas_object_event_callback_del_full(obj, EVAS_CALLBACK_DEL, __evasObjDelCb, this);
		__evasObjBlocked.reset();
	}

	if(__timer)
	{
		ecore_timer_del(__timer);
		__timer = NULL;
	}
}

bool CalAppControlLauncher::isUiBlocked()
{
	if (auto safeObj = __evasObjBlocked.lock())
	{
		return true;
	}

	return false;
}

Eina_Bool CalAppControlLauncher::__timeOutCb(void* userData)
{
	WENTER();

	CalAppControlLauncher* p = (CalAppControlLauncher*)userData;

	if( auto safeObj = p->__evasObjBlocked.lock() )
	{
		Evas_Object* obj = safeObj->getEvasObj();
		evas_object_freeze_events_set(obj, EINA_FALSE);
		evas_object_event_callback_del_full(obj, EVAS_CALLBACK_DEL, __evasObjDelCb, p);
		p->__evasObjBlocked.reset();
	}
	p->__timer = NULL;

	return ECORE_CALLBACK_CANCEL;
}

void CalAppControlLauncher::__evasObjDelCb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	CalAppControlLauncher* p = (CalAppControlLauncher*)data;
	if ( p->__timer)
	{
		ecore_timer_del( p->__timer);
		p->__timer = NULL;
	}
}

void CalAppControlLauncher::__clearAll()
{
	if( __appControl )
	{
		WHIT();
		int ret = app_control_send_terminate_request( __appControl );
		if( ret != APP_CONTROL_ERROR_NONE )
		{
			WWARNING("app_control ret error code= %d", ret);
		}
		app_control_destroy(__appControl);
		__appControl = NULL;
	}
	unblockUi();
}

