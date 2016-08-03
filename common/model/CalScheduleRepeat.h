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
#include "CalDate.h"
#include "CalDateTime.h"

/**
 * @brief The CalScheduleRepeat class represents event repeat rules.
 */
class WAPP_ASSIST_EXPORT CalScheduleRepeat
{
public:

	/**
	 * @brief Unit type.
	 *
	 */
	enum UnitType
	{
		ONE_TIME = 0,
		DAY,
		WEEK,
		MONTH,
		YEAR,
	};

	/**
	 * @brief Monthly repeat type.
	 *
	 */
	enum MonthlyType
	{
		MONTHDAY = 0,
		WEEKDAY,
	};

	/**
	 * @brief Until repeat type.
	 *
	 */
	enum UntilType
	{
		FOREVER = 0,
		TIMES,
		DUE_DATE,
	};

	/**
	 * @brief The Date struct.
	 *
	 */
	struct Date
	{
		int year;
		int month;
		int mday;
	};

	/**
	 * @brief Until info.
	 *
	 */
	struct UntilInfo
	{
		int times;
		Date date;
	};

	/**
	 * @brief Unit info union.
	 *
	 */
	union UnitInfo
	{
		/**
		 * @brief Weekly repeat.
		 *
		 */
		struct Weekly
		{
			bool selected[7];
			int dafaultWeek;
		};

		/**
		 * @brief Monthly repeat.
		 *
		 */
		struct Monthly
		{
			MonthlyType type;
			Date date;
		};

		Weekly weekly;
		Monthly monthly;
	};

	CalScheduleRepeat();
	CalScheduleRepeat( const CalScheduleRepeat& );
	const CalScheduleRepeat& operator=( const CalScheduleRepeat& );

	/**
	 * @brief Set default repeat.
	 *
	 * @param[in] type         repeat type
	 * @param[in] start        start time
	 * @param[in] timezone     time zone
	 *
	 */
	void setDefault(const UnitType type, const CalDateTime& start, const char* timezone);

	/**
	 * @brief Get repeat type text.
	 *
	 * @param[out] text       text buffer
	 *
	 */
	void getString(std::string& text) const;

	/**
	 * @brief Get repeat text.
	 *
	 * @param[in]  date        data
	 * @param[in]  timezone    time zone
	 * @param[out] text        text buffer
	 *
	 */
	void getRepeatString(const Date date, const char* timezone, std::string& text) const;

	/**
	 * @brief Get monthly unit info text.
	 *
	 * @param[in]  date        data
	 * @param[in]  timezone    time zone
	 * @param[in]  type        monthly repeat type
	 * @param[out] text        text buffer
	 *
	 */
	void getMonthlyUnitInfoString(const Date date, const char* timezone, const MonthlyType type, std::string& text) const;

	/**
	 * @brief Print repeat rules to log.
	 */
	void print() const;

	/**
	 * @brief Get item text.
	 *
	 * @param[in]  date               data
	 * @param[in]  timezone           time zone
	 * @param[out] text               text buffer
	 * @param[in]  textType           time type
	 * @param[in]  textUnitInterval   unit interval
	 * @param[in]  isMonth            is month
	 * @param[in]  isWeek             is week
	 * @param[in]  isNone             is none
	 *
	 */
	void getItemString(const Date date, const char* timezone, std::string& text, char* textType, char* textUnitInterval, bool isMonth, bool isWeek, bool isNone) const;

	/**
	 * @brief Get first day of week.
	 *
	 * @return first day number.
	 *
	 */
	int getFirstDayOfWeek() const;

	UnitType unitType;
	int unitInterval;
	UnitInfo unitInfo;
	UntilType untilType;
	UntilInfo untilInfo;
	Date startDate;
};

#endif /* _CAL_SCHEDULE_REPEAT_H_ */
