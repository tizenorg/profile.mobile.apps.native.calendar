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

#include "CalCommon.h"
#include "CalBook.h"
#include "CalEditDialogControl.h"
#include "CalDialogBookEditItem.h"
#include <Elementary.h>
#include "CalBookManager.h"
#include "CalBookEditView.h"

CalDialogBookEditItem::CalDialogBookEditItem(const char* defaultText, const char* guideText, const std::shared_ptr<CalBook>& book)
	:__maxLenReachCb(NULL)
	, __r(0)
	, __g(0)
	, __b(0)
	, __a(0)
	, __entry(NULL)
	, __entryText(NULL)
{
	__defaultText = g_strdup(defaultText);
	__guideText = g_strdup(guideText);
	if (book)
		book->getColor(__r, __g, __b, __a);
	else {
		__r = 0;
		__g = 89;
		__b = 87;
		__a = 255;
	}
	__book = book;
	__color = NULL;
}

CalDialogBookEditItem::~CalDialogBookEditItem()
{
	g_free(__defaultText);
	g_free(__guideText);

	if(__entryText) {
		g_free(__entryText);
	}
}

void CalDialogBookEditItem::setTitleChangedCb(std::function<void ()> titleChangedCb)
{
	__titleChangedCb = titleChangedCb;
}

void CalDialogBookEditItem::setColorChangedCb(std::function<void ()> colorChangedCb)
{
	__colorChangedCb = colorChangedCb;
}

Evas_Object* CalDialogBookEditItem::createEntry(Evas_Object* parent)
{
	__entry = elm_entry_add(parent);
	elm_entry_single_line_set(__entry, EINA_TRUE);

	evas_object_size_hint_weight_set(__entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(__entry, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_entry_autocapital_type_set(__entry, ELM_AUTOCAPITAL_TYPE_SENTENCE);
	elm_entry_cnp_mode_set(__entry, ELM_CNP_MODE_PLAINTEXT);
	elm_entry_input_panel_return_key_type_set(__entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);

	if (__guideText)
	{
		elm_object_part_text_set(__entry, "elm.guide", _L_G_(__guideText));
	}
	if (__defaultText) {
		char* inputText = elm_entry_utf8_to_markup(__defaultText);
		elm_entry_entry_set(__entry, inputText);
		if (inputText) {
			free(inputText);
		}
	}

	Elm_Entry_Filter_Limit_Size limit_filter_data;
	limit_filter_data.max_char_count = 1000;

	elm_entry_markup_filter_append(__entry, elm_entry_filter_limit_size, &limit_filter_data);
	evas_object_smart_callback_add(__entry, "maxlength,reached", [](void *data, Evas_Object *obj, void *event_info)
		{
			CalDialogBookEditItem* self = (CalDialogBookEditItem*)data;
			if(self->__maxLenReachCb)
				self->__maxLenReachCb();
		},
		this);

	auto entryChangeCb = [](void* data, Evas_Object* obj, void* event_info) {
			CalDialogBookEditItem* self = (CalDialogBookEditItem*)data;
			char* input = elm_entry_markup_to_utf8(elm_object_text_get(self->__entry));
			if(CAL_STRCMP(self->__entryText, input)){
				if(self->__entryText) {
					free(self->__entryText);
					self->__entryText = NULL;
				}
				self->__entryText = input;
			} else {
				free(input);
			}
			self->__titleChangedCb();
		};

	evas_object_smart_callback_add(__entry, "changed", entryChangeCb, this);
	evas_object_smart_callback_add(__entry, "preedit,changed", entryChangeCb, this);

	evas_object_smart_callback_add(__entry, "activated", [](void *data, Evas_Object *obj, void *event_info){
		if (ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE == elm_entry_input_panel_return_key_type_get(obj)) {
			elm_entry_input_panel_hide(obj);
		}
	},NULL);

	return __entry;
}

Elm_Genlist_Item_Class* CalDialogBookEditItem::getItemClassStatic()
{
	WENTER();
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++
	itc.item_style = "icon.entry";

	itc.func.content_get = [](void *data, Evas_Object *obj, const char *part)->Evas_Object*
	{
		CalDialogBookEditItem *item = (CalDialogBookEditItem *)data;
		if (strcmp(part, "elm.swallow.textfield") == 0) {
			return item->createEntry(obj);
		} else if (strcmp(part, "elm.swallow.color") == 0) {
			Evas_Object* color = elm_layout_add(obj);
			WASSERT(color);
			elm_layout_file_set(color, CAL_IMAGE_DIR "core_color_picker_palette.png", NULL);
			evas_object_resize(color, 60, 60);
			evas_object_propagate_events_set(color, EINA_FALSE);
			evas_object_size_hint_weight_set(color, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(color, EVAS_HINT_FILL, EVAS_HINT_FILL);
			item->__color = color;

			evas_object_color_set(color, item->__r, item->__g, item->__b, item->__a);
			evas_object_event_callback_add(color, EVAS_CALLBACK_MOUSE_UP,
				[](void *data, Evas *e, Evas_Object *obj, void *eventInfo) {
					CalDialogBookEditItem *item = (CalDialogBookEditItem *)data;
					evas_object_color_get(obj, &item->__r, &item->__g, &item->__b, &item->__a);
					if (item->__colorChangedCb)
						item->__colorChangedCb();
				}
				, item);
			return color;
		} else {
			return NULL;
		}
		return NULL;
	};
	return &itc;
}

const char* CalDialogBookEditItem::getTitle()
{
	return __entryText;
}

void CalDialogBookEditItem::getColor(int& r, int& g, int& b, int& a) const
{
	r = __r;
	g = __g;
	b = __b;
	a = __a;
}

void CalDialogBookEditItem::setColor(int r, int g, int b, int a)
{
	WENTER();
	__r = r;
	__g = g;
	__b = b;
	__a = a;
	if (__color)
		evas_object_color_set(__color, r, g, b, a);
}

void CalDialogBookEditItem::setFocusToEntry()
{
	WENTER();
	if(__entry){
		evas_object_show(__entry);
		elm_object_focus_set(__entry, EINA_TRUE);
		elm_entry_cursor_end_set(__entry);
	}
	WLEAVE();
}

void CalDialogBookEditItem::setEntryMaxLenReachCb(std::function<void ()> maxLenReachCb)
{
	__maxLenReachCb = maxLenReachCb;
}


