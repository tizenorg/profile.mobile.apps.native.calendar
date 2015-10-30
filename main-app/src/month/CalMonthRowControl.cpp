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

#include "CalMonthRowControl.h"
#include "CalCommon.h"

#include <Elementary.h>

#define DAYS_PER_WEEK 7
#define GRID_ROW_COUNT 6

CalMonthRowControl::CalMonthRowControl(const char* edjeGroupName) :
	__edjeGroupName(edjeGroupName),
	__firstWeekday(0),
	__content(NULL)
{
}

CalMonthRowControl::~CalMonthRowControl()
{
}

/**
 * Executes the create action.
 *
 * @param [in]	parent	If non-null, the parent.
 * @param [in]	param 	If non-null, the parameter.
 *
 * @return		null	If it fails, else an Evas_Object*.
 */
Evas_Object* CalMonthRowControl::onCreate(Evas_Object* parent, void* param)
{
	Evas_Object* layout = elm_layout_add(parent);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	if (__edjeGroupName)
		elm_layout_file_set(layout, CAL_EDJE, __edjeGroupName);
	else
		elm_layout_file_set(layout, CAL_EDJE, "CalMonthRowControl");
	return layout;
}

/**
 * Executes the destroy action.
 */
void CalMonthRowControl::onDestroy()
{

}

/**
 * Receive touch input for.
 *
 * @param [in]	touchInputRecognizer	The touch input recognizer.
 */
void CalMonthRowControl::receiveTouchInputFor(CalTouchInputRecognizer& touchInputRecognizer)
{
	for (int j = 0; j < DAYS_PER_WEEK; j++) {
		touchInputRecognizer.addTarget((Evas_Object*)__getCell(j));
	}
}

/**
 * Resets the day nums.
 *
 * @param	firstWeekday	The first weekday.
 * @param	rowStartDate	The row start date.
 * @param	i				Zero-based index of the.
 */
void CalMonthRowControl::resetDayNums(int firstWeekday, const CalDate& rowStartDate, int i)
{
	__firstWeekday = firstWeekday;
	CalDate date(rowStartDate);
	for (int j = 0; j < DAYS_PER_WEEK; j++) {
		char partname[100];
		sprintf(partname, "cell/%d/daynum", j);
		char text[10];
		sprintf(text, "%d", date.getMday());
		elm_object_part_text_set(getEvasObj(), partname, text);
		if (i >= 0)
			__resetDayNumStyle(i, j, date.getMday());
		date.incrementDay();
	}
}

/**
 * Resets the day by bound.
 *
 * @param	rowStartDate	The row start date.
 * @param	lowerBound  	The lower bound.
 * @param	upperBound  	The upper bound.
 */
void CalMonthRowControl::resetDayByBound(const CalDate& rowStartDate, const CalDate& lowerBound, const CalDate& upperBound)
{
	CalDate date(rowStartDate);
	for (int j = 0; j < DAYS_PER_WEEK; j++) {
		if(date < lowerBound || date > upperBound) {
			const int weekday = __getWeekdayOfColumn(j);
			__signal("not-this-month/%d", j);
			if (weekday == 0) {
				__signal("sunday,dim/%d", j);
			}
		}
		date.incrementDay();
	}
}

/**
 * Resets the day number style described by i.
 *
 * @param	i	Zero-based index of the.
 */
void CalMonthRowControl::resetDayNumStyle(int i)
{
	for (int j = 0; j < DAYS_PER_WEEK; j++)
		__resetDayNumStyle(i, j);
}

/**
 * Mark today.
 *
 * @param	j	The int to process.
 */
void CalMonthRowControl::markToday(int j)
{
	__signalContent("today/%d", j);
}

/**
 * Resets the day number style.
 *
 * @param	i	Zero-based index of the.
 * @param	j	The int to process.
 */
void CalMonthRowControl::__resetDayNumStyle(int i, int j)
{
	char partname[100];
	sprintf(partname, "cell/%d/daynum", j);
	const char* text = elm_object_part_text_get(getEvasObj(), partname);
	__resetDayNumStyle(i, j, atoi(text));
}

/**
 * Is this month.
 *
 * @param	i		Zero-based index of the.
 * @param	mday	The mday.
 *
 * @return	true if this month, false if not.
 */
static bool __isThisMonth(int i, int mday)
{
	if (i == 0 && mday > DAYS_PER_WEEK)
		return false;
	else if (i > 3 && mday < 23)
		return false;
	return true;
}

/**
 * Resets the day number style.
 *
 * @param	i		Week number.
 * @param	j		Day number.
 * @param	mday	The mday.
 */
