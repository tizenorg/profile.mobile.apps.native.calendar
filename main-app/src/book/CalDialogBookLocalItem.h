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

/**
 * @brief Calendar dialog book local item.
 *
 * @see CalDialogControl::Item.
 */
class CalDialogBookLocalItem : public CalDialogControl::Item
{
public:
	/**
	 * @brief Calendar book local modes.
	 */
	enum CalBookLocalMode
	{
		LocalBookDisplay = 0,
		LocalBookChange,
		LocalBookOnlySelect,
		LocalBookDelete,
	};

	/**
	 * @brief Create dialog book local item.
	 *
	 * @param[in]   item_mode   Item mode.
	 * @param[in]   book        Shared pointer to CalBook.
	 *
	 * @see CalBookLocalMode
	 * @see CalBook
	 */
	CalDialogBookLocalItem(CalBookLocalMode item_mode, const std::shared_ptr<CalBook>& book);

	virtual ~CalDialogBookLocalItem();

	/**
	 * @brief Set select callback.
	 *
	 * @param[in]   selectCb   Select callback.
	 */
	void setSelectCb(std::function<void (const std::shared_ptr<CalBook>& book)> selectCb);

	/**
	 * @brief Set delete select callback.
	 *
	 * @param[in]   selectCb   Delete select callback.
	 */
	void setDeleteSelectCb(std::function<void (CalDialogBookLocalItem *item)> deleteSelectCb);

	/**
	 * @brief Set check callback.
	 *
	 * @param[in]   selectCb   Check callback.
	 */
	void setCheckboxCb(std::function<void (bool state)> checkboxCb);

	/**
	 * @brief Get local book.
	 *
	 * @return Shared pointer to CalBook.
	 *
	 * @see CalBook
	 */
	std::shared_ptr<CalBook> getLocalBook() const;

	/**
	 * @brief Get item check state.
	 *
	 * @return Item check state.
	 */
	bool getItemCheckState();

	/**
	 * @brief Set item check state.
	 *
	 * @param[in]   state   Item check state.
	 */
	void setItemCheckState(bool state);

private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogBookLocalItem);
	virtual Elm_Genlist_Item_Class* getItemClassStatic();

	virtual void onSelect();
	void onCheckboxCb();

	Evas_Object *__check;
	std::shared_ptr<CalBook> __book;
	std::function<void (const std::shared_ptr<CalBook>& book)> __selectCb;
	std::function<void (CalDialogBookLocalItem *item)> __deleteSelectCb;
	std::function<void (bool state)> __checkboxCb;
	CalBookLocalMode __item_mode;
};

#endif

