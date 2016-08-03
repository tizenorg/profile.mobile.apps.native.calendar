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
#include "CalDate.h"
#include "CalTouchInputRecognizer.h"

/**
 * @brief Calendar month row control.
 *
 * @see CalControl.
 */
class CalMonthRowControl : public CalControl
{
public:
	/**
	 * @brief Create month row control.
	 *
	 * @param[in]   rowEdjeGroupName   Row edje group name.
	 */
	CalMonthRowControl(const char* edjeGroupName = NULL);

	virtual ~CalMonthRowControl();

	/**
	 * @brief Execute the create action.
	 *
	 * @param[in]   parent   If non-null, the parent.
	 * @param[in]   param    If non-null, the parameter.
	 *
	 * @return Null if it fails, else an Evas_Object*.
	 */
	virtual Evas_Object* onCreate(Evas_Object* parent, void* param);

	/**
	 * @brief Execute the destroy action.
	 */
	virtual void onDestroy();

	/**
	 * @brief Receive touch input for.
	 *
	 * @param[in]   touchInputRecognizer   The touch input recognizer.
	 */
	void receiveTouchInputFor(CalTouchInputRecognizer& touchInputRecognizer);

	/**
	 * @brief Reset the day nums.
	 *
	 * @param[in]   firstWeekday   The first weekday.
	 * @param[in]   rowStartDate   The row start date.
	 * @param[in]   i              Zero-based index of the.
	 */
	void resetDayNums(int firstWeekday, const CalDate& rowStartDate, int i = -1);

	/**
	 * @brief Reset the day number style described by i.
	 *
	 * @param[in]   i   Zero-based index of the.
	 */
	void resetDayNumStyle(int i);

	/**
	 * @brief Reset the day by bound.
	 *
	 * @param[in]   rowStartDate   The row start date.
	 * @param[in]   lowerBound     The lower bound.
	 * @param[in]   upperBound     The upper bound.
	 */
	void resetDayByBound(const CalDate& rowStartDate, const CalDate& lowerBound, const CalDate& upperBound);

	/**
	 * @brief Mark today.
	 *
	 * @param[in]   j   The int to process.
	 */
	void markToday(int j);

	/**
	 * @brief Reset the circle described by j.
	 *
	 * @param[in]   j   The int to process.
	 */
	void resetCircle(int j);

	/**
	 * @brief Paint the focus.
	 *
	 * @param[in]   j            The int to process.
	 * @param[in]   eventcount   The eventcount.
	 */
	void paintFocus(int j, int eventcount = 0);

	/**
	 * @brief Erase focus.
	 *
	 * @param[in]   i            Zero-based index of the.
	 * @param[in]   j            The int to process.
	 * @param[in]   today        True to today.
	 * @param[in]   animated     True if animated.
	 * @param[in]   eventcount   The eventcount.
	 */
	void eraseFocus(int i, int j, bool today, bool animated = false, int eventcount = 0);

	/**
	 * @brief Dot.
	 *
	 * @param[in]   j           The int to process.
	 * @param[in]   thisMonth   True to this month.
	 * @param[in]   focus       True to focus.
	 */
	void dot(int j, bool thisMonth, bool focus);

	/**
	 * @brief Clear this object to its blank/initial state.
	 */
	void clear();

	/**
	 * @brief Identify cell.
	 *
	 * @param[in]   cell   If non-null, the cell.
	 *
	 * @return Index of week day.
	 */
	int identifyCell(Evas_Object* cell);

private:
	/**
	 * @brief Reset the day number style.
	 *
	 * @param[in]   i   Zero-based index of the.
	 * @param[in]   j   The int to process.
	 */
	void __resetDayNumStyle(int i, int j);

	/**
	 * @brief Reset the day number style.
	 *
	 * @param[in]   i      Zero-based index of the.
	 * @param[in]   j      The int to process.
	 * @param[in]   mday   The mday.
	 */
	void __resetDayNumStyle(int i, int j, int mday);

	/**
	 * @brief Get weekday of column.
	 *
	 * @param[in]   j   The int to process.
	 *
	 * @return The weekday of column.
	 */
	int __getWeekdayOfColumn(int j);

	/**
	 * @brief Signal.
	 *
	 * @param[in]   signal   The signal.
	 * @param[in]   j        The int to process.
	 */
	void __signal(const char* signal, int j);

	/**
	 * @brief Raise the content event.
	 *
	 * @param[in]   signal   The signal.
	 * @param[in]   j        The int to process.
	 */
	void __signalContent(const char* signal, int j);

	/**
	 * @brief Gets a cell.
	 *
	 * @param[in]   j   The int to process.
	 *
	 * @return Null if it fails, else the cell.
	 */
	const Evas_Object* __getCell(int j);

	/**
	 * @brief Get content object.
	 *
	 * @return Null if it fails, else the content object.
	 */
	Evas_Object* __getContentObj();

	const char* __edjeGroupName;
	int __firstWeekday;
	Evas_Object* __content;
};

#endif
