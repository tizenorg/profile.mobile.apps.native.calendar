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
#include "CalCommon.h"
#include "CalDialogEditDateTimeItem.h"
#include "CalDateTime.h"
#include "CalSettingsManager.h"

#define DATETIME_BUTTON_PADDING 16

CalDialogEditDateTimeItem::CalDialogEditDateTimeItem( int sortIndex,
		const CalDateTime &startDateTime, const CalDateTime &endDateTime) :
	CalDialogControl::Item(sortIndex),
	__changedCb(NULL),
	__dateButtonClickedCb(NULL),
	__timeButtonClickedCb(NULL),
	__parent(NULL),
	__startDateTimeBox(NULL),
	__endDateTimeBox(NULL),
	__allDayCheckBox(NULL),
	__timeStartButton(NULL),
	__timeEndButton(NULL),
	__start(startDateTime),
	__end(endDateTime),
	__isDateChanged(false),
	__isAllDay(false),
	__isStartSelected(true)
{
	if(startDateTime.getHour() == 0 && startDateTime.getMinute() == 0)
	{
		struct tm tmStart;
		struct tm tmEnd;
		startDateTime.getTmTime(&tmStart);
		endDateTime.getTmTime(&tmEnd);

		double seconds = difftime(mktime(&tmEnd), mktime(&tmStart));
		double days = seconds/(60 * 60 * 24.0);

		if((days > 0.0 && !(days - floor(days) > 0.0)) || __end == __start)
		{
			__isAllDay = true;
		}
	}
}

CalDialogEditDateTimeItem::~CalDialogEditDateTimeItem()
{
}

Elm_Genlist_Item_Class* CalDialogEditDateTimeItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++
	itc.item_style = "timepicker";
	itc.func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		CalDialogEditDateTimeItem* item = (CalDialogEditDateTimeItem*)data;
		if (!strcmp(part, "start_datetime"))
		{
			return item->__startDateTimeBox = item->onCreateDateTime(obj, true);
		}
		else if (!strcmp(part, "end_datetime"))
		{
			return item->__endDateTimeBox = item->onCreateDateTime(obj, false);
		}
		else if (!strcmp(part, "all_day_check"))
		{
			return item->__allDayCheckBox = item->onCreateAllDayCheckBox(obj);
		}

		return NULL;
	};
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		char temp[FORMAT_BUFFER] = {0};
		CalDialogEditDateTimeItem* item = (CalDialogEditDateTimeItem*)data;

		if (!strcmp(part, "start_title"))
		{
			snprintf(temp, sizeof(temp) - 1, LABEL_FORMAT, item->getSystemFontSize(), _L_("IDS_CLD_BODY_START_M_DATE"));
			return strdup(temp);
		}
		else if (!strcmp(part, "end_title"))
		{
			snprintf(temp, sizeof(temp) - 1, LABEL_FORMAT, item->getSystemFontSize(), _L_("IDS_CLD_BODY_END_M_DATE"));
			return strdup(temp);
		}
		else if (!strcmp(part, "all_day_title"))
		{
			snprintf(temp, sizeof(temp) - 1, LABEL_FORMAT, item->getSystemFontSize(), _L_("IDS_CLD_OPT_ALL_DAY_ABB2"));
			return strdup(temp);
		}

		return NULL;
	};

	return &itc;
}

void CalDialogEditDateTimeItem::setChangeCb(std::function<void (CalDateTime &startDateTime,CalDateTime &endDateTime)> changedCb)
{
	__changedCb = changedCb;
}

void CalDialogEditDateTimeItem::setDateButtonClickedCb(std::function<void (Evas_Object* dateTime)> dateButtonClickedCb)
{
	__dateButtonClickedCb = dateButtonClickedCb;
}

void CalDialogEditDateTimeItem::setTimeButtonClickedCb(std::function<void (Evas_Object* dateTime)> timeButtonClickedCb)
{
	__timeButtonClickedCb = timeButtonClickedCb;
}

