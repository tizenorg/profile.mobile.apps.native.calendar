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
 * @brief CalEditField should be used in Create/Edit view to support focus & keypad concept.
 *
 * @remark For other views, CalEditField is not needed.
 *
 * @see CalControl
 */
class WAPP_ASSIST_EXPORT CalEditField : public CalControl
{
public:
	/**
	 * @brief Create editfield
	 *
	 * @param multiline Is multiline
	 *
	 */
	CalEditField(bool multiLine = false);

	/**
	 * @breif Destructor
	 */
	virtual ~CalEditField();
public:
	/**
	 * @brief Set text
	 *
	 * @param text Text to set
	 *
	 */
	void setText(const char* text);

	/**
	 * @brief Set guide text
	 *
	 * @param text Text
	 * @param font Font
	 *
	 */
	void setGuideText(const char* text, int font);

	/**
	 * @brief Get text
	 *
	 * @return text
	 */
	const char* getText();

	/**
	 * @brief Get guide text
	 *
	 * @return Guide text
	 */
	const char* getGuideText();

	/**
	 * @brief Set focus to entry
	 *
	 * @param showKeypadIntentionally Is show keypad intentionally
	 */
	void setFocusToEntry(bool showKeypadIntentionally = false);

	/**
	 * @brief Set font size
	 *
	 * @param size Text size
	 */
	void setFontSize(int size);

	/**
	 * @brief SIP return key type
	 */
	enum ReturnKeyType
	{
		DONE = 0,
		NEXT
	};

	/**
	 * @brief Set entry return key type
	 *
	 * @see ReturnKeyType
	 *
	 * @param type Return key type
	 */
	void setEntryReturnKeyType(ReturnKeyType type);

	/**
	 * @brief Set change callback
	 *
	 * @param changeCb Change callback
	 *
	 */
	void setChangeCb(std::function<void (const char* text)> changeCb);

	/**
	 * @brief Set entry complete callback
	 *
	 * @param entryCompleteCb Entry complete callback
	 *
	 */
	void setEntryCompletedCb(std::function<void ()> entryCompletedCb);

	/**
	 * @brief Set entry max length callback
	 *
	 * @param maxLenReachCb Callback
	 * @param maxLength Max length
	 *
	 */
	void setEntryMaxLenReachCb(std::function<void ()> maxLenReachCb, unsigned int maxLength = 1000);

	/**
	 * @brief Set input panel state change callback
	 *
	 * @param inputPanelStateChangeCb Callback
	 *
	 */
	void setInputPanelStateChangeCb(std::function<void ()> inputPanelStateChangeCb);

	/**
	 * @brief Set showed callback
	 *
	 * @param showedCb Callback
	 *
	 */
	void setShowedCb(std::function<void ()> showedCb);

	/**
	 * @brief Entry focus policy should be reset when app is resumed or popup is destroyed.
	 */
	static void resetFocusPolicy();

	/**
	 * @brief Entry list
	 */
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
