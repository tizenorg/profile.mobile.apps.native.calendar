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

#include "CalMonthControl.h"

#include "CalCommon.h"

#include <Elementary.h>

#include "CalListModelFactory.h"

#include "CalDebugInternal.h"

const CalMonthControl::Position CalMonthControl::__invalidPosition = {-1, -1};

CalMonthControl::CalMonthControl(int firstWeekday, int year, int month, const char* rowEdjeGroupName, const CalDateTime* originDate) :
	__cellTapRecognizer([this](Evas_Object* obj) {
		if (!__tapCellCb)
			return;
		Position pos;
		__identifyCell(obj, pos);
		__tapCellCb(pos.i, pos.j);
	}),
	__tapCellCb(NULL),
	__rowEdjeGroupName(rowEdjeGroupName),
	__firstWeekday(firstWeekday),
	__year(year),
	__month(month),
	__gridStartDate(year, month, 1),
	__focusedPosition(__invalidPosition),
	__todayPosition(__invalidPosition),
	__isClean(true),
	__originDate(originDate)
{

	memset( __row, 0, sizeof(__row) );

	for( int i=0; i <GRID_ROW_COUNT; i++ )
		for( int j=0; j < DAYS_PER_WEEK; j++ )
			__eventCount[i][j] = 0;

	__gridStartDate.setToMonthGridStart(firstWeekday);
}

CalMonthControl::~CalMonthControl()
{
}

const char* CalMonthControl::getClassName()
{
	return "CalMonthControl";
}

/**
 * Sets tap cell cb.
 *
 * @param	tapCellCb	The tap cell cb.
 */
void CalMonthControl::setTapCellCb(std::function<void (int i, int j)> tapCellCb)
{
	__tapCellCb = tapCellCb;
}

/**
 * Signals.
 *
 * @param [in]	obj		If non-null, the object.
 * @param		signal	The signal.
 * @param		j		Index of sygnal to process.
 */
static void __signal(Evas_Object* obj, const char* signal, int j);

/**
 * Executes the create action.
 *
 * @param [in]	parent	If non-null, the parent.
 * @param [in]	param 	If non-null, the parameter.
 *
 * @return		null if it fails, else an Evas_Object*.
 */
Evas_Object* CalMonthControl::onCreate(Evas_Object* parent, void* param)
{
	Evas_Object* layout = elm_layout_add(parent);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_file_set(layout, CAL_EDJE, "CalMonthControl");
	evas_object_show(layout);

	for (int i = 0; i < GRID_ROW_COUNT; i++) {
		if (__rowEdjeGroupName)
			__row[i] = new CalMonthRowControl(__rowEdjeGroupName);
		else
			__row[i] = new CalMonthRowControl("CalMonthRowControl");
		__row[i]->create(layout, NULL);

		char partname[100];
		sprintf(partname, "row/%d/sw", i);
		elm_object_part_content_set(layout, partname, __row[i]->getEvasObj());

		__row[i]->receiveTouchInputFor(__cellTapRecognizer);
	}

	__updateTodayPos();
	__paintDayNums(layout);

	return layout;
}

/**
 * Executes the destroy action.
 */
void CalMonthControl::onDestroy()
{

}

/**
 * Resets this object.
 *
 * @param	firstWeekday	The first weekday.
 * @param	year			The year.
 * @param	month			The month.
 */
void CalMonthControl::reset(int firstWeekday, int year, int month)
{
	WDEBUG("%d/%d", year, month);

	__clear();

	if (__focusedPosition.i >= 0)
		__row[__focusedPosition.i]->eraseFocus(__focusedPosition.i, __focusedPosition.j, false, __eventCount[__focusedPosition.i][__focusedPosition.j]);

	__firstWeekday = firstWeekday;
	__year = year;
	__month = month;
	__gridStartDate.setToMonthGridStart(firstWeekday, __year, __month);
	__focusedPosition = __invalidPosition;

	if (__todayPosition.i >= 0) {
		WDEBUG("%d, %d", __todayPosition.i, __todayPosition.j);
		__row[__todayPosition.i]->resetCircle(__todayPosition.j);
	}

	__updateTodayPos();
	__paintDayNums(getEvasObj());
}

/**
 * Focus.
 *
 * @param	mday	The mday.
 */
