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

#include <sstream>
#include "CalEditField.h"
#include "CalCommon.h"

std::list<Evas_Object*> CalEditField::entryList;

CalEditField::CalEditField(bool multiLine) :
	__entry(NULL),
	__maxLenReachCb(NULL),
	__changeCb(NULL),
	__entryCompletedCb(NULL),
	__inputPanelStateChangeCb(NULL),
	__showedCb(NULL)
{
	__multiLine = multiLine;
}

CalEditField::~CalEditField()
{
	Ecore_IMF_Context* ctx = (Ecore_IMF_Context*)elm_entry_imf_context_get(__entry);
	ecore_imf_context_input_panel_event_callback_clear(ctx);
	if (__entry)
		CalEditField::entryList.remove(__entry);
}

void CalEditField::setText(const char* text)
{
	if (!__entry) {
		WERROR("entry is null");
		return;
	}

	elm_entry_entry_set(__entry, text);
}

void CalEditField::setGuideText(const char* text, int font)
{
	if (__entry)
	{
		if(font)
		{
			std::stringstream ss;
			ss << "<p font_size=" << font << ">" << _L_(text) << "</p>";

			elm_object_part_text_set(__entry, "elm.guide", ss.str().c_str());
		}
		else
		{
			elm_object_part_text_set(__entry, "elm.guide", _L_(text));
		}
	}
}

void CalEditField::setFontSize(int size)
{
	if (!__entry)
	{
		return;
	}

	std::stringstream ss;
	ss << "DEFAULT='font_size=" << size << "'";

	elm_entry_text_style_user_push(__entry, ss.str().c_str());
}

const char* CalEditField::getText()
{
	if (!__entry) {
		WERROR("entry is null");
		return NULL;
	}

	return elm_entry_entry_get(__entry);
}

const char* CalEditField::getGuideText()
{
	if (!__entry) {
		WERROR("entry is null");
		return NULL;
	}

	return elm_object_part_text_get(__entry, "elm.guide");
}

void CalEditField::setFocusToEntry(bool showKeypadIntentionally)
{
	if (!__entry) {
		WERROR("entry is null");
		return;
	}

	if (showKeypadIntentionally)
		elm_entry_input_panel_show_on_demand_set(__entry, EINA_FALSE);

	elm_object_focus_set(__entry, EINA_TRUE);
	elm_entry_cursor_end_set(__entry);
}

