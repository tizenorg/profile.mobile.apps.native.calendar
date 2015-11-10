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

#include <string>
#include "CalDatePickerPopup.h"
#include "CalSettingsManager.h"
#include "CalLocaleManager.h"
#include "CalCommon.h"
#include "CalDateTime.h"

CalDatePickerPopup::CalDatePickerPopup(CalDateTime& startDateTime) :
	__dateTime(nullptr)
{
	__dateInitial.set(startDateTime.getYear(), startDateTime.getMonth(), startDateTime.getMday());
}

CalDatePickerPopup::~CalDatePickerPopup()
{
}

Evas_Object* CalDatePickerPopup::onCreate(Evas_Object* parent, void* viewParam)
{
	setContent([this](Evas_Object* parent)->Evas_Object*
	{
		return __createContent(parent);
	});

	return WPopup::onCreate(parent, viewParam);
}

Evas_Object* CalDatePickerPopup::__createContent(Evas_Object* parent)
{
	Evas_Object* layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "application", "noindicator");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	__dateTime = elm_datetime_add(layout);
	evas_object_size_hint_weight_set(__dateTime, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(__dateTime, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_style_set(__dateTime, "date_layout");
	elm_object_part_content_set(layout, "elm.swallow.content", __dateTime);
	elm_datetime_format_set(__dateTime, "%d/%b/%Y %H:%M");
	evas_object_show(layout);

	struct tm time = {};
	time.tm_year = __dateInitial.getYear() - 1900;
	time.tm_mon = __dateInitial.getMonth() - 1;
	time.tm_mday = __dateInitial.getMday();
	time.tm_hour = 0;
	time.tm_min = 0;

	elm_datetime_value_set(__dateTime, &time);

	elm_object_content_set(parent, layout);

	setTextTranslatable(CALENDAR);
	elm_object_part_text_set(parent, "title,text", _L_("IDS_CLD_HEADER_SET_DATE"));
	addButton("IDS_CLD_BUTTON_CANCEL", NULL);
	addButton("IDS_CLD_BUTTON_SET", [this](bool* destroyPopup)
	{
		if(__changedCb)
		{
			struct tm time;
			elm_datetime_value_get(__dateTime, &time);
			__dateInitial.set(&time);

			__changedCb(__dateInitial);
		}

		*destroyPopup = true;
	});

	return layout;
}

void CalDatePickerPopup::onDestroy()
{
}

void CalDatePickerPopup::setChangeCb(std::function<void (CalDate& date)> changedCb)
{
	__changedCb = changedCb;
}

