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

#include "CalNotificationsSelectAllItem.h"
#include "CalCommon.h"

CalNotificationsSelectAllItem::CalNotificationsSelectAllItem(bool isAllVisible, std::function<void (void)> selectCb) :
	__isAllVisible(isAllVisible),
	__checkbox(nullptr),
	__selectCb(selectCb)
{
}

CalNotificationsSelectAllItem::~CalNotificationsSelectAllItem()
{
}

void CalNotificationsSelectAllItem::setSelectAllVisibleState(bool isAllVisible)
{
	__isAllVisible = isAllVisible;
}

Elm_Genlist_Item_Class* CalNotificationsSelectAllItem::getItemClassStatic()
{
	WENTER();
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++
	itc.item_style = "type1";
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		if (!strcmp(part, "elm.text"))
		{
			return strdup(_L_("IDS_CLD_OPT_SELECT_ALL"));
		}

		return NULL;
	};
	itc.func.content_get = [](void *data, Evas_Object *obj, const char *part)->Evas_Object*
	{
		if (!strcmp(part, "elm.swallow.end"))
		{
			CalNotificationsSelectAllItem *self = (CalNotificationsSelectAllItem *)data;

			Evas_Object *check = elm_check_add(obj);
			self->__checkbox = check;

			if (self->__isAllVisible)
			{
				elm_check_state_set(check, EINA_TRUE);
			}
			else
			{
				elm_check_state_set(check, EINA_FALSE);
			}
			evas_object_propagate_events_set(check, EINA_FALSE);
			evas_object_smart_callback_add(check, "changed", [](void *data, Evas_Object *obj, void *event_info) {
					CalNotificationsSelectAllItem *item = (CalNotificationsSelectAllItem *)data;
					item->__selectCb();
				}
				, self);

			return check;
		}
		return NULL;
	};
	return &itc;
}

void CalNotificationsSelectAllItem::onSelect()
{
	WENTER();
	if (__selectCb)
	{
		__selectCb();
	}
}

