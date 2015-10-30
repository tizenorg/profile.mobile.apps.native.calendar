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

#ifdef GBS_BUILD
#ifndef _WWIDGETAPP_H_
#define _WWIDGETAPP_H_

#include <app.h>
#include <app_control.h>
#include <widget.h>
#include <widget_errno.h>
#include <widget_provider_app.h>
#include <widget_internal.h>
#include <Evas.h>

#include "CalCommon.h"
#include "WDefine.h"
#include "WWindow.h"
#include "WWidget.h"

class WWidget;

class WAPP_ASSIST_EXPORT WWidgetApp
{
public:
	WWidgetApp();
	virtual ~WWidgetApp();

public:
	int start(int argc, char* argv[]);

protected:
	void exit();

	virtual bool onAppCreate();
	virtual void onAppTerminate();
	virtual void onAppPause();
	virtual void onAppResume();
	virtual void onAppAppControl(app_control_h request);

	virtual WWidget *onWidgetCreate(const char *id, const char *content, int w, int h);
	virtual int onWidgetDestroy(WWidget *widget, widget_destroy_type_e reason);
	virtual void onWidgetConnected();
	virtual void onWidgetDisconnected();

private:
	WDISABLE_COPY_AND_ASSIGN(WWidgetApp);

private:
	void onWidgetDisconnectedTemplate();
	void onAppControlTemplate(app_control_h request, void *user_data);
	void onAppTerminateTemplate();
	WWidget *onWidgetCreateTemplate(const char *id, const char *content, int w, int h);
	int onWidgetDestroyTemplate(const char *id, widget_destroy_type_e reason);
	int onWidgetResize(const char *id, int w, int h);
	int onWidgetUpdate(const char *id, const char *content, int force);
	int onWidgetSignal(const char *id, const char *signal_name, const char *source, struct widget_event_info *info);
	int onWidgetPause(const char *id);
	int onWidgetResume(const char *id);
};

#endif /* _WWIDGETAPP_H_ */
#endif //GBS_BUILD
