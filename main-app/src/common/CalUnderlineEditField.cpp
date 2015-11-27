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
#include "CalUnderlineEditField.h"
#include "CalCommon.h"

CalUnderlineEditField::CalUnderlineEditField() :
		__layout(nullptr),
		__entry(nullptr),
		__changeCallback(nullptr),
		__entryCompleteCallback(nullptr),
		__maxLenReachCallback(nullptr)
{
}

CalUnderlineEditField::~CalUnderlineEditField()
{
	Ecore_IMF_Context* ctx = (Ecore_IMF_Context*)elm_entry_imf_context_get(__entry);
	ecore_imf_context_input_panel_event_callback_clear(ctx);
}

void CalUnderlineEditField::setText(const char* text)
{
	if (__entry)
	{
		elm_entry_entry_set(__entry, text);
	}
}

void CalUnderlineEditField::setGuideText(const char* text, int font)
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

void CalUnderlineEditField::setFontSize(int size)
{
	if (!__entry)
	{
		return;
	}

	std::stringstream ss;
	ss << "DEFAULT='font_size=" << size << "'";

	elm_entry_text_style_user_push(__entry, ss.str().c_str());
}

const char* CalUnderlineEditField::getText()
{
	if (!__entry)
	{
		return NULL;
	}

	return elm_entry_entry_get(__entry);
}

const char* CalUnderlineEditField::getGuideText()
{
	if (!__entry)
	{
		return NULL;
	}

	return elm_object_part_text_get(__entry, "elm.guide");
}

void CalUnderlineEditField::setChangeCallback(std::function<void (const char* text)> changeCallback)
{
	__changeCallback = changeCallback;
}

void CalUnderlineEditField::setCompleteCallback(std::function<void ()> completeCallback)
{
	__entryCompleteCallback = completeCallback;
}

void CalUnderlineEditField::setFocusToEntry(bool showKeypadIntentionally)
{
	if (__entry)
	{
		if (showKeypadIntentionally)
		{
			elm_entry_input_panel_show_on_demand_set(__entry, EINA_FALSE);
		}

		elm_object_focus_set(__entry, EINA_TRUE);
		elm_entry_cursor_end_set(__entry);
	}
}

void CalUnderlineEditField::setEntryReturnKeyType(ReturnKeyType type)
{
	if (__entry)
	{
		if (type == DONE)
		{
			elm_entry_input_panel_return_key_type_set(__entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
		}
		else
		{
			elm_entry_input_panel_return_key_type_set(__entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT);
		}
	}
}

void CalUnderlineEditField::setEntryMaxLenReachCallback(std::function<void ()> maxLenReachCallback, unsigned int maxLength)
{
	if (__entry)
	{
		static Elm_Entry_Filter_Limit_Size limit_filter_data = {0};
		limit_filter_data.max_char_count = maxLength;

		elm_entry_markup_filter_remove(__entry, elm_entry_filter_limit_size, NULL);
		elm_entry_markup_filter_append(__entry, elm_entry_filter_limit_size, &limit_filter_data);

		__maxLenReachCallback = maxLenReachCallback;
	}
}

Evas_Object* CalUnderlineEditField::onCreate( Evas_Object* parent, void* param )
{
	__layout = elm_layout_add(parent);
	elm_layout_theme_set(__layout, "layout", "editfield", "singleline");

	__entry = elm_entry_add(parent);
	evas_object_show(__entry);
	elm_entry_single_line_set(__entry, EINA_TRUE);
	elm_entry_scrollable_set(__entry, EINA_TRUE);
	elm_object_part_content_set(__layout, "elm.swallow.content", __entry);

	Evas_Object *button = elm_button_add(parent);
	elm_object_style_set(button, "editfield_clear");
	elm_object_focus_allow_set(button, EINA_FALSE);
	elm_object_part_content_set(__layout, "elm.swallow.button", button);

	evas_object_smart_callback_add(button, "clicked", [](void *data, Evas_Object *obj, void *event_info)
	{
		elm_entry_entry_set((Evas_Object*)data, "");
		elm_object_focus_set((Evas_Object*)data, EINA_TRUE);
	},
	__entry);

	auto entryChangeCallback = [](void* data, Evas_Object* obj, void* event_info)
	{
		CalUnderlineEditField* self = (CalUnderlineEditField*)data;
		char *text = elm_entry_markup_to_utf8(elm_entry_entry_get(obj));
		if (self->__changeCallback)
		{
			self->__changeCallback(text);
		}

		free(text);

		if (elm_entry_is_empty(obj))
		{
			elm_object_signal_emit(self->__layout, "elm,action,hide,button", "");
		}
		else
		{
			elm_object_signal_emit(self->__layout, "elm,action,show,button", "");
		}
	};

	evas_object_smart_callback_add(__entry, "changed", entryChangeCallback, this);
	evas_object_smart_callback_add(__entry, "preedit,changed", entryChangeCallback, this);
	evas_object_smart_callback_add(__entry, "activated", [](void *data, Evas_Object *obj, void *event_info)
	{
		CalUnderlineEditField* self = (CalUnderlineEditField*)data;
		if (self->__entryCompleteCallback)
		{
			self->__entryCompleteCallback();
		}

		if (ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE == elm_entry_input_panel_return_key_type_get(obj))
		{
			elm_entry_input_panel_hide(obj);
		}
	},
	this);

	evas_object_smart_callback_add(__entry, "maxlength,reached", [](void *data, Evas_Object *obj, void *event_info)
	{
		CalUnderlineEditField* self = (CalUnderlineEditField*)data;
		if (self->__maxLenReachCallback)
		{
			self->__maxLenReachCallback();
		}
	},
	this);

	return __layout;
}

void CalUnderlineEditField::onDestroy()
{
}
