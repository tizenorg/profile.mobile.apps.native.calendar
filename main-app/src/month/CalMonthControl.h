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
#include "CalDate.h"

#include "CalMonthRowControl.h"

#define GRID_ROW_COUNT 6
#define DAYS_PER_WEEK 7

/**
 * @brief Calendar month control.
 *
 * @see CalControl.
 */
class CalMonthControl : public CalControl
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
	 *
	 * @see CalDate
	 */
	CalMonthControl(int firstWeekday, int year, int month, const char* rowEdjeGroupName = NULL, const CalDate* originDate = NULL);

	virtual ~CalMonthControl();

	/**
	 * @brief Get class name.
	 *
	 * return Class name.
	 */
	virtual const char* getClassName();

	/**
	 * @brief Set tap cell cb.
	 *
	 * @param[in]   tapCellCb   The tap cell cb.
	 */
	void setTapCellCb(std::function<void (int i, int j)> tapCellCb);

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
	 *
	 * @param[in]   animated   True if animated.
	 */
	void eraseFocus(bool animated = false);

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
	 *
	 * @param[in]   lowerBound   The lower bound.
	 * @param[in]   upperBound   The upper bound.
	 */
	void resetByBound(const CalDate& lowerBound, const CalDate& upperBound);

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
	 * @brief Check boundary.
	 *
	 * @return -1 if lower bound, 1 if upper bound, otherwise 0.
	 */
	int checkBoundary();

	/**
	 * @brief Create a header.
	 *
	 * @param[in]   parent   If non-null, the parent.
	 *
	 * @return Null if it fails, else the new header.
	 */
	Evas_Object* createHeader(Evas_Object* parent);

	/**
	 * @brief Update the header text.
	 */
	void updateHeaderText();

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
	 * @param[in]       offset   The offset.
	 * @param[in,out]   x       If non-null, the Evas_Coord to process.
	 * @param[in,out]   y       If non-null, the Evas_Coord to process.
	 * @param[in,out]   w       If non-null, the Evas_Coord to process.
	 * @param[in,out]   h       If non-null, the Evas_Coord to process.
	 */
	void getRowPosition(int offset, Evas_Coord* x, Evas_Coord* y, Evas_Coord* w, Evas_Coord* h) const;

	/**
	 * @brief Get a row.
	 *
	 * @param[in]   index   Zero-based index of the.
	 *
	 * @return The row.
	 */
	CalMonthRowControl& getRow(int index);

	/**
	 * @brief Fade out.
	 */
	void fadeOut();

private:
	struct Position {
		int i;
		int j;
	};

	/**
	 * @brief Execute the create action.
	 *
	 * @param[in]   parent   If non-null, the parent.
	 * @param[in]   param    If non-null, the parameter.
	 *
	 * @return Null If it fails, else an Evas_Object*.
	 */
	virtual Evas_Object* onCreate(Evas_Object* parent, void* param);

	/**
	 * @brief Execute the destroy action.
	 */
	virtual void onDestroy();

	WDISABLE_COPY_AND_ASSIGN(CalMonthControl);

	/**
	 * @brief Update the today position.
	 */
	void __updateTodayPos();

	/**
	 * @brief Paint the day nums described by month.
	 *
	 * @param[in]   month   If non-null, the month.
	 */
	void __paintDayNums(Evas_Object* month);

	/**
	 * @brief Occupie.
	 *
	 * @param[in]   startDate   The start date.
	 * @param[in]   endDate     The end date.
	 * @param[in]   dateMap     The date map.
	 */
	void __occupy(const CalDate& startDate, const CalDate& endDate, std::map<int, int>& dateMap);

	/**
	 * @brief Clear this object to its blank/initial state.
	 */
	void __clear();

	/**
	 * @brief Get a position.
	 *
	 * @param[in]   date   The date.
	 *
	 * @return The position.
	 */
	Position __getPos(const CalDate& date) const;

	/**
	 * @brief Get weekday of column.
	 *
	 * @param[in]   j   The int to process.
	 *
	 * @return The weekday of column.
	 */
	int __getWeekdayOfColumn(int j);

	/**
	 * @brief Same.
	 *
	 * @param[in]   pos1   The first position.
	 * @param[in]   pos2   The second position.
	 *
	 * @return True if it succeeds, false if it fails.
	 */
	bool __same(const Position& pos1, const Position& pos2);

	/**
	 * @brief Identify cell.
	 *
	 * @param[in]   cell       If non-null, the cell.
	 * @param[in]   position   The position.
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
	CalDate __gridStartDate;
	CalMonthRowControl* __row[GRID_ROW_COUNT];
	Position __focusedPosition;
	Position __todayPosition;
	bool __isClean;
	int __eventCount[GRID_ROW_COUNT][DAYS_PER_WEEK];
	const CalDate* __originDate;
	Evas_Object* __header;

};

#endif
