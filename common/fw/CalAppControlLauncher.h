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

#ifndef _CAL_APP_CONTROL_LAUNCHER_H_
#define _CAL_APP_CONTROL_LAUNCHER_H_

#include <app.h>
#include <Evas.h>
#include <Eina.h>
#include <Elementary.h>
#include <memory>
#include "WEvasObject.h"


#define __UI_LOCK_DURATION 3.0

class WAPP_ASSIST_EXPORT CalAppControlLauncher
{
public:
	static CalAppControlLauncher& getInstance();
	static void destroy();

public:
	bool sendLaunchRequest( app_control_h request, app_control_reply_cb callback, void *userData, Evas_Object* objToBlock, double lockTime = __UI_LOCK_DURATION);
	void terminate();
	void unblockUi();
	bool isUiBlocked();

private:
	void __clearAll();
	static Eina_Bool __timeOutCb(void *userData);
	static void __evasObjDelCb(void *data, Evas *e, Evas_Object *obj, void *event_info);
private:
	CalAppControlLauncher();
	virtual ~CalAppControlLauncher();

private:
	static CalAppControlLauncher* __instance;
	Ecore_Timer* __timer;
	WEvasObjectPtr __evasObjBlocked;
	app_control_h __appControl;

};


#endif // _CONTACTS_APP_CONTROL_LAUNCHER_H_
