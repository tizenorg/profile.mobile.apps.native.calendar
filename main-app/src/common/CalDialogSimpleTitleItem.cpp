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

#include "CalCommon.h"
#include "CalDialogControl.h"
#include "CalDialogSimpleTitleItem.h"

CalDialogSimpleTitleItem::CalDialogSimpleTitleItem(const char *title) : __title(title)
{
}

CalDialogSimpleTitleItem::~CalDialogSimpleTitleItem()
{
}

Elm_Genlist_Item_Class* CalDialogSimpleTitleItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {};
	itc.item_style = "group_index";
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogSimpleTitleItem *item = (CalDialogSimpleTitleItem*)data;
		if (0 == strcmp(part, "elm.text"))
		{
			return g_strdup(_L_G_(item->__title));
		}
		return NULL;
	};
	return &itc;
}
