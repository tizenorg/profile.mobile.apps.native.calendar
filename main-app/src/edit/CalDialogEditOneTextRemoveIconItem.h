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

#ifndef _CAL_DIALOG_ONE_TEXT_REMOVE_ICON_ITEM_H_
#define _CAL_DIALOG_ONE_TEXT_REMOVE_ICON_ITEM_H_

#include <functional>
#include <string>

#include "CalDialogControl.h"

/**
 * @brief Item with text and remove icon
 */
class CalDialogEditOneTextRemoveIconItem : public CalDialogControl::Item
{
public:
	/**
	 * @brief Create Item
	 *
	 * @param sortIndex Item sort index
	 * @param subText   Item text
	 */
	CalDialogEditOneTextRemoveIconItem(int sortIndex, const char* subText);

	/**
	 * @brief Destructor
	 */
	virtual ~CalDialogEditOneTextRemoveIconItem();
public:
	/**
	 * @brief Set select callback
	 *
	 * @param selectCb Select callback
	 */
	void setSelectCb(std::function<void (void)> selectCb);

	/**
	 * @brief Set remove callback
	 *
	 * @param removeCb Remove callback
	 */
	void setRemoveCb(std::function<void (void)> removeCb);
public:
	/**
	 * @brief Set text
	 *
	 * @param text Item text
	 */
	void setSubText(const char* text);
private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogEditOneTextRemoveIconItem);
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
	virtual void onSelect();
private:
	std::string __subText;
	std::function<void (void)> __removeCb;
	std::function<void (void)> __selectCb;
};

#endif
