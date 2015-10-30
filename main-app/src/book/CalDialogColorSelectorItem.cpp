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

#include "CalCommon.h"
#include "CalDialogControl.h"
#include "CalDialogColorSelectorItem.h"
#include "ColorSelectorCommon.h"

CalDialogColorSelectorItem::CalDialogColorSelectorItem(	int r, int g, int b, int a) :
	__changedCb(NULL),
	__r(r),
	__g(g),
	__b(b),
	__a(a)
{
}

CalDialogColorSelectorItem::~CalDialogColorSelectorItem()
{
}

void CalDialogColorSelectorItem::setChangedCb(std::function<void (int& r, int& g, int& b, int& a)> changedCb)
{
	__changedCb = changedCb;
}

Evas_Object* CalDialogColorSelectorItem::__createColorSelector(Evas_Object* parent)
{
	Evas_Object* colorSelector = elm_colorselector_add(parent);
	elm_colorselector_mode_set(colorSelector, ELM_COLORSELECTOR_BOTH);

	evas_object_size_hint_align_set(colorSelector, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_weight_set(colorSelector, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(colorSelector);

	for(int i = 0; i < colors_size; i++)
		elm_colorselector_palette_color_add(colorSelector, R_MASK(colors[i]), G_MASK(colors[i]), B_MASK(colors[i]), A_MASK(colors[i]));

	__setColorSelector(colorSelector);

	return colorSelector;
}

void CalDialogColorSelectorItem::__setColorSelector(Evas_Object* colorSelector)
{
	const Eina_List * color_list = elm_colorselector_palette_items_get(colorSelector);

	const Eina_List *list;
	void* it;
	int r,g,b,a;
	bool is_found = false;

	EINA_LIST_FOREACH(color_list, list, it)
	{
		Elm_Object_Item *item = (Elm_Object_Item *)it;
		elm_colorselector_palette_item_color_get(item, &r, &g, &b, &a);
	  	if ((r == __r) && (g == __g) && (b == __b) && (a == __a))
	   	{
			elm_object_item_signal_emit(item, "elm,state,selected", "elm");
			is_found = true;
			break;
	    }
	}

	if(!is_found){
		Elm_Object_Item *item = (Elm_Object_Item *)eina_list_data_get(eina_list_last(color_list));
		elm_object_item_signal_emit(item, "elm,state,custom,show", "");
		elm_colorselector_palette_item_color_set(item, __r, __g, __b, __a);
		elm_object_item_signal_emit(item, "elm,state,selected", "elm");
	}

	evas_object_smart_callback_add(colorSelector, "color,item,selected",
		[](void* data, Evas_Object* obj, void* eventInfo) {
			CalDialogColorSelectorItem* self = (CalDialogColorSelectorItem*)data;
			int r = 0, g = 0, b = 0 ,a = 0;
			Elm_Object_Item* color_it = (Elm_Object_Item*) eventInfo;
			elm_colorselector_palette_item_color_get(color_it, &r, &g, &b, &a);
			elm_colorselector_color_set( obj, r, g, b, a);
			self->__r = r;
			self->__g = g;
			self->__b = b;
			self->__a = a;
			if(self->__changedCb)
				self->__changedCb(r, g, b, a);
		}, this);
}

Elm_Genlist_Item_Class* CalDialogColorSelectorItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++
	itc.item_style = "1line";

	itc.func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		CalDialogColorSelectorItem* item = (CalDialogColorSelectorItem*)data;
		if (strcmp(part, "elm.icon.1") == 0) {
			Evas_Object* colorSelector = item->__createColorSelector(obj);
			return colorSelector;
		}

		return NULL;
	};

	return &itc;
}
