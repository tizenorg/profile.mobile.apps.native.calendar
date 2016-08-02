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

#include "CalInstanceScheduleSet.h"

void CalInstanceScheduleSet::add(std::shared_ptr<CalSchedule> instance)
{
	if(exists(instance)){
		WWARNING("The key is exit!");
		return;
	};

	SetKey key = {0};

	CalDateTime startTime;
	instance->getStart(startTime);

	key.eventId = instance->getIndex();
	key.year = startTime.getYear();
	key.month= startTime.getMonth();
	key.mday= startTime.getMday();

	WDEBUG("Add key %d %d %d %d", key.eventId,key.year, key.month,key.mday);
	std::pair<SetKey, std::shared_ptr<CalSchedule>>(key, instance);
	__map.insert(std::pair<SetKey, std::shared_ptr<CalSchedule>>(key, instance));
}

bool CalInstanceScheduleSet::exists(std::shared_ptr<CalSchedule>& instance)
{
	if(__map.size() == 0)
		return false;

	SetKey key = {0};

	CalDateTime startTime;
	instance->getStart(startTime);

	key.eventId = instance->getIndex();
	key.year = startTime.getYear();
	key.month= startTime.getMonth();
	key.mday= startTime.getMday();

	if(__map.find(key) != __map.end())
		return true;
	return false;
}

void CalInstanceScheduleSet::remove(std::shared_ptr<CalSchedule>& instance)
{

	if(__map.size() == 0)
		return;

	SetKey key = {0};

	CalDateTime startTime;
	instance->getStart(startTime);

	key.eventId = instance->getIndex();
	key.year = startTime.getYear();
	key.month= startTime.getMonth();
	key.mday= startTime.getMday();
	WDEBUG("Remove key %d %d %d %d", key.eventId,key.year, key.month,key.mday);
	__map.erase(key);
}

void CalInstanceScheduleSet::reset()
{
	__map.clear();
}

void CalInstanceScheduleSet::getList(std::list<std::shared_ptr<CalSchedule>>& list)
{
	auto it = __map.begin();
	for( ; it != __map.end(); ++it){
		list.push_back(it->second);
	}
}

int CalInstanceScheduleSet::getCount()
{
	return __map.size();
}

