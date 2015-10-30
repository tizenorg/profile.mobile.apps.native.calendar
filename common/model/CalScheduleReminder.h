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

#ifndef _CAL_SCHEDULE_REMINDER_H_
#define _CAL_SCHEDULE_REMINDER_H_

#include <string>
#include "CalDate.h"
#include "CalCommon.h"

class WAPP_ASSIST_EXPORT CalScheduleReminder
{
public:
	CalScheduleReminder();
	virtual ~CalScheduleReminder();
	CalScheduleReminder(CalScheduleReminder&);
	CalScheduleReminder(const CalScheduleReminder&);
	const CalScheduleReminder& operator=(const CalScheduleReminder&);
	const bool operator<(const CalScheduleReminder&) const;
public:
	int getMinuteValue() const;

public:
	enum UnitType {
		NONE = 0,
		MIN,
		HOUR,
		DAY,
		WEEK,
	};

	void getString(std::string& text) const;

public:
	UnitType unitType;
	int unitValue;
	bool isCustom;
	bool isOffItem;
};

#endif /* _CAL_SCHEDULE_REMINDER_H_ */
