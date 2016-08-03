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

#ifndef _CAL_DIALOG_EDIT_REPEAT_UNIT_ITEM_H_
#define _CAL_DIALOG_EDIT_REPEAT_UNIT_ITEM_H_

/**
 * @brief Repeat unit item
 */
class CalDialogEditRepeatUnitItem : public CalDialogControl::Item
{
public:
	/**
	 * @brief Create item
	 *
	 * @param changedCb Changed callback
	 * @param repeatOri Repeat model
	 * @param unitType  Unit type
	 *
	 */
	CalDialogEditRepeatUnitItem(std::function<void (CalScheduleRepeat::UnitType unitType, int unnitInterval)> changedCb, const CalScheduleRepeat repeatOri, CalScheduleRepeat::UnitType unitType);

	/**
	 * @brief Destructor
	 */
	virtual ~CalDialogEditRepeatUnitItem();

	/**
	 * @brief Set Date button time
	 *
	 * @param repeat Repeat
	 *
	 */
	void setDateButtonTime(const CalScheduleRepeat repeat);

	/**
	 * @brief Set date button clicked callback
	 *
	 * @param dateButtonClickedCb Date button clicked callback
	 *
	 */
	void setDateButtonClickedCb(std::function<void (void)> dateButtonClickedCb);

	/**
	 * @brief Update units label
	 */
	void updateUnitsLabel();
private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogEditRepeatUnitItem);
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
private:
	std::function<void (CalScheduleRepeat::UnitType unitType, int unnitInterval)> __changedCb;
	std::function<void (void)> __dateButtonClickedCb;
	const CalScheduleRepeat __repeat;
	CalScheduleRepeat::UnitType __unitType;
	int __unitInterval;
	Evas_Object *__button;
};

#endif

