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

#include "CalDialogSettingsOneTextOneIconItem.h"

#include <Elementary.h>
#include "CalSettingsManager.h"


CalDialogSettingsOneTextOneIconItem::CalDialogSettingsOneTextOneIconItem(std::function<void (void)> selectCb, const char* text)
	: __checkbox(NULL)
	, __selectCb(selectCb)
	, __radioCb(nullptr)
{
	if(text)
	{
		__text = strdup(text);
	}
}

CalDialogSettingsOneTextOneIconItem::CalDialogSettingsOneTextOneIconItem()
	: __text(NULL)
	, __checkbox(NULL)
	, __selectCb(NULL)
	, __radioCb(NULL)
{
}

Evas_Object * CalDialogSettingsOneTextOneIconItem::getIconObject()
{
	return __checkbox;
}


CalDialogSettingsOneTextOneIconItem::~CalDialogSettingsOneTextOneIconItem()
{
	free(__text);
}

Elm_Genlist_Item_Class* CalDialogSettingsOneTextOneIconItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {};
	itc.item_style = "type1";
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogSettingsOneTextOneIconItem* item = (CalDialogSettingsOneTextOneIconItem*) data;
		if (!strcmp(part, "elm.text"))
		{
			return strdup(_L_G_(item->__text));
		}

		return NULL;
	};
	itc.func.content_get = [](void *data, Evas_Object *obj, const char *part)->Evas_Object*
		{
			if (strcmp(part, "elm.swallow.end") == 0)
			{
				CalDialogSettingsOneTextOneIconItem *item = (CalDialogSettingsOneTextOneIconItem *) data;

				Evas_Object *check = elm_check_add(obj);
				elm_object_style_set(check, "on&off");

				item->__checkbox = check;

				evas_object_propagate_events_set(check, EINA_FALSE);
				elm_check_state_set(check, CalSettingsManager::getInstance().getLockTimeZone());
				evas_object_smart_callback_add(check, "changed", [](void *data, Evas_Object *obj, void *event_info) {
						CalSettingsManager::getInstance().setLockTimeZone(!CalSettingsManager::getInstance().getLockTimeZone());
						CalDialogSettingsOneTextOneIconItem *item = (CalDialogSettingsOneTextOneIconItem *)data;
						if (item && item->__radioCb)
						{
							item->__radioCb();
						}
					}
					, item);
				return check;
			}
			return NULL;
		};
	return &itc;
}

void CalDialogSettingsOneTextOneIconItem::onSelect()
{
	WENTER();
	if (__checkbox)
	{
		elm_check_state_set(__checkbox, !elm_check_state_get(__checkbox));
	}

	if (__selectCb)
	{
		__selectCb();
	}
}


void CalDialogSettingsOneTextOneIconItem::setText(const char* text)
{
	if(text)
	{
		free(__text);
		__text = strdup(text);
	}
}

void CalDialogSettingsOneTextOneIconItem::setRadioCb(std::function<void (void)> radioCb)
{
	__radioCb = radioCb;
}


