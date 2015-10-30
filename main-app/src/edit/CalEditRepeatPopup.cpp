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
#include "CalScheduleRepeat.h"
#include "CalEditRepeatPopup.h"

#define REPEAT_POPUP_VISIBLE_ITEMS 5

CalEditRepeatPopup::CalEditRepeatPopup(const CalScheduleRepeat::UnitType defaultUnitType, std::function<void (const CalScheduleRepeat::UnitType unitType)> doneCb) :
	__doneCb(doneCb)
{
	WHIT();
	__repeatUnitType = defaultUnitType;
	if (defaultUnitType == CalScheduleRepeat::UnitType::ONE_TIME)
		__repeatUnitType = CalScheduleRepeat::UnitType::ONE_TIME;
}

CalEditRepeatPopup::~CalEditRepeatPopup()
{
	WHIT();
}

CalDialogEditRepeatRadioItem* CalEditRepeatPopup::__addRadioItem(Evas_Object *radio_main, int radio_index, CalScheduleRepeat::UnitType unitType)
{
	return new CalDialogEditRepeatRadioItem(
		[radio_main, radio_index]()
		{
			elm_radio_value_set(radio_main, radio_index);
			Evas_Object *radio = elm_radio_selected_object_get(radio_main);
			evas_object_smart_callback_call(radio, "changed", NULL);
		},
		[this](const CalScheduleRepeat::UnitType repeatType)
		{
			WDEBUG("repeat.unitType=%d", repeatType);
			__repeatUnitType = repeatType;
			__doneCb(__repeatUnitType);
			destroy();
		},
		radio_index, unitType, __repeatUnitType
	);
}

Evas_Object* CalEditRepeatPopup::__createContent(Evas_Object *parent)
{
	WHIT();

	Evas_Object* box = NULL;
	box = elm_box_add(parent);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	CalDialogControl *dialog = new CalDialogControl();
	dialog->create(parent, NULL);

	Evas_Object *genlist = dialog->getEvasObj();
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_highlight_mode_set(genlist, EINA_FALSE);

	Evas_Object *radio_main = elm_radio_add(genlist);
	elm_radio_state_value_set(radio_main, 0);
	elm_radio_value_set(radio_main, 0);
	evas_object_data_set(genlist, "radio_main", radio_main);

	int radio_index = 1;
	elm_radio_value_set(radio_main, radio_index);

	// None
	dialog->add(__addRadioItem(radio_main, radio_index++, CalScheduleRepeat::ONE_TIME));

	// day
	dialog->add(__addRadioItem(radio_main, radio_index++, CalScheduleRepeat::DAY));

	// week
	dialog->add(__addRadioItem(radio_main, radio_index++, CalScheduleRepeat::WEEK));

	// month
	dialog->add(__addRadioItem(radio_main, radio_index++, CalScheduleRepeat::MONTH));

	// year
	dialog->add(__addRadioItem(radio_main, radio_index++, CalScheduleRepeat::YEAR));

	elm_box_pack_end(box, genlist);
	evas_object_size_hint_min_set(box, -1, REPEAT_POPUP_VISIBLE_ITEMS * ELM_SCALE_SIZE(CAL_POPUP_ITEM_HEIGHT));

	return box;
}

Evas_Object* CalEditRepeatPopup::onCreate(Evas_Object* parent, void* viewParam)
{
	WHIT();

	setContent([this](Evas_Object* parent)->Evas_Object* {
		return __createContent(parent);
	});

	return WPopup::onCreate( parent, viewParam );
}

void CalEditRepeatPopup::onDestroy()
{
}

