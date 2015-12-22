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

#include "CalView.h"
#include "CalCommon.h"
#include "CalNaviframe.h"

CalView::CalView(const char* name) : WView(name)
{
}

CalView::~CalView()
{
	((CalNaviframe*)getNaviframe())->destroyView(this, getNaviItem());
}

void CalView::activateMenuButton(Elm_Object_Item* naviItem)
{
	Evas_Object* menuBtn = __createMenuBtn(getNaviframe()->getEvasObj(), __menuBtnSelectCb, this);
	elm_object_item_part_content_set(naviItem, "toolbar_more_btn", menuBtn);
}

void CalView::onMenuButton()
{
}

void CalView::onEvent(const CalEvent& event)
{
}

Evas_Object* CalView::__createMenuBtn(Evas_Object* parent, Evas_Smart_Cb func, void* data)
{
	Evas_Object* btn = elm_button_add(parent);
	if (!btn)
	{
		return NULL;
	}

	// TODO: return back when default style is working again
	//elm_object_style_set(btn, "naviframe/more/default");
	evas_object_show(btn);
	evas_object_smart_callback_add(btn, "clicked", func, data);

	return btn;
}

void CalView::__menuBtnSelectCb(void* data, Evas_Object* obj, void* event_info)
{
	CalView* p = (CalView*)data;
	p->onMenuButton();
}
