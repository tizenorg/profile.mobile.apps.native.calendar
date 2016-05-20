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

#include <Elementary.h>

#include "CalDialogControl.h"
#include "CalCommon.h"

CalDialogControl::Item::Item(int sortIndex) :
	__isGroupTitle(false),
	__sortIndex(sortIndex),
	__customData(nullptr)

{
}

CalDialogControl::Item::~Item()
{
}

void CalDialogControl::Item::onSelect()
{
	WENTER();
}

const int CalDialogControl::Item::getSortIndex() const
{
	return __sortIndex;
}

void CalDialogControl::Item::setSortIndex(int index)
{
	__sortIndex = index;
}

int CalDialogControl::Item::getSystemFontSize()
{
	int fontSize = 0;
	Eina_Bool res = edje_text_class_get("list_item", NULL, &fontSize);
	if (!res)
	{
		fontSize = TEXT_ITEM_DEFAULT_SIZE;
	}
	else if (fontSize < 0)
	{
		fontSize = -fontSize * TEXT_ITEM_DEFAULT_SIZE / 100;
	}

	if(fontSize > TEXT_MAX_GOOD_SIZE)
	{
		fontSize = TEXT_MAX_GOOD_SIZE;
	}

	return fontSize;
}

CalDialogControl::CalDialogControl()
{
	WENTER();
}

CalDialogControl::~CalDialogControl()
{
	WENTER();
}

const char* CalDialogControl::getClassName()
{
	return "CalDialogControl";
}

Evas_Object* CalDialogControl::onCreate(Evas_Object* parent, void* param)
{
	WENTER();
	Evas_Object* genlist = elm_genlist_add(parent);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_scroller_policy_set(genlist, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	evas_object_show(genlist);

	WLEAVE();
	return genlist;
}

void CalDialogControl::onDestroy()
{
	WENTER();
}

Elm_Object_Item* CalDialogControl::add(CalDialogControl::Item* item)
{
	WENTER();
	Elm_Object_Item* objectItem = NULL;
	Evas_Object* genlist = getEvasObj();
	WPRET_VM((genlist == NULL),NULL,"get fail");

	objectItem = elm_genlist_item_append(genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE,
			[](void *data, Evas_Object *obj, void *event_info)
			{
				Elm_Object_Item* genlistItem = (Elm_Object_Item*) event_info;
				elm_genlist_item_selected_set(genlistItem, EINA_FALSE);
				CalDialogControl::Item* item = (CalDialogControl::Item*)data;

				item->onSelect();
				WHIT();
			}, item);

	WPRET_VM((objectItem == NULL),NULL,"fail");

	item->setElmObjectItem(objectItem);

	elm_object_item_del_cb_set(objectItem, onItemDelete);

	return objectItem;
}

void CalDialogControl::onItemDelete(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	CalDialogControl::Item* item = (CalDialogControl::Item*)
			(data ? data : elm_object_item_data_get((Elm_Object_Item*) event_info));
	WPRET_M(!item, "item is NULL");
	item->setElmObjectItem(NULL);
	delete item;
}
