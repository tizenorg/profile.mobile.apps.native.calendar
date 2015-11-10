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

#ifndef _CAL_EDIT_FIELD_H_
#define _CAL_EDIT_FIELD_H_

#include <functional>
#include <list>
#include <Elementary.h>

#include "WDefine.h"
#include "CalControl.h"

/**
 * CalEditField should be used in Create/Edit view to support focus & keypad concept.
 * For other views, CalEditField is not needed.
*/

class WAPP_ASSIST_EXPORT CalEditField : public CalControl
{
public:
	CalEditField(bool multiLine = false);
	virtual ~CalEditField();
public:
	void setText(const char* text);
	void setGuideText(const char* text, int font);

	const char* getText();
	const char* getGuideText();

	void setFocusToEntry(bool showKeypadIntentionally = false);
	void setFontSize(int size);

	enum ReturnKeyType
	{
		DONE = 0,
		NEXT
	};
	void setEntryReturnKeyType(ReturnKeyType type);

	void setChangeCb(std::function<void (const char* text)> changeCb);
	void setEntryCompletedCb(std::function<void ()> entryCompletedCb);
	void setEntryMaxLenReachCb(std::function<void ()> maxLenReachCb, unsigned int maxLength = 1000);
	void setInputPanelStateChangeCb(std::function<void ()> inputPanelStateChangeCb);
	void setShowedCb(std::function<void ()> showedCb);

	/**
	 * Entry focus policy should be reset when app is resumed or popup is destroyed.
	*/
	static void resetFocusPolicy();

	static std::list<Evas_Object*> entryList;

protected:
	virtual Evas_Object* onCreate( Evas_Object* parent, void* param );
	virtual void onDestroy();

private:
	WDISABLE_COPY_AND_ASSIGN(CalEditField);

private:
	bool __multiLine;
	Evas_Object* __entry;
	std::function<void ()> __maxLenReachCb;
	std::function<void (const char* text)> __changeCb;
	std::function<void ()> __entryCompletedCb;
	std::function<void ()> __inputPanelStateChangeCb;
	std::function<void ()> __showedCb;
	static void __onEditFieldShowed(void* data, Evas* evas, Evas_Object* obj, void* event_info);
};

#endif
