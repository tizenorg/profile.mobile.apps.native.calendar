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

#ifndef _CAL_DIALOG_BOOK_LOCAL_ITEM_H_
#define _CAL_DIALOG_BOOK_LOCAL_ITEM_H_

#include "CalDialogControl.h"
#include "CalBook.h"

class CalDialogBookLocalItem : public CalDialogControl::Item
{
public:
	enum CalBookLocalMode
	{
		LocalBookDisplay = 0,
		LocalBookChange,
		LocalBookOnlySelect,
		LocalBookDelete,
	};
public:
	CalDialogBookLocalItem(CalBookLocalMode item_mode, const std::shared_ptr<CalBook>& book);
	virtual ~CalDialogBookLocalItem();
public:
	void setSelectCb(std::function<void (const std::shared_ptr<CalBook>& book)> selectCb);
	void setDeleteSelectCb(std::function<void (CalDialogBookLocalItem *item)> deleteSelectCb);
	void setCheckboxCb(std::function<void (bool state)> checkboxCb);
	std::shared_ptr<CalBook> getLocalBook() const;
private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogBookLocalItem);
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
public:
	bool getItemCheckState();
	void setItemCheckState(bool state);
private:
	virtual void onSelect();
	void onCheckboxCb();
private:
	Evas_Object *__check;
	std::shared_ptr<CalBook> __book;
	std::function<void (const std::shared_ptr<CalBook>& book)> __selectCb;
	std::function<void (CalDialogBookLocalItem *item)> __deleteSelectCb;
	std::function<void (bool state)> __checkboxCb;
	CalBookLocalMode __item_mode;
};

#endif

