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

#ifndef _CAL_TIMEZONE_H_
#define _CAL_TIMEZONE_H_

#include <memory>
#include <string>

#define TIMEZONE_UNKNOWN "Etc/GMT"

class __CalTimeZoneImpl;


/**
 * @brief %CalTimeZone class for work with timezones
 */
class CalTimeZone
{
public:
	/**
	 * @brief Enumeration for the string representation of timezone
	 */
	typedef enum {
		TIMEZONE_TIZEN_FORMAT = 0,   /**< Format of string output in tz_databaze format, e.g. Asia/Seoul*/
		TIMEZONE_EAS_FORMAT          /**< Format of string output in EAS format (GMT +9:00) Korean Standart Time*/
	} LabelType;

	CalTimeZone(const CalTimeZone& obj);
	const CalTimeZone& operator=(const CalTimeZone& obj);

	/**
	 * @brief CalTimeZone constructor
	 * @param tz    TimeZone string identificator in tz_database
	 */
	CalTimeZone(const char *tz = TIMEZONE_UNKNOWN);
	~CalTimeZone();

	/**
	 * @brief Return timezone string representation
	 * @description Return formatted output of timezone string
	 * representation depends of current locale of target
	 * @param typeFormat[in] Format of text output of %Timezone instance
	 * @return String of timezone representation
	 */
	std::string getString(LabelType typeFormat = TIMEZONE_TIZEN_FORMAT) const;
	/**
	 * @brief Return UTC offset, the number of seconds to add to UTC to get local time
	 * @return  Amout of offset in second
	 */
	int getOffset() const;

private:
	CalTimeZone& operator=(CalTimeZone&&) = delete;
	CalTimeZone(CalTimeZone&&) = delete;
	std::shared_ptr<__CalTimeZoneImpl> __pv;
};

#endif /* _CAL_TIMEZONE_H_ */
