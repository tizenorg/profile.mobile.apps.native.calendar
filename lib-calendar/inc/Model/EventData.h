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

#ifndef CALENDAR_COMMON_LIST_EVENT_DATA_H
#define CALENDAR_COMMON_LIST_EVENT_DATA_H

#include <string>
#include <calendar.h>

#include "DateTime.h"

namespace Ui
{
	class GenlistItem;
}

namespace Calendar
{
	namespace Model
	{
		/**
		 * @brief EventData model
		 */
		class EventData
		{
		public:
			EventData(calendar_record_h record);
			EventData();

			int	getId();

			const std::string &getSummary();
			const std::string &getDescription();
			const std::string &getLocation();

			const DateTime getTimeStart();
			const DateTime getTimeEnd();

			void onDataChanged();

		private:
			calendar_record_h m_Record;

			int m_Id;

			std::string m_Summary;
			std::string m_Description;
			std::string m_Location;

			DateTime m_TimeStart;
			DateTime m_TimeEnd;
		};
	}
}

#endif /* CALENDAR_COMMON_LIST_EVENT_DATA_H */