void CalMonthControl::focus(int mday)
{
	eraseFocus();

	CalDateTime focusedDate(__year, __month, mday);
	WDEBUG("%s", focusedDate.dump().c_str());
	__focusedPosition = __getPos(focusedDate);

	__row[__focusedPosition.i]->paintFocus(__focusedPosition.j, __eventCount[__focusedPosition.i][__focusedPosition.j]);
}

/**
 * Erase focus.
 *
 * @param	animated	true if animated.
 */
void CalMonthControl::eraseFocus(bool animated)
{
	if (__focusedPosition.i >= 0) {
		const bool today = __focusedPosition.i == __todayPosition.i && __focusedPosition.j == __todayPosition.j;
		__row[__focusedPosition.i]->eraseFocus(__focusedPosition.i, __focusedPosition.j, today, animated, __eventCount[__focusedPosition.i][__focusedPosition.j]);
	}
}

/**
 * Loads this object.
 */
void CalMonthControl::load()
{
	__clear();

	CalDateTime gridEndDate(__gridStartDate);
	gridEndDate.addDays(GRID_ROW_COUNT * DAYS_PER_WEEK);
	WDEBUG("%s", gridEndDate.dump().c_str());

	int thisMonthStartOffset = 0;
	int thisMonthEndOffset = 0;

	CalDateTime thisMonth(__year, __month, 1);
	thisMonthStartOffset = CalDateTime::getDayDiff(thisMonth, __gridStartDate);
	
	thisMonth.incrementMonth();
	thisMonthEndOffset = CalDateTime::getDayDiff(thisMonth, __gridStartDate) - 1;
	
	ICalListModel* model = CalListModelFactory::getInstance().getRangeList(__gridStartDate, gridEndDate);
	model->prefetch(false);
	bool dayChanged = false;
	auto schedule = model->getNext(dayChanged);
	std::map<int, int> offsetCache;
	while (schedule) {
		CalDateTime start, end;
		schedule->getStart(start);
		schedule->getEnd(end, true);
		__occupy(CalDateTime(start.getYear(), start.getMonth(), start.getMday()),
			     CalDateTime(end.getYear(), end.getMonth(), end.getMday()),
			     offsetCache);
		schedule = model->getNext(dayChanged);
	}

	for (int offset = 0; offset < GRID_ROW_COUNT * DAYS_PER_WEEK; offset++) {
		const Position pos = { offset / DAYS_PER_WEEK, offset % DAYS_PER_WEEK };
		if (__eventCount[pos.i][pos.j] > 0) {
			const bool thisMonth = (thisMonthStartOffset <= offset && offset <= thisMonthEndOffset);
			__row[pos.i]->dot(pos.j, thisMonth, __same(pos, __focusedPosition));
		}
	}

	delete model;
}

/**
 * Gets event count.
 *
 * @param	date	The date.
 *
 * @return	The event count.
 */
int CalMonthControl::getEventCount(const CalDateTime& date) const
{
	Position pos = __getPos(date);
	return __eventCount[pos.i][pos.j];
}

/**
 * Gets focused row.
 *
 * @return	The focused row.
 */
int CalMonthControl::getFocusedRow()
{
	return __focusedPosition.i;
}

/**
 * Updates the today position.
 */
void CalMonthControl::__updateTodayPos()
{
	const CalDateTime& today = __originDate ? *__originDate : CalDateTime(CalDateTime::INIT_TODAY);
	if (__year == today.getYear() && __month == today.getMonth())
		__todayPosition = __getPos(today);
	else
		__todayPosition = __invalidPosition;
	WDEBUG("%d, %d", __todayPosition.i, __todayPosition.j);
}

/**
 * Paints the day nums described by month.
 *
 * @param [in]	month	If non-null, the month.
 */
void CalMonthControl::__paintDayNums(Evas_Object* month)
{
	CalDateTime date(__gridStartDate);
	for (int i = 0; i < GRID_ROW_COUNT; i++) {
		__row[i]->resetDayNums(__firstWeekday, date, i);
		if (i == __todayPosition.i)
			__row[i]->markToday(__todayPosition.j);
		date.addDays(DAYS_PER_WEEK);
	}
}

/**
 * Searches for the first from offset cache.
 *
 * @param [in]	offsetCache		The offset cache.
 * @param		value			The value.
 *
 * @return		The found from offset cache.
 */
