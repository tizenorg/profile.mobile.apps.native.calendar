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

#ifndef _CAL_BOOK_EDIT_VIEW_H_
#define _CAL_BOOK_EDIT_VIEW_H_

#include <memory>
#include "CalView.h"
#include "CalBook.h"
#include "CalDialogBookEditItem.h"
#include "CalDialogBookLocalItem.h"


class CalBookEditView : public CalView
{
public:
	enum CalBookMode
	{
		CREATE = 0,
		EDIT,
	};
public:
	CalBookEditView (CalBookMode itemMode, const std::shared_ptr<CalBook>& book = NULL);
	virtual ~CalBookEditView ();
private:
	WDISABLE_COPY_AND_ASSIGN(CalBookEditView);

	/**
	 * Transition finished cb.
	 *
	 * @param [in]	data	 	If non-null, the data.
	 * @param [in]	obj		 	If non-null, the object.
	 * @param [in]	eventInfo	If non-null, information describing the event.
	 */
	static void __transitionFinishedCb(void* data, Evas_Object* obj, void* eventInfo);

private:

	/**
	 * Executes the create action.
	 *
	 * @param [in]	parent   	If non-null, the parent.
	 * @param [in]	viewParam	If non-null, the view parameter.
	 *
	 * @return	null if it fails, else an Evas_Object*.
	 */
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);

	/**
	 * Executes the pushed action.
	 *
	 * @param [in]	naviItem	If non-null, the navi item.
	 */
	virtual void onPushed(Elm_Object_Item* naviItem);

	/**
	 * Executes the destroy action.
	*/
	virtual void onDestroy();

	/**
	 * Executes the save action.
	*/
	void __onSave();

	/**
	 * Updates this object.
	*/
	void __update();

	/**
	 * Removes the space in string end described by str.
	 *
	 * @param [in]	str	If non-null, the string.
	 */
	void __removeSpaceInStrEnd(char* str);

	/** Gets edit item text. */
	void __getEditItemText();

	/**
	 * Executes the event action.
	 *
	 * @param	event	The event.
	 */
	void onEvent(const CalEvent& event);

	/** Updates the view. */
	void __updateView();

protected:

	/**
	 * Executes the pop action.
	 *
	 * @return	true if it succeeds, false if it fails.
	 */
	virtual bool onPop();

private:
	std::shared_ptr<CalBook> __book;
	CalDialogBookEditItem* __editItem;
	CalBookMode __itemMode;
	Evas_Object* __saveButton;
	char *__bookName;
	bool __isDiscard;
	CalDialogControl *__dialog;
	bool __isPrepareExit;
};

#endif /* _CAL_BOOK_EDIT_VIEW_H_ */
