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

#ifndef _CAL_INSTANCE_SCHEDULE_SET_H_
#define _CAL_INSTANCE_SCHEDULE_SET_H_

#include <memory>
#include <map>
#include <list>
#include "WDefine.h"
#include <calendar.h>
#include "CalInstanceSchedule.h"

struct SetKey {
	 int eventId;
	 int year;
	 int month;
	 int mday;
};

struct classcomp {
  bool operator() (const SetKey& lhs, const SetKey& rhs) const
  {
  	if(lhs.eventId < rhs.eventId)
		return true;
	else if(lhs.eventId > rhs.eventId)
		return false;

	if(lhs.year < rhs.year)
		return true;
	else if(lhs.year > rhs.year)
		return false;

	if(lhs.month < rhs.month)
		return true;
	else if(lhs.month > rhs.month)
		return false;

	if(lhs.mday < rhs.mday)
		return true;
	else if(lhs.mday > rhs.mday)
		return false;

	return false;
  }
};


class WAPP_ASSIST_EXPORT CalInstanceScheduleSet
{
public:
	CalInstanceScheduleSet();
	~CalInstanceScheduleSet();
public:
	 void add(std::shared_ptr<CalSchedule> instance);
	 bool exists(std::shared_ptr<CalSchedule>& instance);
	 void remove(std::shared_ptr<CalSchedule>& instance);
	 void reset();
	 void getList(std::list<std::shared_ptr<CalSchedule>>& list);
	 int getCount();
private:
	 std::map<SetKey, std::shared_ptr<CalSchedule>, classcomp> __map;
};

#endif /* _CAL_INSTANCE_SCHEDULE_SET_H_ */
