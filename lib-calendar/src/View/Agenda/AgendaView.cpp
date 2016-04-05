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
#include "App/Path.h"
#include "Utils/Logger.h"

#include "View/Common/EventList.h"
#include "View/Common/EventListItem.h"
#include "View/Agenda/AgendaView.h"

#include "common/edje/inc/UiCommon.h"

using namespace Calendar::View::Agenda;
using namespace Calendar::View::Common;

AgendaView::AgendaView():
		m_EventList(nullptr)
{

}

Evas_Object *AgendaView::onCreate(Evas_Object *parent)
{
	TRACE;
	elm_theme_extension_add(NULL, App::getResourcePath("common/edje/event_item.edj").c_str());

	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_file_set(layout, App::getResourcePath("agenda/edje/agenda_view.edj").c_str(), AGENDA_VIEW);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(layout);

	m_EventList = new EventList();
	m_EventList->create(layout);
	elm_genlist_homogeneous_set(m_EventList->getEvasObject(), EINA_FALSE);

	elm_object_part_content_set(layout, AGENDA_PART_NAME_LIST_CONTROL, m_EventList->getEvasObject());

	return layout;
}

void AgendaView::onPageAttached(Ui::NavigatorPage *page)
{
	TRACE;
	page->setTitle("IDS_CLD_BODY_CALENDAR_M_APPLICATION_NAME_ABB");

	for (int i = 0; i < 5; i++) {
		m_EventList->insert(new EventListItem());
	}
}
