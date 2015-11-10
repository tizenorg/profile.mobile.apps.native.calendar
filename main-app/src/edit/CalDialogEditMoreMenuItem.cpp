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
#include "CalDialogEditMoreMenuItem.h"

CalDialogEditMoreMenuItem::CalDialogEditMoreMenuItem(CalDialogEditMoreMenuItem::TYPE type, const char *text)
{
	__type = type;
	__text = text;
	__selectCb = NULL;
}

CalDialogEditMoreMenuItem::~CalDialogEditMoreMenuItem()
{

}

CalDialogEditMoreMenuItem::CalDialogEditMoreMenuItem()
{
	__type = PARTICIPANT;
	__text = NULL;
	__selectCb= NULL;
}


Elm_Genlist_Item_Class* CalDialogEditMoreMenuItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {};
	itc.item_style = "type1";
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogEditMoreMenuItem *item = (CalDialogEditMoreMenuItem*)data;
		if (0 == strcmp("elm.text", part))
		{
			return g_strdup(item->__text);
		}
		return NULL;
	};
	return &itc;
}

void CalDialogEditMoreMenuItem::onSelect()
{
	WENTER();
	if (__selectCb)
		__selectCb(__type);
}

void CalDialogEditMoreMenuItem::setSelectCb(std::function<void (CalDialogEditMoreMenuItem::TYPE type)> selectCb)
{
	__selectCb = selectCb;
}

