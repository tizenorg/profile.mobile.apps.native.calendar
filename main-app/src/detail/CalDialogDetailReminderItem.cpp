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
#include "CalScheduleReminder.h"
#include "CalDataManager.h"
#include "CalDialogDetailReminderItem.h"

CalDialogDetailReminderItem::CalDialogDetailReminderItem(const std::set<CalScheduleReminder>& reminders) : __reminders(reminders)
{
}

CalDialogDetailReminderItem::~CalDialogDetailReminderItem()
{
}

Elm_Genlist_Item_Class* CalDialogDetailReminderItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++
	itc.item_style = "CalDialogDetailReminderItem";
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogDetailReminderItem* item = (CalDialogDetailReminderItem*)data;
		if (0 == strcmp(part, "elm.text.title")) {
			return g_strdup(_L_("IDS_CLD_BUTTON_REMINDER_ABB"));
		}
		else if (0 == strcmp(part, "elm.text.multiline"))
		{
			std::string strText;
			std::string strResult;
			for (auto it = item->__reminders.begin(); it != item->__reminders.end(); ++it)
			{
				it->getString(strText);
				if(!strResult.empty())
					strResult += "<br>";

				strResult += strText;
			}

			return g_strdup(strResult.c_str());
		}
		return NULL;
	};
	return &itc;
}

