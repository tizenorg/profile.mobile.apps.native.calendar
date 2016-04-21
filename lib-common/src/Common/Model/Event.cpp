/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "Common/Model/DateTime.h"
#include "Common/Model/Event.h"

using namespace Common::Model;

Event::Event(calendar_record_h record)
		:m_Record(record)
{
}

Event::Event()
{
	calendar_record_create(_calendar_event._uri, &m_Record);
}

Event::~Event()
{
	calendar_record_destroy(m_Record, true);
}

int Event::getId() const
{
	int id;
	calendar_record_get_int(m_Record, _calendar_event.id, &id);
	return id;
}

const char *Event::getSummary() const
{
	char *str = nullptr;
	calendar_record_get_str_p(m_Record, _calendar_event.summary, &str);
	return str;
}

const char *Event::getDescription() const
{
	char *str = nullptr;
	calendar_record_get_str_p(m_Record, _calendar_event.description, &str);
	return str;
}

const char *Event::getLocation() const
{
	char *str = nullptr;
	calendar_record_get_str_p(m_Record, _calendar_event.location, &str);
	return str;
}

const DateTime &Event::getTimeStart() const
{
	return m_TimeStart;
}

const DateTime &Event::getTimeEnd() const
{
	return m_TimeEnd;
}

void Event::onChanged()
{
	//TODO: Record changed
}
