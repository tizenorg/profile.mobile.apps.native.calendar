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
#include <string.h>
#include "CalScheduleRepeat.h"
#include "CalDateTime.h"
#include "CalLocaleManager.h"
#include "CalCommon.h"
#include "CalSettingsManager.h"

#define SCHEDULE_BUFFER 256
#define DATE_BUFFER 128

CalScheduleRepeat::CalScheduleRepeat()
{
	unitType = DAY;
	unitInterval = 1;
	memset(&unitInfo, 0x0, sizeof(UnitInfo));
	untilType = FOREVER;
	memset(&untilInfo, 0x0, sizeof(UntilInfo));
	startDate.year = 0;
	startDate.month = 0;
	startDate.mday = 0;
}

CalScheduleRepeat::CalScheduleRepeat( const CalScheduleRepeat& obj)
{
	unitType = obj.unitType;
	unitInterval = obj.unitInterval;
	unitInfo = obj.unitInfo;
	untilType = obj.untilType;
	untilInfo = obj.untilInfo;
	startDate.year = obj.startDate.year;
	startDate.month = obj.startDate.month;
	startDate.mday = obj.startDate.mday;
}

const CalScheduleRepeat& CalScheduleRepeat::operator=( const CalScheduleRepeat& obj)
{
	if (this != &obj)
	{
		unitType = obj.unitType;
		unitInterval = obj.unitInterval;
		unitInfo = obj.unitInfo;
		untilType = obj.untilType;
		untilInfo = obj.untilInfo;
		startDate.year = obj.startDate.year;
		startDate.month = obj.startDate.month;
		startDate.mday = obj.startDate.mday;
	}
	return *this;
}

void CalScheduleRepeat::setDefault(const UnitType type, const CalDateTime& start, const char* timezone)
{
	if (unitType == type) {
		WHIT();
		print();
		return;
	}

	unitType = type;
	unitInterval = 1;

	untilType = DUE_DATE;

	CalDateTime dueDate(start);

	if (unitType == CalScheduleRepeat::DAY) {
		dueDate.addDays(7);
	} else if (unitType == CalScheduleRepeat::WEEK){
		dueDate.addMonths(1);
	} else if (unitType == CalScheduleRepeat::MONTH){
		dueDate.addYears(1);
	} else if (unitType == CalScheduleRepeat::YEAR){
		dueDate.addYears(5);
	}

	untilInfo.date.year = dueDate.getYear();
	untilInfo.date.month = dueDate.getMonth();
	untilInfo.date.mday = dueDate.getMday();
	startDate.year = start.getYear();
	startDate.month = start.getMonth();
	startDate.mday = start.getMday();

	if (unitType == CalScheduleRepeat::WEEK){
		for (int i=0;i<7;i++)
			unitInfo.weekly.selected[i] = false;

		int weekday = start.getWeekday();
		if (weekday >=0 && weekday <=6)
			unitInfo.weekly.selected[weekday]=true;
		else
			WERROR("fail");
	} else if (unitType == CalScheduleRepeat::MONTH){
		unitInfo.monthly.type = MONTHDAY;
		unitInfo.monthly.date.year = start.getYear();
		unitInfo.monthly.date.month = start.getMonth();
		unitInfo.monthly.date.mday = start.getMday();
	}
	print();
}

void CalScheduleRepeat::getString(std::string& text) const
{
	switch(unitType)
	{
	default:
	//case CalScheduleRepeat::UnitType::ONE_TIME:
	//	return "One-time event";
	case CalScheduleRepeat::UnitType::ONE_TIME:
		text = _L_("IDS_CLD_OPT_NEVER");
		return ;
	case CalScheduleRepeat::UnitType::DAY:
		text = _L_("IDS_CLD_OPT_DAILY");
		return;
	case CalScheduleRepeat::UnitType::WEEK:
		text = _L_("IDS_CLD_OPT_WEEKLY");
		return;
	case CalScheduleRepeat::UnitType::MONTH:
		text = _L_("IDS_CLD_OPT_MONTHLY");
		return;
	case CalScheduleRepeat::UnitType::YEAR:
		text = _L_("IDS_CLD_OPT_YEARLY");
		return;
	}
}

int CalScheduleRepeat::getFirstDayOfWeek() const
{
	int result = CalSettingsManager::getInstance().getFirstDayOfWeek();
	if (result == CalSettingsManager::LOCALES)
		return CalLocaleManager::getInstance().getLocaleFirstDayOfWeek();

	return result;
}

