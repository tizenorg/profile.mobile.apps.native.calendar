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

#ifndef _CAL_UNDERLINE_FIELD_H_
#define _CAL_UNDERLINE_FIELD_H_

#include <Elementary.h>
#include <functional>
#include "WDefine.h"
#include "CalControl.h"

class WAPP_ASSIST_EXPORT CalUnderlineEditField : public CalControl
{
public:
	CalUnderlineEditField(bool multiLine = false);
	virtual ~CalUnderlineEditField();
public:
	void setText(const char* text);
	void setGuideText(const char* text, int font = 40);
	void setFontSize(int size);

	const char* getText();
	const char* getGuideText();

	enum ReturnKeyType
	{
		DONE = 0,
		NEXT
	};

	void setEntryReturnKeyType(ReturnKeyType type);
	void setFocusToEntry(bool showKeypadIntentionally = false);
	void setChangeCallback(std::function<void (const char* text)> changeCallback);
	void setCompleteCallback(std::function<void ()> entryCompleteCallback);
	void setEntryMaxLenReachCallback(std::function<void ()> maxLenReachCallback, unsigned int maxLength = 1000);

protected:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* param);
	virtual void onDestroy();

private:
	WDISABLE_COPY_AND_ASSIGN(CalUnderlineEditField);

private:
	bool __multiLine;
	Evas_Object* __layout;
	Evas_Object* __entry;
	std::function<void (const char* text)> __changeCallback;
	std::function<void ()> __entryCompleteCallback;
	std::function<void ()> __maxLenReachCallback;
};

#endif
