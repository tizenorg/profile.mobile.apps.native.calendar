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
#include "CalDialogDetailDescriptionItem.h"

CalDialogDetailDescriptionItem::CalDialogDetailDescriptionItem(const char *description)
{
	 __description = g_strdup(description);
}

CalDialogDetailDescriptionItem::~CalDialogDetailDescriptionItem()
{
	if (__description) {
		g_free(__description);
		__description = NULL;
	}
}

Elm_Genlist_Item_Class* CalDialogDetailDescriptionItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {};
	itc.item_style = "CalDialogDetailDescriptionItem";
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		if (0 == strcmp(part, "elm.text.title"))
			return g_strdup(_L_("IDS_CLD_BUTTON_NOTES_ABB"));
		else if (0 == strcmp(part, "elm.text.multiline"))
		{
			CalDialogDetailDescriptionItem* item = (CalDialogDetailDescriptionItem*)data;
			return g_strdup(item->__description);
		}
		return NULL;
	};

	return &itc;
}
