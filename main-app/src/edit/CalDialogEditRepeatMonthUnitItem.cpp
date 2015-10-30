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

#include <sstream>
#include "CalCommon.h"
#include "CalDataManager.h"
#include "CalDialogControl.h"
#include "CalScheduleRepeat.h"
#include "CalDialogEditRepeatMonthUnitItem.h"

CalDialogEditRepeatMonthUnitItem::CalDialogEditRepeatMonthUnitItem(std::function<void (CalScheduleRepeat::MonthlyType monthlyType)> changedCb, const CalScheduleRepeat repeat, const CalDateTime start, const std::string& timezone, int radioIndex, CalScheduleRepeat::MonthlyType monthlyType) :
	__changedCb(changedCb),
	__repeat(repeat),
	__radioIndex(radioIndex),
	__monthlyType(monthlyType),
	__startTime(start),
	__timezone(timezone)
{

}

CalDialogEditRepeatMonthUnitItem::~CalDialogEditRepeatMonthUnitItem()
{
}

Elm_Genlist_Item_Class* CalDialogEditRepeatMonthUnitItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++
	itc.item_style = "type1";
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogEditRepeatMonthUnitItem *item = (CalDialogEditRepeatMonthUnitItem*) data;
		if (0 == strcmp("elm.text", part))
		{
			std::string text;
			CalScheduleRepeat::Date date;
			const char* tz = item->__timezone.c_str();
			date.year = item->__startTime.getYear(tz);
			date.month = item->__startTime.getMonth(tz);
			date.mday = item->__startTime.getMday(tz);
			item->__repeat.getMonthlyUnitInfoString(date, tz, item->__monthlyType, text);

			std::stringstream ss;
			ss << "<p font_size=" << item->getSystemFontSize() << ">" << text.c_str() << "</p>";

			return strdup(ss.str().c_str());
		}
		return NULL;
	};
	itc.func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		CalDialogEditRepeatMonthUnitItem *item = (CalDialogEditRepeatMonthUnitItem*)data;
		if (0 == strcmp("elm.swallow.end", part)) {
			Evas_Object *radio_main = (Evas_Object *)evas_object_data_get(obj, "unit_item_radio_mian");
			c_retv_if(NULL == radio_main, NULL);

			Evas_Object *radio = elm_radio_add(obj);
			evas_object_propagate_events_set(radio, EINA_FALSE);

			evas_object_size_hint_weight_set(radio, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(radio, EVAS_HINT_FILL, EVAS_HINT_FILL);

			elm_radio_state_value_set(radio, item->__radioIndex);
			elm_radio_group_add(radio, radio_main);

			if (item->__repeat.unitInfo.monthly.type == item->__monthlyType)
				elm_radio_value_set(radio, item->__radioIndex);

			evas_object_smart_callback_add(radio, "changed",
				[](void *data, Evas_Object *obj, void *event_info)->void
				{
					CalDialogEditRepeatMonthUnitItem *item = (CalDialogEditRepeatMonthUnitItem*)data;
					int check_value = elm_radio_state_value_get(obj);
					if (check_value == item->__radioIndex) {
						// TODO: set date
						item->__changedCb(item->__monthlyType);
					}

				}, item
			);
			return radio;
		}
		return NULL;
	};
	return &itc;
}

void CalDialogEditRepeatMonthUnitItem::onSelect()
{
	WENTER();
	Elm_Object_Item* item = Item::getElmObjectItem();
	Evas_Object* genlist = elm_object_item_widget_get(item);
	if(genlist){
		Evas_Object *radio_main = (Evas_Object *)evas_object_data_get(genlist, "unit_item_radio_mian");
		WPRET_M(!radio_main, "radio_main NULL");

		elm_radio_value_set(radio_main, __radioIndex);
		Evas_Object *radio = elm_radio_selected_object_get(radio_main);
		evas_object_smart_callback_call(radio, "changed", NULL);
	}
}


