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
#include "CalSettingFirstDayOfWeekPopup.h"
#include "CalDialogSettingsRadioItem.h"
#include "CalDialogControl.h"
#include "CalSettingsManager.h"

#define SETTINGS_POPUP_VISIBLE_ITEMS 4

CalSettingFirstDayOfWeekPopup::CalSettingFirstDayOfWeekPopup(std::function<void ()> callbackFun):
	__callbackFun(callbackFun)
{

}

CalSettingFirstDayOfWeekPopup::~CalSettingFirstDayOfWeekPopup()
{

}

void CalSettingFirstDayOfWeekPopup::__dialogDeleteCallback(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	CAL_FN_START;
}


Evas_Object* CalSettingFirstDayOfWeekPopup::onCreate(Evas_Object* parent, void* viewParam)
{
	setContent([this](Evas_Object* parent)->Evas_Object* {
		return __createContent(parent);
	});

	WPopup::setTitle(_L_("IDS_CLD_MBODY_WEEK_STARTS_ON"));
	return WPopup::onCreate( parent, viewParam );
}

void CalSettingFirstDayOfWeekPopup::onDestroy()
{
	if (__callbackFun)
		__callbackFun();
}

Evas_Object* CalSettingFirstDayOfWeekPopup::__createContent(Evas_Object* parent)
{
	Evas_Object *box = NULL;
	box = elm_box_add(parent);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	CalDialogControl *dialog = new CalDialogControl;
	dialog->create(parent, NULL);
	Evas_Object *genlist = dialog->getEvasObj();
	evas_object_event_callback_add(genlist, EVAS_CALLBACK_DEL, __dialogDeleteCallback, this);

	elm_genlist_clear(genlist);//clear genlist

	Evas_Object * item_first_day_of_week_radio = elm_radio_add(genlist);
	elm_radio_state_value_set(item_first_day_of_week_radio, CalSettingsManager::getInstance().getFirstDayOfWeek());
	elm_radio_value_set(item_first_day_of_week_radio, CalSettingsManager::getInstance().getFirstDayOfWeek());
	evas_object_data_set(genlist, "radio_firstday", item_first_day_of_week_radio);

	CalDialogSettingsRadioItem *defaultItem = new CalDialogSettingsRadioItem(
					[item_first_day_of_week_radio, this](){
						if (elm_radio_value_get(item_first_day_of_week_radio) != 7) {
							elm_radio_value_set(item_first_day_of_week_radio, 7);
							Evas_Object *radio = elm_radio_selected_object_get(item_first_day_of_week_radio);
							evas_object_smart_callback_call(radio, "changed", NULL);
						} else {
							destroy();
						}
					},
					CalDialogSettingsRadioItem::SUB_LOCALE_DEFAULT, 7 );
	defaultItem->setRadioCb([this](void){
			destroy();
		}
	);
	dialog->add(defaultItem);

	CalDialogSettingsRadioItem *saturdayItem = new CalDialogSettingsRadioItem(
					[item_first_day_of_week_radio, this](){
						if (elm_radio_value_get(item_first_day_of_week_radio) != 6) {
							elm_radio_value_set(item_first_day_of_week_radio, 6);
							Evas_Object *radio = elm_radio_selected_object_get(item_first_day_of_week_radio);
							evas_object_smart_callback_call(radio, "changed", NULL);
						} else {
							destroy();
						}
					},
					CalDialogSettingsRadioItem::SUB_SATURDAY, 6 );
	saturdayItem->setRadioCb([this](void){
			destroy();
		}
	);

	dialog->add(saturdayItem);

	CalDialogSettingsRadioItem *sundayItem = new CalDialogSettingsRadioItem(
					[item_first_day_of_week_radio, this](){
						if (elm_radio_value_get(item_first_day_of_week_radio) != 0) {
							elm_radio_value_set(item_first_day_of_week_radio, 0);
							Evas_Object *radio = elm_radio_selected_object_get(item_first_day_of_week_radio);
							evas_object_smart_callback_call(radio, "changed", NULL);
						} else {
							destroy();
						}
					},
					CalDialogSettingsRadioItem::SUB_SUNDAY, 0 );
	sundayItem->setRadioCb([this](void){
			destroy();
		}
	);
	dialog->add(sundayItem);

	CalDialogSettingsRadioItem *mondayItem = new CalDialogSettingsRadioItem(
					[item_first_day_of_week_radio, this](){
						if (elm_radio_value_get(item_first_day_of_week_radio) != 1) {
							elm_radio_value_set(item_first_day_of_week_radio, 1);
							Evas_Object *radio = elm_radio_selected_object_get(item_first_day_of_week_radio);
							evas_object_smart_callback_call(radio, "changed", NULL);
						} else {
							destroy();
						}
					},
					CalDialogSettingsRadioItem::SUB_MONDAY, 1 );
	mondayItem->setRadioCb([this](void){
			destroy();
		}
	);
	dialog->add(mondayItem);

	elm_box_pack_end(box, genlist);
	evas_object_size_hint_min_set(box, -1, SETTINGS_POPUP_VISIBLE_ITEMS * ELM_SCALE_SIZE(CAL_POPUP_ITEM_HEIGHT));

	return box;
}

