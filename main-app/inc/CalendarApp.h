#ifndef CALENDARAPP_H
#define CALENDARAPP_H

#include "App/Application.h"
#include "Ui/Window.h"
#include "Ui/Naviframe.h"

using namespace App;

namespace Calendar
{
	class EXPORT_API CalendarApp : public Application
	{
	public:
		CalendarApp();
		virtual ~CalendarApp();


private:
	virtual bool onCreate() override;
	virtual void onAppControl(app_control_h request) override;
	virtual void onPause() override;
	virtual void onResume() override;
	virtual void onTerminate() override;
	static void onLanguageChanged(app_event_info_h event, void *data);

	Ui::Window *m_Window;
	Ui::Navigator *m_Navigator;

	Ui::View *m_AgendaView;

	};

}
#endif // CALENDARAPP_H
