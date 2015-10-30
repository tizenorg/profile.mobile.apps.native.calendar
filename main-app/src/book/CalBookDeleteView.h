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

#ifndef _CAL_BOOK_DELETE_VIEW_H_
#define _CAL_BOOK_DELETE_VIEW_H_

#include <memory>
#include <map>
#include "CalView.h"
#include "CalDialogControl.h"
#include "CalDialogBookLocalItem.h"
#include "CalBook.h"
//#include "CalTextPopup.h"

class CalBookDeleteView : public CalView
{
public:
	CalBookDeleteView ();
	virtual ~CalBookDeleteView();
private:
	WDISABLE_COPY_AND_ASSIGN(CalBookDeleteView);
private:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);
	virtual void onPushed(Elm_Object_Item* naviItem);
	virtual void onDestroy();
	virtual void onEvent(const CalEvent& event);
	void __onSelect(void *data);
	void __onCheckBox(bool state);
	void __onSelectAllSelect(bool is_selectall);
	void __updateSelectInfo();
	void __update();
	void __onDeleteButton();
	void __deleteBooks();

private:
	int __checkedCount;
	int __allItemsCount;
	Evas_Object *__deleteBtn;
	Evas_Object *__selectAllCheckbox;
	CalDialogControl *__dialog;
//	CalTextPopup *__popup;
	std::map<int, std::shared_ptr<CalBook>> __bookMap;
	Evas_Object* __selectAllLayout;
};

#endif /* _CAL_BOOK_DELETE_VIEW_H_ */

