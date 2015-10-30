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

#include "CalEditMoreMenuControl.h"
#include "CalCommon.h"

CalEditMoreMenuControl::CalEditMoreMenuControl(bool showDescription, bool showTimezone)
{
	__showDescription = showDescription;
	__showTimezone = showTimezone;
	__selectCb = NULL;
	__dialog = NULL;
	__parentPopup = NULL;
}

CalEditMoreMenuControl::~CalEditMoreMenuControl()
{
	WENTER();
}

const char* CalEditMoreMenuControl::getClassName()
{
	return "CalEditMoreMenuControl";
}

void CalEditMoreMenuControl::onPopupListItemSelect(CalDialogEditMoreMenuItem::TYPE selectType)
{
	if (__selectCb)
		__selectCb(selectType);

	if (__parentPopup ) {
		evas_object_del(__parentPopup);
	}
}


void CalEditMoreMenuControl::__addDescriptionItem()
{
	CalDialogEditMoreMenuItem *descriptionItem = new CalDialogEditMoreMenuItem(
				CalDialogEditMoreMenuItem::DESCRIPTION, _L_("IDS_CLD_BUTTON_NOTES_ABB"));

	descriptionItem->setSelectCb(
		[this](CalDialogEditMoreMenuItem::TYPE type) {
			onPopupListItemSelect(CalDialogEditMoreMenuItem::DESCRIPTION);
		}
	);
	__dialog->add(descriptionItem);

}

void CalEditMoreMenuControl::__addTimezoneItem()
{
	CalDialogEditMoreMenuItem *timezoneItem = new CalDialogEditMoreMenuItem(
				CalDialogEditMoreMenuItem::TIMEZONE, _L_("IDS_CLD_HEADER_TIME_ZONE_ABB"));

	timezoneItem->setSelectCb(
		[this](CalDialogEditMoreMenuItem::TYPE type) {
			onPopupListItemSelect(CalDialogEditMoreMenuItem::TIMEZONE);
		}
	);
	__dialog->add(timezoneItem);

}


Evas_Object* CalEditMoreMenuControl::onCreate(Evas_Object* parent, void* param)
{
	WENTER();

	__parentPopup = parent;

	Evas_Object* box = NULL;
	box = elm_box_add(parent);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	CalDialogControl *dialog = new CalDialogControl;
	__dialog = dialog;
	dialog->create(parent, NULL);

	elm_genlist_mode_set( dialog->getEvasObj(), ELM_LIST_COMPRESS );
	int height = 0;

	// description
	if (__showDescription) {
		__addDescriptionItem();
		height += CAL_POPUP_ITEM_HEIGHT;
	}

	// timezone
	if (__showTimezone) {
		__addTimezoneItem();
		height += CAL_POPUP_ITEM_HEIGHT;
	}

	elm_box_pack_end(box, dialog->getEvasObj());
	evas_object_size_hint_min_set(box, -1, ELM_SCALE_SIZE(height));

	return box;
}

void CalEditMoreMenuControl::setSelectCb(std::function<void (CalDialogEditMoreMenuItem::TYPE type)> selectCb)
{
	__selectCb = selectCb;
}


void CalEditMoreMenuControl::onDestroy()
{
	WENTER();
}

