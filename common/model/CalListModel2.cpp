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

#include "CalListModel2.h"

#include "CalCommon.h"

#include "CalOriginalSchedule.h"

CalListModel2::CalListModel2(const CalDateTime& base, int dir) :
	CalListModel(base, dir),
	__fresh(true),
	__currentMonth(base),
	__currentMonthItemReturned(false),
	__currentSchedule(nullptr),
	__dayChanged(false),
	__enableNoEvent(false)
{
	if (dir < 0)
		__currentMonth.decrementMonth(); // to prevent returning No events for the base month
}

CalListModel2::~CalListModel2()
{
}

//void CalListModel2::prefetch(bool fillBothBuffers)
//{
//	CalListModel::prefetch(fillBothBuffers);
//}

std::shared_ptr<CalSchedule> CalListModel2::getNext(bool& dayChanged)
{
	if (__fresh) {
		__fresh = false;
		__currentSchedule = CalListModel::getNext(__dayChanged);
	}

	WDEBUG("%s %s", __currentMonth.dump().c_str(), CalListModel::getCurrentDate().dump().c_str());

	if (__dir > 0) {

		if (CalDateTime::compareMonth(__currentMonth, CalListModel::getCurrentDate()) < 0 && __currentMonthItemReturned) {
			__currentMonth.incrementMonth();
			__currentMonthItemReturned = false;
		}

		if (__enableNoEvent && CalDateTime::compareMonth(__currentMonth, CalListModel::getCurrentDate()) < 0 && !__currentMonthItemReturned) {
			auto noEvent = std::make_shared<CalOriginalSchedule>(__currentMonth.getYear(), __currentMonth.getMonth());
			dayChanged = true;
			WDEBUG("%s %s", __currentMonth.dump().c_str(), CalListModel::getCurrentDate().dump().c_str());
			__currentMonthItemReturned = true;
			return noEvent;
		} else {
			auto event = __currentSchedule;
			dayChanged = __dayChanged;
			__currentMonth = CalListModel::getCurrentDate();
			__currentSchedule = CalListModel::getNext(__dayChanged);
			WDEBUG("%s %s", __currentMonth.dump().c_str(), CalListModel::getCurrentDate().dump().c_str());
			__currentMonthItemReturned = true;
			return event;
		}

	} else {

		if (CalDateTime::compareMonth(CalListModel::getCurrentDate(), __currentMonth) < 0 && __currentMonthItemReturned) {
			__currentMonth.decrementMonth();
			__currentMonthItemReturned = false;
		}

		if (__enableNoEvent && CalDateTime::compareMonth(CalListModel::getCurrentDate(), __currentMonth) < 0 && !__currentMonthItemReturned) {
			auto noEvent = std::make_shared<CalOriginalSchedule>(__currentMonth.getYear(), __currentMonth.getMonth());
			dayChanged = true;
			WDEBUG("%s %s", __currentMonth.dump().c_str(), CalListModel::getCurrentDate().dump().c_str());
			__currentMonthItemReturned = true;
			return noEvent;
		} else {
			auto event = __currentSchedule;
			dayChanged = __dayChanged;
			__currentMonth = CalListModel::getCurrentDate();
			__currentSchedule = CalListModel::getNext(__dayChanged);
			WDEBUG("%s %s", __currentMonth.dump().c_str(), CalListModel::getCurrentDate().dump().c_str());
			__currentMonthItemReturned = true;
			return event;
		}

	}
}

const CalDateTime& CalListModel2::getCurrentDate()
{
	return __currentMonth;
}

bool CalListModel2::eof()
{
	return !__currentSchedule && CalListModel::eof();
}
