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

#ifndef _CAL_MONTH_CONTROL_2_H_
#define _CAL_MONTH_CONTROL_2_H_

#include "CalControl.h"

#include <functional>

#include <Evas.h>
#include <Ecore.h>
#include "WDefine.h"

#include "CalTapRecognizer.h"
#include "CalDate.h"

#include "CalMonthControl.h"
#include "CalMonthRowControl.h"

#include "CalDragRecognizer.h"
#include "CalSlideAnimator.h"

/**
 * @brief Calendar month control(second version).
 *
 * @see CalControl.
 */
class CalMonthControl2 : public CalControl
{
public:
	/**
	 * @brief Create month control.
	 *
	 * @param[in]   firstWeekday       The first week day.
	 * @param[in]   year               The year.
	 * @param[in]   month              The month.
	 * @param[in]   rowEdjeGroupName   Row edje group name.
	 * @param[in]   originDate         Original date.
	 * @param[in]   lowerBound         The lower bound.
	 * @param[in]   upperBound         The upper bound.
	 *
	 * @see CalDate
	 * @see CalDate::InitialValue
	 */
	CalMonthControl2(int firstWeekday, int year, int month,
		const char* rowEdjeGroupName = NULL,
		const CalDate* originDate = NULL,
		CalDate lowerBound = CalDate(CalDate::INIT_LOWER_BOUND),
		CalDate upperBound = CalDate(CalDate::INIT_UPPER_BOUND));

	virtual ~CalMonthControl2();

	/**
	 * @brief Get class name.
	 *
	 * return Class name.
	 */
	virtual const char* getClassName();

	/**
	 * @brief Set tap cell callback.
	 *
	 * @param[in]   tapCellCb   Tap cell callback.
	 */
	void setTapCellCb(std::function<void (int i, int j)> tapCellCb);

	/**
	 * @brief Set scroll callback.
	 *
	 * @param[in]   scrollStartCb    Scroll start callback.
	 * @param[in]   scrollFinishCb   Scroll finish callback.
	 */
	void setScrollCb(std::function<void ()> scrollStartCb, std::function<void (int dir)> scrollFinishCb);

	/**
	 * @brief Reset this object.
	 *
	 * @param[in]   firstWeekday   The first weekday.
	 * @param[in]   year           The year.
	 * @param[in]   month          The month.
	 */
	void reset(int firstWeekday, int year, int month);

	/**
	 * @brief Focu.
	 *
	 * @param[in]   mday   The mday.
	 */
	void focus(int mday);

	/**
	 * @brief Erase focus.
	 */
	void eraseFocus();

	/**
	 * @brief Update this object.
	 */
	void update();

	/**
	 * @brief Load this object.
	 */
	void load();

	/**
	 * @brief Get event count.
	 *
	 * @param[in]   date   The date.
	 *
	 * @return The event count.
	 */
	int getEventCount(const CalDate& date) const;

	/**
	 * @brief Reset the by bound.
	 */
	void resetByBound();

	/**
	 * @brief Get focused row.
	 *
	 * @return The focused row.
	 */
	int getFocusedRow();

	/**
	 * @brief Get a date.
	 *
	 * @param[in]   i      Zero-based index of the.
	 * @param[in]   j      The int to process.
	 * @param[in]   date   The date.
	 */
	void getDate(int i, int j, CalDate& date);

	/**
	 * @brief Create a header.
	 *
	 * @param[in]   parent   If non-null, the parent.
	 *
	 * @return Null if it fails, else the new header.
	 */
	Evas_Object* createHeader(Evas_Object* parent);

	/**
	 * @brief Set header focus.
	 *
	 * @param[in]   header   If non-null, the header.
	 */
	void setHeaderFocus(Evas_Object* header);

	/**
	 * @brief Get a month.
	 *
	 * @param[in]   year    The year.
	 * @param[in]   month   The month.
	 */
	void getMonth(int& year, int& month);

