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
#include "CalLocaleManager.h"
#include "CalScheduleRepeat.h"
#include "CalDialogEditRepeatUnitItem.h"

CalDialogEditRepeatUnitItem::CalDialogEditRepeatUnitItem(std::function<void (CalScheduleRepeat::UnitType unitType, int unnitInterval)> changedCb, const CalScheduleRepeat repeat, CalScheduleRepeat::UnitType unitType)
	: __changedCb(changedCb)
	, __repeat(repeat)
	, __unitType(unitType)
	, __button(NULL)
{
	if (repeat.unitType == unitType)
	{
		__unitInterval = repeat.unitInterval;
	}
	else
	{
		__unitInterval = 0;
	}
}

CalDialogEditRepeatUnitItem::~CalDialogEditRepeatUnitItem()
{
}

void CalDialogEditRepeatUnitItem::setDateButtonClickedCb(std::function<void (void)> dateButtonClickedCb)
{
	__dateButtonClickedCb = dateButtonClickedCb;
}

void CalDialogEditRepeatUnitItem::updateUnitsLabel()
{
	elm_genlist_item_fields_update(getElmObjectItem(), "elm.text.right", ELM_GENLIST_ITEM_FIELD_TEXT);
}

void CalDialogEditRepeatUnitItem::setDateButtonTime(const CalScheduleRepeat repeat)
{
	__unitInterval = repeat.unitInterval;

	int times = 1;
	if (repeat.unitInterval)
	{
		times = repeat.unitInterval;
	}

	char temp[FORMAT_BUFFER] = {0};
	snprintf(temp, sizeof(temp) - 1, COUNT_FORMAT, getSystemFontSize(), times);
	elm_object_text_set(__button, temp);
}

Elm_Genlist_Item_Class* CalDialogEditRepeatUnitItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {};

	if (CalLocaleManager::getInstance().isRTL())
	{
		itc.item_style = "CalDialogEditRepeatUnitItemRTL";
	}
	else
	{
		itc.item_style = "CalDialogEditRepeatUnitItem";
	}

	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogEditRepeatUnitItem *item = (CalDialogEditRepeatUnitItem*)data;
		if (0 == strcmp("elm.text.left", part))
		{
			char temp[FORMAT_BUFFER] = {0};
			snprintf(temp, sizeof(temp) - 1, LABEL_FORMAT, item->getSystemFontSize(), _L_("IDS_CLD_OPT_EVERY"));

			return strdup(temp);
		}
		else if (0 == strcmp("elm.text.right", part))
		{
			char temp[FORMAT_BUFFER] = {0};
			if (item->__unitInterval > 1)
			{
				switch (item->__unitType)
				{
					case CalScheduleRepeat::DAY:
						snprintf(temp, sizeof(temp) - 1, LABEL_FORMAT, item->getSystemFontSize(), _L_("IDS_CLD_BODY_DAYS_M_DURATION_LC"));
						return strdup(temp);
					case CalScheduleRepeat::WEEK:
						snprintf(temp, sizeof(temp) - 1, LABEL_FORMAT, item->getSystemFontSize(), _L_("IDS_CLD_BODY_WEEKS_M_DURATION_LC"));
						return strdup(temp);
					case CalScheduleRepeat::MONTH:
						snprintf(temp, sizeof(temp) - 1, LABEL_FORMAT, item->getSystemFontSize(), _L_("IDS_CLD_OPT_MONTHS_LC_ABB"));
						return strdup(temp);
					case CalScheduleRepeat::YEAR:
						snprintf(temp, sizeof(temp) - 1, LABEL_FORMAT, item->getSystemFontSize(), _L_("IDS_CLD_OPT_YEARS_LC_ABB"));
						return strdup(temp);
					default:
						WERROR("Invalid unitType(%d)", item->__unitType);
						return NULL;
				}
			}
			else
			{
				switch (item->__unitType)
				{
					case CalScheduleRepeat::DAY:
						snprintf(temp, sizeof(temp) - 1, LABEL_FORMAT, item->getSystemFontSize(), _L_("IDS_CLD_BODY_DAY_M_DURATION_LC"));
						return strdup(temp);
					case CalScheduleRepeat::WEEK:
						snprintf(temp, sizeof(temp) - 1, LABEL_FORMAT, item->getSystemFontSize(), _L_("IDS_CLD_BODY_WEEK_LC_ABB"));
						return strdup(temp);
					case CalScheduleRepeat::MONTH:
						snprintf(temp, sizeof(temp) - 1, LABEL_FORMAT, item->getSystemFontSize(), _L_("IDS_CLD_BODY_MONTH_LC_ABB"));
						return strdup(temp);
					case CalScheduleRepeat::YEAR:
						snprintf(temp, sizeof(temp) - 1, LABEL_FORMAT, item->getSystemFontSize(), _L_("IDS_CLD_BODY_YEAR_LC"));
						return strdup(temp);
					default:
						WERROR("Invalid unitType(%d)", item->__unitType);
						return NULL;
				}
			}
		}
		return NULL;
	};

	itc.func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		CalDialogEditRepeatUnitItem *item = (CalDialogEditRepeatUnitItem*)data;
		if (0 == strcmp("elm.swallow.unit", part))
		{
			Evas_Object *button = elm_button_add(obj);
			evas_object_propagate_events_set(button, EINA_FALSE);
			evas_object_size_hint_min_set(button, ELM_SCALE_SIZE(60), ELM_SCALE_SIZE(60));
			evas_object_smart_callback_add(button, "clicked",
				[](void* data, Evas_Object* obj, void* event_info)
				{
					CalDialogEditRepeatUnitItem *item = (CalDialogEditRepeatUnitItem*)data;
					if(item->__dateButtonClickedCb)
					{
						item->__dateButtonClickedCb();
					}
				}, item
			);

			int times = 1;
			if (item->__unitInterval)
			{
				times = item->__unitInterval;
			}

			char temp[FORMAT_BUFFER] = {0};
			snprintf(temp, sizeof(temp) - 1, COUNT_FORMAT, item->getSystemFontSize(), times);
			elm_object_text_set(button, temp);

			item->__button = button;

			return button;
		}
		return NULL;
	};
	return &itc;
}
