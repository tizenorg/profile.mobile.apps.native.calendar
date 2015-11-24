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

#include <memory>
#include <app_control.h>
#include "CalSchedule.h"
#include "CalDetailView.h"
#include "CalNaviframe.h"

class CalViewApp
{
public:
	CalViewApp(CalNaviframe *naviframe);
	virtual ~CalViewApp();
	void onAppControl(app_control_h request, bool firstLaunch);

private:
	bool __isVtsFile(const char* filePath);
	const char* __getFilePath();
	void __replyError();
	CalDetailView::MenuState __getMenuState();

	CalNaviframe *__naviframe;
	bool __replyToRequest;
	app_control_h __request;
	std::shared_ptr<CalSchedule> __getScheduleFromExtraData(app_control_h request);
};
#endif
