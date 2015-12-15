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

#ifndef _CAL_SCHEDULE_REPEAT_H_
#define _CAL_SCHEDULE_REPEAT_H_

#include <string>
#include "CalDateTime.h"

class WAPP_ASSIST_EXPORT CalScheduleRepeat
{
public:
	CalScheduleRepeat();
	virtual ~CalScheduleRepeat();
	CalScheduleRepeat( const CalScheduleRepeat& );
	const CalScheduleRepeat& operator=( const CalScheduleRepeat& );
public:
	enum UnitType {
		ONE_TIME = 0,
		DAY,
		WEEK,
		MONTH,
		YEAR,
	};

	enum MonthlyType {
		MONTHDAY = 0,
		WEEKDAY,
	};
	struct Date {
		int year;
		int month;
		int mday;
	};

	union UnitInfo {
		struct Weekly {
			bool selected[7]; //0 : sun, 6: sat
			int dafaultWeek;
		};
		struct Monthly {
			MonthlyType type;
			Date date;
		};
		Weekly weekly;
		Monthly monthly;
	};

	enum UntilType {
		FOREVER = 0,
		TIMES,
		DUE_DATE,
	};

	struct UntilInfo {
		int times;
		Date date;
	};

	void setDefault(const UnitType type, const CalDateTime& start, const char* timezone);
	void getString(std::string& text) const;
	void getRepeatString(const Date date, const char* timezone, std::string& text) const;
	void getMonthlyUnitInfoString(const Date date, const char* timezone, const MonthlyType type, std::string& text) const;
	void print() const;

private:
	void getItemString(const Date date, const char* timezone, std::string& text, char* textType, char* textUnitInterval, bool isMonth, bool isWeek, bool isNone) const;
	int getFirstDayOfWeek() const;

public:
	UnitType unitType;
	int unitInterval;
	UnitInfo unitInfo;
	UntilType untilType;
	UntilInfo untilInfo;
	Date startDate;
};

#endif /* _CAL_SCHEDULE_REPEAT_H_ */
