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
#include "CalCommon.h"
#include "CalEditLoadingPopup.h"


CalEditLoadingPopup::CalEditLoadingPopup() :
	__parent(NULL)
{
	WENTER();
}

CalEditLoadingPopup::~CalEditLoadingPopup()
{
	WENTER();
}

const char* CalEditLoadingPopup::getClassName()
{
	return "CalEditLoadingPopup";
}


Evas_Object* CalEditLoadingPopup::onCreate(Evas_Object* parent, void* param)
{
	WENTER();

	__parent = parent;

	Evas_Object *box = elm_box_add(parent);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	Evas_Object *progressbar = elm_progressbar_add(box);
	elm_object_style_set(progressbar, "process_large");
	evas_object_size_hint_min_set(progressbar, -1, ELM_SCALE_SIZE(96));
	evas_object_size_hint_align_set(progressbar, EVAS_HINT_FILL, 0.5);
	evas_object_size_hint_weight_set(progressbar, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_progressbar_pulse(progressbar, EINA_TRUE);
	evas_object_show(progressbar);

	elm_box_pack_end(box, progressbar);
	evas_object_size_hint_min_set(box, -1, 110);

	WLEAVE();
	return box;
}


void CalEditLoadingPopup::onDestroy()
{
	WENTER();
}

