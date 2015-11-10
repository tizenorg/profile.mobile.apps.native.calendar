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
#include "CalDialogEditReminderRadioItem.h"
#include "CalEditReminderPopup.h"

#define REMINDER_POPUP_VISIBLE_ITEMS 7

CalEditReminderPopup::CalEditReminderPopup(const CalScheduleReminder& reminderOri, bool isEditReminder) :
	__doneCb(NULL),
	__customSelectedCb(NULL),
	__reminder(reminderOri),
	__customItem(NULL)
{
	__reminder = CalScheduleReminder(reminderOri);

	if (!isEditReminder) // default 10 min reminder
	{
		__radio_index = 3;
		__reminder.unitType = CalScheduleReminder::MIN;
		__reminder.unitValue = 10;
		__reminder.isCustom = false;
	}
	else if (__reminder.unitType == CalScheduleReminder::NONE)
	{
		__radio_index = 1;
		__reminder.isCustom = false;
	}
	else
	{
		__reminder.isCustom = true;

		int minuteValue = __reminder.getMinuteValue();
		switch(minuteValue)
		{
			case 0:
				__reminder.unitType = CalScheduleReminder::MIN;
				__reminder.unitValue = minuteValue;
				__reminder.isCustom = false;
				__radio_index = 2;
				break;
			case 10:
				__reminder.unitType = CalScheduleReminder::MIN;
				__reminder.unitValue = minuteValue;
				__reminder.isCustom = false;
				__radio_index = 3;
				break;
			case 30:
				__reminder.unitType = CalScheduleReminder::MIN;
				__reminder.unitValue = minuteValue;
				__reminder.isCustom = false;
				__radio_index = 4;
				break;
			case 60:
				__reminder.unitType = CalScheduleReminder::HOUR;
				__reminder.unitValue = minuteValue / 60;
				__reminder.isCustom = false;
				__radio_index = 5;
				break;
			case 60 * 24:
				__reminder.unitType = CalScheduleReminder::DAY;
				__reminder.unitValue = minuteValue / (60 * 24);
				__reminder.isCustom = false;
				__radio_index = 6;
				break;
			default:
				__reminder.isCustom = true;
				__radio_index = 10;
				break;
		}
	}
	__isEditReminder = isEditReminder;
}

CalEditReminderPopup::~CalEditReminderPopup()
{
}

void CalEditReminderPopup::setDoneCb(std::function<void (const CalScheduleReminder& reminder, int radio_index)> doneCb)
{
	__doneCb = doneCb;
}

void CalEditReminderPopup::setCustomSelectedCb(std::function<void (const CalScheduleReminder& reminder, int radio_index)> customSelectedCb)
{
	__customSelectedCb = customSelectedCb;
}

Evas_Object* CalEditReminderPopup::__createContent(Evas_Object* parent)
{
	WHIT();

	Evas_Object* box = NULL;
	box = elm_box_add(parent);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	CalDialogControl *dialog = new CalDialogControl();
	dialog->create(parent, NULL);

	Evas_Object *genlist = dialog->getEvasObj();
	elm_genlist_highlight_mode_set(genlist, EINA_FALSE);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);

	Evas_Object *radio_main = elm_radio_add(genlist);
	elm_radio_state_value_set(radio_main, __radio_index);
	evas_object_data_set(genlist, "radio_main", radio_main);

	int radio_index = 1;

	// Off
	__addRadioItem(dialog, radio_main, radio_index, CalScheduleReminder::NONE, 0);
	// On Time
	__addRadioItem(dialog, radio_main, radio_index, CalScheduleReminder::MIN, 0);
	// 5 min before
	__addRadioItem(dialog, radio_main, radio_index, CalScheduleReminder::MIN, 10);
	// 30 min before
	__addRadioItem(dialog, radio_main, radio_index, CalScheduleReminder::MIN, 30);
	// 1 hour before
	__addRadioItem(dialog, radio_main, radio_index, CalScheduleReminder::HOUR, 1);
	// 1 day before
	__addRadioItem(dialog, radio_main, radio_index, CalScheduleReminder::DAY, 1);
	// Custom
	__addRadioCustomItem(dialog, radio_main, radio_index);

	elm_radio_value_set(radio_main, __radio_index);

	if(__reminder.isCustom)
	{
		__addRadioItem(dialog, radio_main, radio_index, __reminder.unitType, __reminder.unitValue);
	}

	elm_box_pack_end(box, genlist);
	evas_object_size_hint_min_set(box, -1, REMINDER_POPUP_VISIBLE_ITEMS * ELM_SCALE_SIZE(CAL_POPUP_ITEM_HEIGHT));

	return box;
}

Evas_Object* CalEditReminderPopup::onCreate(Evas_Object* parent, void* viewParam)
{
	WHIT();

	setContent([this](Evas_Object* parent)->Evas_Object* {
		return __createContent(parent);
	});

	WPopup::setTitle(_L_("IDS_CLD_BUTTON_REMINDER_ABB"));
	return WPopup::onCreate( parent, viewParam );
}

void CalEditReminderPopup::__addRadioItem(CalDialogControl *dialog, Evas_Object* radioObj, int& radioIndex, CalScheduleReminder::UnitType type, int unitValue)
{
	bool setRadio = false;
	if (__isEditReminder && __reminder.unitType == type && __reminder.unitValue == unitValue)
	{
		setRadio = true;
	}
	CalDialogEditReminderRadioItem* radioItem = new CalDialogEditReminderRadioItem(__reminder, radioIndex, type, unitValue, setRadio, type == CalScheduleReminder::NONE);
	radioItem->setSelectCb([this, radioObj, radioIndex](void)->void{
			if (__radio_index == radioIndex)
			{
				if (__doneCb)
				{
					__doneCb(__reminder, __radio_index);
				}

				destroy();
				return;
			}
			elm_radio_value_set(radioObj, radioIndex);
			__radio_index = radioIndex;
			Evas_Object *radio = elm_radio_selected_object_get(radioObj);
			evas_object_smart_callback_call(radio, "changed", NULL);
		}
	);
	radioItem->setRadioChangeCb([this, radioIndex](const CalScheduleReminder& reminder)->void{
			__reminder = reminder;
			__radio_index = radioIndex;
			std::string reminderText;
			__reminder.getString(reminderText);
			WDEBUG("Reminder: %s", reminderText.c_str());
			if (__doneCb)
				__doneCb(__reminder, __radio_index);
			destroy();
		}
	);
	dialog->add(radioItem);

	radioIndex++;
}

void CalEditReminderPopup::__addRadioCustomItem(CalDialogControl *dialog, Evas_Object* radioObj, int& radioIndex)
{
	bool setRadio = false;
	__customItem = new CalDialogEditReminderRadioItem(__reminder, radioIndex, setRadio, false);
	__customItem->setSelectCb([this, radioObj, radioIndex](void)->void{
			elm_radio_value_set(radioObj, radioIndex);
			__radio_index = 10;
			Evas_Object *radio = elm_radio_selected_object_get(radioObj);
			evas_object_smart_callback_call(radio, "changed", NULL);
		}
	);
	__customItem->setRadioChangeCb([this](const CalScheduleReminder& reminder)->void{
			if (__customSelectedCb)
			{
				__radio_index = 10;
				__customSelectedCb(__reminder, __radio_index);
			}
		}
	);

	dialog->add(__customItem);
	radioIndex++;
}

void CalEditReminderPopup::onDestroy()
{
}

