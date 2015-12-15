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

#include "CalCommon.h"
#include "CalMonthControl2.h"
#include <Elementary.h>
#include <notification.h>

#include "CalDebugInternal.h"

CalMonthControl2::CalMonthControl2(int firstWeekday, int year, int month, const char* rowEdjeGroupName, const CalDateTime* originDate, CalDateTime lowerBound, CalDateTime upperBound)
	: __scrollStartCb(NULL)
	, __scrollFinishCb(NULL)
	, __rowEdjeGroupName(rowEdjeGroupName)
	, __firstWeekday(firstWeekday)
	, __previousMonth(NULL)
	, __nextMonth(NULL)
	, __previousMonthRows()
	, __nextMonthRows()
	, __slideAnimator(NULL)
	, __dragRecognizer(NULL)
	, __monthIdler(NULL)
	, __blockTap(false)
	, __lowerBound(lowerBound)
	, __upperBound(upperBound)
{
	__thisMonth = new CalMonthControl(firstWeekday, year, month, rowEdjeGroupName, originDate);
}

CalMonthControl2::~CalMonthControl2()
{
	if(__monthIdler) {
		ecore_idler_del(__monthIdler);
		__monthIdler = NULL;
	}
}

const char* CalMonthControl2::getClassName()
{
	return "CalMonthControl2";
}

/**
 * Sets tap cell cb.
 *
 * @param	tapCellCb	The tap cell cb.
 */
void CalMonthControl2::setTapCellCb(std::function<void (int i, int j)> tapCellCb)
{
	__thisMonth->setTapCellCb([this, tapCellCb](int i, int j){
		if (__blockTap)
			return;
		CalDateTime newFocusDate;
		__thisMonth->getDate(i, j, newFocusDate);
		if (newFocusDate < __lowerBound	|| newFocusDate > __upperBound ){
			WDEBUG("OutRang %s", newFocusDate.getString());
			__outRangeWarning();
			return;
		}
		tapCellCb(i, j);
	});
}

/**
 * Sets scroll cb.
 *
 * @param	scrollStartCb 	The scroll start cb.
 * @param	scrollFinishCb	The scroll finish cb.
 */
void CalMonthControl2::setScrollCb(std::function<void ()> scrollStartCb, std::function<void (int dir)> scrollFinishCb)
{
	__scrollStartCb = scrollStartCb;
	__scrollFinishCb = scrollFinishCb;
}

/**
 * Out range warning.
 */
void CalMonthControl2::__outRangeWarning()
{
	char warningText[300] = {0};
	char lowBoundText[125] = {0};
	snprintf(lowBoundText, sizeof(lowBoundText)-1, "%s", __lowerBound.getString());
	snprintf(warningText, sizeof(warningText)-1,
		_L_("IDS_CLD_TPOP_DATE_OUTSIDE_ACCEPTED_RANGE_SELECT_DATE_BETWEEN_P1SS_AND_P2SS"),
		lowBoundText, __upperBound.getString());
	notification_status_message_post(warningText);
	return;
}

/**
 * Executes the create action.
 *
 * @param [in]	parent	If non-null, the parent.
 * @param [in]	param 	If non-null, the parameter.
 *
 * @return		null	If it fails, else an Evas_Object*.
 */
