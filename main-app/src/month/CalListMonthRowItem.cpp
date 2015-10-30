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

#include "CalListMonthRowItem.h"
#include "CalCommon.h"

#define DAYS_PER_WEEK 7

CalListMonthRowItem::CalListMonthRowItem(const CalDate& startDate, int num, CalTapRecognizer& cellTapRecognizer) :
	__startDate(startDate),
	__rowControl(NULL),
	__cellTapRecognizer(cellTapRecognizer)
{
	CalDate date(__startDate);
	for (int j = 0; j < DAYS_PER_WEEK; j++) {
		if (date.getMday() == 1) {
			WDEBUG("%s", date.getString());
			__isGroupTitle = true;
			break;
		}
		date.incrementDay();
	}
	__num = num;
}

CalListMonthRowItem::~CalListMonthRowItem()
{
}

CalListMonthRowItem* CalListMonthRowItem::search(int year, int month)
{
	WASSERT(__isGroupTitle);
	WDEBUG("%d/%d", year, month);

	const CalDate date(year, month, 1);
	if (CalDate::compareMonth(__getMonth(), date) == 0)
		return this;

	CalListMonthRowItem* item = this;
	if (CalDate::compareMonth(item->__getMonth(), date) < 0) {
		while (CalDate::compareMonth(item->__getMonth(), date) < 0) {
			WDEBUG("%s", item->__getMonth().getString());
			item = (CalListMonthRowItem*)item->getNext();
			if (item == NULL)
				return NULL;
		}
	} else {
		while (CalDate::compareMonth(item->__getMonth(), date) > 0) {
			WDEBUG("%s", item->__getMonth().getString());
			item = (CalListMonthRowItem*)item->getPrevious();
			if (item == NULL)
				return NULL;
		}
	}
	return item;
}

CalDate CalListMonthRowItem::__getMonth()
{
	if (__startDate.getMday() == 1) {
		return __startDate;
	} else {
		CalDate date(__startDate);
		date.incrementMonth();
		return date;
	}
}

Elm_Genlist_Item_Class* CalListMonthRowItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++
	itc.item_style = "1icon.calendar";
	itc.func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object* {
		CalListMonthRowItem* self = (CalListMonthRowItem*)data;
		return self->__createMonthRow(obj);
	};
	return &itc;
}

Evas_Object* CalListMonthRowItem::__createMonthRow(Evas_Object* parent)
{
	__rowControl = new CalMonthRowControl();
	__rowControl->create(parent, NULL);
	__rowControl->resetDayNums(0, __startDate, __num);
	__rowControl->receiveTouchInputFor(__cellTapRecognizer);
	return __rowControl->getEvasObj();
}

const char* CalListMonthRowItem::getString()
{
	return __startDate.getString();
}
