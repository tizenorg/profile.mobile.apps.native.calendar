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

#include <string>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../common/model/CalDateTime.h"
#include "../common/model/CalLocaleManager.h"

#define bufLength 128

int year = 1989;
int month = 8;
int mday = 7;
int hour = 2;
int min = 12;
int sec = 32;

class TestCalDateTime: public testing::Test
{
	protected:
		virtual void SetUp()
		{
		}

		virtual void TearDown()
		{
		}
	CalDateTime m_CalDateTime;
};

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testEmptyConstruct)
{
	CalDateTime birthday;
	ASSERT_TRUE(birthday.getYear());
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testConsturctTM)
{
	struct tm dateTm = {0};
	dateTm.tm_mday = mday;
	CalDateTime birthday(dateTm);
	ASSERT_EQ(birthday.getMday(), mday);
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testConstructWithYMD)
{
	CalDateTime birthday(year, month, mday);
	ASSERT_EQ(birthday.getYear(),year);
	ASSERT_EQ(birthday.getMonth(),month);
	ASSERT_EQ(birthday.getMday(),mday);
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testConstructWithYMDHMSAndGetYMDHMS)
{
	CalDateTime birthday(year, month, mday, hour, min, sec);
	ASSERT_EQ(birthday.getYear(),year);
	ASSERT_EQ(birthday.getMonth(),month);
	ASSERT_EQ(birthday.getMday(),mday);
	ASSERT_EQ(birthday.getHour(),hour);
	ASSERT_EQ(birthday.getMinute(),min);
	ASSERT_EQ(birthday.getSecond(),sec);
}


//____________________________________________________________________________//

TEST_F(TestCalDateTime, testOperatorAssigmentAndEQ)
{
	CalDateTime birthday1(year, month, mday, hour, min, sec);
	CalDateTime birthday2 = birthday1;
	ASSERT_TRUE(birthday1 == birthday2);
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testOperatorCopy)
{
	CalDateTime birthday1(year, month, mday, hour, min, sec);
	CalDateTime birthday2(birthday1);
	ASSERT_TRUE(birthday1 == birthday2);
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testOperatorNotEQ)
{
	CalDateTime birthday1(year, month, mday, hour, min, sec);
	CalDateTime birthday2(year+10, month, mday+5, hour, min, sec);
	ASSERT_TRUE(birthday1 != birthday2);
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testOperatorsMoreLess)
{
	CalDateTime birthday1(year, month, mday, hour, min, sec);
	CalDateTime birthday2(year+1, month, mday+5, hour, min, sec);
	ASSERT_TRUE(birthday2 > birthday1);
	ASSERT_TRUE(birthday1 < birthday2);
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testOperatorsMoreEQLessEQ)
{
	CalDateTime birthday1(year, month, mday, hour, min, sec);
	CalDateTime birthday2(year+1, month, mday+5, hour, min, sec);
	ASSERT_TRUE(birthday2 >= birthday1);
	ASSERT_TRUE(birthday1 <= birthday2);
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testIsSameDay)
{
	CalDateTime birthday1(year, month, mday, hour, min, sec);
	CalDateTime birthday2;

	birthday2.isSameDay(birthday1);

	ASSERT_EQ(birthday1.getYear(),birthday2.getYear());
	ASSERT_EQ(birthday1.getMonth(),birthday2.getMonth());
	ASSERT_EQ(birthday1.getMday(),birthday2.getMday());
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testSetYMD)
{
	CalDateTime birthday;
	birthday.set(year, month, mday);
	ASSERT_EQ(birthday.getYear(),year);
	ASSERT_EQ(birthday.getMonth(),month);
	ASSERT_EQ(birthday.getMday(),mday);
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testSetUtime)
{
	CalDateTime birthday;
	struct tm time;
	time.tm_year = year - 1900;
	time.tm_mon = month - 1;
	time.tm_mday = mday;
	time.tm_hour = 0;
	time.tm_min = 0;
	time.tm_sec = 0;

	long long int utime = CalLocaleManager::getInstance().getUtime(time);
	birthday.set(utime);
	ASSERT_EQ(birthday.getYear(),year);
	ASSERT_EQ(birthday.getMonth(),month);
	ASSERT_EQ(birthday.getMday(),mday);
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testSetTM)
{
	CalDateTime birthday;
	struct tm time;
	time.tm_year = year;
	time.tm_mon = month;
	time.tm_mday = mday;
	time.tm_hour = 0;
	time.tm_min = 0;
	time.tm_sec = 0;

	birthday.set(time);
	ASSERT_EQ(birthday.getYear(),year);
	ASSERT_EQ(birthday.getMonth(),month);
	ASSERT_EQ(birthday.getMday(),mday);
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testSetIsAllDay)
{
	CalDateTime birthday;

	birthday.setAllDay(true);

	ASSERT_TRUE(birthday.isAllDay());
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testgetTmFromUtimeAndgetUtimeFromTm)
{
	struct tm time;
	long long int utime;

	CalDateTime birthday(year, month, mday, hour, min, sec);
	birthday.getTmFromUtime(&time);

	ASSERT_EQ(time.tm_year, year);
	ASSERT_EQ(time.tm_mon, month);
	ASSERT_EQ(time.tm_mday, mday);
	ASSERT_EQ(time.tm_hour, hour);
	ASSERT_EQ(time.tm_min, min);
	ASSERT_EQ(time.tm_sec, sec);

	utime = birthday.getUtimeFromTm();
	long long int utimeTmp = CalLocaleManager::getInstance().getUtime(time);
	ASSERT_EQ(utime, utimeTmp);
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testgetString)
{
	CalDateTime birthday(year, month, mday, hour, min, sec);
	std::string testString;
	birthday.getString(testString);
	ASSERT_TRUE(testString.size());
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testgetTimeString)
{
	CalDateTime birthday(year, month, mday, hour, min, sec);
	std::string testString;
	birthday.getTimeString(testString);
	ASSERT_TRUE(testString.size());
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testgetDateString)
{
	CalDateTime birthday(year, month, mday, hour, min, sec);
	std::string testString;
	birthday.getDateString(testString);
	ASSERT_TRUE(testString.size());
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testIsAllDay)
{
	CalDateTime birthday(year, month, mday, hour, min, sec);
	ASSERT_FALSE(birthday.isAllDay());

	birthday.set(year, month, mday);
	ASSERT_TRUE(birthday.isAllDay());
}

