/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "CalendarApp.h"
#include "Calendar/Agenda/AgendaView.h"

#include "Ui/Naviframe.h"
#include "Ui/Window.h"
#include "Ui/Scale.h"

#include "App/Path.h"
#include "Utils/Logger.h"

using namespace Calendar::Agenda;

CalendarApp::CalendarApp():
	m_Window(nullptr),
	m_Navigator(nullptr),
	m_AgendaView(nullptr)
{

}

bool CalendarApp::onCreate()
{
	bindtextdomain(TEXT_DOMAIN, App::getLocaleDir().c_str());
	textdomain(TEXT_DOMAIN);

	app_event_handler_h handle = nullptr;
	ui_app_add_event_handler(&handle, APP_EVENT_LANGUAGE_CHANGED,
			&CalendarApp::onLanguageChanged, this);

	elm_app_base_scale_set(UI_BASE_SCALE);

	m_Window = new Ui::Window();
	m_Window->create(nullptr);

	m_Navigator = new Ui::Naviframe;
	m_Window->attachView(m_Navigator);

	m_AgendaView = new AgendaView();
	m_Navigator->navigateTo(m_AgendaView);

	return true;
}

void CalendarApp::onTerminate()
{

}

void CalendarApp::onLanguageChanged(app_event_info_h event, void *data)
{
	char *lang = nullptr;
	app_event_get_language(event, &lang);
	if (lang) {
		elm_language_set(lang);
		free(lang);
	}
}

void CalendarApp::onPause()
{

}

void CalendarApp::onResume()
{

}

void CalendarApp::onAppControl(app_control_h request)
{

}

