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
#include "CalDialogControl.h"
#include "CalScheduleRepeat.h"
#include "CalLocaleManager.h"
#include "CalDialogEditRepeatUntilTimesItem.h"

#define DEFAULT_VALUE     10

CalDialogEditRepeatUntilTimesItem::CalDialogEditRepeatUntilTimesItem(std::function<void (CalScheduleRepeat::UntilType untilType, CalScheduleRepeat::UntilInfo untilInfo)> changedCb, const CalScheduleRepeat repeat, int radioIndex)
	: __changedCb(changedCb)
	, __dateButtonClickedCb(NULL)
	, __repeat(repeat)
	, __radioIndex(radioIndex)
	, __untilInfo(repeat.untilInfo)
	, __radio(NULL)
	, __button(NULL)
{
	if (repeat.untilType == CalScheduleRepeat::TIMES)
	{
		__untilInfo.times = repeat.untilInfo.times;
	}
	else
	{
		__untilInfo.times = DEFAULT_VALUE;
	}
}

CalDialogEditRepeatUntilTimesItem::~CalDialogEditRepeatUntilTimesItem()
{
}

void CalDialogEditRepeatUntilTimesItem::setDateButtonClickedCb(std::function<void (Evas_Object* dateTime)> dateButtonClickedCb)
{
	__dateButtonClickedCb = dateButtonClickedCb;
}

void CalDialogEditRepeatUntilTimesItem::setDateButtonTime(const CalScheduleRepeat repeat)
{
	__untilInfo.times = repeat.untilInfo.times;

	int times = DEFAULT_VALUE;
	if (__untilInfo.times)
	{
		times = __untilInfo.times;
	}

	char temp[FORMAT_BUFFER] = {0};
	snprintf(temp, sizeof(temp) - 1, COUNT_FORMAT, getSystemFontSize(), times);
	elm_object_text_set(__button, temp);
}

Elm_Genlist_Item_Class* CalDialogEditRepeatUntilTimesItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {};

	if (CalLocaleManager::getInstance().isRTL())
	{
		itc.item_style = "CalDialogEditRepeatUntilTimesItemRTL";
	}
	else
	{
		itc.item_style = "CalDialogEditRepeatUntilTimesItem";
	}

	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogEditRepeatUntilTimesItem *item = (CalDialogEditRepeatUntilTimesItem*) data;
		if (0 == strcmp("elm.text", part))
		{
			char temp[FORMAT_BUFFER] = {0};
			snprintf(temp, sizeof(temp) - 1, LABEL_FORMAT, item->getSystemFontSize(), _L_("IDS_CLD_OPT_TIMES_LC"));

			return strdup(temp);
		}

		return NULL;
	};

	itc.func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		CalDialogEditRepeatUntilTimesItem *item = (CalDialogEditRepeatUntilTimesItem*) data;
		if (0 == strcmp("elm.swallow.times", part))
		{
			Evas_Object *button = elm_button_add(obj);
			evas_object_propagate_events_set(button, EINA_FALSE);
			evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
			evas_object_show(button);
			evas_object_smart_callback_add(button, "clicked",
				[](void* data, Evas_Object* obj, void* event_info) {
					CalDialogEditRepeatUntilTimesItem *item = (CalDialogEditRepeatUntilTimesItem*)data;
					elm_radio_value_set(item->__radio, item->__radioIndex);
					Evas_Object* radio = elm_radio_selected_object_get(item->__radio);
					evas_object_smart_callback_call(radio, "changed", NULL);
					if(item->__dateButtonClickedCb)
					{
						item->__dateButtonClickedCb(obj);
					}
				}, item
			);

			int times = DEFAULT_VALUE;
			if (item->__untilInfo.times)
			{
				times = item->__untilInfo.times;
			}

			char temp[FORMAT_BUFFER] = {0};
			snprintf(temp, sizeof(temp) - 1, COUNT_FORMAT, item->getSystemFontSize(), times);
			elm_object_text_set(button, temp);

			item->__button = button;

			return button;
		}
		else if (0 == strcmp("elm.icon.right", part))
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
					CalDialogEditRepeatUntilTimesItem *item = (CalDialogEditRepeatUntilTimesItem *)data;
					if (item->__radioIndex == elm_radio_value_get(obj) && item->__changedCb)
					{
						item->__changedCb(CalScheduleRepeat::TIMES, item->__untilInfo);
					}
				}, item
			);
			elm_radio_state_value_set(radio, item->__radioIndex);
			elm_radio_group_add(radio, radio_main);

			if (item->__repeat.untilType == CalScheduleRepeat::TIMES)
			{
				elm_radio_value_set(radio, item->__radioIndex);
			}

			item->__radio = radio;

			return radio;
		}
		return NULL;
	};
	return &itc;
}

void CalDialogEditRepeatUntilTimesItem::onSelect()
{
	WENTER();

	Elm_Object_Item* item = Item::getElmObjectItem();
	Evas_Object* genlist = elm_object_item_widget_get(item);
	if(genlist)
	{
		Evas_Object *radio_main = (Evas_Object *)evas_object_data_get(genlist, "radio_main");
		WPRET_M(!radio_main, "radio_main NULL");

		elm_radio_value_set(radio_main, __radioIndex);
		Evas_Object *radio = elm_radio_selected_object_get(radio_main);
		evas_object_smart_callback_call(radio, "changed", NULL);
	}
}
