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

#include "CalWidgetApp.h"
#include "CalTheme.h"
#include "GuiDefines.h"
#include "CalSettingsManager.h"
#include "CalEventManager.h"
#include "CalLocaleManager.h"

namespace
{
	inline CalWidget *getWidget(widget_context_h context)
	{
		void *tag = NULL;
		widget_app_context_get_tag(context, &tag);
		return static_cast<CalWidget*>(tag);
	}
}

CalWidgetApp::CalWidgetApp()
{
}

CalWidgetApp::~CalWidgetApp()
{
}

int CalWidgetApp::start(int argc, char *argv[])
{
	int result = WIDGET_ERROR_NONE;

	widget_app_lifecycle_callback_s ops = {0,};
	ops.create = CalWidgetApp::onAppCreate;
	ops.terminate = CalWidgetApp::onAppTerminate;

	result = widget_app_main(argc, argv, &ops, NULL);
	if (result != WIDGET_ERROR_NONE)
	{
		WERROR("widget_app_main() is failed. err = %d", result);
	}

	return result;
}

widget_class_h CalWidgetApp::onAppCreate(void *data)
{
	app_event_handler_h handlers[5] = {NULL, };

	widget_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
		APP_EVENT_LANGUAGE_CHANGED, CalWidgetApp::onAppLangChanged, data);
	widget_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
		APP_EVENT_REGION_FORMAT_CHANGED, CalWidgetApp::onAppRegionChanged, data);

	widget_instance_lifecycle_callback_s ops =
	{
		CalWidgetApp::onWidgetCreate,
		CalWidgetApp::onWidgetDestroy,
		CalWidgetApp::onWidgetPause,
		CalWidgetApp::onWidgetResume,
		CalWidgetApp::onWidgetResize,
		CalWidgetApp::onWidgetUpdate,
	};

	return widget_app_class_create(ops, data);
}

void CalWidgetApp::onAppTerminate(void *data)
{

}

void CalWidgetApp::onAppLangChanged(app_event_info_h event_info, void *data)
{
	// APP_EVENT_LANGUAGE_CHANGED
	char* lang = NULL;

	app_event_get_language( event_info, &lang );
	WDEBUG("changed language=%s", lang);

	CalLocaleManager::getInstance().updateLocaleForEvasObj();

	CalEvent event(CalEvent::LANGUAGE_CHANGED, CalEvent::REMOTE);
	CalEventManager::getInstance().notify(event);

	free(lang);
}

void CalWidgetApp::onAppRegionChanged(app_event_info_h event_info, void *data)
{
	// APP_EVENT_REGION_FORMAT_CHANGED
	char* region = NULL;

	app_event_get_region_format( event_info, &region );
	WDEBUG("changed region=%s", region);

	CalSettingsManager::getInstance().updateRegion();
	CalEvent event(CalEvent::SETTING_CHANGED, CalEvent::REMOTE);
	CalEventManager::getInstance().notify(event);

	free(region);
}

int CalWidgetApp::onWidgetCreate(widget_context_h context, bundle *content, int w, int h, void *data)
{
	CalWidget *widget = new CalWidget();
	if (!widget)
	{
		return WIDGET_ERROR_OUT_OF_MEMORY;
	}

	bindtextdomain(CALENDAR, CAL_LOCALE_DIR);
	CalTheme::initialize();

	elm_app_base_scale_set(APP_BASE_SCALE);
	elm_config_accel_preference_set("3d");

	widget_app_context_set_tag(context, widget);
	widget->create(context, content, w, h);

	return WIDGET_ERROR_NONE;
}

int CalWidgetApp::onWidgetDestroy(widget_context_h context, widget_app_destroy_type_e reason, bundle *content, void *data)
{
	CalWidget *widget = getWidget(context);
	if(widget)
	{
		widget->destroy(reason);
	}

	return WIDGET_ERROR_NONE;
}

int CalWidgetApp::onWidgetPause(widget_context_h context, void *data)
{
	return WIDGET_ERROR_NONE;
}

int CalWidgetApp::onWidgetResume(widget_context_h context, void *data)
{
	return WIDGET_ERROR_NONE;
}

int CalWidgetApp::onWidgetResize(widget_context_h context, int w, int h, void *data)
{
	CalWidget *widget = getWidget(context);
	if(widget)
	{
		widget->resize(w, h);
	}

	return WIDGET_ERROR_NONE;
}

int CalWidgetApp::onWidgetUpdate(widget_context_h context, bundle *content, int force, void *data)
{
	return WIDGET_ERROR_NONE;
}

