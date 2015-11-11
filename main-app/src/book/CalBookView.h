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

#ifndef _CAL_BOOK_VIEW_H_
#define _CAL_BOOK_VIEW_H_

#include <memory>
#include "CalView.h"
#include "CalBook.h"
#include "CalDialogControl.h"
#include "CalDialogBookLocalItem.h"
#include "CalEvent.h"
#include "CalCommon.h"
#include <map>
#include <vector>

/**
 * A cal book view.
 */
class CalBookView : public CalView
{
public:
	CalBookView ();
	virtual ~CalBookView ();

private:
	WDISABLE_COPY_AND_ASSIGN(CalBookView);

	/**
	 * Executes the create action. This function is called when CalBookView object is created.
	 *
	 * @param [in]	parent   	The parent object.
	 * @param [in]	viewParam	Parameters for view creation.
	 *
	 * @return	null if it fails, else an created Evas_Object*.
	 */
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);

	/**
	 * Executes the pushed action. This function is called when our view was successfully pushed into naviframe container.
	 *
	 * @param [in]	naviItem	If non-null, the navi item.
	 */
	virtual void onPushed(Elm_Object_Item* naviItem);

	/**
	 * Executes the destroy action. This function is called when our view object is being destroyed.
	 */
	virtual void onDestroy();

	/**
	 * Updates this object.
	 */
	void __update();

	/**
	 * Executes some action, depending on event it receives.
	 *
	 * @param	event	The event.
	 */
	void onEvent(const CalEvent& event);

private:
	CalDialogControl* __dialog;
	std::map<int, std::shared_ptr<CalBook>> __localBookMap;
	std::vector<CalDialogBookLocalItem*> __localBookItems;
};

#endif /* _CAL_BOOK_VIEW_H_ */
