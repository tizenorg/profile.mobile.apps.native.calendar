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


#ifndef CALWIDGETAPP_H_
#define CALWIDGETAPP_H_

#include <widget_app.h>

class CalWidgetApp
{
public:
	CalWidgetApp();
	~CalWidgetApp();

	int start(int argc, char *argv[]);

private:
	// widget_app_lifecycle_callback_s callbacks
	static widget_class_h onAppCreate(void *data);
	static void onAppTerminate(void *data);

	// event_handler callbacks
	static void onAppLangChanged(app_event_info_h event_info, void *data);
	static void onAppRegionChanged(app_event_info_h event_info, void *data);

	// widget_instance_lifecycle_callback_s callbacks
	static int onWidgetCreate(widget_context_h context, bundle *content, int w, int h, void *data);
	static int onWidgetDestroy(widget_context_h context, widget_app_destroy_type_e reason, bundle *content, void *data);
	static int onWidgetPause(widget_context_h context, void *data);
	static int onWidgetResume(widget_context_h context, void *data);
	static int onWidgetResize(widget_context_h context, int w, int h, void *data);
	static int onWidgetUpdate(widget_context_h context, bundle *content, int force, void *data);
};

#endif /* CALWIDGETAPP_H_ */

