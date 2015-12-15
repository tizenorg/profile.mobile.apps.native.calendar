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

#ifndef _CAL_MONTH_ROW_CONTROL_H_
#define _CAL_MONTH_ROW_CONTROL_H_

#include "CalControl.h"

#include <Evas.h>
#include "CalDateTime.h"
#include "CalTouchInputRecognizer.h"

class CalMonthRowControl : public CalControl
{
public:
	CalMonthRowControl(const char* edjeGroupName = NULL);
	virtual ~CalMonthRowControl();

	/**
	 * Executes the create action.
	 *
	 * @param [in]	parent	If non-null, the parent.
	 * @param [in]	param 	If non-null, the parameter.
	 *
	 * @return		null	If it fails, else an Evas_Object*.
	 */
	virtual Evas_Object* onCreate(Evas_Object* parent, void* param);

	/**
	 * Executes the destroy action.
	 */
	virtual void onDestroy();

	/**
	 * Receive touch input for.
	 *
	 * @param [in]	touchInputRecognizer	The touch input recognizer.
	 */
	void receiveTouchInputFor(CalTouchInputRecognizer& touchInputRecognizer);

	/**
	 * Resets the day nums.
	 *
	 * @param	firstWeekday	The first weekday.
	 * @param	rowStartDate	The row start date.
	 * @param	i				Zero-based index of the.
	 */
	void resetDayNums(int firstWeekday, const CalDateTime& rowStartDate, int i = -1);

	/**
	 * Resets the day number style described by i.
	 *
	 * @param	i	Zero-based index of the.
	 */
	void resetDayNumStyle(int i);

	/**
	 * Resets the day by bound.
	 *
	 * @param	rowStartDate	The row start date.
	 * @param	lowerBound  	The lower bound.
	 * @param	upperBound  	The upper bound.
	 */
	void resetDayByBound(const CalDateTime& rowStartDate, const CalDateTime& lowerBound, const CalDateTime& upperBound);

	/**
	 * Mark today.
	 *
	 * @param	j	The int to process.
	 */
	void markToday(int j);

	/**
	 * Resets the circle described by j.
	 *
	 * @param	j	The int to process.
	 */
	void resetCircle(int j);

	/**
	 * Paints the focus.
	 *
	 * @param	j			The int to process.
	 * @param	eventcount	The eventcount.
	 */
	void paintFocus(int j, int eventcount = 0);

	/**
	 * Erase focus.
	 *
	 * @param	i			Zero-based index of the.
	 * @param	j			The int to process.
	 * @param	today		true to today.
	 * @param	animated	true if animated.
	 * @param	eventcount	The eventcount.
	 */
	void eraseFocus(int i, int j, bool today, bool animated = false, int eventcount = 0);

	/**
	 * Dots.
	 *
	 * @param	j			The int to process.
	 * @param	thisMonth	true to this month.
	 * @param	focus		true to focus.
	 */
	void dot(int j, bool thisMonth, bool focus);

	/**
	 * Clears this object to its blank/initial state.
	 */
	void clear();

	/**
	 * Identify cell.
	 *
	 * @param [in]	cell	If non-null, the cell.
	 *
	 * @return	An int.
	 */
	int identifyCell(Evas_Object* cell);

private:

	/**
	 * Resets the day number style.
	 *
	 * @param	i	Zero-based index of the.
	 * @param	j	The int to process.
	 */
	void __resetDayNumStyle(int i, int j);

	/**
	 * Resets the day number style.
	 *
	 * @param	i		Zero-based index of the.
	 * @param	j		The int to process.
	 * @param	mday	The mday.
	 */
	void __resetDayNumStyle(int i, int j, int mday);

	/**
	 * Gets weekday of column.
	 *
	 * @param	j	The int to process.
	 *
	 * @return	The weekday of column.
	 */
	int __getWeekdayOfColumn(int j);

	/**
	 * Signals.
	 *
	 * @param	signal	The signal.
	 * @param	j		The int to process.
	 */
	void __signal(const char* signal, int j);

	/**
	 * Raises the content event.
	 *
	 * @param	signal	The signal.
	 * @param	j		The int to process.
	 */
	void __signalContent(const char* signal, int j);

	/**
	 * Gets a cell.
	 *
	 * @param	j		The int to process.
	 *
	 * @return	null	If it fails, else the cell.
	 */
	const Evas_Object* __getCell(int j);

	/**
	 * Gets content object.
	 *
	 * @return	null if it fails, else the content object.
	 */
	Evas_Object* __getContentObj();

private:
	const char* __edjeGroupName;
	int __firstWeekday;
private:
	Evas_Object* __content;
};

#endif
