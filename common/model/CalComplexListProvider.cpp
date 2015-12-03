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

#include "CalComplexListProvider.h"

CalComplexListProvider::CalComplexListProvider(const CalDate& base, int dir, bool allDay, calendar_filter_h filter) :
	__base(base),
	__dir(dir),
	__allDay(allDay),
	__fetcher(CalInstanceSchedule::createComplexListQuery(base, dir, allDay, filter))
{
	WASSERT(__currentSchedule == nullptr);
}

CalComplexListProvider::~CalComplexListProvider()
{

}

void CalComplexListProvider::prefetch(bool fillBothBuffers)
{
	WDEBUG("[%d%d]", __dir, __allDay);
	__fetcher.prefetch(fillBothBuffers);
	if (!__currentSchedule)
	{
		loadNext();
	}
}

std::shared_ptr<CalSchedule> CalComplexListProvider::getCurrentSchedule()
{
	return __currentSchedule;
}

const CalDate& CalComplexListProvider::getCurrentDate()
{
	return __currentDate;
}

void CalComplexListProvider::loadNext()
{
	if (__list.size() == 0 || __iter == __list.end())
	{
		__deleteFinishedFromList();
		__updateCurrentDateAndFillList();
		if (__list.size() == 0)
		{
			__currentSchedule = NULL;
			return;
		}
		__list.sort(__dir > 0 ? Occupant::normalOrderPredicate : Occupant::reverseOrderPredicate);
		__iter = __list.begin();
	}

	__currentSchedule =  (*__iter).__schedule;
	(*__iter).__remainingDays--;
	++__iter;
}

