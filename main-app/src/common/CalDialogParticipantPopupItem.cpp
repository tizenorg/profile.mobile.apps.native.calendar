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
#include "CalDialogControl.h"
#include "CalDialogParticipantPopupItem.h"

CalDialogParticipantPopupItem::CalDialogParticipantPopupItem(CalDialogParticipantPopupItem::TYPE type, const char *text)
{
	__type = type;
	__text = text;
	__selectCb= NULL;
}

CalDialogParticipantPopupItem::~CalDialogParticipantPopupItem()
{

}

Elm_Genlist_Item_Class* CalDialogParticipantPopupItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++
	itc.item_style = "1line";
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogParticipantPopupItem *item = (CalDialogParticipantPopupItem*)data;
		if (0 == strcmp("elm.text.main.left", part)) {
			return g_strdup(item->__text);
		}
		return NULL;
	};
	return &itc;
}

void CalDialogParticipantPopupItem::onSelect()
{
	WENTER();
	if (__selectCb)
		__selectCb(__type);
}

void CalDialogParticipantPopupItem::setSelectCb(std::function<void (CalDialogParticipantPopupItem::TYPE type)> selectCb)
{
	__selectCb = selectCb;
}


