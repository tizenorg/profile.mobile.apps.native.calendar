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

#ifndef _CAL_VIEW_APP_H_
#define _CAL_VIEW_APP_H_

#include "WApp.h"
#include "CalSchedule.h"
#include <Ecore.h>
#include <memory>
#include "WAppEventHandler.h"
#include "CalDetailView.h"

class CalViewApp : public WApp
{
public:
	CalViewApp();
	virtual ~CalViewApp();
protected:
	virtual bool onCreate();
	virtual void onTerminate();
	virtual void onAppControl(app_control_h request, bool firstLaunch);
	virtual void onPause();
	virtual void onResume();
private:
	WDISABLE_COPY_AND_ASSIGN(CalViewApp);
private:
	bool __isVtsFile(const char* filePath);
	const char* __getFilePath();
	void __replyError();
	void __makeSchedule();
private:
	bool __replyToRequest;
	app_control_h __request;
	std::shared_ptr<CalSchedule> __getScheduleFromExtraData(app_control_h request);
	WAppEventHandler __languageChangedHandler;
	WAppEventHandler __regionFormatChangedHandler;

};

#endif
