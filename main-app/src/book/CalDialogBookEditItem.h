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

#ifndef _CAL_DIALOG_BOOK_EDIT_ITEM_H_
#define _CAL_DIALOG_BOOK_EDIT_ITEM_H_

#include "CalDialogControl.h"
#include "CalBook.h"

class CalDialogBookEditItem : public CalDialogControl::Item
{
public:
	CalDialogBookEditItem(const char* defaultText, const char* guideText, const std::shared_ptr<CalBook>& book = nullptr);
	virtual ~CalDialogBookEditItem();
public:
	void setColorChangedCb(std::function<void ()> colorSelectedCb);
	void setTitleChangedCb(std::function<void ()> colorSelectedCb);
	const char* getTitle();
	void getColor(int& r, int& g, int& b, int& a) const;
	void setColor(int r, int g, int b, int a);
	void setFocusToEntry();
	void setEntryMaxLenReachCb(std::function<void ()> maxLenReachCb);
private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogBookEditItem);
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
protected:
	Evas_Object* createEntry(Evas_Object* parent);
private:
	std::function<void ()> __maxLenReachCb;
	std::shared_ptr<CalBook> __book;
	std::function<void ()> __colorChangedCb;
	std::function<void ()> __titleChangedCb;
	Evas_Object* __color;
	char* __defaultText;
	char* __guideText;
	int __r;
	int __g;
	int __b;
	int __a;
	Evas_Object* __entry;
	char* __entryText;
};

#endif