	/**
	 * @brief Get row position.
	 *
	 * @param[in]   offset   The offset.
	 * @param[in]   x        If non-null, the Evas_Coord to process.
	 * @param[in]   y        If non-null, the Evas_Coord to process.
	 * @param[in]   w        If non-null, the Evas_Coord to process.
	 * @param[in]   h        If non-null, the Evas_Coord to process.
	 */
	void getRowPosition(int offset, Evas_Coord* x, Evas_Coord* y, Evas_Coord* w, Evas_Coord* h) const;

	/**
	 * @brief Slide.
	 *
	 * @param[in]   dir   The dir.
	 *
	 * @return True if it succeeds, false if it fails.
	 */
	bool slide(int dir);

	/**
	 * @brief Freeze scroll.
	 *
	 * @param[in]   freeze   True to freeze.
	 */
	void freezeScroll(bool freeze);

private:

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
	 * @brief Determine if we can is upper bound.
	 *
	 * @return True if upper bound, false if not.
	 */
	bool __isUpperBound();

	/**
	 * @brief Determine if we can is lower bound.
	 *
	 * @return True if lower bound, false if not.
	 */
	bool __isLowerBound();

	/**
	 * @brief Find the getscrollrange of the given arguments.
	 *
	 * @param[in,out]   min   The minimum.
	 * @param[in,out]   max   The maximum.
	 */
	void __getScrollRange(Evas_Coord& min, Evas_Coord& max);

	/**
	 * @brief Out range warning.
	 */
	void __outRangeWarning();

	/**
	 * @brief Get the previous month row offset.
	 *
	 * @return The previous month row offset.
	 */
	int __getPreviousMonthRowOffset();

	/**
	 * @brief Get the next month row offset.
	 *
	 * @return The next month row offset.
	 */
	int __getNextMonthRowOffset();

	/**
	 * @brief Get a row.
	 *
	 * @param[in]   offset   The offset.
	 *
	 * @return The row.
	 */
	const CalMonthRowControl& __getRow(int offset) const;

	/**
	 * @brief Fade in.
	 *
	 * @param[in]   startRowOffset   The start row offset.
	 */
	void __fadeIn(int startRowOffset);

	/**
	 * @brief Create accessory month.
	 *
	 * @param[in]   parent     If non-null, the parent.
	 * @param[in]   rowArray   If non-null, array of rows.
	 *
	 * @return Null if it fails, else the new accessory month.
	 */
	Evas_Object* __createAccessoryMonth(Evas_Object* parent, CalMonthRowControl* rowArray[]);

	/**
	 * @brief Reset the given dir.
	 *
	 * @param[in]   dir   The dir.
	 */
	void __reset(int dir);

	/**
	 * @brief Reset the accessory months described by date.
	 *
	 * @param[in]   date   The date.
	 */
	void __resetAccessoryMonths(const CalDate& date);

	/**
	 * @brief Reset the day nums.
	 *
	 * @param[in]   rowArray   If non-null, array of rows.
	 * @param[in]   dir        The dir.
	 */
	void __resetDayNums(CalMonthRowControl* rowArray[], int dir);
	WDISABLE_COPY_AND_ASSIGN(CalMonthControl2);

private:
	std::function<void ()> __scrollStartCb;
	std::function<void (int dir)> __scrollFinishCb;
	const char* __rowEdjeGroupName;
	int __firstWeekday;
	CalMonthControl* __thisMonth;
	Evas_Object* __previousMonth;
	Evas_Object* __nextMonth;
	CalMonthRowControl* __previousMonthRows[GRID_ROW_COUNT];
	CalMonthRowControl* __nextMonthRows[GRID_ROW_COUNT];
	CalSlideAnimator* __slideAnimator;
	CalDragRecognizer* __dragRecognizer;
	Ecore_Idler* __monthIdler;
	bool __blockTap;
	CalDate __lowerBound;
	CalDate __upperBound;
};

#endif
