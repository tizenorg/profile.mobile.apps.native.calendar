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

#include <app_control.h>
#include <Ecore.h>
#include "WEvasObject.h"

#define __UI_LOCK_DURATION 3.0

/**
 * @brief AppControl launcher.
 */
class WAPP_ASSIST_EXPORT CalAppControlLauncher
{
public:
	/**
	 * @brief Get the class instance.
	 *
	 * @return The reference to CalAppControlLauncher class instance.
	 */
	static CalAppControlLauncher& getInstance();

	/**
	 * @brief Destroy CalAppControlLauncher class instance.
	 */
	static void destroy();

public:
	/**
	 * @brief Send the launch request.
	 *
	 * @param[in]   request      app_control handle.
	 * @param[in]   callback     Callback function to be called when the reply is delivered.
	 * @param[in]   userData     User data to be passed to the callback function.
	 * @param[in]   objToBlock   Object to block.
	 * @param[in]   lockTime     Lock time.
	 *
	 * @return True if succeed, otherwise false.
	 */
	bool sendLaunchRequest( app_control_h request, app_control_reply_cb callback, void *userData, Evas_Object* objToBlock, double lockTime = __UI_LOCK_DURATION);

	/**
	 * @brief Send the terminate request to the application that is launched by app_control.
	 */
	void terminate();

	/**
	 * @brief Unblock Ui.
	 */
	void unblockUi();

	/**
	 * @brief Get Ui blocked state.
	 *
	 * @return Ui blocked state.
	 */
	bool isUiBlocked();

private:
	CalAppControlLauncher();
	virtual ~CalAppControlLauncher();
	void __clearAll();
	static Eina_Bool __timeOutCb(void *userData);
	static void __evasObjDelCb(void *data, Evas *e, Evas_Object *obj, void *event_info);

	static CalAppControlLauncher* __instance;
	Ecore_Timer* __timer;
	WEvasObjectPtr __evasObjBlocked;
	app_control_h __appControl;

};

#endif // _CONTACTS_APP_CONTROL_LAUNCHER_H_
