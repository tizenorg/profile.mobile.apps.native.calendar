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

#ifndef _CAL_DIALOG_SETTINGS_TWO_TEXT_ITEM_H_
#define _CAL_DIALOG_SETTINGS_TWO_TEXT_ITEM_H_

#include "CalCommon.h"
#include "CalDialogControl.h"

/**
 * @brief Two text item in calendar's settings dialog
 */
class CalDialogSettingsTwoTextItem : public CalDialogControl::Item
{
public:
	/**
	 * @brief Sub item type
	 */
	enum sub_item_type
	{
		SUB_FIRST_DAY_OF_WEEK = 0,
		SUB_TIME_ZONE,
		SUB_ALERT_TYPE,
		SUB_REMINDER_TYPE,
		SUB_NOTIFICATION_SOUND,
	};

	/**
	 * @brief Construct two text item
	 *
	 * @param selectCb Select callback
	 * @param mainText Main text
	 * @param subText  Sub text
	 * @param type Sub item type
	 */
	CalDialogSettingsTwoTextItem(std::function<void (void)> selectCb, const char* mainText, const char* subText, sub_item_type type);

	/**
	 * @brief Default constructor
	 */
	CalDialogSettingsTwoTextItem();

	/**
	 * @brief Destructor
	 */
	virtual ~CalDialogSettingsTwoTextItem();
public:
	/**
	 * @brief Set main text
	 *
	 * @param text Main text
	 */
	void setMainText(const char* text);

	/**
	 * @brief Set sub text
	 *
	 * @param text Sub text
	 */
	void setSubText(const char* text);
private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogSettingsTwoTextItem);
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
	virtual void onSelect();
private:
	char* __mainText;
	char* __subText;
	std::function<void (void)> __selectCb;
	sub_item_type __type;
};

#endif /* _CAL_DIALOG_SETTINGS_TWO_TEXT_ITEM_H_ */
