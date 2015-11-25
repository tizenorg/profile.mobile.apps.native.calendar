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

#include "CalDialogBookLocalItem.h"

#include <calendar.h>
#include "CalCommon.h"

CalDialogBookLocalItem::CalDialogBookLocalItem(CalBookLocalMode item_mode, const std::shared_ptr<CalBook>& book) :
	__check(nullptr),
	__book(book),
	__selectCb(nullptr),
	__deleteSelectCb(nullptr),
	__checkboxCb(nullptr),
	__item_mode(item_mode)
{
}

CalDialogBookLocalItem::~CalDialogBookLocalItem()
{
}

std::shared_ptr<CalBook>CalDialogBookLocalItem:: getLocalBook() const
{
	return __book;
}

Elm_Genlist_Item_Class* CalDialogBookLocalItem::getItemClassStatic()
{
	WENTER();
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++
	if(__item_mode == LocalBookDisplay)
	{
		itc.item_style = "1text.1colorbar";
	}
	else
	{
		itc.item_style = "1text.1colorbar.1check";
	}

	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		if(strcmp(part, "elm.text") == 0)
		{
			CalDialogBookLocalItem* self = (CalDialogBookLocalItem*)data;

			if(self->__book->getIndex() == DEFAULT_BIRTHDAY_CALENDAR_BOOK_ID)
			{
				return strdup(_L_("IDS_CLD_BODY_CONTACTS_BIRTHDAYS_ABB"));
			}
			else if(self->__book->getIndex() == DEFAULT_EVENT_CALENDAR_BOOK_ID)
			{
				return strdup(_L_("IDS_CLD_OPT_MY_CALENDAR_ABB"));
			}
			else
			{
				return elm_entry_utf8_to_markup(self->__book->getName());
			}
		}
		return NULL;
	};

	itc.func.content_get = [](void *data, Evas_Object *obj, const char *part)->Evas_Object*
	{
		if(!strcmp(part, "elm.swallow.checkbox"))
		{
			CalDialogBookLocalItem *item = (CalDialogBookLocalItem *)data;

			if(item->__item_mode == LocalBookDisplay)
			{
				return NULL;
			}
			else if(item->__item_mode == LocalBookChange)
			{
				Evas_Object *check = elm_check_add(obj);
				if(item->__book->getVisibility())
				{
					elm_check_state_set(check, EINA_TRUE);
				}
				else
				{
					elm_check_state_set(check, EINA_FALSE);
				}
				evas_object_propagate_events_set(check, EINA_FALSE);
				evas_object_smart_callback_add(check, "changed", [](void *data, Evas_Object *obj, void *event_info) {
						CalDialogBookLocalItem *item = (CalDialogBookLocalItem *)data;
						item->onSelect();
					}, item);
				item->__check = check;
				return check;
			}
			else if (item->__item_mode == LocalBookDelete)
			{
				Evas_Object *check = elm_check_add(obj);
				elm_check_state_set(check, EINA_FALSE);
				evas_object_propagate_events_set(check, EINA_FALSE);
				evas_object_smart_callback_add(check, "changed", [](void *data, Evas_Object *obj, void *event_info) {
						CalDialogBookLocalItem *item = (CalDialogBookLocalItem *)data;
						item->onCheckboxCb();
					}, item);
				item->__check = check;
				return check;
			}
		}
		else if(!strcmp(part, "elm.swallow.color.bar"))
		{
			CalDialogBookLocalItem *item = (CalDialogBookLocalItem *)data;

			if (item->__item_mode == LocalBookDisplay || item->__item_mode == LocalBookChange
				|| item->__item_mode == LocalBookDelete ||  item->__item_mode == LocalBookOnlySelect)
			{
				Evas_Object* icon = evas_object_rectangle_add(evas_object_evas_get(obj));
				evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);

				int r,g,b,a;
				item->__book->getColor(r,g,b,a);
				evas_object_color_set(icon, r, g, b, a);
				//evas_object_color_set(icon, 255, 0, 0, 255);

				return icon;
			}
		}

		return NULL;
	};
	return &itc;
}

void CalDialogBookLocalItem::onSelect()
{
	if(__item_mode == LocalBookDisplay)
	{
		if (__selectCb)
		{
			__selectCb(__book);
		}
	}
	else if(__item_mode == LocalBookChange)
	{
		__book->setVisibility(!(__book->getVisibility()));

		if(__book->getVisibility())
		{
			elm_check_state_set(__check, EINA_TRUE);
		}
		else
		{
			elm_check_state_set(__check, EINA_FALSE);
		}

		if (__selectCb)
		{
			__selectCb(__book);
		}
	}
	else if(__item_mode == LocalBookOnlySelect)
	{
		if (__selectCb)
		{
			__selectCb(__book);
		}
	}
	else if(__item_mode == LocalBookDelete)
	{
		if (__deleteSelectCb)
		{
			__deleteSelectCb(this);
		}
	}
}

void CalDialogBookLocalItem::onCheckboxCb()
{
	if(__checkboxCb)
	{
		__checkboxCb(this->getItemCheckState());
	}
}


bool CalDialogBookLocalItem::getItemCheckState()
{
	if(__check)
	{
		return elm_check_state_get(__check);
	}

	return false;
}

void CalDialogBookLocalItem::setItemCheckState(bool state)
{
	if(__check)
	{
		elm_check_state_set(__check, state);
	}
}

void CalDialogBookLocalItem::setSelectCb(std::function<void (const std::shared_ptr<CalBook>& book)> selectCb)
{
	__selectCb = selectCb;
}

void CalDialogBookLocalItem::setDeleteSelectCb(std::function<void (CalDialogBookLocalItem *item)> deleteSelectCb)
{
	__deleteSelectCb = deleteSelectCb;
}


void CalDialogBookLocalItem::setCheckboxCb(std::function<void (bool state)> checkboxCb)
{
	__checkboxCb = checkboxCb;
}

