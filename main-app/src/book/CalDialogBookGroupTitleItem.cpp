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

#include "CalDialogBookGroupTitleItem.h"
#include "CalCommon.h"

CalDialogBookGroupTitleItem::CalDialogBookGroupTitleItem(const char *name) : __name(name)
{
	__isGroupTitle = true;
}

CalDialogBookGroupTitleItem::~CalDialogBookGroupTitleItem()
{
}

Elm_Genlist_Item_Class* CalDialogBookGroupTitleItem::getItemClassStatic()
{
	WENTER();
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++
	//itc.item_style = "CalDialogDetailSummaryItem";
	itc.item_style = "group_index";
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogBookGroupTitleItem* self = (CalDialogBookGroupTitleItem*)data;
		if (strcmp(part, "elm.text") == 0) {
			return g_strdup(self->__name);
		}
		return NULL;
	};
	return &itc;
}
