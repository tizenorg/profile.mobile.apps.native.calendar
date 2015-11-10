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
#include "CalScheduleRepeat.h"
#include "CalDataManager.h"
#include "CalDialogDetailRepeatItem.h"

CalDialogDetailRepeatItem::CalDialogDetailRepeatItem(const CalScheduleRepeat& repeat, const CalDateTime& start, const char* timezone)
	: __repeat(repeat)
	, __start(start)
	, __tz(NULL)
{
	if (timezone)
		__tz = strdup(timezone);
}

CalDialogDetailRepeatItem::~CalDialogDetailRepeatItem()
{
	if (__tz)
		free(__tz);
}

Elm_Genlist_Item_Class* CalDialogDetailRepeatItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {};
	itc.item_style = "CalDialogDetailDescriptionItem";
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogDetailRepeatItem* item = (CalDialogDetailRepeatItem*)data;
		if (0 == strcmp(part, "elm.text.title")) {
			return strdup(_L_("IDS_CLD_BUTTON_REPEAT_ABB"));
		}
		else if (0 == strcmp(part, "elm.text.multiline")) {
			std::string repeatText;
			CalScheduleRepeat::Date date;
			date.year = item->__start.getYear();
			date.month = item->__start.getMonth();
			date.mday = item->__start.getMday();
			item->__repeat.getRepeatString(date,item->__tz,repeatText);
			return strdup(repeatText.c_str());
		}
		return NULL;
	};
	return &itc;
}