Evas_Object* CalMonthControl2::onCreate(Evas_Object* parent, void* param)
{
	Evas_Object* layout = elm_layout_add(parent);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_file_set(layout, CAL_EDJE, "CalMonthControl2");
	evas_object_show(layout);

	__thisMonth->create(layout, param);
	elm_object_part_content_set(layout, "this", __thisMonth->getEvasObj());

	__slideAnimator = new CalSlideAnimator(
		layout,
		[this](int dir) {
			if (dir == 0) {
				__blockTap = false;
				if (__scrollFinishCb)
					__scrollFinishCb(0);
			} else {
				__reset(-dir);
				__slideAnimator->resetPosition();
				__blockTap = false;
				if (__scrollFinishCb)
					__scrollFinishCb(-dir);
				edje_message_signal_process();
			}
		}
	);

	__dragRecognizer = new CalDragRecognizer(
		[this](int x, int y) {
			Evas_Coord min, max;
			__getScrollRange(min, max);
			__slideAnimator->begin(CalSlideAnimator::VERTICAL_MOVE, min, max);
		},
		[this](int dx, int dy) {
			if (!__slideAnimator->isActive()) {
				return;
			}
			__slideAnimator->update(dy);
			// WARNING: doing something here will degrade animation performance!
		},
		[this](int dx, int dy, bool isValidDrag) {
			if (!isValidDrag) {
				return;
			}
			if (!__slideAnimator->isActive()) {
				return;
			}
			__blockTap = true;
			if (__scrollStartCb)
				__scrollStartCb();

			if ((dy > 0 && __isLowerBound()) || (dy <= 0 && __isUpperBound())) {
				__outRangeWarning();
				__slideAnimator->cancel();
				return;
			}

			Evas_Coord h;
			evas_object_geometry_get(__thisMonth->getEvasObj(), NULL, NULL, NULL, &h);

			if (ABS(dy) > h / 6)
				__slideAnimator->finish(dy);
			else
				__slideAnimator->cancel();
		}
	);

	__monthIdler = ecore_idler_add([](void* data) {
		CalMonthControl2* self = (CalMonthControl2*)data;

		self->__monthIdler = NULL;
		self->__previousMonth = self->__createAccessoryMonth(self->getEvasObj(), self->__previousMonthRows);
		elm_object_part_content_set(self->getEvasObj(), "previous", self->__previousMonth);

		self->__nextMonth = self->__createAccessoryMonth(self->getEvasObj(), self->__nextMonthRows);
		elm_object_part_content_set(self->getEvasObj(), "next", self->__nextMonth);

//		self->__resetDayNums(self->__previousMonthRows, -1);
//		self->__resetDayNums(self->__nextMonthRows, 1);
		int year, month;
		self->__thisMonth->getMonth(year, month);
		CalDateTime date(year, month, 1);
		self->__resetAccessoryMonths(date);
		self->__dragRecognizer->addTarget(self->getEvasObj()); // Now enable month scrolling.

		return ECORE_CALLBACK_CANCEL;

	}, this);

	WLEAVE();
	return layout;
}

/**
 * Executes the destroy action.
 */
void CalMonthControl2::onDestroy()
{
	if(__dragRecognizer){
		delete __dragRecognizer;
		__dragRecognizer = NULL;
	}

	if(__slideAnimator){
		delete __slideAnimator;
		__slideAnimator = NULL;
	}
}

/**
 * Resets this object.
 *
 * @param	firstWeekday	The first weekday.
 * @param	year			The year.
 * @param	month			The month.
 */
void CalMonthControl2::reset(int firstWeekday, int year, int month)
{
	__firstWeekday = firstWeekday;
	__thisMonth->reset(firstWeekday, year, month);
	if (__monthIdler == NULL) {
		WASSERT(__previousMonth);
		WASSERT(__nextMonth);
		__resetAccessoryMonths(CalDateTime(year, month, 1));
	}
}

/**
 * Focus.
 *
 * @param	mday	The mday.
 */
void CalMonthControl2::focus(int mday)
{
	__thisMonth->focus(mday);
}

/**
 * Erase focus.
 */
void CalMonthControl2::eraseFocus()
{
	__thisMonth->eraseFocus();
}

/**
 * Updates this object.
 */
void CalMonthControl2::update()
{
	__thisMonth->updateHeaderText();
}

/**
 * Loads this object.
 */
void CalMonthControl2::load()
{
	__thisMonth->load();
}

/*only called by custom date picker
   The date outsie bound will be set as "dim" status
*/
/**
 * Resets the by bound.
 */
void CalMonthControl2::resetByBound()
{
	__thisMonth->resetByBound(__lowerBound, __upperBound);
}

/**
 * Gets event count.
 *
 * @param	date	The date.
 *
 * @return	The event count.
 */
int CalMonthControl2::getEventCount(const CalDateTime& date) const
{
	return __thisMonth->getEventCount(date);
}

/**
 * Gets focused row.
 *
 * @return	The focused row.
 */
int CalMonthControl2::getFocusedRow()
{
	return __thisMonth->getFocusedRow();
}

/**
 * Gets a date.
 *
 * @param		i		Zero-based index of the.
 * @param		j		The int to process.
 * @param [in]	date	The date.
 */
void CalMonthControl2::getDate(int i, int j, CalDateTime& date)
{
	__thisMonth->getDate(i, j, date);
}

/**
 * Creates a header.
 *
 * @param [in]	parent	If non-null, the parent.
 *
 * @return		null	If it fails, else the new header.
 */
Evas_Object* CalMonthControl2::createHeader(Evas_Object* parent)
{
	return __thisMonth->createHeader(parent);
}

/**
 * Sets header focus.
 *
 * @param [in]	header	If non-null, the header.
 */
