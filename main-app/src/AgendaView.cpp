#include "AgendaView.h"
#include "App/Path.h"
#include "Utils/Logger.h"

using namespace App;
using namespace Calendar;

AgendaView::AgendaView()
{

}

AgendaView::~AgendaView()
{

}

Evas_Object *AgendaView::onCreate(Evas_Object *parent)
{
	TRACE;
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "application", "default");

	Evas_Object *image = elm_image_add(layout);
	elm_image_file_set(image, App::getResourcePath("common/images/org.tizen.calendar.png").c_str(), NULL);
	evas_object_size_hint_weight_set(image, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(image, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(image);

	elm_layout_content_set(layout, "elm.swallow.content", image);

	evas_object_show(layout);
	return layout;
}

void AgendaView::onPageAttached()
{
	getPage()->setTitle("IDS_CLD_BODY_CALENDAR_M_APPLICATION_NAME_ABB");
}