static int __findFromOffsetCache(std::map<int, int>& offsetCache, int value)
{
	auto finder = offsetCache.find(value);
	if (finder == offsetCache.end())
		return -1;
	WHIT();
	return finder->second;
}

/**
 * Adds to the offset cache.
 *
 * @param [in]	offsetCache		The offset cache.
 * @param		value			The value.
 * @param		offset			The offset.
 */
static void __addToOffsetCache(std::map<int, int>& offsetCache, int value, int offset)
{
	offsetCache.insert(std::pair<int, int>(value, offset));
}

/**
 * Occupies.
 *
 * @param			startDate		The start date.
 * @param			endDate			The end date.
 * @param [in,out]	offsetCache		The offset cache.
 */
void CalMonthControl::__occupy(const CalDateTime& startDate, const CalDateTime& endDate, std::map<int, int>& offsetCache)
{
	int startDateCompareVal = startDate.getDateCompareVal();
	int startOffset = __findFromOffsetCache(offsetCache, startDateCompareVal);
	if (startOffset < 0) {
		startOffset = CalDateTime::getDayDiff(startDate, __gridStartDate);
		__addToOffsetCache(offsetCache, startDateCompareVal, startOffset);
	}
	int endDateCompareVal = endDate.getDateCompareVal();
	int endOffset = __findFromOffsetCache(offsetCache, endDateCompareVal);
	if (endOffset < 0) {
		endOffset = CalDateTime::getDayDiff(endDate, __gridStartDate);
		__addToOffsetCache(offsetCache, endDateCompareVal, endOffset);
	}

	if (startOffset < 0)
		startOffset = 0;
	if (endOffset >= GRID_ROW_COUNT * DAYS_PER_WEEK)
		endOffset = GRID_ROW_COUNT * DAYS_PER_WEEK - 1;
	
	for (int offset = startOffset; offset <= endOffset; offset++) {
		const Position pos = { offset / DAYS_PER_WEEK, offset % DAYS_PER_WEEK };
		__eventCount[pos.i][pos.j]++;
	}

	__isClean = false;
}

/**
 * Clears this object to its blank/initial state.
 */
void CalMonthControl::__clear()
{
	if (__isClean)
		return;

	for (int i = 0; i < GRID_ROW_COUNT; i++) {
		__row[i]->clear();
	}

	__isClean = true;
	memset(__eventCount, 0, sizeof(__eventCount));
}

/**
 * Resets the by bound.
 *
 * @param	lowerBound	The lower bound.
 * @param	upperBound	The upper bound.
 */
void CalMonthControl::resetByBound(const CalDateTime& lowerBound, const CalDateTime& upperBound)
{
	CalDateTime date(__gridStartDate);
	for (int i = 0; i < GRID_ROW_COUNT; i++) {
		__row[i]->resetDayByBound(date, lowerBound, upperBound);
		date.addDays(DAYS_PER_WEEK);
	}
}

/**
 * Gets a position.
 *
 * @param	date	The date.
 *
 * @return	The position.
 */
CalMonthControl::Position CalMonthControl::__getPos(const CalDateTime& date) const
{
	WDEBUG("%s", __gridStartDate.dump().c_str());
	const int dayDiff = CalDateTime::getDayDiff(date, __gridStartDate);
	WDEBUG("%s - %s = %d days?", date.dump().c_str(), __gridStartDate.dump().c_str(), dayDiff);
	return { dayDiff / DAYS_PER_WEEK, dayDiff % DAYS_PER_WEEK };
}

/**
 * Identify cell.
 *
 * @param [in]	cell		If non-null, the cell.
 * @param [in]	position	The position.
 */
void CalMonthControl::__identifyCell(Evas_Object* cell, Position& position)
{
	for (int i = 0; i < GRID_ROW_COUNT; i++) {
		int j = __row[i]->identifyCell(cell);
		if (j >= 0) {
			position = {i, j};
			return;
		}
	}
	position = __invalidPosition;
}

/**
 * Signals.
 *
 * @param [in]	obj		If non-null, the object.
 * @param		signal	The signal.
 * @param		j		Index of sygnal to process.
 */
