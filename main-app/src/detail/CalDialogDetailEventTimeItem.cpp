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
#include "CalDataManager.h"
#include "CalDialogDetailEventTimeItem.h"

CalDialogDetailEventTimeItem::CalDialogDetailEventTimeItem(const std::shared_ptr<CalSchedule>& schedule) : __schedule(schedule)
{

}

CalDialogDetailEventTimeItem::~CalDialogDetailEventTimeItem()
{

}

Elm_Genlist_Item_Class* CalDialogDetailEventTimeItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {};
	itc.item_style = "multiline";
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogDetailEventTimeItem* item = (CalDialogDetailEventTimeItem*)data;
		if (!strcmp(part, "elm.text.multiline"))
		{
			std::string timeText;
			item->__schedule->getFromToDisplayString(timeText);
			return strdup(timeText.c_str());
		}

		return NULL;
	};
	return &itc;
}

