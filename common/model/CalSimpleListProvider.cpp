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

#include "CalSimpleListProvider.h"
#include "CalInstanceSchedule.h"

CalSimpleListProvider::CalSimpleListProvider(const CalDateTime& base, int dir, bool allDay, calendar_filter_h filter) :
	__fetcher(CalInstanceSchedule::createSimpleListQuery(base, dir, allDay, filter))
{
}

CalSimpleListProvider::CalSimpleListProvider(const CalDateTime& from, const CalDateTime& to, bool allDay, calendar_filter_h filter) :
	__fetcher(CalInstanceSchedule::createRangeListQuery(from, to, allDay, filter), true)
{
}

CalSimpleListProvider::~CalSimpleListProvider()
{
}

void CalSimpleListProvider::prefetch(bool fillBothBuffers)
{
	__fetcher.prefetch(fillBothBuffers);
	if (!__currentSchedule)
	{
		loadNext();
	}
}

std::shared_ptr<CalSchedule> CalSimpleListProvider::getCurrentSchedule()
{
	return __currentSchedule;
}

const CalDateTime& CalSimpleListProvider::getCurrentDate()
{
	return __currentDate;
}

void CalSimpleListProvider::loadNext()
{
	calendar_record_h record = __fetcher.getNext();

	if (record == NULL)
	{
		__currentSchedule = nullptr;
		return;
	}

	__currentSchedule = std::make_shared<CalInstanceSchedule>(record);

	CalDateTime start;
	__currentSchedule->getStart(start);
	__currentDate.set(start.getYear(), start.getMonth(), start.getMday());
}

bool CalSimpleListProvider::eof()
{
	return __fetcher.eof();
}