void CalComplexListProvider::__deleteFinishedFromList()
{
	for (auto it = __list.begin(); it != __list.end();)
	{
		WASSERT((*it).__remainingDays >= 0);
		if ((*it).__remainingDays == 0)
		{
			it = __list.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void CalComplexListProvider::__updateCurrentDateAndFillList()
{
	WDEBUG("[%d%d] %s %d", __dir, __allDay, __currentDate.dump().c_str(), __list.size());

	if (!__pending)
	{
		__pending = __getNextScheduleFromDbList(__pendingDate);
	}

	if (__list.size() > 0 || !__pending)
	{
		__currentDate.addDays(__dir > 0 ? 1 : -1);
	}
	else
	{
		__currentDate = __pendingDate;
	}

	while (__pending && __pendingDate == __currentDate)
	{
		const int duration = __getDuration(*__pending);
		WASSERT(duration >= 0);
		WDEBUG("%d", duration);
		if (duration > 0)
		{
			__list.push_back(Occupant(__pending, duration));
		}
		__pending = __getNextScheduleFromDbList(__pendingDate);
	}

	WDEBUG("[%d%d] %s %d", __dir, __allDay, __currentDate.dump().c_str(), __list.size());
}

std::shared_ptr<CalInstanceSchedule> CalComplexListProvider::__getNextScheduleFromDbList(CalDate& date)
{
	calendar_record_h record = __fetcher.getNext(true);
	if (record == NULL)
	{
		return nullptr;
	}

	auto schedule = std::make_shared<CalInstanceSchedule>(record);

	CalDate adjustedStart, adjustedEnd;
	__getAdjustedDates(*schedule, adjustedStart, adjustedEnd);

	if (adjustedStart < CalDate(CalDate::INIT_LOWER_BOUND))
	{
		WDEBUG("Start is out of supported range(%s)", adjustedStart.dump().c_str());
		return nullptr;
	}

	if (CalDate(CalDate::INIT_UPPER_BOUND) < adjustedEnd)
	{
		WDEBUG("End is out of supported range(%s)", adjustedEnd.dump().c_str());
		return nullptr;
	}

	date = __dir > 0 ? adjustedStart : adjustedEnd;
	return schedule;
}

void CalComplexListProvider::__getAdjustedDates(const CalInstanceSchedule& schedule, CalDate& adjustedStart, CalDate& adjustedEnd) const
{
	WDEBUG("__base(%s)", __base.dump().c_str());

	CalDateTime startDateTime;
	schedule.getStart(startDateTime);
	const CalDate start(startDateTime.getYear(), startDateTime.getMonth(), startDateTime.getMday());

	CalDateTime endDateTime;
	schedule.getEndForComplexList(endDateTime);
	const CalDate end(endDateTime.getYear(), endDateTime.getMonth(), endDateTime.getMday());

	if (__allDay)
	{
		WASSERT(start <= end);
		if (__dir > 0)
		{
			WASSERT(__base <= end); // 2014/5/4 ~ 2014/5/5 event should show on 2014/5/5
		}
		else
		{
			WASSERT(start < __base);
		}
	}
	else
	{
		WASSERT(startDateTime <= endDateTime);
		if (__dir > 0)
		{
			// TODO CALDATETIME: Uncomment this after CalDateTime class and all the changes from CalLocateManager in other classes are fixed
			//WASSERT(__base <= end); // 2014/5/5 00:00 ~ 2014/5/5 00:00 event should show on 2014/5/5
		}
		else
		{
//			http://en.wikipedia.org/wiki/Magadan_Time
//			http://www.timeanddate.com/time/zone/russia/magadan
			WASSERT(start <= __base);
		}
	}

	if (__dir > 0)
	{
		adjustedStart = __base <= start ? start : __base;
		adjustedEnd = end;
	}
	else
	{
		adjustedStart = start;
		if (end < __base)
		{
			adjustedEnd =  end;
		}
		else
		{
			adjustedEnd =  __base;
			if (start < end)
			{
				WINFO("The 2014/10/26 2:00 AM Magadan timezone case");
				adjustedEnd.addDays(-1);
			}
		}
	}
}

int CalComplexListProvider::__getDuration(const CalInstanceSchedule& schedule) const
{
	CalDate adjustedStart, adjustedEnd;
	__getAdjustedDates(schedule, adjustedStart, adjustedEnd);

	if (__allDay)
	{
		return CalDate::getDayDiff(adjustedEnd, adjustedStart) + 1;

	}
	else
	{
		CalDateTime startDateTime, endDateTime;
		schedule.getStart(startDateTime);
		schedule.getEndForComplexList(endDateTime);

		if (startDateTime.getDateCompareVal() < endDateTime.getDateCompareVal() &&
		    endDateTime.getHour() == 0 && endDateTime.getMinute() == 0 && endDateTime.getSecond() == 0)
		{
			return CalDate::getDayDiff(adjustedEnd, adjustedStart);
		}
		else
		{
			return CalDate::getDayDiff(adjustedEnd, adjustedStart) + 1;
		}
	}
}

CalComplexListProvider::Occupant::Occupant(std::shared_ptr<CalInstanceSchedule>& schedule, int remainingDays) :
	__schedule(schedule),
	__remainingDays(remainingDays)
{
	WASSERT(remainingDays > 0);
	WDEBUG("%s %d", __schedule->getSummary(), remainingDays);
}

CalComplexListProvider::Occupant::~Occupant()
{
}

bool CalComplexListProvider::Occupant::normalOrderPredicate(const Occupant& a, const Occupant& b)
{
	CalDateTime startA;
	CalDateTime startB;

	a.__schedule->getStart(startA);
	b.__schedule->getStart(startB);

	if (startA == startB)
	{
		return CAL_STRCMP(a.__schedule->getSummary(), b.__schedule->getSummary()) < 0;
	}
	else
	{
		return startA < startB;
	}
}

bool CalComplexListProvider::Occupant::reverseOrderPredicate(const Occupant& a, const Occupant& b)
{
	return !normalOrderPredicate(a, b);
}

bool CalComplexListProvider::eof()
{
	return __fetcher.eof();
}
