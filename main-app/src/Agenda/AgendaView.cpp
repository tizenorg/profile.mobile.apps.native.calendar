/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "Agenda/AgendaView.h"
#include "App/Path.h"
#include "Utils/Logger.h"

using namespace App;
using namespace Calendar::Agenda;

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
	elm_image_file_set(image, App::getResourcePath("images/org.tizen.calendar.png").c_str(), NULL);
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
