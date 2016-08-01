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
#include "CalSchedule.h"
#include "CalInstanceSchedule.h"

/**
 * @brief Event key for map.
 *
 */
struct SetKey
{
	int eventId;
	int year;
	int month;
	int mday;
};

/**
 * @brief Event key comparison class.
 *
 */
struct classcomp
{
	/**
	* @brief Event key comparison function.
	*
	*/
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

/**
 * @brief The CalInstanceScheduleSet class
 */
class WAPP_ASSIST_EXPORT CalInstanceScheduleSet
{
public:
	/**
	 * @brief Add event to set.
	 *
	 * @param[in] instance    event instance
	 *
	 */
	 void add(std::shared_ptr<CalSchedule> instance);

	/**
	 * @brief Check whether the instance of event is present in set.
	 *
	 * @param[in] instance    event instance
	 *
	 * @return true if present, false otherwise.
	 *
	 */
	bool exists(std::shared_ptr<CalSchedule>& instance);

	/**
	 * @brief Remove instance of event from the set.
	 *
	 * @param[in] instance    event instance
	 *
	 */
	void remove(std::shared_ptr<CalSchedule>& instance);

	/**
	 * @brief Clear the set.
	 *
	 */
	void reset();

	/**
	 * @brief Get event set as plain list.
	 *
	 * @param[out] list    event list
	 *
	 */
	void getList(std::list<std::shared_ptr<CalSchedule>>& list);

	/**
	 * @brief Get events count.
	 *
	 * @return events count
	 *
	 */
	int getCount();

private:
	 std::map<SetKey, std::shared_ptr<CalSchedule>, classcomp> __map;
};

#endif /* _CAL_INSTANCE_SCHEDULE_SET_H_ */
