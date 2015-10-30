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

#include "CalAlertItem.h"
#include "CalCommon.h"
#include "CalSchedule.h"
#include "CalBook.h"
#include "CalBookManager.h"


CalAlertItem::CalAlertItem(const std::shared_ptr<CalSchedule>& schedule) :
	__schedule(schedule),
	__check(NULL),
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

		if(self && self->__schedule.get())
		{
			const char *summary = self->__schedule->getSummary();
			if (!strcmp(part, "elm.text.main.top"))
			{
				if (summary && *summary)
				{
					return strdup(summary);
				}
				else
				{
					return strdup(_L_("IDS_CLD_MBODY_MY_EVENT"));
				}
			}
			else if (!strcmp(part, "elm.text.time.up"))
			{
				CalDateTime startTime;
				self->__schedule->getStart(startTime);
				std::string startText;
				startTime.getTimeString(startText);

				return strdup(startText.c_str());
			}
			else if (!strcmp(part, "elm.text.time.down"))
			{
				CalDateTime endTime;
				self->__schedule->getEnd(endTime);
				std::string endText;
				endTime.getTimeString(endText);

				return strdup(endText.c_str());
			}
			else if (!strcmp(part, "elm.text.location"))
			{
				const char *location = self->__schedule->getLocation();
				if (location && *location)
				{
					return strdup(location);
				}
				else
				{
					return strdup("");
				}
			}
		}
		return NULL;
	};
	itc.func.content_get = [](void *data, Evas_Object *obj, const char *part)->Evas_Object*
	{
		if (!strcmp(part, "elm.swallow.checkbox"))
		{
			CalAlertItem* self = (CalAlertItem*)data;
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
			CalAlertItem *item = (CalAlertItem *)data;

			Evas_Object* icon = evas_object_rectangle_add(evas_object_evas_get(obj));
			evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);

			int r,g,b,a;
			std::shared_ptr<CalBook> book = CalBookManager::getInstance().getBook(item->__schedule->getBookId());
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
		onSelect();
	}
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

Evas_Object * CalAlertItem::getCheckObject()
{
	return __check;
}

