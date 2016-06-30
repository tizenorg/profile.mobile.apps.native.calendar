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

#include "CalListControl.h"

#include "CalCommon.h"

CalListControl::CalListControl() :
	__scrollDir(false),
	__lastAppendedIndex(-1),
	__lastPrependedIndex(0)
{
}

CalListControl::~CalListControl()
{
}

/**
 * Gets class name.
 *
 * @return	null if it fails, else the class name.
 */
const char* CalListControl::getClassName()
{
	return "CalListControl";
}

/**
 * Executes the create action.
 *
 * @param [in]	parent	If non-null, the parent.
 * @param [in]	param 	If non-null, the parameter.
 *
 * @return	null if it fails, else an Evas_Object*.
 */
Evas_Object* CalListControl::onCreate(Evas_Object* parent, void* param)
{
	Evas_Object* genlist = elm_genlist_add(parent);

	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_homogeneous_set(genlist, EINA_FALSE);

	evas_object_smart_callback_add(genlist, "drag,start,up", [](void* data, Evas_Object* obj, void* event_info) {
		WDEBUG("%s", "drag,start,up");
		CalListControl* self = (CalListControl*)data;
		self->__scrollDir = 1;
	}, this);
	evas_object_smart_callback_add(genlist, "drag,start,down", [](void* data, Evas_Object* obj, void* event_info) {
		WDEBUG("%s", "drag,start,down");
		CalListControl* self = (CalListControl*)data;
		self->__scrollDir = -1;
	}, this);

	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_scroller_policy_set( genlist, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
	evas_object_show(genlist);

	return genlist;
}

/**
 * Executes the destroy action.
 */
void CalListControl::onDestroy()
{
}

CalListControl::Item::Item()
	: __isGroupTitle(false)
	, __date()
	, __elmObjectItem(NULL)
	, __index(0)
	, __showCheckBox(true)
{
}

CalListControl::Item::~Item()
{
}

void CalListControl::Item::setShowCheckBox(bool showCheckBox)
{
	__showCheckBox = showCheckBox;
}

bool CalListControl::Item::getShowCheckBox() const
{
	return __showCheckBox;
}

/**
 * Clears this object to its blank/initial state.
 */
void CalListControl::clear()
{
	elm_genlist_clear(getEvasObj());
}

/**
 * Adds an item.
 *
 * @param [in]	item		If non-null, the item.
 * @param		dir			The dir.
 * @param		selectCb	The select cb.
 *
 * @return	null if it fails, else an Elm_Object_Item*.
 */
Elm_Object_Item* CalListControl::__addItem(CalListControl::Item* item, int dir, Evas_Smart_Cb selectCb)
{
	Elm_Genlist_Item_Class* itc = item->getItemClassStatic();

	Elm_Object_Item* objectItem = NULL;

	if(!item->isGroupTitle())
	{
		if (dir > 0)
		{
			objectItem = elm_genlist_item_append(getEvasObj(), itc, item, NULL, ELM_GENLIST_ITEM_NONE, selectCb, this);
		}
		else
		{
			objectItem = elm_genlist_item_prepend(getEvasObj(), itc, item, NULL, ELM_GENLIST_ITEM_NONE, selectCb, this);
		}
	}
	else
	{
		Elm_Object_Item* monthMarker = NULL;

		CalDate date = item->getDate();
		if (dir > 0)
		{
			date.incrementMonth();

			Elm_Object_Item* it = elm_genlist_last_item_get(getEvasObj());
			while (it)
			{
				Item* thisItem = (Item*)elm_object_item_data_get(it);
				if(thisItem && !thisItem->isGroupTitle() && date.isSameMonth(thisItem->getDate()))
				{
					monthMarker = it;
					break;
				}

				it = elm_genlist_item_prev_get(it);
			}

			if(monthMarker)
			{
				objectItem = elm_genlist_item_insert_before(getEvasObj(), itc, item, NULL, monthMarker, ELM_GENLIST_ITEM_NONE, selectCb, this);
			}
			else
			{
				objectItem = elm_genlist_item_append(getEvasObj(), itc, item, NULL, ELM_GENLIST_ITEM_NONE, selectCb, this);
			}
		}
		else
		{
			Elm_Object_Item* it = elm_genlist_first_item_get(getEvasObj());
			while (it)
			{
				Item* thisItem = (Item*)elm_object_item_data_get(it);
				if(thisItem && !thisItem->isGroupTitle() && date.isSameMonth(thisItem->getDate()))
				{
					monthMarker = it;
					break;
				}

				it = elm_genlist_item_next_get(it);
			}

			if(monthMarker)
			{
				objectItem = elm_genlist_item_insert_after(getEvasObj(), itc, item, NULL, monthMarker, ELM_GENLIST_ITEM_NONE, selectCb, this);
			}
		}
	}

	WPRET_VM((objectItem == NULL),NULL,"fail");

	elm_genlist_item_select_mode_set(objectItem, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	elm_object_item_del_cb_set(objectItem,
		[](void *data, Evas_Object *obj, void *event_info)
		{
			WHIT();
			CalListControl::Item* item = (CalListControl::Item*)
					(data ? data : elm_object_item_data_get((Elm_Object_Item*) event_info));
			delete item;
		}
	);
	item->__elmObjectItem = objectItem;
	item->__index = dir > 0 ? ++__lastAppendedIndex : --__lastPrependedIndex;

	return objectItem;
}

/**
 * Searches for the first item.
 *
 * @param	date	The date.
 *
 * @return	null if it fails, else the found item.
 */
CalListControl::Item* CalListControl::__findItem(const CalDate& date)
{
	Item* retItem = NULL;

	Elm_Object_Item* item = elm_genlist_first_item_get(getEvasObj());
	while (item)
	{
		Item* thisItem = (Item*)elm_object_item_data_get(item);
		if(thisItem && thisItem->isGroupTitle() && thisItem->getDate() == date)
		{
			retItem = thisItem;
			elm_genlist_item_update(item);
			break;
		}

		item = elm_genlist_item_next_get(item);
	}

	return retItem;
}

/**
 * Gets nearest group item.
 *
 * @param	dir	The dir.
 *
 * @return	null if it fails, else the nearest group item.
 */
CalListControl::Item* CalListControl::Item::__getNearestGroupItem(int dir)
{
	Item* item = this;
	Elm_Object_Item* it = __elmObjectItem;
	while (!item->__isGroupTitle)
	{
		WDEBUG("%s", item->getString());
		if (dir > 0)
		{
			it = elm_genlist_item_next_get(it);
		}
		else
		{
			it = elm_genlist_item_prev_get(it);
		}

		if (it == NULL)
		{
			return __getNearestGroupItem(-dir);
		}

		item = (Item*)elm_object_item_data_get(it);
	}

	return item;
}

/**
 * Gets top showing item.
 *
 * @param	offset	The offset.
 *
 * @return	null if it fails, else the top showing item.
 */
CalListControl::Item* CalListControl::__getTopShowingItem(int offset)
{
	Evas_Coord_Rectangle rect = {0};
	evas_object_geometry_get(getEvasObj(), &rect.x, &rect.y, &rect.w, &rect.h);

	int posret;
	Elm_Object_Item* it = elm_genlist_at_xy_item_get(getEvasObj(), rect.x, rect.y + offset, &posret);
	if (it == NULL)
	{
		return NULL;
	}

	Item* item = (Item*)elm_object_item_data_get(it);
	WASSERT(item);
	return item;
}

/**
 * Gets top showing group item.
 *
 * @param	offset	The offset.
 *
 * @return	null if it fails, else the top showing group item.
 */
CalListControl::Item* CalListControl::__getTopShowingGroupItem(int offset)
{
	Item* item = __getTopShowingItem(offset);
	if (item)
	{
		if(item->isGroupTitle())
		{
			return item;
		}
		else
		{
			return item->__getNearestGroupItem(-1);
		}
	}
	else
	{
		return NULL;
	}
}

/**
 * Gets the next item.
 *
 * @param [in]	item	If non-null, the item.
 *
 * @return		null if it fails, else the next item.
 */
CalListControl::Item* CalListControl::__getNextItem(CalListControl::Item* item)
{
	Elm_Object_Item* it = elm_genlist_item_next_get(item->__elmObjectItem);
	if (it == NULL)
	{
		return NULL;
	}

	item = (Item*)elm_object_item_data_get(it);
	WASSERT(item);
	return item;
}

/**
 * Gets the previous item.
 *
 * @param [in]	item	If non-null, the item.
 *
 * @return		null if it fails, else the previous item.
 */
CalListControl::Item* CalListControl::__getPreviousItem(CalListControl::Item* item)
{
	Elm_Object_Item* it = elm_genlist_item_prev_get(item->__elmObjectItem);
	if (it == NULL)
	{
		return NULL;
	}

	item = (Item*)elm_object_item_data_get(it);
	WASSERT(item);
	return item;
}
