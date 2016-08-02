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

#ifndef _CAL_DIALOG_EDIT_REPEAT_UNTIL_FOREVER_ITEM_H_
#define _CAL_DIALOG_EDIT_REPEAT_UNTIL_FOREVER_ITEM_H_

#include "CalDialogControl.h"

/**
 * @brief Until forever radio item
 */
class CalDialogEditRepeatUntilForeverItem : public CalDialogControl::Item
{
public:
	/**
	 * @Brief Create item
	 *
	 * @param ChangedCb Changed callback
	 * @param repeatOri Repeat model
	 * @param radioIndex Radio index
	 *
	 */
	CalDialogEditRepeatUntilForeverItem(std::function<void (CalScheduleRepeat::UntilType untilType, CalScheduleRepeat::UntilInfo untilInfo)> ChangedCb, const CalScheduleRepeat repeatOri, int radioIndex);

	/**
	 * @brief Destructor
	 */
	virtual ~CalDialogEditRepeatUntilForeverItem();
private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogEditRepeatUntilForeverItem);
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
	virtual void onSelect();
private:
	std::function<void (CalScheduleRepeat::UntilType untilType, CalScheduleRepeat::UntilInfo untilInfo)> __changedCb;
	const CalScheduleRepeat __repeat;
	int __radioIndex;
};

#endif