void CalMonthRowControl::__resetDayNumStyle(int i, int j, int mday)
{
	WDEBUG("%d %d %d", i, j, mday);
	const int weekday = __getWeekdayOfColumn(j);
	if (i < 0 || i >= GRID_ROW_COUNT || !__isThisMonth(i, mday)) {
		__signal("not-this-month/%d", j);
		if (weekday == 0) {
			__signal("sunday,dim/%d", j);
		}
	} else if (weekday == 0)
		__signal("sunday/%d", j);
	else
		__signal("normal/%d", j);
}

/**
 * Gets weekday of column.
 *
 * @param	j	The int to process.
 *
 * @return	The weekday of column.
 */
int CalMonthRowControl::__getWeekdayOfColumn(int j)
{
	return __firstWeekday + j > 6 ? __firstWeekday + j - 7 : __firstWeekday + j;
}

/**
 * Resets the circle described by j.
 *
 * @param	j	The int to process.
 */
void CalMonthRowControl::resetCircle(int j)
{
	__signalContent("reset/%d", j);
}

/**
 * Paints the focus.
 *
 * @param	j			The int to process.
 * @param	eventcount	The eventcount.
 */
void CalMonthRowControl::paintFocus(int j, int eventcount)
{
	__signal("focus/%d", j);
	__signalContent("focus/%d", j);
	if(eventcount)
		__signalContent("dot/focus/%d", j);
}

/**
 * Erase focus.
 *
 * @param	i			Week number
 * @param	j			Day number.
 * @param	today		True, if current day is 'today'.
 * @param	animated	True if animated.
 * @param	eventcount	The eventcount.
 */
void CalMonthRowControl::eraseFocus(int i, int j, bool today, bool animated, int eventcount)
{
	__resetDayNumStyle(i, j);
	if (today) {
		__signalContent(animated ? "today/%d/animated" : "today/%d", j);
		if(eventcount)
			__signalContent("dot/%d", j);
	} else {
		__signalContent(animated ? "reset/%d/animated" : "reset/%d", j);
		if(eventcount)
			__signalContent("dot/%d", j);
	}
}

/**
 * Dots.
 *
 * @param	j			The int to process.
 * @param	thisMonth	true to this month.
 * @param	focus		true to focus.
 */
void CalMonthRowControl::dot(int j, bool thisMonth, bool focus)
{
	if(focus){
		__signalContent("dot/focus/%d", j);
	} else if(thisMonth){
		__signalContent("dot/%d", j);
	} else {
		__signalContent("dot/dim/%d", j);
	}
}

/**
 * Clears this object to its blank/initial state.
 */
void CalMonthRowControl::clear()
{
	for (int j = 0; j < DAYS_PER_WEEK; j++) {
		__signalContent("undot/%d", j);
	}
}

/**
 * Identify cell.
 *
 * @param [in]	cell	If non-null, the cell.
 *
 * @return	An int.
 */
int CalMonthRowControl::identifyCell(Evas_Object* cell)
{
	for (int j = 0; j < DAYS_PER_WEEK; j++) {
		if (__getCell(j) == cell)
			return j;
	}
	return -1;
}

/**
 * Signals.
 *
 * @param	signal	The signal.
 * @param	j		The int to process.
 */
void CalMonthRowControl::__signal(const char* signal, int j)
{
	char buffer[100];
	sprintf(buffer, signal, j);
	elm_layout_signal_emit(getEvasObj(), buffer, "");
}

/**
 * Raises the content event.
 *
 * @param	signal	The signal.
 * @param	j		The int to process.
 */
void CalMonthRowControl::__signalContent(const char* signal, int j)
{
	char buffer[100];
	sprintf(buffer, signal, j);
	elm_layout_signal_emit(__getContentObj(), buffer, "");
}

/**
 * Gets a cell.
 *
 * @param	j		The int to process.
 *
 * @return	null	If it fails, else the cell.
 */
const Evas_Object* CalMonthRowControl::__getCell(int j)
{
	char partname[100];
	sprintf(partname, "cell/%d", j);
	return edje_object_part_object_get(elm_layout_edje_get(getEvasObj()), partname);
}

/**
 * Gets content object.
 *
 * @return	null if it fails, else the content object.
 */
Evas_Object* CalMonthRowControl::__getContentObj()
{
	if (__content == NULL) {
		 __content = elm_layout_add(getEvasObj());
		evas_object_size_hint_weight_set(__content, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		char groupName[100];
		if (__edjeGroupName)
			sprintf(groupName, "%s.content", __edjeGroupName);
		else
			strcpy(groupName, "CalMonthRowControl.content");
		elm_layout_file_set(__content, CAL_EDJE, groupName);
		elm_object_part_content_set(getEvasObj(), "cells/base", __content);
	}
	return __content;
}
