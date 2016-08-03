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


#ifndef _CALWIDGET_H_
#define _CALWIDGET_H_

#include <widget_app.h>

#include <Ecore.h>
#include <memory>
#include "CalDate.h"
#include "CalCommon.h"
#include "CalMonthControl2.h"
#include "CalEvent.h"
#include "ICalEventListener.h"

/**
 * @brief Calendar widget.
 *
 * @see ICalEventListener.
 */
class CalWidget : private ICalEventListener
{
public:
	CalWidget();

	~CalWidget();

	/**
	 * @brief Create widget.
	 *
	 * @param[in]   context   The handle for widget context.
	 * @param[in]   content   The bundle handle.
	 * @param[in]   w   The width.
	 * @param[in]   h   The height.
	 *
	 * @see widget_context_h
	 * @see bundle
	 */
	void create(widget_context_h context, bundle *content, int w, int h);

	/**
	 * @brief Destroy widget.
	 *
	 * @param[in]   reason   Destroy type of widget instance.
	 *
	 * @see widget_app_destroy_type_e
	 */
	void destroy(widget_app_destroy_type_e reason);

	/**
	 * @brief Resize widget.
	 *
	 * @param[in]   w   The width.
	 * @param[in]   h   The height.
	 */
	void resize(int w, int h);

private:
	static Eina_Bool __onTimer(void* data);
	void __createCalendar();
	Evas_Object *__addBoxButton(Evas_Object* box, const char *text, Evas_Smart_Cb cb);
	int  __getFirstDayOfWeek();
	void __setMonthLabel();
	void __displayCurrentDate();
	void __focusToday();
	void __AddEvent(CalDate *dateTime = nullptr);
	const char *__getStartTime(CalDate dateTime);
	void onEvent(const CalEvent& event);

	Evas_Object *__window;
	Evas_Object *__conform;

	int __width;
	int __height;

	CalMonthControl2 *__monthCalendar;
	CalDate __lowerBound;
	CalDate __upperBound;
	CalDate __dateTime;
	CalDate __originDate;
	CalDate __customFocusDate;
	Evas_Object* __buttonToday;
	Evas_Object* __labelMonth;
	Ecore_Timer* __timer;
	bool __isCutomFocus;
};

#endif // CALWIDGET_H_

