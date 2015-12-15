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

#ifndef _CAL_LIST_CONTROL_H_
#define _CAL_LIST_CONTROL_H_

#include "CalControl.h"

#include <functional>

#include <Evas.h>
#include <Elementary.h>

#include "CalSearchChain.h"

#include "CalDateTime.h"

#define ITEM_SHOWING_THRESHOLD_HEIGHT 50

class CalListControl : public CalControl
{
public:
	class Item : public CalSearchChain::Node
	{
	public:
		Item();
		virtual ~Item();

	public:

		/**
		 * Query if this object is group title.
		 *
		 * @return	true if group title, false if not.
		 */
		bool isGroupTitle() const {return __isGroupTitle;}

		/**
		 * Gets elm object item.
		 *
		 * @return	null if it fails, else the elm object item.
		 */
		Elm_Object_Item* getElmObjectItem() {return __elmObjectItem;}

		/**
		 * Gets the index.
		 *
		 * @return	The index.
		 */
		int getIndex() const {return __index;}

		/**
		 * Gets the date.
		 *
		 * @return	The date.
		 */
		const CalDateTime& getDate() const {return __date;}

	protected:

		/**
		 * Gets item class static.
		 *
		 * @return	null if it fails, else the item class static.
		 */
		virtual Elm_Genlist_Item_Class* getItemClassStatic() = 0;

		/**
		 * Gets the string.
		 *
		 * @return	null if it fails, else the string.
		 */
		virtual const char* getString() {return "(some item)";}

	protected:
		bool __isGroupTitle;
		CalDateTime __date;

	private:

		/**
		 * Gets nearest group item.
		 *
		 * @param	dir	The dir.
		 *
		 * @return	null if it fails, else the nearest group item.
		 */
		Item* __getNearestGroupItem(int dir);

	private:
		Elm_Object_Item* __elmObjectItem;
		int __index;

	friend class CalListControl;
	};
public:
	CalListControl();
	virtual ~CalListControl();
	virtual const char* getClassName();
	void clear();

protected:

	/**
	 * Executes the create action.
	 *
	 * @param [in]	parent	If non-null, the parent.
	 * @param [in]	param 	If non-null, the parameter.
	 *
	 * @return	null if it fails, else an Evas_Object*.
	 */
	virtual Evas_Object* onCreate(Evas_Object* parent, void* param);

	/**
	 * Executes the destroy action.
	 */
	virtual void onDestroy();

	/**
	 * Adds an item.
	 *
	 * @param	[in]	item		If non-null, the item.
	 * @param			dir			The dir.
	 * @param			selectCb	The select cb.
	 *
	 * @return	null if it fails, else an Elm_Object_Item*.
	 */
	Elm_Object_Item* __addItem(CalListControl::Item* item, int dir, Evas_Smart_Cb selectCb);

	/**
	 * Gets the first item.
	 *
	 * @return	null if it fails, else the first item.
	 */
	Elm_Object_Item* __getFirstItem() {return elm_genlist_first_item_get(getEvasObj());}

	/**
	 * Gets the last item.
	 *
	 * @return	null if it fails, else the last item.
	 */
	Elm_Object_Item* __getLastItem() {return elm_genlist_last_item_get(getEvasObj());}

	/**
	 * Searches for the first item.
	 *
	 * @param	date	The date.
	 *
	 * @return	null if it fails, else the found item.
	 */
	CalListControl::Item* __findItem(const CalDateTime& date);

	/**
	 * Gets top showing item.
	 *
	 * @param	offset	The offset.
	 *
	 * @return	null if it fails, else the top showing item.
	 */
	CalListControl::Item* __getTopShowingItem(int offset = ITEM_SHOWING_THRESHOLD_HEIGHT);

	/**
	 * Gets top showing group item.
	 *
	 * @param	offset	The offset.
	 *
	 * @return	null if it fails, else the top showing group item.
	 */
	CalListControl::Item* __getTopShowingGroupItem(int offset = ITEM_SHOWING_THRESHOLD_HEIGHT);

	/**
	 * Gets the next item.
	 *
	 * @param [in]	item	If non-null, the item.
	 *
	 * @return	null if it fails, else the next item.
	 */
	CalListControl::Item* __getNextItem(CalListControl::Item* item);

	/**
	 * Gets the previous item.
	 *
	 * @param [in]	item	If non-null, the item.
	 *
	 * @return	null if it fails, else the previous item.
	 */
	CalListControl::Item* __getPreviousItem(CalListControl::Item* item);

	/**
	 * Gets the last appended index.
	 *
	 * @return	The last appended index.
	 */
	int getLastAppendedIndex() const {return __lastAppendedIndex;}

	/**
	 * Gets the last prepended index.
	 *
	 * @return	The last prepended index.
	 */
	int getLastPrependedIndex() const {return __lastPrependedIndex;}

protected:
	bool __scrollDir;

private:
	WDISABLE_COPY_AND_ASSIGN(CalListControl);

private:
	int __lastAppendedIndex;
	int __lastPrependedIndex;
};

#endif
