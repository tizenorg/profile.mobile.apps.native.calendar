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

#include <app.h>
#include "CalCommon.h"
#include "CalDataManager.h"
#include "CalDialogDetailLocationItem.h"

#define DEFAULT_COLOR 0, 0, 0, 255

CalDialogDetailLocationItem::CalDialogDetailLocationItem(const char *location) : __location(location)
{
}

CalDialogDetailLocationItem::~CalDialogDetailLocationItem()
{
}

Elm_Genlist_Item_Class* CalDialogDetailLocationItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++
	itc.item_style = "CalDialogDetailDescriptionItem";
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogDetailLocationItem* item = (CalDialogDetailLocationItem*)data;
		if (0 == strcmp(part, "elm.text.title"))
			return g_strdup(_L_("IDS_CLD_BUTTON_LOCATION_ABB"));
		if (0 == strcmp(part, "elm.text.multiline")) {
			return g_strdup(item->__location.c_str());
		}
		return NULL;
	};

	return &itc;
}

