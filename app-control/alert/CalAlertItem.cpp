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

#include "CalAlertItem.h"
#include "CalCommon.h"
#include "CalSchedule.h"
#include "CalBook.h"
#include "CalBookManager.h"
#include "CalAlertNotificationItem.h"

#define PART_CHECK "elm.swallow.checkbox"

CalAlertItem::CalAlertItem(const std::shared_ptr<CalAlertNotificationItem> &alertItem) :
	__alertItem(alertItem),
	__check(NULL),
	__isCheckVisible(true),
	__selectCb(nullptr),
	__checkCb(nullptr)
{
}

CalAlertItem::~CalAlertItem()
{
}

Elm_Genlist_Item_Class* CalAlertItem::getItemClassStatic()
{
	WENTER();
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++
	itc.item_style = "4text.1colorbar.1check";
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalAlertItem* self = (CalAlertItem*)data;

		if(self && self->__alertItem.get())
		{
			if (!strcmp(part, "elm.text.main.top"))
			{
				return self->__alertItem->getEventName();
			}
			else if (!strcmp(part, "elm.text.time.up"))
			{
				return self->__alertItem->getStartTime();
			}
			else if (!strcmp(part, "elm.text.time.down"))
			{
				return self->__alertItem->getEndTime();
			}
			else if (!strcmp(part, "elm.text.location"))
			{
				return self->__alertItem->getLocation();
			}
		}
		return NULL;
	};
	itc.func.content_get = [](void *data, Evas_Object *obj, const char *part)->Evas_Object*
	{
		CalAlertItem* self = (CalAlertItem*)data;
		if (!strcmp(part, PART_CHECK) && self->__isCheckVisible)
		{
			Evas_Object *check = elm_check_add(obj);
			elm_check_state_set(check, EINA_FALSE);
			evas_object_propagate_events_set(check, EINA_FALSE);
			evas_object_smart_callback_add(check, "changed", [](void *data, Evas_Object *obj, void *event_info) {
					CalAlertItem *item = (CalAlertItem *)data;
					item->onCheck();
				}
				, self);
			self->__check = check;
			return self->__check;
		}
		else if (!strcmp(part, "elm.swallow.color.bar"))
		{
			Evas_Object* icon = evas_object_rectangle_add(evas_object_evas_get(obj));
			evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);

			int r,g,b,a;
			std::shared_ptr<CalBook> book = CalBookManager::getInstance().getBook(self->__alertItem->getSchedule()->getBookId());
			book->getColor(r,g,b,a);
			evas_object_color_set(icon, r, g, b, a);

			return icon;
		}

		return NULL;
	};
	return &itc;
}

void CalAlertItem::onCheck()
{
	if (__check)
	{
		elm_check_state_set(__check, !elm_check_state_get(__check));
		if (__checkCb)
		{
			__checkCb();
		}
	}
}

void CalAlertItem::setCheckCb(std::function<void (void)> checkCb)
{
	__checkCb = checkCb;
}

void CalAlertItem::onSelect()
{
	if (__selectCb)
	{
		__selectCb();
	}
}

void CalAlertItem::setSelectCb(std::function<void (void)> selectCb)
{
	__selectCb = selectCb;
}

void CalAlertItem::setCheckVisibility(bool isVisible)
{
	if (__isCheckVisible != isVisible)
	{
		__isCheckVisible = isVisible;
		elm_genlist_item_fields_update (getElmObjectItem(), PART_CHECK, ELM_GENLIST_ITEM_FIELD_CONTENT);
	}
}

Evas_Object * CalAlertItem::getCheckObject()
{
	return __check;
}

bool CalAlertItem::isSnoozedItem()
{
	return __alertItem->isSnoozed();
}


