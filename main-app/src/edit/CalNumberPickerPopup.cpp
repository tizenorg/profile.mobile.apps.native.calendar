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
#include "CalNumberPickerPopup.h"
#include "CalEventManager.h"
#include "WControl.h"
#include "CalPath.h"

#define MIN_UNIT_VALUE 1
#define MAX_UNIT_VALUE 99

CalNumberPickerPopup::CalNumberPickerPopup(const CalScheduleRepeat& repeat, bool isTime)
{
	__repeat = repeat;
	__isTime = isTime;
	__unitValueSp = NULL;

	if (isTime) {
		if (repeat.untilInfo.times) {
			__number = repeat.untilInfo.times;
		} else {
			__number = 10;
		}

	} else {
		if (repeat.unitInterval) {
			__number = repeat.unitInterval;
		} else {
			__number = 1;
		}

	}
}

CalNumberPickerPopup::~CalNumberPickerPopup()
{
	WHIT();
}

void CalNumberPickerPopup::setDoneCb(std::function<void (const CalScheduleRepeat& repeat)> doneCb)
{
	__doneCb = doneCb;
}

void CalNumberPickerPopup::setCancelCb(std::function<void (void)> canceldCb)
{
	__cancelCb = canceldCb;
}

void CalNumberPickerPopup::__getRepeat( bool isTime)
{
	if (isTime) {
		__repeat.untilType = CalScheduleRepeat::TIMES;
		__repeat.untilInfo.times = (int)elm_spinner_value_get(__unitValueSp);
	} else {
		__repeat.unitInterval = (int)elm_spinner_value_get(__unitValueSp);
	}
}

Evas_Object* CalNumberPickerPopup::__createContent(Evas_Object *parent)
{
	WHIT();
	Evas_Object* layout = elm_layout_add(parent);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_file_set(layout, CalPath::getPath(CAL_EDJE).c_str(), "CalNumberPickerPopup");

	__unitValueSp = elm_spinner_add(layout);
	elm_object_style_set(__unitValueSp, "vertical");
	elm_spinner_editable_set(__unitValueSp, EINA_TRUE);
	elm_spinner_min_max_set(__unitValueSp, MIN_UNIT_VALUE, MAX_UNIT_VALUE);
	elm_spinner_value_set(__unitValueSp, __number);
	elm_spinner_interval_set(__unitValueSp, 0.1);

	evas_object_smart_callback_add(__unitValueSp, "focused", [](void *data, Evas_Object *obj, void *event_info EINA_UNUSED){
		Evas_Object* entry = elm_object_part_content_get(obj, "elm.swallow.entry");
		WDEBUG("spinner entry :%d %d", obj, entry);
		elm_entry_context_menu_disabled_set(entry, EINA_TRUE);
		elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_NUMBERONLY);
		elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);

		static Elm_Entry_Filter_Accept_Set digits_filter_data;
		static Elm_Entry_Filter_Limit_Size limit_filter_data;
		digits_filter_data.accepted = "0123456789";
		digits_filter_data.rejected = NULL;
		limit_filter_data.max_byte_count = 0;
		limit_filter_data.max_char_count = 2;
		elm_entry_markup_filter_append(entry, elm_entry_filter_accept_set, &digits_filter_data);
		elm_entry_markup_filter_append(entry, elm_entry_filter_limit_size, &limit_filter_data);
	}, NULL);

	evas_object_size_hint_align_set(__unitValueSp, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(__unitValueSp, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(__unitValueSp);
	elm_object_part_content_set(layout, "value.sw", __unitValueSp);
	evas_object_show(layout);

	return layout;
}

Evas_Object* CalNumberPickerPopup::onCreate(Evas_Object* parent, void* viewParam)
{
	WHIT();

	// !IMPORTANT popup's parent have to genlist. this is for IME.
	Evas_Object* popup = elm_popup_add(parent);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, eext_popup_back_cb, NULL);

	//title
	elm_object_part_text_set(popup, "title,text", _L_("IDS_CLD_BUTTON_REPEAT_ABB"));

	// set content
	Evas_Object* content = __createContent(parent);
	elm_object_content_set(popup, content);

	//set button
	// cancel
	Evas_Object* button = elm_button_add(popup);
	elm_object_style_set(button, "popup");
	elm_object_text_set(button, _L_("IDS_CLD_BUTTON_CANCEL"));
	char partname[100] = {0};
	snprintf(partname, sizeof(partname) - 1, "button%d", 1);
	elm_object_part_content_set(popup, partname, button);
	evas_object_smart_callback_add(button, "clicked",
			[](void* data, Evas_Object* obj, void* eventInfo) {
				CalNumberPickerPopup* self = (CalNumberPickerPopup*)data;
				if(self->__cancelCb)
					self->__cancelCb();
				evas_object_del(self->getEvasObj());
			}, this);
	evas_object_show(button);

	// set
	button = elm_button_add(popup);
	elm_object_style_set(button, "popup");
	elm_object_text_set(button, _L_("IDS_CLD_BUTTON_SET"));
	snprintf(partname, sizeof(partname) - 1, "button%d", 2);
	elm_object_part_content_set(popup, partname, button);
	evas_object_smart_callback_add(button, "clicked",
			[](void* data, Evas_Object* obj, void* eventInfo) {
				CalNumberPickerPopup* self = (CalNumberPickerPopup*)data;
				WHIT();
				self->__getRepeat( self->__isTime);
				if(self->__doneCb)
					self->__doneCb(self->__repeat);
				evas_object_del(self->getEvasObj());
			}, this);
	evas_object_show(button);

	evas_object_show(popup);
	return popup;
}

void CalNumberPickerPopup::onDestroy()
{
}

