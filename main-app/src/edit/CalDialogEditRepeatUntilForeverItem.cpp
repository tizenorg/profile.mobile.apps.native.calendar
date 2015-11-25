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

#include <functional>
#include "CalCommon.h"
#include "CalDataManager.h"
#include "CalScheduleRepeat.h"
#include "CalDialogEditRepeatUntilForeverItem.h"
#include <efl_extension.h>

CalDialogEditRepeatUntilForeverItem::CalDialogEditRepeatUntilForeverItem(std::function<void (CalScheduleRepeat::UntilType untilType, CalScheduleRepeat::UntilInfo untilInfo)> changedCb, const CalScheduleRepeat repeat, int radioIndex) :
	__changedCb(changedCb),
	__repeat(repeat),
	__radioIndex(radioIndex)
{
}

CalDialogEditRepeatUntilForeverItem::~CalDialogEditRepeatUntilForeverItem()
{
}

Elm_Genlist_Item_Class* CalDialogEditRepeatUntilForeverItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {};

	itc.item_style = "type1";
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogEditRepeatUntilForeverItem *item = (CalDialogEditRepeatUntilForeverItem*)data;
		if (0 == strcmp("elm.text", part))
		{
			char temp[FORMAT_BUFFER] = {0};
			snprintf(temp, sizeof(temp) - 1, LABEL_FORMAT, item->getSystemFontSize(), _L_("IDS_CLD_OPT_FOREVER"));

			return strdup(temp);
		}
		return NULL;
	};

	itc.func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		CalDialogEditRepeatUntilForeverItem *item = (CalDialogEditRepeatUntilForeverItem*)data;
		if (0 == strcmp("elm.swallow.end", part))
		{
			Evas_Object *radio_main = (Evas_Object *)evas_object_data_get(obj, "radio_main");
			c_retv_if(NULL == radio_main, NULL);

			Evas_Object *radio = elm_radio_add(obj);
			evas_object_propagate_events_set(radio, EINA_FALSE);

			evas_object_size_hint_weight_set(radio, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(radio, EVAS_HINT_FILL, EVAS_HINT_FILL);
			evas_object_smart_callback_add(radio, "changed",
				[](void *data, Evas_Object *obj, void *event_info)->void
				{
					CalDialogEditRepeatUntilForeverItem *item = (CalDialogEditRepeatUntilForeverItem *)data;
					if (item->__radioIndex == elm_radio_value_get(obj) && item->__changedCb) {
						CalScheduleRepeat::UntilInfo untilInfo;
						item->__changedCb(CalScheduleRepeat::FOREVER, untilInfo);
					}
				}, item
			);
			elm_radio_state_value_set(radio, item->__radioIndex);
			elm_radio_group_add(radio, radio_main);

			if (item->__repeat.untilType == CalScheduleRepeat::FOREVER)
				elm_radio_value_set(radio, item->__radioIndex);

			return radio;
		}
		return NULL;
	};
	return &itc;
}

void CalDialogEditRepeatUntilForeverItem::onSelect()
{
	WENTER();

	Elm_Object_Item* item = Item::getElmObjectItem();
	Evas_Object* genlist = elm_object_item_widget_get(item);
	if(genlist){
		Evas_Object *radio_main = (Evas_Object *)evas_object_data_get(genlist, "radio_main");
		WPRET_M(!radio_main, "radio_main NULL");

		elm_radio_value_set(radio_main, __radioIndex);
		Evas_Object *radio = elm_radio_selected_object_get(radio_main);
		evas_object_smart_callback_call(radio, "changed", NULL);
	}
}


