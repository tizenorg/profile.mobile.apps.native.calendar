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
#include "CalShareAsPopup.h"
#include "CalEventManager.h"
#include "CalDialogControl.h"
#include "CalDialogOneLineItem.h"

#define SHARE_AS_POPUP_VISIBLE_ITEMS 2

CalShareAsPopup::CalShareAsPopup(const char *title)
{
	if(title){
		__title = title;
	}
}
CalShareAsPopup::~CalShareAsPopup()
{
	WHIT();
}

void CalShareAsPopup::setSelectCb(std::function<void (shareAsType type)> selectedCb)
{
	__selectedCb = selectedCb;
}

Evas_Object* CalShareAsPopup::__createContent(Evas_Object *parent)
{
	WHIT();
	Evas_Object *box = NULL;
	box = elm_box_add(parent);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	CalDialogControl *dialog = new CalDialogControl();
	dialog->create(box, NULL);

	dialog->add(new CalDialogOneLineItem(
		[this](void) {
			if(__selectedCb)
				__selectedCb(BY_VCS);
			destroy();
		},
		_L_("IDS_CLD_OPT_VCS_FILE")
	));

	dialog->add(new CalDialogOneLineItem(
		[this](void) {
			if(__selectedCb)
				__selectedCb(BY_TEXT);
			destroy();
		},
		_L_("IDS_CLD_OPT_TEXT")
	));

	elm_box_pack_start(box, dialog->getEvasObj());
	evas_object_size_hint_min_set(box, -1, SHARE_AS_POPUP_VISIBLE_ITEMS * ELM_SCALE_SIZE(CAL_POPUP_ITEM_HEIGHT));

	return box;
}

Evas_Object* CalShareAsPopup::onCreate(Evas_Object* parent, void* viewParam)
{
	WHIT();

	if(!__title.empty())
		setTitle(__title.c_str());

	setContent([this](Evas_Object* parent)->Evas_Object* {
		return __createContent(parent);
	});

	return WPopup::onCreate( parent, viewParam );
}

void CalShareAsPopup::onDestroy()
{
}

