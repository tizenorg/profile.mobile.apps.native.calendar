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

#include "CalCustomListModel.h"
#include "CalInstanceSchedule.h"
#include "CalDebugInternal.h"

CalCustomListModel::CalCustomListModel(const std::list<std::shared_ptr<CalSchedule>>& schedules) :
	__schedules(schedules)
{
	WENTER();
	__it = __schedules.begin();
	if (!schedules.empty())
	{
		CalDateTime start;
		(*__it)->getStart(start);
		__currentDate.set(start.getYear(), start.getMonth(), start.getMday());
	}
}

CalCustomListModel::~CalCustomListModel()
{
	WENTER();
}

std::shared_ptr<CalSchedule> CalCustomListModel::getNext(bool& dayChanged)
{
	WENTER();
	if (__it == __schedules.end())
	{
		return nullptr;
	}

	std::shared_ptr<CalSchedule> schedule = *__it;

	CalDateTime start;
	CalDateTime date;
	schedule->getStart(start);
	date.set(start.getYear(), start.getMonth(), start.getMday());

	dayChanged = (date != __currentDate) ? true : false;
	__currentDate = date;

	++__it;
	return schedule;
}

const CalDateTime& CalCustomListModel::getCurrentDate()
{
	WENTER();
	return __currentDate;
}

bool CalCustomListModel::eof()
{
	WASSERT(0);
	return false;
}
