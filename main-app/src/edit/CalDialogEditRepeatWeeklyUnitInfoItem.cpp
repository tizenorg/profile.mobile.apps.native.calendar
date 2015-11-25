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

#include <functional>
#include "CalCommon.h"
#include "CalDialogControl.h"
#include "CalScheduleRepeat.h"
#include "CalDialogEditRepeatWeeklyUnitInfoItem.h"
#include "CalSettingsManager.h"
#include "CalLocaleManager.h"



CalDialogEditRepeatWeeklyUnitInfoItem::CalDialogEditRepeatWeeklyUnitInfoItem(std::function<void (CalScheduleRepeat::UnitInfo unitInfo)> changedCb, const CalScheduleRepeat repeat, const CalDateTime & startDate, const std::string& timezone) :
	__changedCb(changedCb),
	__repeat(repeat),
	__startDate(startDate),
	__timezone(timezone)
{
	__unitInfo = repeat.unitInfo;
	if ( (__unitInfo.weekly.selected[0] ||__unitInfo.weekly.selected[1] || __unitInfo.weekly.selected[2] || __unitInfo.weekly.selected[3]
		|| __unitInfo.weekly.selected[4] || __unitInfo.weekly.selected[5] || __unitInfo.weekly.selected[6]) == false) {
		__unitInfo.weekly.selected[__startDate.getWeekday(__timezone.c_str())] = true;
	}
}

CalDialogEditRepeatWeeklyUnitInfoItem::~CalDialogEditRepeatWeeklyUnitInfoItem()
{
}

Evas_Object* CalDialogEditRepeatWeeklyUnitInfoItem::createCheck(Evas_Object *parent, const char *text, int index, CalDialogEditRepeatWeeklyUnitInfoItem *item, int day)
{
	Evas_Object *check = elm_check_add(parent);

	if (day == 0)
		elm_object_style_set(check, "WeeklyCheckSunItem");
	else
		elm_object_style_set(check, "WeeklyCheckItem");

	elm_object_part_text_set(check, "button", text);
	elm_check_state_set(check, item->__unitInfo.weekly.selected[day]);
	evas_object_data_set(check, "day", (void *)day);
	evas_object_smart_callback_add(check, "changed",
		[](void *data, Evas_Object *obj, void *event_info)->void
		{
			CalDialogEditRepeatWeeklyUnitInfoItem *item = (CalDialogEditRepeatWeeklyUnitInfoItem*)data;
			int day = reinterpret_cast<long long>(evas_object_data_get(obj, "day"));
			item->__unitInfo.weekly.selected[day] = elm_check_state_get(obj);
			if (item->__changedCb)
			{
				item->__changedCb(item->__unitInfo);
			}
		}, item
	);

	return check;
}

static int __getFirstDayOfWeek()
{
	int result = CalSettingsManager::getInstance().getFirstDayOfWeek();
	if (result == CalSettingsManager::LOCALES)
		return CalLocaleManager::getInstance().getLocaleFirstDayOfWeek();

	return result;
}

Elm_Genlist_Item_Class* CalDialogEditRepeatWeeklyUnitInfoItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++

	itc.item_style = "CalDialogEditRepeatWeeklyUnitInfoItem";

	itc.func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		CalDialogEditRepeatWeeklyUnitInfoItem *item = (CalDialogEditRepeatWeeklyUnitInfoItem*)data;

		int i = 0;
		     if (strcmp("elm.icon.1", part) == 0) i = 0;
		else if (strcmp("elm.icon.2", part) == 0) i = 1;
		else if (strcmp("elm.icon.3", part) == 0) i = 2;
		else if (strcmp("elm.icon.4", part) == 0) i = 3;
		else if (strcmp("elm.icon.5", part) == 0) i = 4;
		else if (strcmp("elm.icon.6", part) == 0) i = 5;
		else if (strcmp("elm.icon.7", part) == 0) i = 6;

		int j = (__getFirstDayOfWeek() + i)%7;
		const char* wdayText = CalDate::getWeekdayShortText(j);

		return item->createCheck(obj, wdayText, i, item, j);
	};

	return &itc;
}
