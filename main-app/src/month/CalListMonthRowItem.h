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

#ifndef _CAL_LIST_MONTH_ROW_ITEM_H_
#define _CAL_LIST_MONTH_ROW_ITEM_H_

#include "CalListControl.h"

#include "CalMonthRowControl.h"

#include "CalTapRecognizer.h"

class CalListMonthRowItem : public CalListControl::Item
{
public:
	CalListMonthRowItem(const CalDateTime& startDate, int num, CalTapRecognizer& cellTapRecognizer);
	virtual ~CalListMonthRowItem();
public:
	CalListMonthRowItem* search(int year, int month);
	const char* dump() {return getString();}
public:
	CalMonthRowControl& getRowControl() {return *__rowControl;}
	const CalDateTime& getStartDate() {return __startDate;}
private:
	CalDateTime __getMonth();
	Evas_Object* __createMonthRow(Evas_Object* parent);
private:
	WDISABLE_COPY_AND_ASSIGN(CalListMonthRowItem);
private:
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
	virtual const char* getString();
private:
	CalDateTime __startDate;
	int __num;
	CalMonthRowControl* __rowControl;
private:
	CalTapRecognizer& __cellTapRecognizer;
};

#endif
