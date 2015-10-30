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

#ifndef _CAL_APP_H_
#define _CAL_APP_H_

#include "WApp.h"
#include "WAppEventHandler.h"

class CalApp : public WApp
{
public:
	CalApp();
	virtual ~CalApp();
private:

	/**
	 * Executes the create action.
	 *
	 * @return	true if it succeeds, false if it fails.
	 */
	virtual bool onCreate();

	/**
	 * Executes the terminate action.
	 */
	virtual void onTerminate();

	/**
	 * Executes the application control action.
	 *
	 * @param	request	   	The request.
	 * @param	firstLaunch	true to first launch.
	 */
	virtual void onAppControl(app_control_h request, bool firstLaunch);

	/**
	 * Executes the pause action.
	 */
	virtual void onPause();

	/**
	 * Executes the resume action.
	 */
	virtual void onResume();
private:
	WDISABLE_COPY_AND_ASSIGN(CalApp);
private:

	/**
	 * Shows the main.
	 *
	 * @param	request	   	The request.
	 * @param	firstLaunch	true to first launch.
	 */
	void __showMain(app_control_h request, bool firstLaunch);

	/**
	 * Shows the detail.
	 *
	 * @param	request	The request.
	 */
	void __showDetail(app_control_h request);
private:
	WAppEventHandler __languageChangedHandler;
	WAppEventHandler __regionFormatChangedHandler;
};

#endif
