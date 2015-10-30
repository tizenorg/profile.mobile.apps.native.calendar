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
#include "CalDataManager.h"
#include "CalDialogEditReminderRadioItem.h"

CalDialogEditReminderRadioItem::CalDialogEditReminderRadioItem(const CalScheduleReminder& reminderOri, int radioIndex, bool setRadio, bool isOffItem) :
	__reminderOri(reminderOri),
	__radioIndex(radioIndex),
	__reminder()
{
	__reminder.isCustom = true;
	if (isOffItem) {
		__reminder.isCustom = false;
	}
	__reminder.isOffItem = isOffItem;
	__setRadio = setRadio;
}

CalDialogEditReminderRadioItem::CalDialogEditReminderRadioItem(const CalScheduleReminder& reminderOri, int radioIndex, CalScheduleReminder::UnitType unitType, int unitValue, bool setRadio, bool isOffItem) :
	__reminderOri(reminderOri),
	__radioIndex(radioIndex),
	__reminder()
{
	__reminder.unitType = unitType;
	__reminder.unitValue = unitValue;
	__reminder.isCustom = false;
	__reminder.isOffItem = isOffItem;
	__setRadio = setRadio;
}

CalDialogEditReminderRadioItem::~CalDialogEditReminderRadioItem()
{
}

void CalDialogEditReminderRadioItem::setSelectCb(std::function<void (void)> selectCb)
{
	__selectCb = selectCb;
}
void CalDialogEditReminderRadioItem::setRadioChangeCb(std::function<void (const CalScheduleReminder& reminder)> changeCb)
{
	__changeCb = changeCb;
}

Elm_Genlist_Item_Class* CalDialogEditReminderRadioItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++
	itc.item_style = "type1";
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogEditReminderRadioItem *item = (CalDialogEditReminderRadioItem*)data;
		if (0 == strcmp("elm.text", part)) {
			if (item->__reminder.isOffItem){
					return g_strdup(_L_("IDS_CLD_OPT_NONE"));
			} else if (item->__reminder.isCustom){
					return g_strdup(_L_("IDS_CLD_OPT_CUSTOM"));
			} else {
				std::string reminderText;
				item->__reminder.getString(reminderText);
				return g_strdup(reminderText.c_str());
			}
		}
		return NULL;
	};
	itc.func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		CalDialogEditReminderRadioItem *item = (CalDialogEditReminderRadioItem*)data;
		if (0 == strcmp("elm.swallow.end", part)) {
			Evas_Object *radio_main = (Evas_Object *)evas_object_data_get(obj, "radio_main");
			c_retv_if(NULL == radio_main, NULL);

			Evas_Object *radio = elm_radio_add(obj);
			evas_object_propagate_events_set(radio, EINA_FALSE);

			evas_object_size_hint_weight_set(radio, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(radio, EVAS_HINT_FILL, EVAS_HINT_FILL);

			elm_radio_state_value_set(radio, item->__radioIndex);
			elm_radio_group_add(radio, radio_main);

			if (item->__setRadio)
				elm_radio_value_set(radio, item->__radioIndex);

			evas_object_smart_callback_add(radio, "changed",
				[](void *data, Evas_Object *obj, void *event_info)
				{
					CalDialogEditReminderRadioItem *item = (CalDialogEditReminderRadioItem*)data;
					if (item->__radioIndex == elm_radio_value_get(obj) && item->__changeCb) {
						item->__changeCb(item->__reminder);
					}
				}, item
			);

			return radio;
		}
		return NULL;
	};
	return &itc;
}

void CalDialogEditReminderRadioItem::onSelect()
{
	WENTER();
	if (__selectCb)
		__selectCb();
}