void CalMonthControl2::setHeaderFocus(Evas_Object* header)
{
	if (header == NULL)
		return;
	__thisMonth->setHeaderFocus(header);
}

/**
 * Gets a month.
 *
 * @param [in]	year 	The year.
 * @param [in]	month	The month.
 */
void CalMonthControl2::getMonth(int& year, int& month)
{
	__thisMonth->getMonth(year, month);
}

/**
 * Gets row position.
 *
 * @param		offset	The offset.
 * @param [in]	x		If non-null, the Evas_Coord to process.
 * @param [in]	y		If non-null, the Evas_Coord to process.
 * @param [in]	w		If non-null, the Evas_Coord to process.
 * @param [in]	h		If non-null, the Evas_Coord to process.
 */
void CalMonthControl2::getRowPosition(int offset, Evas_Coord* x, Evas_Coord* y, Evas_Coord* w, Evas_Coord* h) const
{
	const CalMonthRowControl& row = __getRow(offset);
	evas_object_geometry_get(row.getConstEvasObj(), x, y, w, h);
}

/**
 * Slides.
 *
 * @param	dir	The dir.
 *
 * @return	true if it succeeds, false if it fails.
 */
bool CalMonthControl2::slide(int dir)
{
	if (dir < 0 && __isLowerBound())
		return false;

	if (dir > 0 && __isUpperBound())
		return false;

	Evas_Coord min, max;
	__getScrollRange(min, max);

	__blockTap = true;
	if (__scrollStartCb)
		__scrollStartCb();

	__slideAnimator->begin(CalSlideAnimator::VERTICAL_MOVE, min, max);
	__slideAnimator->finish(-dir);

	return true;
}

/**
 * Freeze scroll.
 *
 * @param	freeze	true to freeze.
 */
void CalMonthControl2::freezeScroll(bool freeze)
{
	if (freeze)
		__dragRecognizer->block();
	else
		__dragRecognizer->unblock();
}

/**
 * Gets the previous month row offset.
 *
 * @return	The previous month row offset.
 */
int CalMonthControl2::__getPreviousMonthRowOffset()
{
	int year, month;
	__thisMonth->getMonth(year, month);

	CalDateTime date(year, month, 1);
	CalDateTime date2(date);
	date2.decrementMonth();

	date.setToMonthGridStart(__firstWeekday);
	date2.setToMonthGridStart(__firstWeekday);

	const int rowOffset = CalDateTime::getDayDiff(date2, date) / DAYS_PER_WEEK;
	WDEBUG("%s %s %d", date.dump().c_str(), date2.dump().c_str(), rowOffset);

	return rowOffset;
}

/**
 * Gets the next month row offset.
 *
 * @return	The next month row offset.
 */
int CalMonthControl2::__getNextMonthRowOffset()
{
	int year, month;
	__thisMonth->getMonth(year, month);

	CalDateTime date(year, month, 1);
	CalDateTime date2(date);
	date2.incrementMonth();

	date.setToMonthGridStart(__firstWeekday);
	date2.setToMonthGridStart(__firstWeekday);

	const int rowOffset = CalDateTime::getDayDiff(date2, date) / DAYS_PER_WEEK;
	WDEBUG("%s %s %d", date.dump().c_str(), date2.dump().c_str(), rowOffset);

	return rowOffset;
}

/**
 * Gets a row.
 *
 * @param	offset	The offset.
 *
 * @return	The row.
 */
const CalMonthRowControl& CalMonthControl2::__getRow(int offset) const
{
	WASSERT(-GRID_ROW_COUNT <= offset && offset < GRID_ROW_COUNT * 2)
	if (offset < 0)
		return *__previousMonthRows[GRID_ROW_COUNT + offset];
	else if (offset < GRID_ROW_COUNT)
		return __thisMonth->getRow(offset);
	else
		return *__nextMonthRows[offset - GRID_ROW_COUNT];
}

/**
 * Fade in.
 *
 * @param	startRowOffset	The start row offset.
 */
void CalMonthControl2::__fadeIn(int startRowOffset)
{
	for (int i = 0; i < GRID_ROW_COUNT; i++) {
		((CalMonthRowControl&)__getRow(startRowOffset + i)).resetDayNumStyle(i);
	}
}

/**
 * Creates accessory month.
 *
 * @param [in]	parent		If non-null, the parent.
 * @param [in]	rowArray	If non-null, array of rows.
 *
 * @return		null		If it fails, else the new accessory month.
 */
