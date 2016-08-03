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

#ifndef _WMENU_POPUP_H_
#define _WMENU_POPUP_H_

#include <Evas.h>
#include <Elementary.h>
#include <functional>
#include "WControl.h"

/**
* @class WMenuPopup.
*
* @brief This class is the menu pop-up class to make easy to manage menus.
*
* The %WMenuPopup is the menu pop-up class.
*/
class WAPP_ASSIST_EXPORT WMenuPopup: public WControl
{
public:
	/**
	 * This is the default constructor for this class.
	 */
	WMenuPopup();

	/**
	 * @brief Initialize and pre-creates menu pop-up to append menu items.
	 *
	 * @param[in]   win         Parent window evas object.
	 * @param[in]   naviframe   Naviframe evas object of the caller.
	 *
	 * @see create()
	 */
	void prepare(Evas_Object* win, Evas_Object* naviframe);

	/**
	 * @brief Append menu items.
	 *
	 * @param[in]   label          Label text of menu item.
	 * @param[in]   selectItemCb   Callback function object when the item is selected.
	 *
	 * @return Appended item. NULL, if failed.
	 */
	Elm_Object_Item* appendItem(const char* label, std::function< void()> selectItemCb);

	/**
	 * @brief Create and show menu pop-up.
	 *
	 * @see onCreate()
	 */
	void createMenu();

protected:
	/**
	 * @brief This is the destructor for this class.
	 */
	virtual ~WMenuPopup();

private:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* param);
	virtual void onDestroy();

	WDECLARE_PRIVATE_IMPL(WMenuPopup);
	WDISABLE_COPY_AND_ASSIGN(WMenuPopup);
};

#endif // _WMENU_POPUP_H_
