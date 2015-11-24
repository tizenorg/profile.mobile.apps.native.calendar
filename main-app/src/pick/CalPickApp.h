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

#ifndef _CAL_PICK_APP_H_
#define _CAL_PICK_APP_H_

#include <app_control.h>
#include <Ecore.h>
#include <memory>
#include <list>
#include "CalPickView.h"
#include "CalSchedule.h"
#include "CalNaviframe.h"

#define MULTI_SELECT_MAX 10

class CalPickApp
{
public:
	CalPickApp(CalNaviframe *naviframe);
	virtual ~CalPickApp();

	enum SelectionMode
	{
		SELECTION_MODE_SINGLE,
		SELECTION_MODE_MULTIPLE
	};

	enum MimeType
	{
		MIME_VCS,
		MIME_ICS
	};

	void onAppControl(app_control_h request, bool firstLaunch);

private:
	void __processResult(const std::list<std::shared_ptr<CalSchedule>>& schedules);

	CalPickView::ResultType __resultType;
	int __mode;
	int __mimeType;
	int __maxLimit;
	bool __isDone;
	app_control_h __request;
	CalNaviframe *__naviframe;
};
#endif
