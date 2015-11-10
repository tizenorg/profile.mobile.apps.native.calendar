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

class CalDialogEditMoreItem: public CalDialogControl::Item
{
public:
	enum ButtonType {
		LOCATION = 1,
		REPEAT,
		REMINDER,
		MORE,
	};
public:
	CalDialogEditMoreItem(std::function<void (ButtonType buttonType, Evas_Object* button)> buttonClickedCb, int sortIndex, bool disableLocation, bool disableRepeat,  bool disableReminder);
	CalDialogEditMoreItem();
	virtual ~CalDialogEditMoreItem();
public:
	void setButtonDisable(ButtonType buttonType, bool isDisable);
	void getButtonDisable(ButtonType buttonType, bool& isDisable);
	void setButtonStatus(bool locationDisabled, bool reminderDisable, bool repeatDisabled, bool MoreDisabled);
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
