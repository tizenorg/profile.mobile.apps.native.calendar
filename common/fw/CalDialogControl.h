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

#ifndef _CAL_DIALOG_CONTROL_H_
#define _CAL_DIALOG_CONTROL_H_

#include "CalControl.h"

#include <memory>
#include <Evas.h>
#include <Elementary.h>

#include "WDefine.h"
#include "WGenlistItem.h"

#define TEXT_ITEM_DEFAULT_SIZE 40
#define TEXT_MAX_GOOD_SIZE 60
#define FORMAT_BUFFER 128
#define COUNT_FORMAT "<p font_size=%d>%d</p>"
#define LABEL_FORMAT "<p font_size=%d>%s</p>"

/**
 * @brief Calendar dialog control.
 *
 * @see CalControl
 */
class WAPP_ASSIST_EXPORT CalDialogControl : public CalControl
{
public:
	/**
	 * @brief Genlist item.
	 *
	 * @see WGenlistItem
	 */
	class Item : public WGenlistItem
	{
	public:
		/**
		 * @brief Create item.
		 *
		 * @param[in]   sortIndex   Sort index
		 */
		Item(int sortIndex = 0);

		virtual ~Item();

		/**
		 * @brief Called when item is selected.
		 */
		virtual void onSelect();

		/**
		 * @brief Get sort index.
		 *
		 * @return Sort index.
		 */
		const int getSortIndex() const;

		/**
		 * @brief Set sort index.
		 *
		 * @param[in]   index   Sort index
		 */
		void setSortIndex(int index);

		/**
		 * @brief Get system font size.
		 *
		 * @return System font size.
		 */
		int getSystemFontSize();

		/**
		 * @brief Called when item is selected.
		 *
		 * @return Whether the item is a group title.
		 */
		bool isGroupTitle() const {return __isGroupTitle;}

		/**
		 * @brief Set custom data.
		 *
		 * @param[in]   customData   Custom data.
		 */
		void setCustomData(void* customData) {__customData = customData;}

		/**
		 * @brief Get custom data.
		 *
		 * @return Custom data.
		 */
		void* getCustomData() const {return __customData;}

		/**
		 * @brief Called when item is realized (became visible).
		 */
		virtual void onRealized() {}

	protected:
		using WGenlistItem::onSelect;

		bool __isGroupTitle;

	private:
		int __sortIndex;
		void* __customData;

		friend class CalDialogControl;
	};

public:
	CalDialogControl();

	virtual ~CalDialogControl();

	/**
	 * @brief Get Class name.
	 *
	 * @return Class name.
	 */
	virtual const char* getClassName();

	/**
	 * @brief Add item to Dialog control.
	 *
	 * @param[in]   item   Item.
	 *
	 * @return Object item.
	 */
	virtual Elm_Object_Item* add(CalDialogControl::Item* item);

protected:
	static void onItemDelete(void *data, Evas_Object *obj, void *event_info);

private:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* param);
	virtual void onDestroy();

	WDISABLE_COPY_AND_ASSIGN(CalDialogControl);
};

#endif /* _CAL_DIALOG_CONTROL_H_ */
