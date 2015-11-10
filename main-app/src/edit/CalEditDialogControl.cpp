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

#include "CalEditDialogControl.h"
#include "CalCommon.h"

CalEditDialogControl::CalEditDialogControl() :
	CalDialogControl()
{
}

CalEditDialogControl::~CalEditDialogControl()
{
}

const char* CalEditDialogControl::getClassName()
{
	return "CalEditDialogControl";
}

Elm_Object_Item* CalEditDialogControl::add(CalDialogControl::Item* item)
{
	WENTER();
	Elm_Object_Item* objectItem = NULL;
	Evas_Object* genlist = getEvasObj();
	WPRET_VM((genlist == NULL),NULL,"get fail");
	if (item == NULL)
	{
		WERROR("item is NULL");
		return NULL;
	}

	int sortIndex = item->getSortIndex();
	WDEBUG("sortIndex:%d", sortIndex);
	if (sortIndex == 0)
	{
		WERROR("fail");
		return NULL;
	}

	std::set<int>::iterator it=__sortedSet.find(sortIndex);
	if (it != __sortedSet.end())
	{
		WERROR("already add");
		return NULL;
	}

	Elm_Object_Item* curObj = NULL;

	curObj = elm_genlist_first_item_get(genlist);
	if (curObj == NULL)
	{
		objectItem = elm_genlist_item_append(genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE,
				[](void *data, Evas_Object *obj, void *event_info)
				{
					Elm_Object_Item* genlistItem = (Elm_Object_Item*) event_info;
					elm_genlist_item_selected_set(genlistItem, EINA_FALSE);

					CalDialogControl::Item* item = (CalDialogControl::Item*)data;
					item->onSelect();
				}, item);
	}
	else
	{
		do
		{
			Elm_Object_Item* nextObj = NULL;
			CalDialogControl::Item* curItem = (CalDialogControl::Item*)elm_object_item_data_get(curObj);
			CalDialogControl::Item* nextItem = NULL;
			int curIndex = curItem->getSortIndex();
			int nextIndex = 0;

			nextObj = elm_genlist_item_next_get(curObj);

			if (nextObj == NULL)
			{
				if (sortIndex > curIndex)
				{
					objectItem = elm_genlist_item_append(genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE,
							[](void *data, Evas_Object *obj, void *event_info)
							{
								Elm_Object_Item* genlistItem = (Elm_Object_Item*) event_info;
								elm_genlist_item_selected_set(genlistItem, EINA_FALSE);

								CalDialogControl::Item* item = (CalDialogControl::Item*)data;
								item->onSelect();
							}, item);
				}
				else
				{
					objectItem = elm_genlist_item_insert_before(genlist, item->getItemClassStatic(), item, NULL,
							curObj, ELM_GENLIST_ITEM_NONE,
							[](void *data, Evas_Object *obj, void *event_info)
							{
								Elm_Object_Item* genlistItem = (Elm_Object_Item*) event_info;
								elm_genlist_item_selected_set(genlistItem, EINA_FALSE);

								CalDialogControl::Item* item = (CalDialogControl::Item*)data;
								item->onSelect();
							}, item);
				}
				break;
			}
			else
			{
				nextItem = (CalDialogControl::Item*)elm_object_item_data_get(nextObj);
				nextIndex = nextItem->getSortIndex();
				if (curIndex < sortIndex && sortIndex < nextIndex)
				{
					objectItem = elm_genlist_item_insert_after(genlist, item->getItemClassStatic(), item, NULL,
							curObj, ELM_GENLIST_ITEM_NONE,
							[](void *data, Evas_Object *obj, void *event_info)
							{
								Elm_Object_Item* genlistItem = (Elm_Object_Item*) event_info;
								elm_genlist_item_selected_set(genlistItem, EINA_FALSE);

								CalDialogControl::Item* item = (CalDialogControl::Item*)data;
								item->onSelect();
							}, item);
					break;
				}
				else if (curIndex > sortIndex)
				{
					objectItem = elm_genlist_item_insert_before(genlist, item->getItemClassStatic(), item, NULL,
							curObj, ELM_GENLIST_ITEM_NONE,
							[](void *data, Evas_Object *obj, void *event_info)
							{
								Elm_Object_Item* genlistItem = (Elm_Object_Item*) event_info;
								elm_genlist_item_selected_set(genlistItem, EINA_FALSE);

								CalDialogControl::Item* item = (CalDialogControl::Item*)data;
								item->onSelect();
							}, item);
					break;
				}

				curObj = nextObj;
			}
		} while(1);
	}

	__sortedSet.insert(sortIndex);

	WPRET_VM((objectItem == NULL),NULL,"fail");

	item->setElmObjectItem(objectItem);

	elm_object_item_del_cb_set(objectItem, onItemDelete);

	return objectItem;
}

void CalEditDialogControl::remove(CalDialogControl::Item* item)
{
	WENTER();
	const Elm_Object_Item* objectItem = item->getElmObjectItem();
	if (objectItem)
	{
		__sortedSet.erase(item->getSortIndex());
		elm_object_item_del((Elm_Object_Item*)objectItem);
	}
	else
	{
		WDEBUG("already remove or not yet add");
	}
}

void CalEditDialogControl::setNull(CalDialogControl::Item* item)
{
	WENTER();
	item->setElmObjectItem(NULL);
}

