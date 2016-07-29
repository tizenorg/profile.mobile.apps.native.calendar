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

#ifndef _CAL_DIALOG_SETTINGS_ONE_TEXT_ONE_ICON_ITEM_H_
#define _CAL_DIALOG_SETTINGS_ONE_TEXT_ONE_ICON_ITEM_H_

#include "CalCommon.h"
#include "CalDialogControl.h"

/**
 * @brief Genlist item with text and icon in calendar settings
 */
class CalDialogSettingsOneTextOneIconItem: public CalDialogControl::Item
{
public:
	/**
	 * @brief Create item with icon and text
	 *
	 * @param selectCb Select callback
	 * @param Text     Text that appears in item
	 */
	CalDialogSettingsOneTextOneIconItem(std::function<void (void)> selectCb, const char* Text);

	/**
	 * @brief Default constructor
	 */
	CalDialogSettingsOneTextOneIconItem();

	/**
	 * @brief Destructor
	 */
	virtual ~CalDialogSettingsOneTextOneIconItem();

	/**
	 * @brief Set radio button check callback
	 *
	 * @param radioCb Check callback
	 */
	void setRadioCb(std::function<void (void)> radioCb);

public:
	/**
	 * @brief Set item text
	 *
	 * @param text Item text
	 */
	void setText(const char* text);

	/**
	 * @brief Get item content
	 *
	 * @return content on success, otherwise NULL
	 */
	Evas_Object * getIconObject();
private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogSettingsOneTextOneIconItem);
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
	virtual void onSelect();
private:
	char* __text;
	Evas_Object* __checkbox;
	std::function<void (void)> __selectCb;
	std::function<void (void)> __radioCb;
};

#endif /* _CAL_DIALOG_SETTINGS_ONE_TEXT_ONE_ICON_ITEM_H_ */