//TODO: investigate getWeekDayText function
//____________________________________________________________________________//

TEST_F(TestCalDateTime, testGetWeekday)
{
	CalDateTime birthday(year, month, mday, hour, min, sec);
	ASSERT_TRUE(birthday.getWeekday() >= 0);
	ASSERT_TRUE(birthday.getWeekday() <= 30);
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testAddSeconds)
{
	CalDateTime birthday(year, month, mday, hour, min, sec);
	const long long addedSeconds = 2;
	birthday.addSeconds(addedSeconds, true);
	ASSERT_EQ(birthday.getSecond(),sec + addedSeconds);
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testAddHours)
{
	CalDateTime birthday(year, month, mday, hour, min, sec);
	const long long addedHours = 2;
	birthday.addHours(addedHours, true);
	ASSERT_EQ(birthday.getHour(),hour + addedHours);
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testAddDays)
{
	CalDateTime birthday(year, month, mday, hour, min, sec);
	const long long addedDays = 2;
	birthday.addDays(addedDays, true);
	ASSERT_EQ(birthday.getMday(),mday * addedDays);
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testAddMonths)
{
	CalDateTime birthday(year, month, mday, hour, min, sec);
	const long long addedMonth = 2;
	birthday.addMonths(addedMonth, true);
	ASSERT_EQ(birthday.getMonth(),month + addedMonth);
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testAddYears)
{
	CalDateTime birthday(year, month, mday, hour, min, sec);
	const long long addedYears = 2;
	birthday.addYears(addedYears, true);
	ASSERT_EQ(birthday.getYear(),year + addedYears);
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testGetStringParam)
{
	CalDateTime birthday(year, month, mday, hour, min, sec);
	char buffer[bufLength] = {0};
	birthday.getStringParam(buffer);
	ASSERT_TRUE(strlen(buffer));
}

//____________________________________________________________________________//

TEST_F(TestCalDateTime, testGetDateCompareVal)
{
	CalDateTime birthday(year, month, mday, hour, min, sec);
	int date = 0;
	date = birthday.getDateCompareVal();
	ASSERT_TRUE(date);
}
