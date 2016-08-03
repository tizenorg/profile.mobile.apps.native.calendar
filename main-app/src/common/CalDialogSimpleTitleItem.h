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

#ifndef _CAL_DIALOG_SIMPLE_TITLE_ITEM_H_
#define _CAL_DIALOG_SIMPLE_TITLE_ITEM_H_

/**
 * @brief Calendar dialog simple title item.
 *
 * @see CalDialogControl::Item.
 */
class CalDialogSimpleTitleItem : public CalDialogControl::Item
{
public:
	/**
	 * @brief Create dialog simple title item.
	 *
	 * @param[in]   title   The title.
	 */
	CalDialogSimpleTitleItem(const char *title);
	virtual ~CalDialogSimpleTitleItem();

private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogSimpleTitleItem);
	virtual Elm_Genlist_Item_Class* getItemClassStatic();

	const char *__title;
};

#endif

