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

#ifndef _WWIDGET_H_
#define _WWIDGET_H_

#include <widget.h>
#include <app_control.h>
#include <widget_errno.h>
#include <widget_provider_app.h>
#include <Evas.h>

#include "CalCommon.h"
#include "WDefine.h"
#include "WWindow.h"

#include <string>

class WWidgetApp;

class WAPP_ASSIST_EXPORT WWidget
{
public:
	WWidget(const char *id, int w, int h);
	virtual ~WWidget();

	const std::string &getId() const;
	int getWidth() const;
	int getHeight() const;

protected:
	WWindow *getWindow() const;

protected:
	virtual int onResize(int w, int h);
	virtual int onUpdate(const char *content, int force);
	virtual int onSignal(const char *signal_name, const char *source, widget_event_info *info);
	virtual int onPause();
	virtual int onResume();

private:
	int resize(int w, int h);
	int destroy(widget_destroy_type_e reason);
	int update(const char *content, int force);
	int signal(const char *signal_name, const char *source, widget_event_info *info);
	int pause();
	int resume();

private:
	friend class WWidgetApp;
	WDISABLE_COPY_AND_ASSIGN(WWidget);

private:
	void createWindow();

private:
	int __width;
	int __height;
	WWindow *__win;
	Evas_Object *__winEvasObject;
	Evas_Object *__bgEvasObject;
	Evas_Object *__layoutEvasObject;
	std::string __id;
};

#endif /* _WWIDGET_H_ */
#endif // GBS_BUILD