void CalScheduleRepeat::getItemString(const Date date, const char* timezone, std::string& text,char* textType, char* textUnitInterval, bool isMonth, bool isWeek, bool isNone) const
{
	char buffer[SCHEDULE_BUFFER * 2] = {0};

	char textUntilType[SCHEDULE_BUFFER] = {0};

	if (isNone) {
		snprintf(buffer, sizeof(buffer),"%s", _L_("IDS_CLD_OPT_NEVER"));
	} else {
		if (!isWeek) {
			char monthText[DATE_BUFFER] = {0};
			if(isMonth) {
				switch(unitInfo.monthly.type)
				{
					default:
					case CalScheduleRepeat::MONTHDAY:
						break;
					case CalScheduleRepeat::WEEKDAY:
						{
							std::string text;
							getMonthlyUnitInfoString(date, timezone, CalScheduleRepeat::WEEKDAY, text);
							snprintf(monthText, sizeof(monthText),"(%s)", text.c_str());
						}
						break;
				}
			}

			if (unitInterval == 1 || unitInterval == 0) {
				snprintf(textUntilType, sizeof(textUntilType),"%s%s", textType, monthText);
			} else {
				char intervalMonthText[DATE_BUFFER] = {0};
				snprintf(intervalMonthText, sizeof(intervalMonthText), "%s%s", textUnitInterval, monthText);
				snprintf(textUntilType, sizeof(textUntilType),_L_("IDS_CLD_BODY_EVERY_P1SD_P2SS"), unitInterval, intervalMonthText);
			}

		} else {

			int firstDayNumber = getFirstDayOfWeek();
			char weekText[SCHEDULE_BUFFER] = {0};
			static const char* weekTextString[] = {"IDS_CLD_OPT_SUNDAY", "IDS_CLD_OPT_MONDAY", "IDS_CLD_BODY_TUESDAY", "IDS_CLD_BODY_WEDNESDAY", "IDS_CLD_BODY_THURSDAY", "IDS_CLD_BODY_FRIDAY", "IDS_CLD_OPT_SATURDAY"};

			int i = 0;
			int selectedNumber = 0;
			int count[7] = {0};
			for (i = 0; i < 7; i++) {
				int tmp = (i + firstDayNumber) % 7;
				if(unitInfo.weekly.selected[tmp]) {
					count[selectedNumber] = tmp;
					selectedNumber++;
				}
			}

			std::string textString;

			if (selectedNumber == 0){
				strncpy(weekText, _L_(weekTextString[unitInfo.weekly.dafaultWeek]), sizeof(weekText) - 1);
				weekText[sizeof(weekText) - 1] = 0;
			} else {
				for ( i = 0; i < selectedNumber; i++) {
					textString.append(_L_(weekTextString[count[i]]));
					if (i != (selectedNumber - 1))
						textString.append(",");
				}
				strncpy(weekText, textString.c_str(), sizeof(weekText) - 1);
				weekText[sizeof(weekText) - 1] = 0;
			}
			WDEBUG("weekText = [%s]", weekText);

			char everyWeekText[SCHEDULE_BUFFER * 2] = {0};
			snprintf(everyWeekText, sizeof(everyWeekText), _L_("IDS_CLD_BODY_EVERY_PS_LC"), weekText);

			if (unitInterval == 1 || unitInterval == 0) {
				snprintf(textUntilType, sizeof(textUntilType),"%s (%s)", textType, everyWeekText);
			} else {
				snprintf(textUntilType, sizeof(textUntilType),_L_("IDS_CLD_BODY_EVERY_P1SD_P2SS_ON_P3SS"), unitInterval, textUnitInterval, weekText);
			}
		}

		char textTimes[DATE_BUFFER] = {0};
		snprintf(textTimes, sizeof(textTimes)," (%d %s)", untilInfo.times, _L_("IDS_CLD_OPT_TIMES_LC"));

		char textUntilDate[DATE_BUFFER] = {0};
		char textDate[DATE_BUFFER] = {0};
		snprintf(textDate, sizeof(textDate),"%d/%d/%d", untilInfo.date.mday, untilInfo.date.month, untilInfo.date.year);
		snprintf(textUntilDate, sizeof(textUntilDate), _L_("IDS_CLD_BODY_UNTIL_PS_LC"), textDate);

		switch(untilType)
		{
			default:
			case CalScheduleRepeat::UntilType::FOREVER:
				snprintf(buffer, sizeof(buffer),"%s", textUntilType);
				break;
			case CalScheduleRepeat::UntilType::TIMES:
				snprintf(buffer, sizeof(buffer),"%s%s", textUntilType, textTimes);
				break;
			case CalScheduleRepeat::UntilType::DUE_DATE:
				snprintf(buffer, sizeof(buffer),"%s; %s", textUntilType, textUntilDate);
				break;
		}
	}
	text = buffer;
}

void CalScheduleRepeat::getRepeatString(const Date date, const char* timezone, std::string& text) const
{
	switch(unitType)
	{
	default:
	case CalScheduleRepeat::UnitType::ONE_TIME:
		getItemString(date, timezone, text, _L_("IDS_CLD_OPT_NEVER"), NULL, false, false, true);
		return ;
	case CalScheduleRepeat::UnitType::DAY:
		getItemString(date, timezone, text, _L_("IDS_CLD_OPT_DAILY"), _L_("IDS_CLD_BODY_DAYS_M_DURATION_LC"), false, false, false);
		return;
	case CalScheduleRepeat::UnitType::WEEK:
		getItemString(date, timezone, text, _L_("IDS_CLD_OPT_WEEKLY"), _L_("IDS_CLD_BODY_WEEKS_M_DURATION_LC"), false, true, false);
		return;
	case CalScheduleRepeat::UnitType::MONTH:
		getItemString(date, timezone, text,_L_("IDS_CLD_OPT_MONTHLY"), _L_("IDS_CLD_OPT_MONTHS_LC_ABB"), true, false, false);
		return;
	case CalScheduleRepeat::UnitType::YEAR:
		getItemString(date, timezone, text,_L_("IDS_CLD_OPT_YEARLY"), _L_("IDS_CLD_OPT_YEARS_LC_ABB"), false, false, false);
		return;
	}
}

