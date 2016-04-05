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

#include "Common/Model/EventData.h"
#include "Common/Model/DateTime.h"

#include "common/edje/inc/UiCommon.h"

#define TEXT_BUFFER_SIZE 512;

using namespace Common::Model;

EventData::EventData(calendar_record_h record):
		m_Record(record)
{
	calendar_record_get_int(m_Record, _calendar_event.id, &m_Id);

	char *text = nullptr;

	calendar_record_get_str(m_Record, _calendar_event.summary, &text);
	m_Summary = text;
	free(text);

	calendar_record_get_str(m_Record, _calendar_event.description, &text);
	m_Description = text;
	free(text);

	calendar_record_get_str(m_Record, _calendar_event.location, &text);
	m_Location = text;
	free(text);

}

EventData::EventData()
{
	calendar_record_create(_calendar_event._uri, &m_Record);
}

int EventData::getId()
{
	return m_Id;
}

const std::string &EventData::getSummary()
{
	return m_Summary;
}

const std::string &EventData::getDescription()
{
	return m_Description;
}

const std::string &EventData::getLocation()
{
	return m_Location;
}

const DateTime EventData::getTimeStart()
{
	return m_TimeStart;
}

const DateTime EventData::getTimeEnd()
{
	return m_TimeEnd;
}

void EventData::onDataChanged()
{
	//TODO: Record changed
}
