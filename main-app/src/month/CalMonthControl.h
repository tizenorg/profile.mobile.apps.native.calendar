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

#ifndef _CAL_MONTH_CONTROL_H_
#define _CAL_MONTH_CONTROL_H_

#include "CalControl.h"

#include <functional>
#include <map>

#include <Evas.h>

#include "WDefine.h"

#include "CalTapRecognizer.h"
#include "CalDateTime.h"

#include "CalMonthRowControl.h"

#define GRID_ROW_COUNT 6
#define DAYS_PER_WEEK 7

class CalMonthControl : public CalControl
{
public:
	CalMonthControl(int firstWeekday, int year, int month, const char* rowEdjeGroupName = NULL, const CalDateTime* originDate = NULL);
	virtual ~CalMonthControl();
	virtual const char* getClassName();

	/**
	 * Sets tap cell cb.
	 *
	 * @param	tapCellCb	The tap cell cb.
	 */
	void setTapCellCb(std::function<void (int i, int j)> tapCellCb);

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
	 *
	 * @param	animated	true if animated.
	 */
	void eraseFocus(bool animated = false);

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
	int getEventCount(const CalDateTime& date) const;

	/**
	 * Resets the by bound.
	 *
	 * @param	lowerBound	The lower bound.
	 * @param	upperBound	The upper bound.
	 */
	void resetByBound(const CalDateTime& lowerBound, const CalDateTime& upperBound);

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
	void getDate(int i, int j, CalDateTime& date);

	/**
	 * Check boundary.
	 *
	 * @return	An int.
	 */
	int checkBoundary();

	/**
	 * Creates a header.
	 *
	 * @param [in]	parent	If non-null, the parent.
	 *
	 * @return	null if it fails, else the new header.
	 */
	Evas_Object* createHeader(Evas_Object* parent);

	/**
	 * Updates the header text.
	 */
	void updateHeaderText();

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
	 * @param			offset	The offset.
	 * @param [in,out]	x		If non-null, the Evas_Coord to process.
	 * @param [in,out]	y		If non-null, the Evas_Coord to process.
	 * @param [in,out]	w		If non-null, the Evas_Coord to process.
	 * @param [in,out]	h		If non-null, the Evas_Coord to process.
	 */
	void getRowPosition(int offset, Evas_Coord* x, Evas_Coord* y, Evas_Coord* w, Evas_Coord* h) const;

	/**
	 * Gets a row.
	 *
	 * @param	index	Zero-based index of the.
	 *
	 * @return	The row.
	 */
	CalMonthRowControl& getRow(int index);

	/**
	 * Fade out.
	 */
	void fadeOut();

private:
	struct Position {
		int i;
		int j;
	};

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

	WDISABLE_COPY_AND_ASSIGN(CalMonthControl);

	/**
	 * Updates the today position.
	 */
	void __updateTodayPos();

	/**
	 * Paints the day nums described by month.
	 *
	 * @param [in]	month	If non-null, the month.
	 */
	void __paintDayNums(Evas_Object* month);

	/**
	 * Occupies.
	 *
	 * @param		startDate	The start date.
	 * @param		endDate		The end date.
	 * @param [in]	dateMap		The date map.
	 */
	void __occupy(const CalDateTime& startDate, const CalDateTime& endDate, std::map<int, int>& dateMap);

	/**
	 * Clears this object to its blank/initial state.
	 */
	void __clear();

	/**
	 * Gets a position.
	 *
	 * @param	date	The date.
	 *
	 * @return	The position.
	 */
	Position __getPos(const CalDateTime& date) const;

	/**
	 * Gets weekday of column.
	 *
	 * @param	j	The int to process.
	 *
	 * @return	The weekday of column.
	 */
	int __getWeekdayOfColumn(int j);

	/**
	 * Sames.
	 *
	 * @param	pos1	The first position.
	 * @param	pos2	The second position.
	 *
	 * @return	true if it succeeds, false if it fails.
	 */
	bool __same(const Position& pos1, const Position& pos2);

	/**
	 * Identify cell.
	 *
	 * @param [in]	cell		If non-null, the cell.
	 * @param [in]	position	The position.
	 */
	void __identifyCell(Evas_Object* cell, Position& position);
	CalTapRecognizer __cellTapRecognizer;
	std::function<void (int i, int j)> __tapCellCb;

private:
	static const Position __invalidPosition;
	const char* __rowEdjeGroupName;
	int __firstWeekday;
	int __year;
	int __month;
	CalDateTime __gridStartDate;
	CalMonthRowControl* __row[GRID_ROW_COUNT];
	Position __focusedPosition;
	Position __todayPosition;
	bool __isClean;
	int __eventCount[GRID_ROW_COUNT][DAYS_PER_WEEK];
	const CalDateTime* __originDate;
	Evas_Object* __header;

};

#endif
