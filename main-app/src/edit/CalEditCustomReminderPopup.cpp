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

#include <efl_extension.h>
#include "CalCommon.h"
#include "CalEditCustomReminderPopup.h"
#include "CalEventManager.h"

#include "CalPath.h"

#define MIN_UNIT_VALUE 0
#define MAX_UNIT_VALUE 99

CalEditCustomReminderPopup::CalEditCustomReminderPopup(const CalScheduleReminder& reminder) :
	__reminder(reminder),
	__unitTypeSp(NULL),
	__unitValueSp(NULL)
{
}

CalEditCustomReminderPopup::~CalEditCustomReminderPopup()
{
	WHIT();
}

void CalEditCustomReminderPopup::setDoneCb(std::function<void (const CalScheduleReminder& reminder)> doneCb)
{
	__doneCb = doneCb;
}

void CalEditCustomReminderPopup::setCancelCb(std::function<void (void)> canceldCb)
{
	__cancelCb = canceldCb;
}

void CalEditCustomReminderPopup::__getReminder()
{
	__reminder.unitType = (CalScheduleReminder::UnitType)elm_spinner_value_get(__unitTypeSp);
	__reminder.unitValue = (int)elm_spinner_value_get(__unitValueSp);

	int minuteValue = __reminder.getMinuteValue();

	//60 minutes --> 1 hour
	//48 hours --> 2 days
	if(minuteValue == 0){
		__reminder.unitType = CalScheduleReminder::MIN;
		__reminder.unitValue = 0;
	}else if(minuteValue % (60 * 24 * 7) == 0){
		__reminder.unitType = CalScheduleReminder::WEEK;
		__reminder.unitValue = minuteValue / (60 * 24 * 7);
	}else if(minuteValue % (60 * 24) == 0){
		__reminder.unitType = CalScheduleReminder::DAY;
		__reminder.unitValue = minuteValue / (60 * 24);
	}else if(minuteValue % 60 == 0){
		__reminder.unitType = CalScheduleReminder::HOUR;
		__reminder.unitValue = minuteValue / 60;
	}else {
		__reminder.unitType = CalScheduleReminder::MIN;
		__reminder.unitValue = minuteValue;
	}
}

Evas_Object* CalEditCustomReminderPopup::__createContent(Evas_Object *parent)
{
	WHIT();
	Evas_Object* layout = elm_layout_add(parent);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_file_set(layout, CalPath::getPath(CAL_EDJE).c_str(), "CalEditCustomReminderPopup");

	__unitTypeSp = elm_spinner_add(layout);
	elm_object_style_set(__unitTypeSp, "vertical");
	elm_spinner_editable_set(__unitTypeSp, EINA_FALSE);
	elm_spinner_wrap_set(__unitTypeSp, EINA_TRUE);
	elm_spinner_interval_set(__unitTypeSp, 0.1);
	elm_spinner_min_max_set(__unitTypeSp, CalScheduleReminder::MIN, CalScheduleReminder::WEEK);
	elm_spinner_special_value_add(__unitTypeSp, CalScheduleReminder::MIN, _L_("IDS_CLD_BODY_MIN_LC_ABB2"));
	elm_spinner_special_value_add(__unitTypeSp, CalScheduleReminder::HOUR, _L_("IDS_CLD_BODY_HOUR_LC_ABB"));
	elm_spinner_special_value_add(__unitTypeSp, CalScheduleReminder::DAY, _L_("IDS_CLD_BODY_DAY_M_DURATION_LC"));
	elm_spinner_special_value_add(__unitTypeSp, CalScheduleReminder::WEEK, _L_("IDS_CLD_BODY_WEEK_LC_ABB"));
	elm_spinner_value_set(__unitTypeSp, (double)__reminder.unitType);
	evas_object_size_hint_align_set(__unitTypeSp, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(__unitTypeSp, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(__unitTypeSp);
	elm_object_part_content_set(layout, "type.sw", __unitTypeSp);

	__unitValueSp = elm_spinner_add(layout);
	elm_object_style_set(__unitValueSp, "vertical");
	elm_spinner_editable_set(__unitValueSp, EINA_TRUE);
	elm_spinner_min_max_set(__unitValueSp, MIN_UNIT_VALUE, MAX_UNIT_VALUE);
	elm_spinner_value_set(__unitValueSp, (double)__reminder.unitValue);
	elm_spinner_interval_set(__unitValueSp, 0.1);
	evas_object_size_hint_align_set(__unitValueSp, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(__unitValueSp, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(__unitValueSp);
	elm_object_part_content_set(layout, "value.sw", __unitValueSp);

	evas_object_smart_callback_add(__unitValueSp, "focused", [](void *data, Evas_Object *obj, void *event_info EINA_UNUSED){
		Evas_Object* entry = elm_object_part_content_get(obj, "elm.swallow.entry");
		WDEBUG("spinner entry :%d %d", obj, entry);
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

	evas_object_show(layout);

	return layout;
}

Evas_Object* CalEditCustomReminderPopup::onCreate(Evas_Object* parent, void* viewParam)
{
	WHIT();

	// !IMPORTANT popup's parent have to genlist. this is for IME.
	Evas_Object* popup = elm_popup_add(parent);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, eext_popup_back_cb, NULL);

	//title
	elm_object_part_text_set(popup, "title,text", _L_("IDS_CLD_HEADER_SET_REMINDER_ABB"));

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
			CalEditCustomReminderPopup* self = (CalEditCustomReminderPopup*)data;
			if(self->__cancelCb)
				self->__cancelCb();
			evas_object_del(self->getEvasObj());
		}, this);
	evas_object_show(button);

	// set
	button = elm_button_add(popup);
	elm_object_style_set(button, "popup");
	elm_object_text_set(button, _L_("IDS_CLD_BUTTON_SET") );
	snprintf(partname, sizeof(partname) - 1, "button%d", 2);
	elm_object_part_content_set(popup, partname, button);
	evas_object_smart_callback_add(button, "clicked",
		[](void* data, Evas_Object* obj, void* eventInfo) {
			CalEditCustomReminderPopup* self = (CalEditCustomReminderPopup*)data;
			WHIT();
			self->__getReminder();
			if (self->__doneCb)
				self->__doneCb(self->__reminder);
			evas_object_del(self->getEvasObj());
		}, this);
	evas_object_show(button);

	evas_object_show(popup);

	WHIT();
	return popup;
}

void CalEditCustomReminderPopup::onDestroy()
{
}

