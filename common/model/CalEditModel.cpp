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

#include <app_preference.h>

#include "CalCommon.h"
#include "CalEditModel.h"
#include "CalBookManager.h"

#define CAL_EDIT_KEY_PREFIX "edit_view_"
#define CAL_EDIT_KEY_SUFFIX "_on_off"
/*
 * time zone
 * location
 * reminder
 * participant
 * show me as
 * privacy
 * repeat
 * description
 *
 */

#define CAL_EDIT_SETTING_INT_DEFAULT 0

CalEditModel::CalEditModel()
{
}

CalEditModel::~CalEditModel()
{
}

bool CalEditModel::isOn(const CalEditExpandField field)
{
	const char* key = __getKey(field);
	int value = __getInt(key);
	if (value == 1)
		return true;
	return false;
}

void CalEditModel::setOn(const CalEditExpandField field, bool isOn)
{
	const char* key = __getKey(field);
	int value = (isOn==true)?1:0;
	__setInt(key, value);
}

int CalEditModel::__getInt(const char *key)
{
	int value = CAL_EDIT_SETTING_INT_DEFAULT;

	int error = PREFERENCE_ERROR_NONE;
	bool check = false;
	error = preference_is_existing(key, &check);
	if (PREFERENCE_ERROR_NONE == error && !check)
	{
		__setInt(key, CAL_EDIT_SETTING_INT_DEFAULT);
	}

	error = preference_get_int(key, &value);
	if (error != PREFERENCE_ERROR_NONE)
	{
		WERROR("preference_get_int(%s) is failed(%x)", key, error);
		return -1;
	} else
		return value;
}

void CalEditModel::__setInt(const char *key, int value)
{
	int error = PREFERENCE_ERROR_NONE;

	error = preference_set_int(key, value);
	if (error != PREFERENCE_ERROR_NONE)
		WERROR("preference_set_int(%s) is failed(%x)", key, error);
}

const char* CalEditModel::__getKey(CalEditExpandField field)
{
	switch (field)
	{
	case TIME_ZONE:
		return CAL_EDIT_KEY_PREFIX "time_zone" CAL_EDIT_KEY_SUFFIX;
	case LOCATION:
		return CAL_EDIT_KEY_PREFIX "location" CAL_EDIT_KEY_SUFFIX;
	case REMINDER:
		return CAL_EDIT_KEY_PREFIX "reminder" CAL_EDIT_KEY_SUFFIX;
	case REPEAT:
		return CAL_EDIT_KEY_PREFIX "repeat" CAL_EDIT_KEY_SUFFIX;
	case DESCRIPTION:
		return CAL_EDIT_KEY_PREFIX "description" CAL_EDIT_KEY_SUFFIX;
	}
	WERROR("");
	return NULL;
}
