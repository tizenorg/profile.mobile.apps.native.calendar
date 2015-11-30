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

#include <app.h>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <string>
#include <Config.h>
#include <dlog.h>
#include <Elementary.h>

static int instancesNum = 0;

struct FinishTestListener : public testing::EmptyTestEventListener
{
	virtual ~FinishTestListener()
	{}
	virtual void OnTestProgramEnd(const testing::UnitTest& /*unit_test*/)
	{
		elm_exit();
	}
};


static void _win_focused_cb(void *data, Evas_Object *obj, void *event)
{
	if (instancesNum == 0) // preventing from multiple call of RUN_ALL_TESTS()
	{
		RUN_ALL_TESTS();
		++instancesNum;
	}
}


EAPI_MAIN int elm_main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);

	::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
	listeners.Append(new FinishTestListener());

	Evas_Object *win = elm_win_util_standard_add("dummy-win", "Dummy Test Window");
	elm_win_autodel_set(win, EINA_TRUE);
	elm_win_focus_highlight_enabled_set(win, EINA_TRUE);
	evas_object_smart_callback_add(win, "focus,in", _win_focused_cb, NULL);

	evas_object_resize(win, 400, 400);
	evas_object_show(win);
	elm_run();
	return 0;
}

ELM_MAIN()
