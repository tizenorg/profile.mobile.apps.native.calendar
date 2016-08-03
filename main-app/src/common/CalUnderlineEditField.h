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

/**
 * @brief Calendar underline edit field.
 *
 * @see CalControl.
 */
class WAPP_ASSIST_EXPORT CalUnderlineEditField : public CalControl
{
public:
	/**
	 * @brief Return key types.
	 */
	enum ReturnKeyType
	{
		DONE = 0,
		NEXT
	};

	/**
	 * @brief Create underline edit field.
	 *
	 * @param[in]   multiLine   Multiline state.
	 */
	CalUnderlineEditField(bool multiLine = false);

	virtual ~CalUnderlineEditField();

	/**
	 * @brief Set text.
	 *
	 * @param[in]   text   The text.
	 */
	void setText(const char* text);

	/**
	 * @brief Set guide text.
	 *
	 * @param[in]   text   The guide text.
	 * @param[in]   font   The text font.
	 */
	void setGuideText(const char* text, int font = 40);

	/**
	 * @brief Set font size.
	 *
	 * @param[in]   size   The font size.
	 */
	void setFontSize(int size);

	/**
	 * @brief Get text.
	 *
	 * @return The text.
	 */
	const char* getText();

	/**
	 * @brief Get guide text.
	 *
	 * @return The guide text.
	 */
	const char* getGuideText();

	/**
	 * @brief Set entry return key type.
	 *
	 * @param[in]   type   Return key type.
	 *
	 * @see ReturnKeyType
	 */
	void setEntryReturnKeyType(ReturnKeyType type);

	/**
	 * @brief Set focus to entry.
	 *
	 * @param[in]   showKeypadIntentionally   Show keypad intentionally state.
	 */
	void setFocusToEntry(bool showKeypadIntentionally = false);

	/**
	 * @brief Set change callback.
	 *
	 * @param[in]   changeCallback   Change callback.
	 */
	void setChangeCallback(std::function<void (const char* text)> changeCallback);

	/**
	 * @brief Set complete callback to entry.
	 *
	 * @param[in]   entryCompleteCallback   Complete callback.
	 */
	void setCompleteCallback(std::function<void ()> entryCompleteCallback);

	/**
	 * @brief Set maximum length reach callback to entry.
	 *
	 * @param[in]   maxLenReachCallback   Maximum length reach callback.
	 * @param[in]   maxLength             Maximum length.
	 */
	void setEntryMaxLenReachCallback(std::function<void ()> maxLenReachCallback, unsigned int maxLength = 1000);

protected:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* param);
	virtual void onDestroy();

private:
	WDISABLE_COPY_AND_ASSIGN(CalUnderlineEditField);

	bool __multiLine;
	Evas_Object* __layout;
	Evas_Object* __entry;
	std::function<void (const char* text)> __changeCallback;
	std::function<void ()> __entryCompleteCallback;
	std::function<void ()> __maxLenReachCallback;
};

#endif
