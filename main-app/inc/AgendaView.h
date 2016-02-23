#ifndef AGENDAVIEW_H
#define AGENDAVIEW_H

#include "Ui/View.h"

using namespace Ui;

namespace Calendar
{

	class AgendaView : public View
	{
	public:
		AgendaView();
		~AgendaView();

		virtual Evas_Object* onCreate(Evas_Object *parent) override;
		virtual void onPageAttached() override;
	};

}
#endif // AGENDAVIEW_H
