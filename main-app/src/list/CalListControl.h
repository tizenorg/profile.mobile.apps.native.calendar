/*
 * Copyright (c) 2009-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef _CAL_LIST_CONTROL_H_
#define _CAL_LIST_CONTROL_H_

#include "CalControl.h"

#include <functional>

#include <Evas.h>
#include <Elementary.h>

#include "CalSearchChain.h"

#include "CalDate.h"

#define ITEM_SHOWING_THRESHOLD_HEIGHT 50

/**
 * @brief Calendar list contol.
 *
 * @see CalControl
 */
class CalListControl : public CalControl
{
public:
	/**
	 * @brief Item.
	 *
	 * @see CalSearchChain::Node
	 */
	class Item : public CalSearchChain::Node
	{
	public:
		/**
		 * @brief Create item.
		 */
		Item();
		virtual ~Item();

		/**
		 * @brief Query if this object is group title.
		 *
		 * @return true if group title, false if not.
		 */
		bool isGroupTitle() const { return __isGroupTitle; }

		/**
		 * @brief Get elm object item.
		 *
		 * @return null if it fails, else the elm object item.
		 */
		Elm_Object_Item *getElmObjectItem() { return __elmObjectItem; }

		/**
		 * @brief Get the index.
		 *
		 * @return the index.
		 */
		int getIndex() const { return __index; }

		/**
		 * @brief Get the date.
		 *
		 * @return the date.
		 */
		const CalDate &getDate() const { return __date; }

		/**
		 * @brief Set checkable state.
		 *
		 * @param[in]   isCheckable    The checkable state.
		 */
		void setCheckable(bool isCheckable);

	protected:
		/**
		 * @brief Get item class static.
		 *
		 * @return null if it fails, else the item class static.
		 */
		virtual Elm_Genlist_Item_Class *getItemClassStatic() = 0;

		/**
		 * @brief Get the string.
		 *
		 * @return null if it fails, else the string.
		 */
		virtual const char* getString() { return "(some item)"; }

		/**
		 * @brief Get checkable state.
		 *
		 * @return checkable state.
		 */
		bool getCheckable() const;

		bool __isGroupTitle;
		CalDate __date;

	private:
		/**
		 * @brief Get nearest group item.
		 *
		 * @param[in]   dir     The dir.
		 *
		 * @return null if it fails, else the nearest group item.
		 */
		Item *__getNearestGroupItem(int dir);

		Elm_Object_Item *__elmObjectItem;
		int __index;
		bool __isCheckable;

		friend class CalListControl;
	};

	/**
	 * @brief Create calendar list control.
	 */
	CalListControl();
	virtual ~CalListControl();

	/**
	 * @brief Get class name.
	 *
	 * @return null if it fails, else the class name.
	 */
	virtual const char* getClassName();

	/**
	 * @brief Clear this object to its blank/initial state.
	 */
	void clear();

protected:
	/**
	 * @brief Execute the create action.
	 *
	 * @param[in]   parent  If non-null, the parent.
	 * @param[in]   param   If non-null, the parameter.
	 *
	 * @return null if it fails, else an Evas_Object*.
	 */
	virtual Evas_Object *onCreate(Evas_Object *parent, void *param);

	/**
	 * @brief Execute the destroy action.
	 */
	virtual void onDestroy();

	/**
	 * @brief Add an item.
	 *
	 * @param[in]   item        If non-null, the item.
	 * @param[in]   dir         The dir.
	 * @param[in]   selectCb    The select cb.
	 *
	 * @return null if it fails, else an Elm_Object_Item*.
	 */
	Elm_Object_Item *__addItem(CalListControl::Item *item, int dir, Evas_Smart_Cb selectCb);

	/**
	 * @brief Get the first item.
	 *
	 * @return null if it fails, else the first item.
	 */
	Elm_Object_Item *__getFirstItem() { return elm_genlist_first_item_get(getEvasObj()); }

	/**
	 * @brief Get the last item.
	 *
	 * @return null if it fails, else the last item.
	 */
	Elm_Object_Item *__getLastItem() { return elm_genlist_last_item_get(getEvasObj()); }

	/**
	 * @brief Search for the first item.
	 *
	 * @param[in]   date    The date.
	 *
	 * @return null if it fails, else the found item.
	 */
	CalListControl::Item *__findItem(const CalDate &date);

	/**
	 * @brief Get top showing item.
	 *
	 * @param[in]   offset  The offset.
	 *
	 * @return null if it fails, else the top showing item.
	 */
	CalListControl::Item *__getTopShowingItem(int offset = ITEM_SHOWING_THRESHOLD_HEIGHT);

	/**
	 * @brief Get top showing group item.
	 *
	 * @param[in]   offset  The offset.
	 *
	 * @return null if it fails, else the top showing group item.
	 */
	CalListControl::Item *__getTopShowingGroupItem(int offset = ITEM_SHOWING_THRESHOLD_HEIGHT);

	/**
	 * @brief Get the next item.
	 *
	 * @param[in]   item    If non-null, the item.
	 *
	 * @return null if it fails, else the next item.
	 */
	CalListControl::Item *__getNextItem(CalListControl::Item *item);

	/**
	 * @brief Get the previous item.
	 *
	 * @param[in]   item    If non-null, the item.
	 *
	 * @return null if it fails, else the previous item.
	 */
	CalListControl::Item *__getPreviousItem(CalListControl::Item *item);

	/**
	 * @brief Get the last appended index.
	 *
	 * @return the last appended index.
	 */
	int getLastAppendedIndex() const { return __lastAppendedIndex; }

	/**
	 * @brief Get the last prepended index.
	 *
	 * @return the last prepended index.
	 */
	int getLastPrependedIndex() const { return __lastPrependedIndex; }

	bool __scrollDir;

private:
	WDISABLE_COPY_AND_ASSIGN(CalListControl);

	int __lastAppendedIndex;
	int __lastPrependedIndex;
};

#endif