void CalEditField::setEntryReturnKeyType(ReturnKeyType type)
{
	if (!__entry) {
		WERROR("entry is null");
		return;
	}

	if (__multiLine)
		return;

	if (type == DONE)
		elm_entry_input_panel_return_key_type_set(__entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
	else
		elm_entry_input_panel_return_key_type_set(__entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT);
}

void CalEditField::setChangeCb(std::function<void (const char* text)> changeCb)
{
	__changeCb = changeCb;
}

void CalEditField::setEntryCompletedCb(std::function<void ()> entryCompletedCb)
{
	if (__multiLine)
		return;

	__entryCompletedCb = entryCompletedCb;
}

void CalEditField::setEntryMaxLenReachCb(std::function<void ()> maxLenReachCb, unsigned int maxLength)
{
	if (!__entry) {
		WERROR("entry is null");
		return;
	}

	Elm_Entry_Filter_Limit_Size limit_filter_data = {0};
	limit_filter_data.max_char_count = maxLength;

	elm_entry_markup_filter_remove(__entry, elm_entry_filter_limit_size, NULL);
	elm_entry_markup_filter_append(__entry, elm_entry_filter_limit_size, &limit_filter_data);

	__maxLenReachCb = maxLenReachCb;
}

void CalEditField::setShowedCb(std::function<void ()> showedCb)
{
	__showedCb = showedCb;
}

void CalEditField::resetFocusPolicy()
{
	for (auto it = CalEditField::entryList.begin(); it != CalEditField::entryList.end();) {
		/* Entry should be reset when app is resumed or popup is destroyed to show keypad automatically. */
		elm_entry_input_panel_show_on_demand_set(*it, EINA_FALSE);
		++it;
	}
}

void CalEditField::__onEditFieldShowed(void* data, Evas* evas, Evas_Object* obj, void* event_info)
{
	CalEditField *self = (CalEditField*)data;
	if (self->__showedCb) {
		self->__showedCb();
	}
	evas_object_event_callback_del(self->__entry, EVAS_CALLBACK_SHOW, self->__onEditFieldShowed);
}

Evas_Object* CalEditField::onCreate( Evas_Object* parent, void* param )
{
	__entry = elm_entry_add(parent);

	if(__multiLine)
		elm_entry_single_line_set(__entry, EINA_FALSE);
	else
		elm_entry_single_line_set(__entry, EINA_TRUE);

	CAL_ASSERT(__entry);

	elm_entry_scrollable_set(__entry, EINA_TRUE);

	CalEditField::entryList.push_back(__entry);

	elm_entry_autocapital_type_set(__entry, ELM_AUTOCAPITAL_TYPE_SENTENCE);
	elm_entry_cnp_mode_set(__entry, ELM_CNP_MODE_PLAINTEXT);

	auto entryChangeCb = [](void* data, Evas_Object* obj, void* event_info) {
			CalEditField* self = (CalEditField*)data;
			char *text = elm_entry_markup_to_utf8(elm_entry_entry_get(obj));
			if (self->__changeCb)
				self->__changeCb(text);
			if (text)
				free(text);
		};

	evas_object_smart_callback_add(__entry, "changed", entryChangeCb, this);
	evas_object_smart_callback_add(__entry, "preedit,changed", entryChangeCb, this);

	evas_object_smart_callback_add(__entry, "activated",
		[](void *data, Evas_Object *obj, void *event_info) {
			CalEditField* self = (CalEditField*)data;
			if (self->__entryCompletedCb)
				self->__entryCompletedCb();

			if (ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE == elm_entry_input_panel_return_key_type_get(obj))
				elm_entry_input_panel_hide(obj);
		},
		this);

	evas_object_smart_callback_add(__entry, "maxlength,reached",
		[](void *data, Evas_Object *obj, void *event_info) {
			CalEditField* self = (CalEditField*)data;
			if (self->__maxLenReachCb)
				self->__maxLenReachCb();
		},
		this);

	evas_object_event_callback_add(__entry, EVAS_CALLBACK_SHOW, __onEditFieldShowed, this);

	/*
		Default value of elm_entry_input_panel_show_on_demand() is EINA_FALSE.

		With default value, when entry is focused, keypad will be shown automatically.

		With non-default value(EINA_TRUE), when entry is focused, keypad will not be shown automatically.
		Keypad will be shown when user clicked elm_entry.

		# Policy
		1. All entry should be have same value for elm_entry_input_panel_show_on_demand().
			When keypad is shonw, set EINA_TRUE to all entry.
			When keypad is hidden, set EINA_FALSE to all entry.

		2. Keypad will be shown below case.
			When Create/Edit view is pushed.
			When new elm_entry is created by user action.
			When elm_entry is clicked.
	*/
	Ecore_IMF_Context* ctx = (Ecore_IMF_Context*)elm_entry_imf_context_get(__entry);
	ecore_imf_context_input_panel_event_callback_add(ctx, ECORE_IMF_INPUT_PANEL_STATE_EVENT,
		[](void *data, Ecore_IMF_Context *ctx, int value) {
			for (auto it = CalEditField::entryList.begin(); it != CalEditField::entryList.end();) {
				elm_entry_input_panel_show_on_demand_set(*it, value == ECORE_IMF_INPUT_PANEL_STATE_SHOW ? EINA_FALSE : EINA_TRUE);
				++it;
			}
			CalEditField* self = (CalEditField*)data;
			if (self->__inputPanelStateChangeCb)
				self->__inputPanelStateChangeCb();
		},
		this);

	return __entry;
}

void CalEditField::onDestroy()
{
}

void CalEditField::setInputPanelStateChangeCb(std::function<void ()> inputPanelStateChangeCb)
{
	__inputPanelStateChangeCb = inputPanelStateChangeCb;
}
