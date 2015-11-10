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
//#include "CalDataManager.h"
#include "CalDialogSettingsRadioItem.h"
#include "CalSettingsManager.h"

CalDialogSettingsRadioItem::CalDialogSettingsRadioItem(std::function<void (void)> selectCb,  sub_item_type type, int radioIndex) :
	__radioIndex(radioIndex),
	__type(type),
	__radio(nullptr),
	__selectCb(selectCb),
	__radioCb(nullptr)
{
}

CalDialogSettingsRadioItem::~CalDialogSettingsRadioItem()
{
}

CalDialogSettingsRadioItem::sub_item_type CalDialogSettingsRadioItem::getSubItemType()
{
	return __type;
}

Evas_Object *  CalDialogSettingsRadioItem::getItemRadioObject()
{
	return __radio;
}

Elm_Genlist_Item_Class* CalDialogSettingsRadioItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++
	itc.item_style = "type1";
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogSettingsRadioItem *item = (CalDialogSettingsRadioItem*)data;
		if (!strcmp("elm.text", part))
		{
			if (item->__type == SUB_LOCALE_DEFAULT)
			{
				return strdup(_L_("IDS_CLD_OPT_LOCALE_DEFAULT_ABB"));
			}
			else if(item->__type == SUB_SATURDAY)
			{
				return strdup(_L_("IDS_CLD_OPT_SATURDAY"));
			}
			else if (item->__type == SUB_SUNDAY)
			{
				return strdup(_L_("IDS_CLD_OPT_SUNDAY"));
			}
			else if (item->__type == SUB_MONDAY)
			{
				return strdup(_L_("IDS_CLD_OPT_MONDAY"));
			}
			else if (item->__type == SUB_ALERTS)
			{
				return strdup(_L_("IDS_CLD_OPT_POP_UP_NOTIFICATIONS_ABB"));
			}
			else if (item->__type == SUB_NOTIFICATION)
			{
				return strdup(_L_("IDS_CLD_OPT_STATUS_BAR_NOTIFICATIONS_ABB"));
			}
			else if (item->__type == SUB_OFF)
			{
				return strdup(_L_("IDS_CLD_OPT_NO_ALERTS"));
			}
		}
		return NULL;
	};
	itc.func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		CalDialogSettingsRadioItem *item = (CalDialogSettingsRadioItem*)data;
		if (!strcmp("elm.swallow.end", part))
		{
			Evas_Object *radio_main = nullptr;

			if (item->__type >= CalDialogSettingsRadioItem::SUB_LOCALE_DEFAULT && item->__type <= CalDialogSettingsRadioItem::SUB_MONDAY)
			{
				radio_main = (Evas_Object *)evas_object_data_get(obj, "radio_firstday");
			}
			else
			{
				radio_main = (Evas_Object *)evas_object_data_get(obj, "alert");
			}

			c_retv_if(NULL == radio_main, NULL);

			Evas_Object *radio = elm_radio_add(obj);
			evas_object_propagate_events_set(radio, EINA_FALSE);
			evas_object_size_hint_weight_set(radio, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(radio, EVAS_HINT_FILL, EVAS_HINT_FILL);

			elm_radio_state_value_set(radio, item->__radioIndex);
			elm_radio_group_add(radio, radio_main);

			if (item->__type >= CalDialogSettingsRadioItem::SUB_LOCALE_DEFAULT && item->__type <= CalDialogSettingsRadioItem::SUB_MONDAY)
			{
				int weekType = CalSettingsManager::getInstance().getFirstDayOfWeek();
				if((weekType == CalSettingsManager::SUN && item->__type == SUB_SUNDAY) ||
					(weekType == CalSettingsManager::MON && item->__type == SUB_MONDAY) ||
					(weekType == CalSettingsManager::SAT && item->__type == SUB_SATURDAY) ||
					(weekType == CalSettingsManager::LOCALES && item->__type == SUB_LOCALE_DEFAULT))
				{
					elm_radio_value_set(radio, item->__radioIndex);
				}
			}
			else
			{
				CalSettingsManager::AlertType alertType;
				alertType = CalSettingsManager::getInstance().getAlertType();
				if (alertType == (CalSettingsManager::AlertType)item->__radioIndex)
				{
					elm_radio_value_set(radio, item->__radioIndex);
				}
			}

			evas_object_smart_callback_add(radio, "changed", [](void *data, Evas_Object *obj, void *event_info) {
					CalDialogSettingsRadioItem *item = (CalDialogSettingsRadioItem*)data;
					if (item->__radioIndex == elm_radio_value_get(obj))
					{
						if (item->__type >= CalDialogSettingsRadioItem::SUB_LOCALE_DEFAULT && item->__type <= CalDialogSettingsRadioItem::SUB_MONDAY)
						{
							if (item->__radioIndex == 7)
							{
								CalSettingsManager::getInstance().setFirstDayOfWeek(CalSettingsManager::LOCALES);
							}
							else if (item->__radioIndex == 6)
							{
								CalSettingsManager::getInstance().setFirstDayOfWeek(CalSettingsManager::SAT);
							}
							else if (item->__radioIndex == 0)
							{
								CalSettingsManager::getInstance().setFirstDayOfWeek(CalSettingsManager::SUN);
							}
							else if (item->__radioIndex == 1)
							{
								CalSettingsManager::getInstance().setFirstDayOfWeek(CalSettingsManager::MON);
							}
						}
						else
						{
							CalSettingsManager::getInstance().setAlertType((CalSettingsManager::AlertType)item->__radioIndex);
						}
					}
					if (item->__radioCb)
					{
						item->__radioCb();
					}
				}, item
			);
			item->__radio = radio;
			return radio;
		}
		return NULL;
	};
	return &itc;
}

void CalDialogSettingsRadioItem::onSelect()
{
	if(__selectCb)
	{
		__selectCb();
	}
}

void CalDialogSettingsRadioItem::setRadioCb(std::function<void (void)> radioCb)
{
	__radioCb = radioCb;
}



