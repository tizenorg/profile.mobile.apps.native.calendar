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
#include "CalDialogEditRepeatUntilDueDateItem.h"
#include "CalSettingsManager.h"
#include "CalDateTime.h"
#include "CalLocaleManager.h"

CalDialogEditRepeatUntilDueDateItem::CalDialogEditRepeatUntilDueDateItem(std::function<void (CalScheduleRepeat::UntilType untilType, CalScheduleRepeat::UntilInfo untilInfo)> changedCb, const CalScheduleRepeat repeat, int radioIndex,  const CalDateTime& dateTime)
	: __dateButtonClickedCb(NULL)
	, __changedCb(changedCb)
	, __repeat(repeat)
	, __radioIndex(radioIndex)
	, __dateTime(dateTime)
	, __radio(NULL)
	, __button(NULL)
{
	if (repeat.untilType == CalScheduleRepeat::DUE_DATE)
	{
		__untilInfo.date = repeat.untilInfo.date;
		__dateTime.set(__untilInfo.date.year, __untilInfo.date.month, __untilInfo.date.mday);
	}
	else
	{
		if (repeat.unitType == CalScheduleRepeat::DAY)
		{
			__dateTime.addDays(7);
		}
		else if (repeat.unitType == CalScheduleRepeat::WEEK)
		{
			__dateTime.addMonths(1);
		}
		else if (repeat.unitType == CalScheduleRepeat::MONTH)
		{
			__dateTime.addYears(1);
		}
		else if (repeat.unitType == CalScheduleRepeat::YEAR)
		{
			__dateTime.addYears(5);
		}

		__untilInfo.date.year = __dateTime.getYear();
		__untilInfo.date.month = __dateTime.getMonth();
		__untilInfo.date.mday = __dateTime.getMday();
	}

}

CalDialogEditRepeatUntilDueDateItem::~CalDialogEditRepeatUntilDueDateItem()
{
}

CalDateTime& CalDialogEditRepeatUntilDueDateItem::getUntilDate()
{
	return __dateTime;
}

void CalDialogEditRepeatUntilDueDateItem::__onDateButtonClicked(void *data, Evas_Object *obj, void *event_info)
{
	CalDialogEditRepeatUntilDueDateItem* item = static_cast<CalDialogEditRepeatUntilDueDateItem*>(data);
	if(item)
	{
		item->__dateButtonClickedCb(obj);

		elm_radio_value_set(item->__radio, item->__radioIndex);
		Evas_Object* radio = elm_radio_selected_object_get(item->__radio);
		evas_object_smart_callback_call(radio, "changed", NULL);
		item->__changedCb(CalScheduleRepeat::DUE_DATE, item->__untilInfo);
	}
}

void CalDialogEditRepeatUntilDueDateItem::setDateButtonClickedCb(std::function<void (Evas_Object* dateTime)> dateButtonClickedCb)
{
	__dateButtonClickedCb = dateButtonClickedCb;
}

void CalDialogEditRepeatUntilDueDateItem::setDateButtonTime(const CalScheduleRepeat repeat)
{
	__untilInfo = repeat.untilInfo;
	__dateTime.set(repeat.untilInfo.date.year, repeat.untilInfo.date.month, repeat.untilInfo.date.mday);

	std::string dateString;
	__dateTime.getDateString(dateString);
	if(!dateString.empty())
	{
		int fontSize = getSystemFontSize();
		if(fontSize > TEXT_ITEM_DEFAULT_SIZE)
		{
			fontSize = TEXT_ITEM_DEFAULT_SIZE;
		}

		char temp[FORMAT_BUFFER] = {0};
		snprintf(temp, sizeof(temp) - 1, LABEL_FORMAT, fontSize, dateString.c_str());
		elm_object_text_set(__button, temp);
	}
}

Elm_Genlist_Item_Class* CalDialogEditRepeatUntilDueDateItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++

	if (CalLocaleManager::getInstance().isRTL())
	{
		itc.item_style = "CalDialogEditRepeatUntilDueDateItemRTL";
	}
	else
	{
		itc.item_style = "CalDialogEditRepeatUntilDueDateItem";
	}

	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogEditRepeatUntilDueDateItem *item = (CalDialogEditRepeatUntilDueDateItem*)data;
		if (!strcmp("elm.text", part))
		{
			char temp[FORMAT_BUFFER] = {0};
			snprintf(temp, sizeof(temp) - 1, LABEL_FORMAT, item->getSystemFontSize(), "~");

			return strdup(temp);
		}

		return NULL;
	};
	itc.func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		CalDialogEditRepeatUntilDueDateItem *item = (CalDialogEditRepeatUntilDueDateItem*)data;
		if (!strcmp("elm.swallow.date", part))
		{
			Evas_Object *button = elm_button_add(obj);
			evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
			evas_object_smart_callback_add(button, "clicked", item->__onDateButtonClicked, item);
			evas_object_propagate_events_set(button, EINA_FALSE);
			evas_object_show(button);

			std::string dateString;
			item->__dateTime.getDateString(dateString);
			if(!dateString.empty())
			{
				int fontSize = item->getSystemFontSize();
				if(fontSize > TEXT_ITEM_DEFAULT_SIZE)
				{
					fontSize = TEXT_ITEM_DEFAULT_SIZE;
				}

				char temp[FORMAT_BUFFER] = {0};
				snprintf(temp, sizeof(temp) - 1, LABEL_FORMAT, fontSize, dateString.c_str());
				elm_object_text_set(button, temp);
			}

			item->__button = button;

			return button;
		}
		else if (!strcmp("elm.icon.right", part))
		{
			Evas_Object *radio_main = (Evas_Object *)evas_object_data_get(obj, "radio_main");
			c_retv_if(NULL == radio_main, NULL);

			Evas_Object *radio = elm_radio_add(obj);
			evas_object_propagate_events_set(radio, EINA_FALSE);

			evas_object_size_hint_weight_set(radio, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(radio, EVAS_HINT_FILL, EVAS_HINT_FILL);
			evas_object_smart_callback_add(radio, "changed", [](void *data, Evas_Object *obj, void *event_info)->void {
					CalDialogEditRepeatUntilDueDateItem *item = (CalDialogEditRepeatUntilDueDateItem *)data;
					if (item->__radioIndex == elm_radio_value_get(obj) && item->__changedCb)
					{
						item->__changedCb(CalScheduleRepeat::DUE_DATE, item->__untilInfo);
					}
				}, item
			);
			elm_radio_state_value_set(radio, item->__radioIndex);
			elm_radio_group_add(radio, radio_main);

			if (item->__repeat.untilType == CalScheduleRepeat::DUE_DATE)
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


