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
#include "CalSettingAlertPopup.h"
#include "CalDialogSettingsRadioItem.h"
#include "CalDialogControl.h"
#include "CalSettingsManager.h"


CalSettingAlertPopup::CalSettingAlertPopup(std::function<void ()> callbackFun):
	__callbackFun(callbackFun)
{

}

CalSettingAlertPopup::~CalSettingAlertPopup()
{

}

void CalSettingAlertPopup::__dialogDeleteCallback(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	WENTER();
}


Evas_Object* CalSettingAlertPopup::onCreate(Evas_Object* parent, void* viewParam)
{
	setContent([this](Evas_Object* parent)->Evas_Object* {
		return __createContent(parent);
	});

	WPopup::setTitle(_L_("IDS_CLD_MBODY_ALERT_TYPE"));
	return WPopup::onCreate( parent, viewParam );
}

void CalSettingAlertPopup::onDestroy()
{
	if (__callbackFun)
		__callbackFun();
}

Evas_Object* CalSettingAlertPopup::__createContent(Evas_Object* parent)
{
	Evas_Object *box = NULL;
	box = elm_box_add(parent);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);


	CalDialogControl *dialog = new CalDialogControl;
	dialog->create(parent, NULL);
	Evas_Object *genlist = dialog->getEvasObj();
	evas_object_event_callback_add(genlist, EVAS_CALLBACK_DEL, __dialogDeleteCallback, this);

	elm_genlist_clear(genlist);//clear genlist

	Evas_Object * item_alert_radio = elm_radio_add(genlist);
	elm_radio_state_value_set(item_alert_radio, CalSettingsManager::getInstance().getAlertType());
	elm_radio_value_set(item_alert_radio, CalSettingsManager::getInstance().getAlertType());
	evas_object_data_set(genlist, "alert", item_alert_radio);


	CalDialogSettingsRadioItem *alertItem = new CalDialogSettingsRadioItem(
					[item_alert_radio, this](){
						if (elm_radio_value_get(item_alert_radio) != 0) {
							elm_radio_value_set(item_alert_radio, 0);
							Evas_Object *radio = elm_radio_selected_object_get(item_alert_radio);
							evas_object_smart_callback_call(radio, "changed", NULL);
						} else {
							destroy();
						}
					},
					CalDialogSettingsRadioItem::SUB_ALERTS, 0 );
	alertItem->setRadioCb([this](void){
			destroy();
		}
	);
	dialog->add(alertItem);

	CalDialogSettingsRadioItem *notificationItem =
		new CalDialogSettingsRadioItem(
					[item_alert_radio, this](){
						if (elm_radio_value_get(item_alert_radio) != 1) {
							elm_radio_value_set(item_alert_radio, 1);
							Evas_Object *radio = elm_radio_selected_object_get(item_alert_radio);
							evas_object_smart_callback_call(radio, "changed", NULL);
						} else {
							destroy();
						}
					},
					CalDialogSettingsRadioItem::SUB_NOTIFICATION, 1 );
	notificationItem->setRadioCb([this](void){
			destroy();
		}
	);

	dialog->add(notificationItem);

	CalDialogSettingsRadioItem *offItem = new CalDialogSettingsRadioItem(
					[item_alert_radio,this](){
						if (elm_radio_value_get(item_alert_radio) != 2) {
							elm_radio_value_set(item_alert_radio, 2);
							Evas_Object *radio = elm_radio_selected_object_get(item_alert_radio);
							evas_object_smart_callback_call(radio, "changed", NULL);
						} else {
							destroy();
						}
					},
					CalDialogSettingsRadioItem::SUB_OFF, 2 );
	offItem->setRadioCb([this](void){
			destroy();
		}
	);

	dialog->add(offItem);

	elm_box_pack_end(box,  genlist);
	evas_object_size_hint_min_set(box, -1, 3*98);
	//evas_object_show(box);

	return box;
}

