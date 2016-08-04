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

#ifndef _CAL_DIALOG_EDIT_REPEAT_UNTIL_TIMES_ITEM_H_
#define _CAL_DIALOG_EDIT_REPEAT_UNTIL_TIMES_ITEM_H_

/**
 * @brief Represents item with radio and repeat times button
 */
class CalDialogEditRepeatUntilTimesItem : public CalDialogControl::Item
{
public:
	/**
	 * @brief Create item
	 *
	 * @param changedCb  Changed callback
	 * @param repeatOri  Repeat model
	 * @param radioIndex Radio index
	 *
	 */
	CalDialogEditRepeatUntilTimesItem(std::function<void (CalScheduleRepeat::UntilType untilType, CalScheduleRepeat::UntilInfo untilInfo)> changedCb, const CalScheduleRepeat repeatOri, int radioIndex);

	/**
	 * @brief Destructor
	 */
	virtual ~CalDialogEditRepeatUntilTimesItem();

	/**
	 * @brief Set date button clicked callback
	 *
	 * @param dateButtonClickedCb Callback
	 *
	 */
	void setDateButtonClickedCb(std::function<void (Evas_Object* dateTime)> dateButtonClickedCb);

	/**
	 * @brief Set button time
	 *
	 * @param repeat Repeat info
	 *
	 */
	void setDateButtonTime(const CalScheduleRepeat repeat);
private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogEditRepeatUntilTimesItem);
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
	void onSelect();
private:
	std::function<void (CalScheduleRepeat::UntilType untilType, CalScheduleRepeat::UntilInfo untilInfo)> __changedCb;
	std::function<void (Evas_Object* dateTime)> __dateButtonClickedCb;
	const CalScheduleRepeat __repeat;
	int __radioIndex;
	CalScheduleRepeat::UntilInfo __untilInfo;
	Evas_Object *__radio;
	Evas_Object *__button;
};

#endif

