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

#include <algorithm>
#include "CalEventItem.h"
#include "CalCommon.h"
#include "CalLocaleManager.h"

CalEventItem::CalEventItem(const CalDateTime& dayOfGroup, const std::shared_ptr<CalSchedule> schedule, bool isCheckboxRequired, bool isCheckboxSelected, const std::string& searchText):
	__onTranslateTapToGroup(nullptr),
	__schedule(schedule),
	__isCheckboxRequired(isCheckboxRequired),
	__isCheckboxSelected(isCheckboxSelected),
	__checkbox(nullptr),
	__searchText(searchText),
	__dateText()
{
	if(!__searchText.empty())
	{
		std::string lowerSearchText;
		lowerSearchText.resize(__searchText.size());
		std::transform(__searchText.begin(), __searchText.end(), lowerSearchText.begin(), std::ptr_fun(tolower));
		if(!lowerSearchText.empty())
		{
			__searchText = lowerSearchText;
		}
	}
	if(schedule)
	{
		CalDateTime eventStartDate;
		CalDateTime eventEndDate;

		schedule->getStart(eventStartDate);
		schedule->getEnd(eventEndDate);
		schedule->getDisplayTrimedSummary(-1, __title);
		if (!__title.empty())
		{
			if(!searchText.empty())
			{
				getHilightText(__title);
			}
		}
		else
		{
			__title = _L_("IDS_CLD_MBODY_MY_EVENT");
		}
		schedule->getDisplayTrimedLocation(-1, __location);
		if (!__location.empty())
		{
			if(!searchText.empty())
			{
				getHilightText(__location);
			}
		}
		if(schedule->getBookId() == DEFAULT_BIRTHDAY_CALENDAR_BOOK_ID)
		{
			__dateText = _L_("IDS_CLD_OPT_ALL_DAY_ABB2");
			return;
		}
		CalDateTime itemNextDay, groupDateTime(dayOfGroup.getYear(), dayOfGroup.getMonth(), dayOfGroup.getMday(), 0, 0, 0);

		std::string textStartTime, textEndTime;
		eventStartDate.getTimeString(textStartTime);
		eventEndDate.getTimeString(textEndTime);

		itemNextDay = groupDateTime;
		itemNextDay.addDays(1);
		if(groupDateTime >= eventStartDate && eventEndDate >= itemNextDay)
		{
			textStartTime = _L_("IDS_CLD_OPT_ALL_DAY_ABB2");
		}
		else if(!eventEndDate.isSameDay(groupDateTime))
		{
			textStartTime += " - ";
		}
		else if(!eventStartDate.isSameDay(groupDateTime))
		{
			textStartTime = " - " + textEndTime;
		}
		else if(textStartTime != textEndTime)
		{
			textStartTime += " - " + textEndTime;
		}
		__dateText = textStartTime;
	}
}

CalEventItem::~CalEventItem()
{
}

bool CalEventItem::isCheckboxRequired()
{
	return __isCheckboxRequired;
}

bool CalEventItem::isCheckboxSelected()
{
	return __isCheckboxSelected;
}

void CalEventItem::setCheckObject(Evas_Object* checkbox)
{
	__checkbox = checkbox;
}

void CalEventItem::setCheckboxSelected(bool isCheckboxSelected)
{
	__isCheckboxSelected = isCheckboxSelected;
	if(__checkbox)
	{
		elm_check_state_set(__checkbox, isCheckboxSelected);
	}
}
const std::shared_ptr<CalSchedule> CalEventItem::getSchedule()
{
	return __schedule;
}

const char* CalEventItem::getTitle()
{
	return __title.c_str();
}

const char* CalEventItem::getLocation()
{
	return __location.c_str();
}

const char* CalEventItem::getDateInterval()
{
	return __dateText.c_str();
}

void CalEventItem::setOnTapCallback(std::function<void (CalEventItem* item)> onTap)
{
	__onTranslateTapToGroup = onTap;
}

std::function<void (CalEventItem* item)> CalEventItem::getOnTapCallback()
{
	return __onTranslateTapToGroup;
}

void CalEventItem::getHilightText(std::string& hilightText)
{
	if(__searchText.length() == 0 || hilightText.length() == 0)
	{
		return;
	}

	std::string lowerText;
	lowerText.resize(hilightText.size());
	std::transform(hilightText.begin(), hilightText.end(), lowerText.begin(), std::ptr_fun(tolower));

	int startPos = lowerText.find(__searchText);
	if (startPos != (int)lowerText.npos)
	{
		int endPos = startPos + __searchText.length();

		hilightText.insert(endPos, "</match>");
		hilightText.insert(startPos, "<match>");
	}
}
