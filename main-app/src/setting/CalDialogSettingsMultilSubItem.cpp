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

#include "CalDialogSettingsMultilSubItem.h"

CalDialogSettingsMultilSubItem::CalDialogSettingsMultilSubItem(std::function<void (void)> selectCb, const char *text)
	: __selectCb(selectCb)
{
	__text = g_strdup(text);
}

CalDialogSettingsMultilSubItem::CalDialogSettingsMultilSubItem()
	: __text(NULL)
{
}

CalDialogSettingsMultilSubItem::~CalDialogSettingsMultilSubItem()
{
	g_free(__text);
}

Elm_Genlist_Item_Class* CalDialogSettingsMultilSubItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {};
	itc.item_style = "multiline";

	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogSettingsMultilSubItem *item = (CalDialogSettingsMultilSubItem*)data;
		if (0 == strcmp("elm.text.multiline", part))
		{
			return g_strdup(_L_G_(item->__text));
		}
		return NULL;
	};
	return &itc;
}

void CalDialogSettingsMultilSubItem::onSelect()
{
	WENTER();
	if (__selectCb)
		__selectCb();
}

void CalDialogSettingsMultilSubItem::setText(const char* text)
{
	if (__text)
		g_free(__text);

	__text = g_strdup(text);
}
