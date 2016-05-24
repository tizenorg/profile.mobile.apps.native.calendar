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

#include <algorithm>
#include <widget_app_efl.h>
#include "CalWidget.h"
#include "WDebugBase.h"
#include "CalEventManager.h"
#include "CalDataManager.h"
#include "CalBookManager.h"
#include "CalSettingsManager.h"
#include "CalLocaleManager.h"
#include "CalAppControlLauncher.h"
#include "CalPath.h"

#define DATETIME_BUFFER 64
#define BOX_W_PAD 10

CalWidget::CalWidget() :
	__window(NULL),
	__conform(NULL),
	__width(0),
	__height(0),
	__monthCalendar(NULL),
	__buttonToday(NULL),
	__labelMonth(NULL),
	__timer(NULL),
	__isCutomFocus(false)
{
}

CalWidget::~CalWidget()
{
}

void CalWidget::create(widget_context_h context, bundle *content, int w, int h)
{
	if (content)
	{
		// Recover the previous status with the bundle object
	}

	__width = w;
	__height = h;

	// Window
	int ret = widget_app_get_elm_win(context, &__window);
	if (ret == WIDGET_ERROR_NONE)
	{
		evas_object_resize(__window, w, h);
		evas_object_color_set(__window, 255, 255, 255, 0);

		// Conformant
		__conform = elm_conformant_add(__window);
		evas_object_size_hint_weight_set(__conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		elm_win_resize_object_add(__window, __conform);

		__createCalendar();

		evas_object_show(__conform);
		evas_object_show(__window);
	}
	else
	{
		WERROR("failed to get window. err = %d", ret);
	}
}

void CalWidget::destroy(widget_app_destroy_type_e reason)
{
	if(__monthCalendar)
	{
		__monthCalendar->destroy();
		__monthCalendar = NULL;
	}
}

void CalWidget::resize(int w, int h)
{
	__width = w;
	__height = h;

	evas_object_resize(__window, w, h);
	evas_object_resize(__conform, w, h);
}

void CalWidget::__createCalendar()
{
	WENTER();

	CalDataManager::getInstance();
	CalBookManager::getInstance();
	CalSettingsManager::getInstance();
	CalLocaleManager::getInstance();
	CalAppControlLauncher::getInstance();
	CalEventManager::getInstance().attach(this);

	Evas_Object* layout = elm_layout_add(__window);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_layout_file_set(layout, CalPath::getPath(CAL_EDJE).c_str(), "WidgetLayout");
	evas_object_show(layout);

	Evas_Object* boxTop = elm_box_add(layout);
	evas_object_size_hint_weight_set(boxTop, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(boxTop, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_box_horizontal_set(boxTop, EINA_TRUE);
	elm_box_padding_set(boxTop, ELM_SCALE_SIZE(BOX_W_PAD), 0);
	elm_box_homogeneous_set(boxTop, EINA_FALSE);

	__addBoxButton(boxTop, "IDS_CLD_OPT_CREATE",
		[](void* data, Evas_Object* obj, void* event_info)
		{
			CalWidget* self = (CalWidget*)data;
			self->__AddEvent();
		}
	);

	__buttonToday = __addBoxButton(boxTop, "IDS_CLD_ACBUTTON_TODAY",
		[](void* data, Evas_Object* obj, void* event_info)
		{
			CalWidget* self = (CalWidget*)data;
			self->__focusToday();
		}
	);

	elm_object_part_content_set(layout, "widget.month.today", boxTop);

	Evas_Object* boxLeft = elm_box_add(layout);
	evas_object_size_hint_weight_set(boxLeft, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(boxLeft, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_box_horizontal_set(boxLeft, EINA_TRUE);
	elm_box_homogeneous_set(boxLeft, EINA_FALSE);

	__labelMonth = elm_label_add(boxLeft);
	elm_label_ellipsis_set(__labelMonth, EINA_FALSE);
	elm_label_line_wrap_set(__labelMonth, ELM_WRAP_NONE);
	evas_object_show(__labelMonth);

	elm_box_pack_end(boxLeft, __labelMonth);
	evas_object_show(boxLeft);

	elm_object_part_content_set(layout, "widget.month.date", boxLeft);

	__dateTime = CalDate(CalDate::INIT_TODAY);
	__originDate = __dateTime;
	__customFocusDate = __dateTime;
	__lowerBound = CalDate(CalDate::INIT_LOWER_BOUND),
	__upperBound = CalDate(CalDate::INIT_UPPER_BOUND);

	__monthCalendar = new CalMonthControl2(__getFirstDayOfWeek(), __dateTime.getYear(), __dateTime.getMonth(), "CalMonthRowWidget", &__originDate,	__lowerBound, __upperBound);
	__monthCalendar->create(layout, NULL);
	__monthCalendar->resetByBound();
	__monthCalendar->focus(__dateTime.getMday());
	__monthCalendar->setScrollCb([this]() { }, [this](int dir)
	{
		if (dir == 0)
		{
			return;
		}

		if (dir > 0)
		{
			__dateTime.incrementMonth();
		}
		else
		{
			__dateTime.decrementMonth();
		}

		if(__isCutomFocus)
		{
			__isCutomFocus = false;
			__dateTime = __customFocusDate;
			__monthCalendar->focus(__dateTime.getMday());
		}
		else
		{
			__monthCalendar->focus(1);
		}

		if(__dateTime < __lowerBound)
		{
			__dateTime = __lowerBound;
		}
		else if(__dateTime > __upperBound)
		{
			__dateTime = __upperBound;
		}

		__displayCurrentDate();
		__monthCalendar->resetByBound();
	});

	__monthCalendar->setTapCellCb([this](int i, int j)
	{
		CalDate newFocusDate;
		__monthCalendar->getDate(i, j, newFocusDate);

		if(newFocusDate < __lowerBound || newFocusDate > __upperBound)
		{
			WDEBUG("Invalid date %s", newFocusDate.getString());
			return;
		}

		if (newFocusDate.getMonth() == __dateTime.getMonth())
		{
			__dateTime = newFocusDate;
			__monthCalendar->focus(newFocusDate.getMday());
		}
		else
		{
			__isCutomFocus = true;
			__customFocusDate = newFocusDate;
			if(!__monthCalendar->slide(newFocusDate > __dateTime ? 1 : -1))
			{
				__isCutomFocus = false;
			}

			return;
		}

		if (__timer)
		{
			ecore_timer_del(__timer);
		}

		__timer = ecore_timer_add(0.1, CalWidget::__onTimer, this);
	});

	Evas_Object* header = __monthCalendar->createHeader(layout);
	elm_object_signal_emit(header, "base/widget", "prog");
	elm_object_part_content_set(layout, "widget.month.header", header);
	elm_object_part_content_set(layout, "widget.month.body", __monthCalendar->getEvasObj());

	elm_object_part_content_set(__conform, "elm.swallow.content", layout);
	evas_object_resize(__conform, __width, __height);

	__displayCurrentDate();
}

Evas_Object* CalWidget::__addBoxButton(Evas_Object* box, const char *text, Evas_Smart_Cb cb)
{
	Evas_Object* button = elm_button_add(box);
	elm_object_style_set(button, "transparent");
	evas_object_smart_callback_add(button, "clicked", cb, this);

	Evas_Object* layoutButton = elm_layout_add(button);
	evas_object_size_hint_weight_set(layoutButton, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layoutButton, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_layout_file_set(layoutButton, CalPath::getPath(CAL_EDJE).c_str(), "box_buttons");

	elm_object_domain_translatable_part_text_set(layoutButton, "elm.text", CALENDAR, text);

	evas_object_show(layoutButton);
	elm_object_content_set(button, layoutButton);
	evas_object_show(button);
	elm_box_pack_end(box, button);

	return button;
}

int CalWidget::__getFirstDayOfWeek()
{
	int result = CalSettingsManager::getInstance().getFirstDayOfWeek();
	if (result == CalSettingsManager::LOCALES)
	{
		return CalLocaleManager::getInstance().getLocaleFirstDayOfWeek();
	}

	return result;
}

Eina_Bool CalWidget::__onTimer(void* data)
{
	CalWidget* widget = (CalWidget*)data;

	int eventCount = widget->__monthCalendar->getEventCount(widget->__dateTime);
	if (eventCount == 0) {
		widget->__AddEvent(&widget->__dateTime);
	} else {
		app_control_h service = NULL;
		app_control_create(&service);
		if(service)
		{
			app_control_add_extra_data(service, CAL_APPSVC_PARAM_VIEW, CAL_APPSVC_PARAM_VIEW_MAIN);
			app_control_add_extra_data(service, CAL_APPSVC_PARAM_DATE, widget->__dateTime.getUnixTimeString());
			app_control_set_operation(service, APP_CONTROL_OPERATION_MAIN);
			app_control_set_app_id(service, CAL_PKGNAME);
			CalAppControlLauncher::getInstance().sendLaunchRequest(service, NULL, NULL, NULL);
		}
	}
	return ECORE_CALLBACK_CANCEL;
}

void CalWidget::__AddEvent(CalDate *dateTime)
{
	app_control_h service = NULL;
	app_control_create(&service);
	if(service)
	{
		app_control_set_operation(service, APP_CONTROL_OPERATION_ADD);
		app_control_set_mime(service, APP_CONTROL_MIME_CALENDAR);
		if (dateTime) {
			app_control_add_extra_data(service, APP_CONTROL_DATA_CALENDAR_START_TIME, __getStartTime(*dateTime));
		}
		CalAppControlLauncher::getInstance().sendLaunchRequest(service, NULL, NULL, NULL);
	}
}

const char *CalWidget::__getStartTime(CalDate dateTime)
{
	CalDateTime startTime;
	struct tm startTm = {0};
	startTime.getTmFromUtime(&startTm);
	bool isToday = false;
	if (startTm.tm_year == (dateTime.getYear() - 1900) &&
			startTm.tm_mon == (dateTime.getMonth() - 1) &&
			startTm.tm_mday == dateTime.getMday() ) {
		isToday = true;
	}
	startTm.tm_year = dateTime.getYear() - 1900;
	startTm.tm_mon = dateTime.getMonth() - 1;
	startTm.tm_mday = dateTime.getMday();
	startTm.tm_min = 0;
	startTm.tm_sec = 0;
	if (isToday == false) {
		// If not Today, 08:00.
		startTm.tm_hour = 8;
		startTime.set(startTm);
	} else {
		// If Today, current hour + 1
		startTime.set(startTm);
		startTime.addHours(1);
	}

	static char buffer[DATETIME_BUFFER];
	snprintf(buffer, DATETIME_BUFFER, "%04d-%.02d-%.02d %.02d:%.02d:%.02d",
	startTime.getYear(), startTime.getMonth(), startTime.getMday(),
	startTime.getHour(), startTime.getMinute(), startTime.getSecond());
	return buffer;
}

void CalWidget::__focusToday()
{
	__originDate = CalDate(CalDate::INIT_TODAY);

	int year = __originDate.getYear();
	int month = __originDate.getMonth();

	__monthCalendar->getMonth(year, month);
	if (month != __originDate.getMonth())
	{
		__dateTime = __originDate;
		__monthCalendar->reset(__getFirstDayOfWeek(), __originDate.getYear(), __originDate.getMonth());
		__displayCurrentDate();
	}
	else if (year != __originDate.getYear())
	{
		__dateTime = __originDate;
		__setMonthLabel();
	}

	__monthCalendar->focus(__originDate.getMday());
}

void CalWidget:: __setMonthLabel()
{
	WENTER();
	std::string strMonth;
	CalLocaleManager::getInstance().getDateText(CalLocaleManager::DATEFORMAT_6, __dateTime, strMonth);

	strMonth = "<p font_size=52 align=center color=#FFFFFF style=far_shadow,bottom shadow_color=#999999FF>" + strMonth + "</p>";
	elm_object_text_set(__labelMonth, strMonth.c_str());

	Evas_Object* layoutToday = elm_object_content_get(__buttonToday);
	elm_object_domain_translatable_part_text_set(layoutToday, "elm.text", CALENDAR, "IDS_CLD_ACBUTTON_TODAY");
}

void CalWidget::__displayCurrentDate()
{
	__setMonthLabel();
	__monthCalendar->load();
}

void CalWidget::onEvent(const CalEvent& event)
{
	WENTER();
	WDEBUG("type = %u, source = %u", event.type, event.source);

	c_retm_if(!__monthCalendar, "__monthCalendar is null onEvent()");

	switch (event.type)
	{
		case CalEvent::LANGUAGE_CHANGED:
			__setMonthLabel();
			break;
		case CalEvent::FIRST_DAY_OF_WEEK_CHANGED:
			__monthCalendar->reset(__getFirstDayOfWeek(), __dateTime.getYear(), __dateTime.getMonth());
			__monthCalendar->update();
			break;
		case CalEvent::SETTING_CHANGED:
		case CalEvent::TIME_CHANGED:
			__originDate = CalDate(CalDate::INIT_TODAY);
			__monthCalendar->reset(__getFirstDayOfWeek(), __originDate.getYear(), __originDate.getMonth());
			__monthCalendar->update();
			__setMonthLabel();
			break;
		case CalEvent::DB_CHANGED:
			__monthCalendar->load();
			break;
		case CalEvent::APP_PAUSED:
		case CalEvent::APP_RESUMED:
			break;
		default:
			WERROR("invalid type = %u", event.type);
			break;
	}
}

