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

#ifndef _CAL_DIALOG_EDIT_REPEAT_RADIO_ITEM_H_
#define _CAL_DIALOG_EDIT_REPEAT_RADIO_ITEM_H_

#include "CalDialogControl.h"

/**
 * @brief Radio item for repeat option
 */
class CalDialogEditRepeatRadioItem : public CalDialogControl::Item
{
public:
	/**
	 * @brief Create item
	 *
	 * @param selectCb Select callback
	 * @param changeCb Change callback
	 * @param radioIndex Radio index
	 * @param unitType Repeat unit type
	 * @param defaultUnitType Default repeat unit type
	 *
	 */
	CalDialogEditRepeatRadioItem(std::function<void (void)> selectCb, std::function<void (const CalScheduleRepeat::UnitType repeatType)> changeCb, int radioIndex, CalScheduleRepeat::UnitType unitType, CalScheduleRepeat::UnitType defaultUnitType);

	/**
	 * @brief Destructor
	 */
	virtual ~CalDialogEditRepeatRadioItem();
private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogEditRepeatRadioItem);
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
	virtual void onSelect();
private:
	std::function<void (const CalScheduleRepeat::UnitType repeatType)> __changeCb;
	//const CalScheduleRepeat __repeatOri;
	int __radioIndex;
	//CalScheduleRepeat __repeat;
	CalScheduleRepeat::UnitType __repeatType;
	std::function<void (void)> __selectCb;
	CalScheduleRepeat::UnitType __defaultRepeatType;
};


#endif

