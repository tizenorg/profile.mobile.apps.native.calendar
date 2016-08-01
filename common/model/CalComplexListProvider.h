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

#ifndef _CAL_COMPLEX_LIST_PROVIDER_H_
#define _CAL_COMPLEX_LIST_PROVIDER_H_

#include "ICalListProvider.h"

#include <memory>
#include <list>

#include "CalRecordBlockFetcher.h"
#include "CalInstanceSchedule.h"
#include "CalDate.h"

/**
* @class    CalComplexListProvider
*
* @brief    This class provides %CalSchedule objects in complex list model.
*
*/
class CalComplexListProvider : public ICalListProvider
{
	class Occupant
	{
	public:
		Occupant(std::shared_ptr<CalInstanceSchedule>& schedule, int remainingDays);
		virtual ~Occupant();

		static bool normalOrderPredicate(const Occupant& a, const Occupant& b);
		static bool reverseOrderPredicate(const Occupant& a, const Occupant& b);

		std::shared_ptr<CalInstanceSchedule> __schedule;
		int __remainingDays;
	};

public:
	CalComplexListProvider(const CalDate& base, int dir, bool allDay, calendar_filter_h filter);
	virtual ~CalComplexListProvider();

	/**
	 * @brief Prefetch events from calendar service.
	 *
	 * @param fillBothBuffers       prefetch current buffer or current and next
	 *
	 */
	virtual void prefetch(bool fillBothBuffers);

	/**
	 * @brief Get current event.
	 *
	 * @return current event object.
	 *
	 */
	virtual std::shared_ptr<CalSchedule> getCurrentSchedule();

	/**
	 * @brief Get date of currently provided event.
	 *
	 * @return event date.
	 *
	 */
	virtual const CalDate& getCurrentDate();

	/**
	 * @brief Prefetch continue.
	 *
	 */
	virtual void loadNext();

	/**
	 * @brief Check whether list is ended.
	 *
	 */
	virtual bool eof();

private:
	void __deleteFinishedFromList();
	void __updateCurrentDateAndFillList();
	std::shared_ptr<CalInstanceSchedule> __getNextScheduleFromDbList(CalDate& date);
	void __getAdjustedDates(const CalInstanceSchedule& schedule, CalDate& start, CalDate& end) const;
	int __getDuration(const CalInstanceSchedule& schedule) const;

	const CalDate __base;
	const int __dir;
	const int __allDay;
	CalDate __currentDate;

	std::shared_ptr<CalInstanceSchedule> __currentSchedule;
	CalRecordBlockFetcher __fetcher;

	std::shared_ptr<CalInstanceSchedule> __pending;
	CalDate __pendingDate;

	std::list<Occupant> __list;
	std::list<Occupant>::iterator __iter;

};

#endif
