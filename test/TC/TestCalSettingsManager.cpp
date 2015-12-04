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

#include "../common/model/CalSettingsManager.h"

class TestCalSettingsManager: public testing::Test
{
	protected:
		virtual void SetUp()
		{
		}

		virtual void TearDown()
		{
		}
};

//____________________________________________________________________________//

TEST_F(TestCalSettingsManager, TestSetAlertType)
{
	CalSettingsManager::getInstance().setAlertType(CalSettingsManager::ALERT);
	ASSERT_EQ(CalSettingsManager::ALERT, CalSettingsManager::getInstance().getAlertType());
}

//____________________________________________________________________________//

TEST_F(TestCalSettingsManager, TestSetFirstDayOfWeek)
{
	CalSettingsManager::getInstance().setFirstDayOfWeek(CalSettingsManager::MON);
	ASSERT_EQ(CalSettingsManager::getInstance().getFirstDayOfWeek(), CalSettingsManager::MON);
}

//____________________________________________________________________________//

TEST_F(TestCalSettingsManager, TestSetLockTimeZone)
{
	CalSettingsManager::getInstance().setLockTimeZone(true);
	ASSERT_TRUE(CalSettingsManager::getInstance().getLockTimeZone());
}

//____________________________________________________________________________//

TEST_F(TestCalSettingsManager, TestSetLastUsedCalendar)
{
	CalSettingsManager::getInstance().setLastUsedCalendar(25);
	ASSERT_EQ(CalSettingsManager::getInstance().getLastUsedCalendar(), 25);
}

//____________________________________________________________________________//

TEST_F(TestCalSettingsManager, TestSetTimeZone)
{
	std::string tmp;
	std::string testString = "Europe/London";
	CalSettingsManager::getInstance().setTimeZone(testString);
	CalSettingsManager::getInstance().getTimeZone(tmp);
	ASSERT_EQ(tmp, testString);
}

//____________________________________________________________________________//

TEST_F(TestCalSettingsManager, TestSetAlertSound)
{
	CalSettingsManager::getInstance().setAlertSound("/opt/usr/share/settings/Ringtones/Rolling_Tone.ogg");
	ASSERT_EQ(CalSettingsManager::getInstance().getAlertSound(),"Rolling_Tone.ogg");
}