static void __signal(Evas_Object* obj, const char* signal, int j)
{
	char buffer[100];
	sprintf(buffer, signal, j);
	elm_layout_signal_emit(obj, buffer, "");
}

/**
 * Gets a date.
 *
 * @param				i		Number of weeks.
 * @param				j		Number of days.
 * @param	[in, out]	date	The date.
 */
void CalMonthControl::getDate(int i, int j, CalDateTime& date)
{
	date = __gridStartDate;
	date.addDays(i * DAYS_PER_WEEK + j);
}

/**
 * Check boundary.
 *
 * @return	An int.
 */
int CalMonthControl::checkBoundary()
{
	CalDateTime lowerBound(CalDateTime::INIT_LOWER_BOUND);
	if (CalDateTime::compareMonth(__year, __month, lowerBound.getYear(), lowerBound.getMonth()) <= 0)
		return -1;

	CalDateTime upperBound(CalDateTime::INIT_UPPER_BOUND);
	if (CalDateTime::compareMonth(upperBound.getYear(), upperBound.getMonth(), __year, __month) <= 0)
		return 1;

	return 0;
}

/**
 * Creates a header.
 *
 * @param [in]	parent	If non-null, the parent.
 *
 * @return	null if it fails, else the new header.
 */
Evas_Object* CalMonthControl::createHeader(Evas_Object* parent)
{
	__header = elm_layout_add(parent);
	evas_object_size_hint_weight_set(__header, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_file_set(__header, CAL_EDJE, "CalMonthControl.Header");
	updateHeaderText();
	return __header;
}

/**
 * Updates the header text.
 */
void CalMonthControl::updateHeaderText()
{
	WENTER();
	for (int j = 0; j < DAYS_PER_WEEK; j++) {
		char partname[100];
		sprintf(partname, "cell/%d/weekday", j);
		elm_object_part_text_set(__header, partname, CalDateTime::getWeekdayText(__getWeekdayOfColumn(j)));
	}
}

/**
 * Sets header focus.
 *
 * @param [in]	header	If non-null, the header.
 */
void CalMonthControl::setHeaderFocus(Evas_Object* header)
{
	for (int j = 0; j < DAYS_PER_WEEK; j++) {
		__signal(header, j == __focusedPosition.j ? "focus/%d" : "reset/%d", j);
	}
}

/**
 * Gets a month.
 *
 * @param [in]	year 	The year.
 * @param [in]	month	The month.
 */
void CalMonthControl::getMonth(int& year, int& month)
{
	year = __year;
	month = __month;
}

/**
 * Gets row position.
 *
 * @param			offset	The offset.
 * @param [in,out]	x		If non-null, the Evas_Coord to process.
 * @param [in,out]	y		If non-null, the Evas_Coord to process.
 * @param [in,out]	w		If non-null, the Evas_Coord to process.
 * @param [in,out]	h		If non-null, the Evas_Coord to process.
 */
void CalMonthControl::getRowPosition(int offset, Evas_Coord* x, Evas_Coord* y, Evas_Coord* w, Evas_Coord* h) const
{
	WASSERT(0 <= offset && offset < GRID_ROW_COUNT);
	evas_object_geometry_get(__row[offset]->getEvasObj(), x, y, w, h);
}

/**
 * Gets a row.
 *
 * @param	index	Zero-based index of the.
 *
 * @return	The row.
 */
CalMonthRowControl& CalMonthControl::getRow(int index)
{
	return *__row[index];
}

/**
 * Fade out.
 */
void CalMonthControl::fadeOut()
{
	eraseFocus();
	for (int i = 0; i < GRID_ROW_COUNT; i++) {
		__row[i]->resetDayNumStyle(-1);
	}
}

/**
 * Gets weekday of column.
 *
 * @param	j	The int to process.
 *
 * @return	The weekday of column.
 */
int CalMonthControl::__getWeekdayOfColumn(int j)
{
	return __firstWeekday + j > 6 ? __firstWeekday + j - 7 : __firstWeekday + j;
}

/**
 * Sames.
 *
 * @param	pos1	The first position.
 * @param	pos2	The second position.
 *
 * @return	true if it succeeds, false if it fails.
 */
bool CalMonthControl::__same(const CalMonthControl::Position& pos1, const CalMonthControl::Position& pos2)
{
	return pos1.i == pos2.i && pos1.j == pos2.j;
}
