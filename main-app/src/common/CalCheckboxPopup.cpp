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

#include <Elementary.h>

#include "CalCheckboxPopup.h"
#include "CalCommon.h"


CalCheckboxPopup::CalCheckboxPopup(const char* title, const char* message, const char* checkboxMessage) :
    WPopup(title, NULL),
	__message(message),
	__checkboxMessage(checkboxMessage),
    __checkbox(NULL)
{
	WENTER();
}

CalCheckboxPopup::~CalCheckboxPopup()
{
	WENTER();
}

const char* CalCheckboxPopup::getClassName()
{
	return "CalCheckboxPopup";
}

Evas_Object* CalCheckboxPopup::__createContent(Evas_Object* parent)
{
    Evas_Object* layout = elm_layout_add(parent);

    elm_layout_file_set(layout, CAL_EDJE, getClassName());

    evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	elm_object_part_text_set(layout, "text", __message.c_str());
	elm_object_part_text_set(layout, "checkbox_text", __checkboxMessage.c_str());

    evas_object_show(layout);

    __checkbox = elm_check_add(layout);
	elm_object_style_set(__checkbox, "popup");
    evas_object_size_hint_weight_set(__checkbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(__checkbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_text_set( __checkbox, __checkboxMessage.c_str());
    elm_object_part_content_set(layout, "elm.swallow.end", __checkbox);

	Evas_Object* label = elm_label_add(layout);
	elm_object_style_set(label, "popup/default");
	elm_label_line_wrap_set(label, ELM_WRAP_MIXED);
	elm_object_text_set(label, __message.c_str());
	evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(label);
	elm_object_part_content_set(layout, "elm.swallow.content", label);

	return layout;
}

Evas_Object* CalCheckboxPopup::onCreate(Evas_Object* parent, void* param)
{
	WENTER();

	setContent([this](Evas_Object* parent)->Evas_Object* {
		return __createContent(parent);
	});

	return WPopup::onCreate(parent, param);
}

void CalCheckboxPopup::onDestroy()
{
	WENTER();
}

bool CalCheckboxPopup::getCheckboxState()
{
	WENTER();

	return elm_check_state_get(__checkbox) ? true : false;
}
