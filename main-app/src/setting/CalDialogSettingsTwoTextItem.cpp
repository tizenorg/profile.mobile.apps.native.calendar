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
#include "CalDialogSettingsTwoTextItem.h"

#include <Elementary.h>
#include "CalSettingsManager.h"

CalDialogSettingsTwoTextItem::CalDialogSettingsTwoTextItem(std::function<void (void)> selectCb, const char* mainText, const char* subText,CalDialogSettingsTwoTextItem::sub_item_type type)
	: __selectCb(selectCb)
{
	__mainText = g_strdup(mainText);
	__subText = g_strdup(subText);
	__type = type;
}
CalDialogSettingsTwoTextItem::CalDialogSettingsTwoTextItem()
	: __mainText(NULL)
	, __subText(NULL)
	, __selectCb(NULL)
	, __type(SUB_FIRST_DAY_OF_WEEK)
{
}

CalDialogSettingsTwoTextItem::~CalDialogSettingsTwoTextItem()
{
}

Elm_Genlist_Item_Class* CalDialogSettingsTwoTextItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++
	itc.item_style = "type1";
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogSettingsTwoTextItem* item = (CalDialogSettingsTwoTextItem*)data;
		if (strcmp(part, "elm.text") == 0)
			return strdup(_L_G_(item->__mainText));
		else if (strcmp(part, "elm.text.sub") == 0)
		{
			if (item->__type == CalDialogSettingsTwoTextItem::SUB_FIRST_DAY_OF_WEEK)
			{
				int result = CalSettingsManager::getInstance().getFirstDayOfWeek();
				if (result == CalSettingsManager::LOCALES)
					return strdup(_L_("IDS_CLD_OPT_LOCALE_DEFAULT_ABB"));
				else if (result == CalSettingsManager::SAT)
					return strdup(_L_("IDS_CLD_OPT_SATURDAY"));
				else if (result == CalSettingsManager::SUN)
					return strdup(_L_("IDS_CLD_OPT_SUNDAY"));
				else if (result == CalSettingsManager::MON)
					return strdup(_L_("IDS_CLD_OPT_MONDAY"));
			}
			else if (item->__type == CalDialogSettingsTwoTextItem::SUB_TIME_ZONE ||
					 item->__type == CalDialogSettingsTwoTextItem::SUB_REMINDER_TYPE ||
					 item->__type == CalDialogSettingsTwoTextItem::SUB_NOTIFICATION_SOUND)
				return strdup(_L_G_(item->__subText));
			else if (item->__type == CalDialogSettingsTwoTextItem::SUB_ALERT_TYPE)
			{
				int result = CalSettingsManager::getInstance().getAlertType();
				if (result == CalSettingsManager::ALERT)
					return strdup(_L_("IDS_CLD_OPT_POP_UP_NOTIFICATIONS_ABB"));
				else if (result == CalSettingsManager::STATUS_BAR_NOTIFICATION)
					return strdup(_L_("IDS_CLD_OPT_STATUS_BAR_NOTIFICATIONS_ABB"));
				else if (result == CalSettingsManager::OFF)
					return strdup(_L_("IDS_CLD_OPT_NO_ALERTS"));
			}
		}

		return NULL;
	};

	return &itc;
}

void CalDialogSettingsTwoTextItem::onSelect()
{
	WENTER();
	if (__selectCb)
		__selectCb();
}

void CalDialogSettingsTwoTextItem::setMainText(const char* text)
{
	if (__mainText)
		g_free(__mainText);

	__mainText = g_strdup(text);
}

void CalDialogSettingsTwoTextItem::setSubText(const char* text)
{
	if (__subText)
		g_free(__subText);

	__subText = g_strdup(text);
}