void CalDialogEditDateTimeItem::setTimeZone(const std::string& timezone)
{

}

void CalDialogEditDateTimeItem::getDateTime(CalDateTime* startDateTime, CalDateTime* endDateTime, Eina_Bool* allDay)
{
	if(startDateTime)
	{
		*startDateTime = __start;
	}

	if(endDateTime)
	{
		*endDateTime = __end;
	}

	if(allDay)
	{
		*allDay = __isAllDay;
	}
}


void CalDialogEditDateTimeItem::setDate(const CalDateTime & date)
{
	CalDateTime &selectedDate = __isStartSelected ? __start : __end;

	CalDateTime tmpDate(date.getYear(), date.getMonth(),
			date.getMday(), selectedDate.getHour(), selectedDate.getMinute(), selectedDate.getSecond());

	selectedDate = tmpDate;

	__isDateChanged = true;

	update();
}

void CalDialogEditDateTimeItem::setTime(const CalDateTime & time)
{
	CalDateTime &selectedTime = __isStartSelected ? __start : __end;

	CalDateTime tmpTime(selectedTime.getYear(), selectedTime.getMonth(), selectedTime.getMday(),
			time.getHour(), time.getMinute(), time.getSecond());

	selectedTime = tmpTime;

	update();
}

bool CalDialogEditDateTimeItem::isAllDay()
{
	return __isAllDay;
}

bool CalDialogEditDateTimeItem::isStartSelected()
{
	return __isStartSelected;
}

bool CalDialogEditDateTimeItem::isDateChanged()
{
	return __isDateChanged;
}

void CalDialogEditDateTimeItem::update()
{
	elm_genlist_item_update(getElmObjectItem());
}

void CalDialogEditDateTimeItem::updateStartAndEndTime(const CalDateTime & startDateTime, const CalDateTime &endDateTime)
{
	__start = startDateTime;
	__end = endDateTime;

	update();
}

void CalDialogEditDateTimeItem::__onDateButtonClicked(void* data, Evas_Object* obj, void* event_info)
{
	CalDialogEditDateTimeItem* self = static_cast<CalDialogEditDateTimeItem*>(data);
	if (self->__dateButtonClickedCb)
	{
		self->__dateButtonClickedCb(obj);
	}
}

void CalDialogEditDateTimeItem::__onTimeButtonClicked(void* data, Evas_Object* obj, void* event_info)
{
	CalDialogEditDateTimeItem* self = static_cast<CalDialogEditDateTimeItem*>(data);
	if (self->__timeButtonClickedCb)
	{
		self->__timeButtonClickedCb(obj);
	}
}

void CalDialogEditDateTimeItem::__onStartDateClicked(void *data, Evas_Object *obj, void *event_info)
{
	CalDialogEditDateTimeItem* self = static_cast<CalDialogEditDateTimeItem*>(data);
	if(self)
	{
		self->__isStartSelected = true;
		if(self->__dateButtonClickedCb)
		{
			self->__dateButtonClickedCb(obj);
		}
	}
}

void CalDialogEditDateTimeItem::__onEndDateClicked(void *data, Evas_Object *obj, void *event_info)
{
	CalDialogEditDateTimeItem* self = static_cast<CalDialogEditDateTimeItem*>(data);
	if(self)
	{
		self->__isStartSelected = false;
		if(self->__dateButtonClickedCb)
		{
			self->__dateButtonClickedCb(obj);
		}
	}
}

void CalDialogEditDateTimeItem::__onStartTimeClicked(void *data, Evas_Object *obj, void *event_info)
{
	CalDialogEditDateTimeItem* self = static_cast<CalDialogEditDateTimeItem*>(data);
	if(self)
	{
		self->__isStartSelected = true;
		if (self->__timeButtonClickedCb)
		{
			self->__timeButtonClickedCb(obj);
		}
	}
}

