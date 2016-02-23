#include "CalendarApp.h"
#include "AgendaView.h"

#include "Ui/Scale.h"
#include "App/Path.h"
#include "Utils/Logger.h"


using namespace Calendar;
using namespace Ui;

CalendarApp::CalendarApp():
	m_Window(nullptr),
	m_Navigator(nullptr),
	m_AgendaView(nullptr)
{

}

CalendarApp::~CalendarApp()
{

}

bool CalendarApp::onCreate()
{
	TRACE;
	bindtextdomain(TEXT_DOMAIN, App::getLocaleDir().c_str());
	textdomain(TEXT_DOMAIN);

	app_event_handler_h handle = nullptr;
	ui_app_add_event_handler(&handle, APP_EVENT_LANGUAGE_CHANGED,
			&CalendarApp::onLanguageChanged, this);

	elm_app_base_scale_set(UI_BASE_SCALE);

	m_Window = new Ui::Window();
	m_Window->create(nullptr);

	m_Navigator = new Ui::Naviframe();
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

