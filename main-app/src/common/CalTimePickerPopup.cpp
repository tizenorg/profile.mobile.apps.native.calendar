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

#include "CalTimePickerPopup.h"
#include "CalCommon.h"
#include "CalDateTime.h"
#include "CalSettingsManager.h"


CalTimePickerPopup::CalTimePickerPopup(Evas_Object* dateTime, const char* title, CalDateTime startDateTime, bool isRepeat) :
	__changedCb(nullptr),
	__dateTime(nullptr),
	__startDateTime(startDateTime),
	__isRepeat(isRepeat),
	__title(title)
{
}

CalTimePickerPopup::~CalTimePickerPopup()
{
}

Evas_Object* CalTimePickerPopup::onCreate(Evas_Object* parent, void* viewParam)
{
	setContent([this](Evas_Object* parent)->Evas_Object*
	{
		return __createContent(parent);
	});

	return WPopup::onCreate(parent, viewParam);
}

Evas_Object* CalTimePickerPopup::__createContent(Evas_Object *parent)
{
	Evas_Object* layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "application", "noindicator");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	__dateTime = elm_datetime_add(layout);
	evas_object_size_hint_weight_set(__dateTime, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(__dateTime, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_part_content_set(layout, "elm.swallow.content", __dateTime);
	evas_object_show(layout);

	if (CalSettingsManager::getInstance().isHour24())
	{
		elm_object_style_set(__dateTime, "time_layout_24hr");
		elm_datetime_format_set(__dateTime, "%d/%b/%Y %H:%M");
	}
	else
	{
		elm_object_style_set(__dateTime, "time_layout");
		elm_datetime_format_set(__dateTime, "%d/%b/%Y %I:%M %p");
	}

	struct tm time = {};
	__startDateTime.getTmTime(&time);

	elm_datetime_value_set(__dateTime, &time);

	if(__isRepeat)
	{
		time.tm_hour = 0;
		time.tm_min = 0;
		elm_datetime_value_min_set(__dateTime, &time);
	}

	elm_object_content_set(parent, layout);

	setTextTranslatable(CALENDAR);
	elm_object_part_text_set(parent, "title,text", __title.c_str());
	addButton("IDS_CLD_BUTTON_CANCEL", NULL);
	addButton("IDS_CLD_BUTTON_SET", [this](bool* destroyPopup)
	{
		WHIT();
		struct tm time;
		elm_datetime_value_get(__dateTime, &time);

		CalDateTime newTime;
		newTime.set(time);

		if(__changedCb)
		{
			__changedCb(newTime);
		}

		*destroyPopup = true;
	});

	return layout;
}

void CalTimePickerPopup::onDestroy()
{
}

void CalTimePickerPopup::setChangeCb(std::function<void (CalDateTime& newTime)> changedCb)
{
	__changedCb = changedCb;
}