void CalDialogEditDateTimeItem::__onEndTimeClicked(void *data, Evas_Object *obj, void *event_info)
{
	CalDialogEditDateTimeItem* self = static_cast<CalDialogEditDateTimeItem*>(data);
	if(self)
	{
		self->__isStartSelected = false;
		if (self->__timeButtonClickedCb)
		{
			self->__timeButtonClickedCb(obj);
		}
	}
}


Evas_Object* CalDialogEditDateTimeItem::onCreateDateTime(Evas_Object* parent, bool start)
{
	std::string strTime;
	std::string strDate;
	if (start)
	{
		__start.getTimeString(strTime);
		__start.getDateString(strDate);
	}
	else
	{
		__end.getTimeString(strTime);
		__end.getDateString(strDate);
	}

	Evas_Object* layout = elm_layout_add(parent);
	elm_layout_file_set(layout, CAL_EDJE, "DateTimeLayout");
	evas_object_show(layout);

	Evas_Object* dateBtn = elm_button_add(layout);
	evas_object_size_hint_align_set(dateBtn, EVAS_HINT_FILL, EVAS_HINT_FILL);
	Evas_Smart_Cb dateCB = start ? __onStartDateClicked : __onEndDateClicked;
	evas_object_smart_callback_add(dateBtn, "clicked", dateCB, this);
	elm_object_part_content_set(layout, "elm.swallow.left", dateBtn);

	evas_object_show(dateBtn);

	int fontSize = getSystemFontSize();
	if(fontSize > TEXT_ITEM_DEFAULT_SIZE)
	{
		fontSize = TEXT_ITEM_DEFAULT_SIZE;
	}

	char temp[FORMAT_BUFFER] = {0};
	snprintf(temp, sizeof(temp) - 1, LABEL_FORMAT, fontSize, strDate.c_str());

	elm_object_text_set(dateBtn, temp);

	if (!__isAllDay)
	{
		Evas_Object* timeBtn = elm_button_add(layout);
		evas_object_size_hint_align_set(timeBtn, EVAS_HINT_FILL, EVAS_HINT_FILL);
		Evas_Smart_Cb timeCB = start ? __onStartTimeClicked : __onEndTimeClicked;
		evas_object_smart_callback_add(timeBtn, "clicked", timeCB, this);
		elm_object_part_content_set(layout, "elm.swallow.right", timeBtn);
		evas_object_show(timeBtn);

		snprintf(temp, sizeof(temp) - 1, LABEL_FORMAT, fontSize, strTime.c_str());
		elm_object_text_set(timeBtn, temp);

		if (start)
		{
			__timeStartButton = timeBtn;
		}
		else
		{
			__timeEndButton = timeBtn;
		}
	}

	evas_object_smart_callback_add(parent, "changed",
		[](void* data, Evas_Object* obj, void* event_info) {

			CalDialogEditDateTimeItem* item = (CalDialogEditDateTimeItem*)data;
			if (item->__changedCb)
			{
				item->__changedCb(item->__start, item->__end);
			}

		}, this);

	return layout;
}

Evas_Object* CalDialogEditDateTimeItem::onCreateAllDayCheckBox(Evas_Object* parent)
{
	Evas_Object* allDayCheckBox = elm_check_add(parent);

	evas_object_smart_callback_add(allDayCheckBox, "changed",
		[](void* data, Evas_Object* obj, void* event_info) {

			CalDialogEditDateTimeItem* item = (CalDialogEditDateTimeItem*)data;

			Eina_Bool isAllDay = elm_check_state_get(obj);
			item->__isAllDay= isAllDay;

			if (item->__changedCb)
			{
				item->__changedCb(item->__start, item->__end);
			}

		}, this);

	elm_check_state_set(allDayCheckBox, __isAllDay);
	evas_object_show(allDayCheckBox);

	return allDayCheckBox;
}
