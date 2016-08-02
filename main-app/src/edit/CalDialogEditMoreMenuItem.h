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

#ifndef _CAL_DIALOG_EDIT_MORE_MENU_ITEM_H_
#define _CAL_DIALOG_EDIT_MORE_MENU_ITEM_H_

#include "CalDialogControl.h"

/**
 * @brief More popup item
 */
class CalDialogEditMoreMenuItem : public CalDialogControl::Item
{
public:
	/**
	 * @Brief More popup type
	 */
	enum TYPE
	{
		PARTICIPANT = 0,//!< PARTICIPANT
		SHOWME,         //!< SHOWME
		PRIVACY,        //!< PRIVACY
		STATUS,         //!< STATUS
		DESCRIPTION,    //!< DESCRIPTION
		TIMEZONE        //!< TIMEZONE
	};
public:
	/**
	 * @brief Default constructor
	 */
	CalDialogEditMoreMenuItem();

	/**
	 * @brief Create item
	 *
	 * @param type Item type
	 * @param text Item text
	 */
	CalDialogEditMoreMenuItem(CalDialogEditMoreMenuItem::TYPE type, const char *text);

	/**
	 * @brief Destructor
	 */
	virtual ~CalDialogEditMoreMenuItem();

	/**
	 * @brief Set select callback
	 *
	 * @param selectCb Item select callback
	 */
	void setSelectCb(std::function<void (CalDialogEditMoreMenuItem::TYPE type)> selectCb);
private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogEditMoreMenuItem);
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
	virtual void onSelect();
private:
	const char *__text;
	CalDialogEditMoreMenuItem::TYPE  __type;
	std::function<void (TYPE type)> __selectCb;
};

#endif

