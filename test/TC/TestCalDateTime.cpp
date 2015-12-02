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

TEST_F(TestCalDateTime, test1)
{
	int year = 1989;
	int month = 8;
	int mday = 7;

	CalDateTime birthday(year, month, mday);
	if(birthday.getYear() == year && birthday.getMonth() == month && birthday.getMday() == mday)
	{		
		ASSERT_TRUE(true);
	}
	else
	{
		ASSERT_TRUE(false);
	}
}

