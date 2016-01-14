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
#include "CalDialogEditOneTextRemoveIconItem.h"
#include <sstream>
#include <Elementary.h>

CalDialogEditOneTextRemoveIconItem::CalDialogEditOneTextRemoveIconItem(int sortIndex, const char* subText)
	:CalDialogControl::Item(sortIndex),
	__selectCb(nullptr)
{
	if(subText)
	{
		int fontSize = getSystemFontSize();
		if(abs(fontSize) > TEXT_ITEM_DEFAULT_SIZE)
		{
			std::stringstream ss;
			ss << "<p font_size=" << fontSize << ">" << subText << "</p>";

			__subText = ss.str();
		}
		else
		{
			__subText = subText;
		}
	}
}

CalDialogEditOneTextRemoveIconItem::~CalDialogEditOneTextRemoveIconItem()
{
}

Elm_Genlist_Item_Class* CalDialogEditOneTextRemoveIconItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++

	itc.item_style = "end_icon";

	itc.func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		CalDialogEditOneTextRemoveIconItem* item = (CalDialogEditOneTextRemoveIconItem*)data;
		if(!strcmp(part, "elm.swallow.end"))
		{
			Evas_Object* box = elm_box_add(obj);
			evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
			elm_box_homogeneous_set(box, EINA_FALSE);

			Evas_Object* button = elm_button_add(box);
			elm_object_style_set(button, "icon_expand_delete");
			evas_object_show(button);
			evas_object_smart_callback_add(button, "clicked", [](void* data, Evas_Object* obj, void* event_info) {
					CalDialogEditOneTextRemoveIconItem* self = (CalDialogEditOneTextRemoveIconItem*)data;
					self->__removeCb();
				}, item);

			elm_box_pack_end(box, button);
			evas_object_show(box);

			return box;
		}

		return NULL;
	};
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogEditOneTextRemoveIconItem* item = (CalDialogEditOneTextRemoveIconItem*)data;
		if(!strcmp(part, "elm.text") && !item->__subText.empty())
		{
			return strdup(_L_(item->__subText.c_str()));
		}

		return NULL;
	};

	return &itc;
}

void CalDialogEditOneTextRemoveIconItem::setSubText(const char* text)
{
	if(text)
	{
		__subText = text;
	}
}

void CalDialogEditOneTextRemoveIconItem::onSelect()
{
	WENTER();
	if(__selectCb)
	{
		__selectCb();
	}
}

void CalDialogEditOneTextRemoveIconItem::setSelectCb(std::function<void (void)> selectCb)
{
	__selectCb = selectCb;
}

void CalDialogEditOneTextRemoveIconItem::setRemoveCb(std::function<void (void)> removeCb)
{
	__removeCb = removeCb;
}

