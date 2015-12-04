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

TEST_F(TestCalSettingsManager, test1)
{

	CalSettingsManager::getInstance().setAlertType(CalSettingsManager::getInstance().STATUS_BAR_NOTIFICATION);
	ASSERT_EQ(CalSettingsManager::getInstance().STATUS_BAR_NOTIFICATION, CalSettingsManager::getInstance().getAlertType());

	CalSettingsManager::getInstance().setFirstDayOfWeek(CalSettingsManager::getInstance().MON);
	ASSERT_TRUE(2 >= CalSettingsManager::getInstance().getFirstDayOfWeek() >= 0);

	CalSettingsManager::getInstance().setLockTimeZone(false);
	ASSERT_EQ(false, CalSettingsManager::getInstance().getLockTimeZone());
}
