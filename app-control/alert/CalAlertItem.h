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

#ifndef _CAL_ALERT_ITEM_H_
#define _CAL_ALERT_ITEM_H_

#include "CalDialogControl.h"
#include "CalSchedule.h"
#include "CalBook.h"

class CalAlertItem : public CalDialogControl::Item
{
public:
	CalAlertItem(const std::shared_ptr<CalSchedule>& schedule);
	virtual ~CalAlertItem();
public:
	// TODO please remove CalBook
	void setSelectCb(std::function<void (void)> selectCb);
	Evas_Object * getCheckObject();
private:
	WDISABLE_COPY_AND_ASSIGN(CalAlertItem);
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
private:
	virtual void onSelect();
	virtual void onCheck();
private:
	std::shared_ptr<CalSchedule> __schedule;
	Evas_Object *__check;
	int __count;
	std::function<void (void)> __selectCb;
	std::function<void (void)> __checkCb;
};

#endif

