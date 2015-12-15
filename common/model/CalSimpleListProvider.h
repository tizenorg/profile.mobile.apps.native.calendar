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

#ifndef _CAL_SIMPLE_LIST_PROVIDER_H_
#define _CAL_SIMPLE_LIST_PROVIDER_H_

#include "ICalListProvider.h"

#include <memory>

#include "CalRecordBlockFetcher.h"

class CalSimpleListProvider : public ICalListProvider
{
public:
	CalSimpleListProvider(const CalDateTime& base, int dir, bool allDay, calendar_filter_h filter);
	CalSimpleListProvider(const CalDateTime& from, const CalDateTime& to, bool allDay, calendar_filter_h filter);
	virtual ~CalSimpleListProvider();
public:
	virtual void prefetch(bool fillBothBuffers);
	virtual std::shared_ptr<CalSchedule> getCurrentSchedule();
	virtual const CalDateTime& getCurrentDate();
	virtual void loadNext();
	virtual bool eof();
private:
	CalRecordBlockFetcher __fetcher;
	CalDateTime __currentDate;
	std::shared_ptr<CalSchedule> __currentSchedule;
};

#endif
