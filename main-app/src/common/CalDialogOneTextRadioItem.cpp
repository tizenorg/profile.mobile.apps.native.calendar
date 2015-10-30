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
#include "CalDialogOneTextRadioItem.h"

CalDialogOneTextRadioItem::CalDialogOneTextRadioItem(const char* text, int radioIndex, bool isSelected)
{
	if(text)
		__displayString = text;
	else
		__displayString = "Error!";

	__radioIndex = radioIndex;
	__isSelected = isSelected;
}

CalDialogOneTextRadioItem::~CalDialogOneTextRadioItem()
{
}


Elm_Genlist_Item_Class* CalDialogOneTextRadioItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++
	itc.item_style = "1line";
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogOneTextRadioItem *item = (CalDialogOneTextRadioItem*)data;
		if (0 == strcmp("elm.text.main.left", part)) {
				return g_strdup(item->__displayString.c_str());
		}
		return NULL;
	};
	itc.func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		CalDialogOneTextRadioItem *item = (CalDialogOneTextRadioItem*)data;
		if (0 == strcmp("elm.icon.right", part)) {
			Evas_Object *radio_main;

			radio_main = (Evas_Object *)evas_object_data_get(obj, "radio_main");
			c_retv_if(NULL == radio_main, NULL);

			Evas_Object *radio = elm_radio_add(obj);
			evas_object_propagate_events_set(radio, EINA_FALSE);

			evas_object_size_hint_weight_set(radio, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(radio, EVAS_HINT_FILL, EVAS_HINT_FILL);

			elm_radio_state_value_set(radio, item->__radioIndex);
			elm_radio_group_add(radio, radio_main);

			if(item->__isSelected)
				elm_radio_value_set(radio, item->__radioIndex);

			evas_object_smart_callback_add(radio, "changed",
				[](void *data, Evas_Object *obj, void *event_info)
				{
					CalDialogOneTextRadioItem *item = (CalDialogOneTextRadioItem*)data;
					if (item->__radioIndex == elm_radio_value_get(obj)) {
					}
					if (item->__radioCb) {
						item->__radioCb();
					}
				}, item
			);
			return radio;
		}
		return NULL;
	};
	return &itc;
}

void CalDialogOneTextRadioItem::onSelect()
{
	if (__selectCb)
		__selectCb();
}

void CalDialogOneTextRadioItem::setSelectCb(std::function<void (void)> selectCb)
{
	__selectCb = selectCb;
}

void CalDialogOneTextRadioItem::setRadioCb(std::function<void (void)> radioCb)
{
	__radioCb = radioCb;
}



