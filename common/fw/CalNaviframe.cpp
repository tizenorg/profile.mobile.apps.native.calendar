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

#include <memory.h>
#include "CalCommon.h"
#include "CalView.h"
#include "CalEventManager.h"
#include "CalNaviframe.h"

CalNaviframe::CalNaviframe()
{
}

CalNaviframe::~CalNaviframe()
{
}

WView* CalNaviframe::getTopView()
{
	Elm_Object_Item* item = elm_naviframe_top_item_get(getEvasObj());
	Evas_Object* topViewObj = elm_object_item_part_content_get(item, "elm.swallow.content");
	return WView_getInstanceFromEvasObj(topViewObj);
}

bool CalNaviframe::onPush(WView* view, void* viewParam)
{
	WENTER();
	CalEventManager::getInstance().attach((CalView*)view);
	Elm_Object_Item* item = elm_naviframe_top_item_get(getEvasObj());
	if (item)
	{
		CalView* targetView = (CalView*)WView_getInstanceFromEvasObj(
				elm_object_item_part_content_get(item, "elm.swallow.content"));

		WASSERT(targetView);
		CalEventManager::getInstance().block(targetView);
	}

	WLEAVE();
	return WNaviframe::onPush(view, viewParam);
}

bool CalNaviframe::onItemPop(WView* view, Elm_Object_Item* item)
{
	WENTER();
	bool ret = WNaviframe::onItemPop(view, item);
	if (!ret)
	{
		return false;
	}

	__arrangeEventManager(view, item);
	WLEAVE();

	return true;
}

void CalNaviframe::__arrangeEventManager(WView* view, Elm_Object_Item* item)
{
	if (item != elm_naviframe_bottom_item_get(getEvasObj())) // it's not the main view
	{
		CalEventManager::getInstance().detach((CalView*)view);
		Eina_List* itemList = elm_naviframe_items_get(getEvasObj());
		if (itemList)
		{
			int nth = eina_list_count(itemList) - 2;
			if (nth >= 0)
			{
				Elm_Object_Item* tempItem = (Elm_Object_Item*)eina_list_nth(itemList, nth);
				if (tempItem)
				{
					CalView* targetView = (CalView*)WView_getInstanceFromEvasObj(
							elm_object_item_part_content_get(tempItem, "elm.swallow.content"));
					WASSERT(targetView);
					CalEventManager::getInstance().unblock(targetView);
				}
			}

			eina_list_free(itemList);
		}
	}
}

void CalNaviframe::destroyView(WView* view, Elm_Object_Item* item)
{
	WENTER();

	if (!CalEventManager::getInstance().isDetached((CalView*)view))
	{
		__arrangeEventManager(view, item);
	}

	WLEAVE();
}
