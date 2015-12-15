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
#include "CalDateTime.h"

class CalComplexListProvider : public ICalListProvider
{
public:
	CalComplexListProvider(const CalDateTime& base, int dir, bool allDay, calendar_filter_h filter);
	virtual ~CalComplexListProvider();
public:
	virtual void prefetch(bool fillBothBuffers);
	virtual std::shared_ptr<CalSchedule> getCurrentSchedule();
	virtual const CalDateTime& getCurrentDate();
	virtual void loadNext();
	virtual bool eof();
private:
	void __deleteFinishedFromList();
	void __updateCurrentDateAndFillList();
	std::shared_ptr<CalInstanceSchedule> __getNextScheduleFromDbList(CalDateTime& date);
	void __getAdjustedDates(const CalInstanceSchedule& schedule, CalDateTime& start, CalDateTime& end) const;
	int __getDuration(const CalInstanceSchedule& schedule) const;
private:
	const CalDateTime __base;
	const int __dir;
	const int __allDay;
private:
	CalDateTime __currentDate;
	std::shared_ptr<CalInstanceSchedule> __currentSchedule;
private:
	CalRecordBlockFetcher __fetcher;
	std::shared_ptr<CalInstanceSchedule> __pending;
	CalDateTime __pendingDate;
private:
	class Occupant
	{
	public:
		Occupant(std::shared_ptr<CalInstanceSchedule>& schedule, int remainingDays);
		virtual ~Occupant();
	public:
		static bool normalOrderPredicate(const Occupant& a, const Occupant& b);
		static bool reverseOrderPredicate(const Occupant& a, const Occupant& b);
	public:
		std::shared_ptr<CalInstanceSchedule> __schedule;
		int __remainingDays;
	};
private:
	std::list<Occupant> __list;
	std::list<Occupant>::iterator __iter;
};

#endif
