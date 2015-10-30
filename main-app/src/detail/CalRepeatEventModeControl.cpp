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

#include "CalRepeatEventModeControl.h"
#include "CalCommon.h"
#include "CalDialogOneLineItem.h"

#define MULTIPLE_REPEAT_POPUP_VISIBLE_ITEMS 3

CalRepeatEventModeControl::CalRepeatEventModeControl(std::function<void (CalDataManager::OperationMode mode)> selectCb)
	: __parent(NULL)
	, __selectCb(selectCb)
{
	WENTER();
}

CalRepeatEventModeControl::~CalRepeatEventModeControl()
{
	WENTER();
}

const char* CalRepeatEventModeControl::getClassName()
{
	return "CalRepeatEventModeControl";
}

Evas_Object* CalRepeatEventModeControl::onCreate(Evas_Object* parent, void* param)
{
	WENTER();

	__parent = parent;

	Evas_Object *box = elm_box_add(parent);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	CalDialogControl *dialog = new CalDialogControl;
	dialog->create(box, NULL);

	elm_box_pack_end(box, dialog->getEvasObj());
	evas_object_size_hint_min_set(box, -1, MULTIPLE_REPEAT_POPUP_VISIBLE_ITEMS * ELM_SCALE_SIZE(CAL_POPUP_ITEM_HEIGHT));

	// only this event
	dialog->add(new CalDialogOneLineItem(
		[this](void) {
			if (__selectCb)
				__selectCb(CalDataManager::ONLY_THIS);
		},
		_L_("IDS_CLD_OPT_ONLY_THIS_EVENT_ABB")
	));

	// this and funture events
	dialog->add(new CalDialogOneLineItem(
		[this](void) {
			if (__selectCb)
				__selectCb(CalDataManager::THIS_AND_FUTURE);
		},
		_L_("IDS_CLD_OPT_THIS_AND_FUTURE_EVENTS_ABB")
	));

	// all repetitive events
	dialog->add(new CalDialogOneLineItem(
		[this](void) {
			if (__selectCb)
				__selectCb(CalDataManager::ALL);
		},
		_L_("IDS_CLD_OPT_ALL_REPEATED_EVENTS_ABB")
	));

	WLEAVE();
	return box;
}

void CalRepeatEventModeControl::onDestroy()
{
	WENTER();
}