void CalScheduleRepeat::getMonthlyUnitInfoString(const Date date, const char* timezone, const MonthlyType type, std::string& text) const
{
	char buffer[SCHEDULE_BUFFER] = {0};
	WDEBUG("Monthly[%d/%d/%d", date.year, date.month, date.mday);
	if (type == CalScheduleRepeat::MonthlyType::WEEKDAY)
	{
		CalDateTime start(date.year, date.month, date.mday);
		int weekNum = CalLocaleManager::getInstance().getDayOfWeekInMonth(timezone, start.getUtimeFromTm());
		std::string text;
		CalLocaleManager::getInstance().getDateTimeText(CalLocaleManager::DATEFORMAT_14,
				CalLocaleManager::TIMEFORMAT_NONE, start, text);
		switch(weekNum)	{
			case 1:
				//format = C_("IDS_CLD_OPT_MONTHLY_HEVERY_FIRST_PS");
				snprintf(buffer, sizeof(buffer), _L_("IDS_CLD_OPT_EVERY_FIRST_PS_ABB"), text.c_str());
				break;
			case 2:
				//format = C_("IDS_CLD_OPT_MONTHLY_HEVERY_SECOND_PS");
				snprintf(buffer, sizeof(buffer), _L_("IDS_CLD_OPT_EVERY_SECOND_PS_ABB"), text.c_str());
				break;
			case 3:
				//format = C_("IDS_CLD_OPT_MONTHLY_HEVERY_THIRD_PS");
				snprintf(buffer, sizeof(buffer), _L_("IDS_CLD_OPT_EVERY_THIRD_PS_ABB"), text.c_str());
				break;
			case 4:
				//format = C_("IDS_CLD_OPT_MONTHLY_HEVERY_FOURTH_PS");
				snprintf(buffer, sizeof(buffer), _L_("IDS_CLD_OPT_EVERY_FOURTH_PS_ABB"), text.c_str());
				break;
			case 5:
			default:
				//format = C_("IDS_CLD_OPT_MONTHLY_HEVERY_FIFTH_PS");
				snprintf(buffer, sizeof(buffer), _L_("IDS_CLD_OPT_EVERY_FIFTH_PS_ABB"), text.c_str());
				break;
		}
	}
	else
	{
		// ex) On day 14
		snprintf(buffer, sizeof(buffer), _L_("IDS_CLD_OPT_ON_DAY_PD"), date.mday);
	}
	text = buffer;
	return ;
}

void CalScheduleRepeat::print() const
{
	WDEBUG("unitInterval[%d]",unitInterval);
	switch(unitType)
	{
	default:
	case CalScheduleRepeat::UnitType::DAY:
		WDEBUG("DAY");
		break;
	case CalScheduleRepeat::UnitType::WEEK:
		WDEBUG("WEEK");
		WDEBUG("%d/%d/%d/%d/%d/%d/%d",unitInfo.weekly.selected[0],
				unitInfo.weekly.selected[1],
				unitInfo.weekly.selected[2],
				unitInfo.weekly.selected[3],
				unitInfo.weekly.selected[4],
				unitInfo.weekly.selected[5],
				unitInfo.weekly.selected[6]);
		break;
	case CalScheduleRepeat::UnitType::MONTH:
		WDEBUG("MONTH");
		WDEBUG("MonthlyType:%d",unitInfo.monthly.type);
		WDEBUG("date:%d/%d/%d",unitInfo.monthly.date.year,
				unitInfo.monthly.date.month,
				unitInfo.monthly.date.mday);
		break;
	case CalScheduleRepeat::UnitType::YEAR:
		WDEBUG("YEAR");
		WDEBUG("MonthlyType:%d",unitInfo.monthly.type);
		WDEBUG("date:%d/%d/%d",unitInfo.monthly.date.year,
				unitInfo.monthly.date.month,
				unitInfo.monthly.date.mday);
		break;
	}

	switch(untilType)
	{
	case CalScheduleRepeat::UntilType::FOREVER:
		WDEBUG("FOREVER");
		break;
	case CalScheduleRepeat::UntilType::TIMES:
		WDEBUG("TIMES");
		WDEBUG("%d",untilInfo.times);
		break;
	case CalScheduleRepeat::UntilType::DUE_DATE:
		WDEBUG("DUE_DATE");
		WDEBUG("%d/%d/%d",untilInfo.date.year,
				untilInfo.date.month,
				untilInfo.date.mday);
		break;
	}
}
