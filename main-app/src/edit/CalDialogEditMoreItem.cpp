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

#include <Elementary.h>
#include <string>
#include "CalPath.h"
#include "CalCommon.h"
#include "CalDialogEditMoreItem.h"

CalDialogEditMoreItem::CalDialogEditMoreItem(std::function<void (ButtonType buttonType, Evas_Object* button)> buttonClickedCb, int sortIndex, bool disableLocation, bool disableRepeat,  bool disableReminder)
	:CalDialogControl::Item(sortIndex),
	__buttonClickedCb(buttonClickedCb),
	__button1(NULL),
	__button2(NULL),
	__button3(NULL),
	__button4(NULL),
	__disableLocation(disableLocation),
	__disableRepeat(disableRepeat),
	__disableReminder(disableReminder),
	__disableMore(false)
{
}

CalDialogEditMoreItem::CalDialogEditMoreItem()
	:__buttonClickedCb(NULL),
	__button1(NULL),
	__button2(NULL),
	__button3(NULL),
	__button4(NULL),
	__disableLocation(false),
	__disableRepeat(false),
	__disableReminder(false),
	__disableMore(false)
{
}

CalDialogEditMoreItem::~CalDialogEditMoreItem()
{
}

void CalDialogEditMoreItem::setButtonStatus(bool locationDisabled, bool reminderDisable, bool repeatDisabled, bool MoreDisabled)
{
	__disableLocation = locationDisabled;
	__disableReminder = reminderDisable;
	__disableRepeat = repeatDisabled;
	__disableMore = MoreDisabled;
}


Elm_Genlist_Item_Class* CalDialogEditMoreItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++
	itc.item_style = "more";
	itc.func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		CalDialogEditMoreItem* item = (CalDialogEditMoreItem*) data;
		if (!strcmp(part, "button1"))
		{
			return item->__button1 = item->onCreateIcon(obj, CalPath::getPath("icon_button_location.png", CalPath::IMAGE).c_str(), LOCATION);
		}
		else if (!strcmp(part, "button3"))
		{
			return item->__button3 = item->onCreateIcon(obj, CalPath::getPath("icon_button_repeat.png", CalPath::IMAGE).c_str(), REPEAT);
		}
		else if (!strcmp(part, "button2"))
		{
			return item->__button2 = item->onCreateIcon(obj, CalPath::getPath("icon_button_reminder.png", CalPath::IMAGE).c_str(), REMINDER);
		}
		else if (!strcmp(part, "button4"))
		{
			return item->__button4 = item->onCreateIcon(obj, CalPath::getPath("icon_button_more.png", CalPath::IMAGE).c_str(), MORE);
		}
		else
		{
			return NULL;
		}
	};
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		if (strcmp(part, "label1") == 0)
			return g_strdup(_L_("IDS_CLD_BUTTON_LOCATION_ABB"));
		else if (strcmp(part, "label3") == 0)
			return g_strdup(_L_("IDS_CLD_BUTTON_REPEAT_ABB"));
		else if (strcmp(part, "label2") == 0)
			return g_strdup(_L_("IDS_CLD_BUTTON_REMINDER_ABB"));
		else if (strcmp(part, "label4") == 0)
			return g_strdup(_L_("IDS_CLD_BUTTON_MORE"));
		else
			return NULL;
	};

	return &itc;
}

Evas_Object* CalDialogEditMoreItem::onCreateIcon(Evas_Object* parent, const char* icon_name, ButtonType buttonType)
{
	Evas_Object* image = NULL;

	image = elm_image_add(parent);
	//evas_object_data_set(image, "buttonType", buttonType);

	bool result = (EINA_TRUE == elm_image_file_set(image, icon_name, NULL));
	if (!result)
	{
		ERR("error with elm_image_file_set, image path %s", icon_name);
	}
	evas_object_size_hint_weight_set(image, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(image, EVAS_HINT_FILL, EVAS_HINT_FILL);

	Evas_Object *button = elm_button_add(parent);

	elm_object_style_set(button, "circle");
	evas_object_size_hint_min_set(button, 46, 46);
	elm_object_part_content_set(button, "icon", image);
	evas_object_data_set(button, "buttonType", (void*)buttonType);

	if (buttonType == LOCATION) {
		if (__disableLocation) {
			elm_object_disabled_set(button, EINA_TRUE);
		}
	} else if (buttonType == REPEAT) {
		if (__disableRepeat) {
			elm_object_disabled_set(button, EINA_TRUE);
		}
	} else if (buttonType == REMINDER) {
		if (__disableReminder) {
			elm_object_disabled_set(button, EINA_TRUE);
		}
	} else if (buttonType == MORE) {
		if (__disableMore) {
			elm_object_disabled_set(button, EINA_TRUE);
		}
	}

	evas_object_smart_callback_add(button, "clicked",
		[](void* data, Evas_Object* obj, void* event_info) {
			if (elm_object_disabled_get(obj))
				return;

			CalDialogEditMoreItem* pCalDialogDateTimeItem = (CalDialogEditMoreItem*)data;
			ButtonType buttonType = static_cast<ButtonType> (reinterpret_cast<long long> (evas_object_data_get(obj, "buttonType")));
			pCalDialogDateTimeItem->__buttonClickedCb(buttonType, obj);
		}
	, this);

	evas_object_show(button);

	return button;
}

Evas_Object* CalDialogEditMoreItem::getButton(ButtonType buttonType)
{
	Evas_Object* button = NULL;

	switch (buttonType) {
		case LOCATION:
			button = __button1;
			break;
		case REPEAT:
			button = __button3;
			break;
		case REMINDER:
			button = __button2;
			break;
		case MORE:
			button = __button4;
			break;
		default:
			break;
	}

	return button;
}

void CalDialogEditMoreItem::setButtonDisable(ButtonType buttonType, bool isDisable)
{
	Evas_Object* button = getButton(buttonType);

	if (button)
		elm_object_disabled_set(button, isDisable ? EINA_TRUE : EINA_FALSE);
}

void CalDialogEditMoreItem::getButtonDisable(ButtonType buttonType, bool& isDisable)
{
	Evas_Object* button = getButton(buttonType);

	if (button)
		isDisable = elm_object_disabled_get(button) == EINA_TRUE ? true : false;
}