Evas_Object* CalMonthControl2::__createAccessoryMonth(Evas_Object* parent, CalMonthRowControl* rowArray[])
{
	Evas_Object* layout = elm_layout_add(parent);
	elm_layout_file_set(layout, CAL_EDJE, "CalMonthControl");
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(layout);

	for (int i = 0; i < GRID_ROW_COUNT; i++) {
		char groupName[100];
		if (__rowEdjeGroupName)
			sprintf(groupName, "%s.accessory", __rowEdjeGroupName);
		else
			strcpy(groupName, "CalMonthRowControl.accessory");
		rowArray[i] = new CalMonthRowControl(groupName);
		rowArray[i]->create(layout, NULL);

		char partname[100];
		sprintf(partname, "row/%d/sw", i);
		WDEBUG("%s", partname);
		elm_object_part_content_set(layout, partname, rowArray[i]->getEvasObj());
	}

	return layout;
}

/**
 * Resets the given dir.
 *
 * @param	dir	The dir.
 */
void CalMonthControl2::__reset(int dir)
{
	WDEBUG("%d", dir);

	int year, month;
	__thisMonth->getMonth(year, month);
	CalDateTime date(year, month, 1);

	if (dir > 0)
		date.incrementMonth();
	else
		date.decrementMonth();

	__thisMonth->reset(__firstWeekday, date.getYear(), date.getMonth());

	__resetAccessoryMonths(date);
}

/**
 * Resets the accessory months described by date.
 *
 * @param	date	The date.
 */
void CalMonthControl2::__resetAccessoryMonths(const CalDateTime& date)
{
	if (__isLowerBound()) {
		WDEBUG("Lower-bound reached");
		evas_object_hide(__previousMonth);
	} else {
		__resetDayNums(__previousMonthRows, -1);
		evas_object_show(__previousMonth);
	}

	if (__isUpperBound()) {
		WDEBUG("Upper-bound reached");
		evas_object_hide(__nextMonth);
	} else {
		__resetDayNums(__nextMonthRows, 1);
		evas_object_show(__nextMonth);
	}
}

/**
 * Resets the day nums.
 *
 * @param [in]	rowArray	If non-null, array of rows.
 * @param		dir			The dir.
 */
void CalMonthControl2::__resetDayNums(CalMonthRowControl* rowArray[], int dir)
{
	const int rowOffset = dir > 0 ? GRID_ROW_COUNT : -GRID_ROW_COUNT;

	CalDateTime rowStartDate;
	__thisMonth->getDate(0, 0, rowStartDate);
	rowStartDate.addDays(rowOffset * DAYS_PER_WEEK);

	for (int i = 0; i < GRID_ROW_COUNT; i++) {
		rowArray[i]->resetDayNums(__firstWeekday, rowStartDate, rowOffset + i);
		rowStartDate.addDays(DAYS_PER_WEEK);
	}
}

/**
 * Determines if we can is upper bound.
 *
 * @return	true if upper bound, false if not.
 */
bool CalMonthControl2::__isUpperBound()
{
	int year, month;
	__thisMonth->getMonth(year, month);
	CalDateTime date(year, month, 1);

	if (CalDateTime::compareMonth(__upperBound, date) <= 0)
		return true;

	return false;
}

/**
 * Determines if we can is lower bound.
 *
 * @return	true if lower bound, false if not.
 */
bool CalMonthControl2::__isLowerBound()
{
	int year, month;
	__thisMonth->getMonth(year, month);
	CalDateTime date(year, month, 1);

	if (CalDateTime::compareMonth(date, __lowerBound) <= 0)
		return true;

	return false;
}

/**
 * Finds the getscrollrange of the given arguments.
 *
 * @param [in]	min	The minimum.
 * @param [in]	max	The maximum.
 */
void CalMonthControl2::__getScrollRange(Evas_Coord& min, Evas_Coord& max)
{
	Evas_Coord h;
	evas_object_geometry_get(__thisMonth->getEvasObj(), NULL, NULL, NULL, &h);

	if (__isUpperBound()) // upper boundary - sliding up should be blocked
		min = 0;
	else
		min = -__getNextMonthRowOffset() * h / GRID_ROW_COUNT;

	if (__isLowerBound()) // lower boundary - sliding down should be blocked
		max = 0;
	else
		max = -__getPreviousMonthRowOffset() * h / GRID_ROW_COUNT;

	WDEBUG("min(%s%d) max(%s%d)", min > 0 ? "+" : "", min, max > 0 ? "+" : "", max);
}
