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

class CalMonthControl2 : public CalControl
{
public:
	CalMonthControl2(int firstWeekday, int year, int month,
		const char* rowEdjeGroupName = NULL,
		const CalDate* originDate = NULL,
		CalDate lowerBound = CalDate(CalDate::INIT_LOWER_BOUND),
		CalDate upperBound = CalDate(CalDate::INIT_UPPER_BOUND));
	virtual ~CalMonthControl2();
	virtual const char* getClassName();
	void setTapCellCb(std::function<void (int i, int j)> tapCellCb);
	void setScrollCb(std::function<void ()> scrollStartCb, std::function<void (int dir)> scrollFinishCb);

	/**
	 * Resets this object.
	 *
	 * @param	firstWeekday	The first weekday.
	 * @param	year			The year.
	 * @param	month			The month.
	 */
	void reset(int firstWeekday, int year, int month);

	/**
	 * Focus.
	 *
	 * @param	mday	The mday.
	 */
	void focus(int mday);

	/**
	 * Erase focus.
	 */
	void eraseFocus();

	/**
	 * Updates this object.
	 */
	void update();

	/**
	 * Loads this object.
	 */
	void load();

	/**
	 * Gets event count.
	 *
	 * @param	date	The date.
	 *
	 * @return	The event count.
	 */
	int getEventCount(const CalDate& date) const;

	/**
	 * Resets the by bound.
	 */
	void resetByBound();

	/**
	 * Gets focused row.
	 *
	 * @return	The focused row.
	 */
	int getFocusedRow();

	/**
	 * Gets a date.
	 *
	 * @param	i				Zero-based index of the.
	 * @param	j				The int to process.
	 * @param [in]	date	The date.
	 */
	void getDate(int i, int j, CalDate& date);

	/**
	 * Creates a header.
	 *
	 * @param [in]	parent	If non-null, the parent.
	 *
	 * @return	null if it fails, else the new header.
	 */
	Evas_Object* createHeader(Evas_Object* parent);

	/**
	 * Sets header focus.
	 *
	 * @param [in]	header	If non-null, the header.
	 */
	void setHeaderFocus(Evas_Object* header);

	/**
	 * Gets a month.
	 *
	 * @param [in]	year 	The year.
	 * @param [in]	month	The month.
	 */
	void getMonth(int& year, int& month);

	/**
	 * Gets row position.
	 *
	 * @param		offset	The offset.
	 * @param [in]	x		If non-null, the Evas_Coord to process.
	 * @param [in]	y		If non-null, the Evas_Coord to process.
	 * @param [in]	w		If non-null, the Evas_Coord to process.
	 * @param [in]	h		If non-null, the Evas_Coord to process.
	 */
	void getRowPosition(int offset, Evas_Coord* x, Evas_Coord* y, Evas_Coord* w, Evas_Coord* h) const;

	/**
	 * Slides.
	 *
	 * @param	dir	The dir.
	 *
	 * @return	true if it succeeds, false if it fails.
	 */
	bool slide(int dir);

	/**
	 * Freeze scroll.
	 *
	 * @param	freeze	true to freeze.
	 */
	void freezeScroll(bool freeze);

private:

	/**
	 * Executes the create action.
	 *
	 * @param [in]	parent	If non-null, the parent.
	 * @param [in]	param 	If non-null, the parameter.
	 *
	 * @return	null if it fails, else an Evas_Object*.
	 */
	virtual Evas_Object* onCreate(Evas_Object* parent, void* param);

	/**
	 * Executes the destroy action.
	 */
	virtual void onDestroy();

	/**
	 * Determines if we can is upper bound.
	 *
	 * @return	true if upper bound, false if not.
	 */
	bool __isUpperBound();

	/**
	 * Determines if we can is lower bound.
	 *
	 * @return	true if lower bound, false if not.
	 */
	bool __isLowerBound();

	/**
	 * Finds the getscrollrange of the given arguments.
	 *
	 * @param [in,out]	min	The minimum.
	 * @param [in,out]	max	The maximum.
	 */
	void __getScrollRange(Evas_Coord& min, Evas_Coord& max);

	/**
	 * Out range warning.
	 */
	void __outRangeWarning();

	/**
	 * Gets the previous month row offset.
	 *
	 * @return	The previous month row offset.
	 */
	int __getPreviousMonthRowOffset();

	/**
	 * Gets the next month row offset.
	 *
	 * @return	The next month row offset.
	 */
	int __getNextMonthRowOffset();

	/**
	 * Gets a row.
	 *
	 * @param	offset	The offset.
	 *
	 * @return	The row.
	 */
	const CalMonthRowControl& __getRow(int offset) const;

	/**
	 * Fade in.
	 *
	 * @param	startRowOffset	The start row offset.
	 */
	void __fadeIn(int startRowOffset);

	/**
	 * Creates accessory month.
	 *
	 * @param [in]	parent		If non-null, the parent.
	 * @param [in]	rowArray	If non-null, array of rows.
	 *
	 * @return	null if it fails, else the new accessory month.
	 */
	Evas_Object* __createAccessoryMonth(Evas_Object* parent, CalMonthRowControl* rowArray[]);

	/**
	 * Resets the given dir.
	 *
	 * @param	dir	The dir.
	 */
	void __reset(int dir);

	/**
	 * Resets the accessory months described by date.
	 *
	 * @param	date	The date.
	 */
	void __resetAccessoryMonths(const CalDate& date);

	/**
	 * Resets the day nums.
	 *
	 * @param [in]	rowArray	If non-null, array of rows.
	 * @param		dir			The dir.
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
