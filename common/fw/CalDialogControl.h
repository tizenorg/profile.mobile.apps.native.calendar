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

class WAPP_ASSIST_EXPORT CalDialogControl : public CalControl
{
public:
	class Item : public WGenlistItem
	{
	public:
		Item(int sortIndex = 0);
		virtual ~Item();
	public:
		virtual void onSelect();
		const int getSortIndex() const;
		void setSortIndex( int index);
		int getSystemFontSize();
		bool isGroupTitle() const {return __isGroupTitle;}
	public:
		void setCustomData(void* customData) {__customData = customData;}
		void* getCustomData() const {return __customData;}
	private:
		int __sortIndex;
		void* __customData;
	protected:
		bool __isGroupTitle;
	friend class CalDialogControl;
	};
public:
	CalDialogControl();
	virtual ~CalDialogControl();
	virtual const char* getClassName();
public:
	virtual Elm_Object_Item* add(CalDialogControl::Item* item);
protected:
	static void onItemDelete(void *data, Evas_Object *obj, void *event_info);
private:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* param);
	virtual void onDestroy();
private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogControl);
};

#endif /* _CAL_DIALOG_CONTROL_H_ */
