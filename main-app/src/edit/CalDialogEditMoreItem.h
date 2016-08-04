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

#ifndef _CAL_DIALOG_MORE_ITEM_H_
#define _CAL_DIALOG_MORE_ITEM_H_

#include <functional>

#include "CalDialogControl.h"

/**
 * @brief Item with buttons to add primary options to edit view
 */
class CalDialogEditMoreItem: public CalDialogControl::Item
{
public:
	/**
	 * @brief Primary button type
	 */
	enum ButtonType {
		LOCATION = 1,
		REPEAT,
		REMINDER,
		MORE,
	};
public:
	/**
	 * @brief Create more item
	 *
	 * @param buttonClickedCb Button clicked callback
	 * @param sortIndex       Item sort index
	 * @param disableLocation Determines whether location button is disabled
	 * @param disableRepeat   Determines whether repeat button is disabled
	 * @param disableReminder Determines whether reminder button is disabled
	 *
	 */
	CalDialogEditMoreItem(std::function<void (ButtonType buttonType, Evas_Object* button)> buttonClickedCb, int sortIndex, bool disableLocation, bool disableRepeat,  bool disableReminder);

	/**
	 * @brief Default constructor
	 */
	CalDialogEditMoreItem();

	/**
	 * @brief Destructor
	 */
	virtual ~CalDialogEditMoreItem();
public:
	/**
	 * @brief Set button disable state
	 *
	 * @param buttonType Button type @see ButtonType
	 * @param isDisable Should be button disabled or not
	 *
	 */
	void setButtonDisable(ButtonType buttonType, bool isDisable);

	/**
	 * @brief Get button disabled state
	 *
	 * @param buttonType     Button type @see ButtonType
	 * @param[out] isDisable Is disabled
	 *
	 */
	void getButtonDisable(ButtonType buttonType, bool& isDisable);

	/**
	 * @brief Set item buttons disabled state
	 *
	 * @param locationDisabled Is location disabled
	 * @param reminderDisable  Is reminder disabled
	 * @param repeatDisabled   Is repeat disabled
	 * @param MoreDisabled     Is more disabled
	 *
	 */
	void setButtonStatus(bool locationDisabled, bool reminderDisable, bool repeatDisabled, bool MoreDisabled);

	/**
	 * @brief Get button
	 *
	 * @param buttonType Button type @see ButtonType
	 *
	 * @return Button on success, otherwise NULL
	 */
	Evas_Object* getButton(ButtonType buttonType);
private:
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
	Evas_Object* onCreateIcon(Evas_Object* parent, const char* icon_name, ButtonType buttonType);
private:
	std::function<void (ButtonType buttonType, Evas_Object* button)> __buttonClickedCb;
	Evas_Object* __button1;
	Evas_Object* __button2;
	Evas_Object* __button3;
	Evas_Object* __button4;
	bool __disableLocation;
	bool __disableRepeat;
	bool __disableReminder;
	bool __disableMore;
};

#endif /* _CAL_DIALOG_MORE_ITEM_H_ */
