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

#ifndef _CAL_DIALOG_EDIT_OPTIONAL_TEXT_FIELD_ITEM_H_
#define _CAL_DIALOG_EDIT_OPTIONAL_TEXT_FIELD_ITEM_H_

#include <functional>

#include "CalDialogEditTextFieldItem.h"
#include "CalEditField.h"

/**
 * @brief Optional text item
 */
class CalDialogEditOptionalTextFieldItem : public CalDialogControl::Item
{
public:
	/**
	 * @brief Create item
	 *
	 * @param sortIndex         Sort index
	 * @param onCreateEditField onCreate callback
	 * @param multiLine         Is multiline
	 *
	 */
	CalDialogEditOptionalTextFieldItem(int sortIndex, std::function<void (CalUnderlineEditField* editField)> onCreateEditField, bool multiLine = false);

	/**
	 * @brief Default constructor
	 */
	CalDialogEditOptionalTextFieldItem();

	/**
	 * @brief Destructor
	 */
	virtual ~CalDialogEditOptionalTextFieldItem();

public:
	/**
	 * @brief Set remove callback
	 *
	 * @param removeCb Remove callback
	 *
	 */
	void setRemoveCb(std::function<void ()> removeCb);

	/**
	 * @brief Get editfield
	 *
	 * @return Editfield on success, otherwise false
	 */
	CalUnderlineEditField* getEditField();

private:
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
	Evas_Object* createEntry(Evas_Object* parent);
	Evas_Object* createRemoveButton(Evas_Object* parent);

private:
	bool __multiLine;
	CalUnderlineEditField* __editField;
	std::function<void ()> __removeCb;
	std::function<void (CalUnderlineEditField* editField)> __onCreateEditField;
};

#endif /* _CAL_DIALOG_EDIT_OPTIONAL_TEXT_FIELD_ITEM_H_ */
