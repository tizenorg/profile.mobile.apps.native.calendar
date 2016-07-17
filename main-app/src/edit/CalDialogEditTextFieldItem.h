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

#ifndef _CAL_DIALOG_TEXTFIELD_ITEM_H_
#define _CAL_DIALOG_TEXTFIELD_ITEM_H_

#include <Elementary.h>
#include <functional>

#include "CalDialogControl.h"
#include "CalUnderlineEditField.h"

class CalDialogEditTextFieldItem: public CalDialogControl::Item
{
public:
	CalDialogEditTextFieldItem(int sortIndex, std::function<void (CalUnderlineEditField* editField)> onCreateEditField,
	std::function<void (CalUnderlineEditField* editField)> onFocusSet);
	CalDialogEditTextFieldItem();
	virtual ~CalDialogEditTextFieldItem();
public:
	CalUnderlineEditField* getEditField();
	virtual void onRealized() override;
private:
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
	void onFocusPending();
private:
	static void onEditFieldDestroyed(void* data, Evas* evas, Evas_Object* obj, void* event_info);
protected:
	Evas_Object* createEntry(Evas_Object* parent);
protected:
	CalUnderlineEditField* __editField;
	std::function<void (CalUnderlineEditField* editField)> __onCreateEditField;
	std::function<void (CalUnderlineEditField* editField)> __onFocusSet;
};

#endif /* _CAL_DIALOG_TEXTFIELD_ITEM_H_ */
