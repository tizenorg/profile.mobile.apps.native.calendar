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

#include <stdio.h>
#include "CalScheduleReminder.h"

CalScheduleReminder::CalScheduleReminder() :
	unitType(NONE),
	unitValue(0),
	isCustom(false),
	isOffItem(false)
{
}

CalScheduleReminder::~CalScheduleReminder()
{
}

CalScheduleReminder::CalScheduleReminder(CalScheduleReminder& obj)
{
	unitType = obj.unitType;
	unitValue = obj.unitValue;
	isCustom = obj.isCustom;
	isOffItem = obj.isOffItem;
}

CalScheduleReminder::CalScheduleReminder(const CalScheduleReminder& obj)
{
	unitType = obj.unitType;
	unitValue = obj.unitValue;
	isCustom = obj.isCustom;
	isOffItem = obj.isOffItem;
}

const CalScheduleReminder& CalScheduleReminder::operator=(const CalScheduleReminder& obj)
{
	if (this != &obj)
	{
		unitType = obj.unitType;
		unitValue = obj.unitValue;
		isCustom = obj.isCustom;
	}
	return *this;
}

const bool CalScheduleReminder::operator<(const CalScheduleReminder& obj) const
{
	return (getMinuteValue() < obj.getMinuteValue());
}

void CalScheduleReminder::getString(std::string& text) const
{
	if (unitType == NONE)
	{
		text = _L_("IDS_CLD_OPT_NONE");
		return;
	}
	if (unitType != NONE && unitValue == 0)
	{
		text = _L_("IDS_CLD_OPT_AT_START_TIME_ABB");
		return;
	}

	const int bufferLen = 256;
	char buffer[bufferLen] = {0};

	UnitType displayUnitType = unitType;
	int displayUnitValue = unitValue;

	int minuteValue = getMinuteValue();

	//60 minutes --> 1 hour
	//48 hours --> 2 days
	if(minuteValue == 0){
		displayUnitType = MIN;
		displayUnitValue = 0;
	}else if(minuteValue % (60 * 24 * 7) == 0){
		displayUnitType = WEEK;
		displayUnitValue = minuteValue / (60 * 24 * 7);
	}else if(minuteValue % (60 * 24) == 0){
		displayUnitType = DAY;
		displayUnitValue = minuteValue / (60 * 24);
	}else if(minuteValue % 60 == 0){
		displayUnitType = HOUR;
		displayUnitValue = minuteValue / 60;
	}else {
		displayUnitType = MIN;
		displayUnitValue = minuteValue;
	}

	switch(displayUnitType)
	{
	default:
	case NONE:
		snprintf(buffer, sizeof(buffer)-1, _L_("IDS_CLD_OPT_NONE"));
		break;
	case MIN:
		if (displayUnitValue == 0)
			snprintf(buffer, sizeof(buffer)-1, _L_("IDS_CLD_OPT_AT_START_TIME_ABB"));
		else if(displayUnitValue == 1)
			snprintf(buffer, sizeof(buffer)-1, _L_("IDS_CLD_OPT_1_MINUTE_BEFORE_ABB"));
		else
			snprintf(buffer, sizeof(buffer)-1, _L_("IDS_CLD_OPT_PD_MINUTES_BEFORE_ABB"), displayUnitValue);
		break;
	case HOUR:
		if (displayUnitValue == 1) {
			snprintf(buffer, sizeof(buffer)-1, _L_("IDS_CLD_OPT_1_HOUR_BEFORE_ABB"));
		} else {
			snprintf(buffer, sizeof(buffer)-1, _L_("IDS_CLD_OPT_PD_HOURS_BEFORE_ABB"), displayUnitValue);
		}
		break;
	case DAY:
		if (displayUnitValue == 1) {
			snprintf(buffer, sizeof(buffer)-1, _L_("IDS_CLD_OPT_1_DAY_BEFORE_ABB"));
		} else {
			snprintf(buffer, sizeof(buffer)-1, _L_("IDS_CLD_OPT_PD_DAYS_BEFORE_ABB2"), displayUnitValue);
		}
		break;
	case WEEK:
		if (displayUnitValue == 1) {
			snprintf(buffer, sizeof(buffer)-1, _L_("IDS_CLD_OPT_1_WEEK_BEFORE_ABB"));
		} else {
			snprintf(buffer, sizeof(buffer)-1, _L_("IDS_CLD_OPT_PD_WEEKS_BEFORE_ABB"), displayUnitValue);
		}
		break;
	}

	text = buffer;
	return;
}

int CalScheduleReminder::getMinuteValue() const
{
	switch (unitType) {
	case NONE:
		return 0;
	case MIN:
		return unitValue;
	case HOUR:
		return unitValue * 60;
	case DAY:
		return unitValue * 60 * 24;
	case WEEK:
		return unitValue * 60 * 24 * 7;
	default:
		return 0;
	}
}
