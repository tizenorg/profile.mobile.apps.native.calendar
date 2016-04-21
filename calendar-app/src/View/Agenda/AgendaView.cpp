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

#include "App/Path.h"
#include "Utils/Logger.h"

#include "View/Agenda/AgendaView.h"
#include "agenda/edje/inc/AgendaLayout.h"

using namespace View::Agenda;

AgendaView::AgendaView()
{
}

Evas_Object *AgendaView::onCreate(Evas_Object *parent)
{
	TRACE;
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_file_set(layout, App::getResourcePath(AGENDA_LAYOUT_PATH).c_str(), AGENDA_LAYOUT);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	evas_object_show(layout);
	return layout;
}

void AgendaView::onPageAttached(Ui::NavigatorPage *page)
{
	TRACE;
	page->setTitle("IDS_CLD_BODY_CALENDAR_M_APPLICATION_NAME_ABB");

}
