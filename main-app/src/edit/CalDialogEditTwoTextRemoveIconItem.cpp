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
#include "CalDialogEditTwoTextRemoveIconItem.h"
#include <sstream>
#include <Elementary.h>

CalDialogEditTwoTextRemoveIconItem::CalDialogEditTwoTextRemoveIconItem(int sortIndex, const char* mainText, const char* subText)
	:CalDialogControl::Item(sortIndex),
	 __selectCb(nullptr)
{
	if(mainText)
	{
		__mainText = mainText;
	}

	if(subText)
	{
		setSubText(subText);
	}
}

CalDialogEditTwoTextRemoveIconItem::~CalDialogEditTwoTextRemoveIconItem()
{
}

Elm_Genlist_Item_Class* CalDialogEditTwoTextRemoveIconItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {};
	itc.item_style = "type2";
	itc.func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		CalDialogEditTwoTextRemoveIconItem* item = (CalDialogEditTwoTextRemoveIconItem*)data;
		if(!strcmp(part, "elm.swallow.end"))
		{
			Evas_Object* button = elm_button_add(obj);
			elm_object_style_set(button, "icon_expand_delete");
			evas_object_propagate_events_set(button, EINA_FALSE);

			evas_object_smart_callback_add(button, "clicked", [](void* data, Evas_Object* obj, void* event_info) {
				CalDialogEditTwoTextRemoveIconItem* self = (CalDialogEditTwoTextRemoveIconItem*)data;
				self->__removeCb();
				}, item);

			evas_object_show(button);

			return button;
		}

		return NULL;
	};
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalDialogEditTwoTextRemoveIconItem* item = (CalDialogEditTwoTextRemoveIconItem*)data;
		if(!strcmp(part, "elm.text.sub"))
		{
			if(!item->__mainText.empty())
			{
				return strdup(_L_G_(item->__mainText.c_str()));
			}
		}
		else if(!strcmp(part, "elm.text"))
		{
			if(!item->__subText.empty())
			{
				return strdup(_L_G_(item->__subText.c_str()));
			}
		}

		return NULL;
	};

	return &itc;
}

void CalDialogEditTwoTextRemoveIconItem::setMainText(const char* text)
{
	if(text)
	{
		__mainText = text;
	}
}

void CalDialogEditTwoTextRemoveIconItem::setSubText(const char* text)
{
	if(text)
	{
		int fontSize = getSystemFontSize();
		if(abs(fontSize) > TEXT_ITEM_DEFAULT_SIZE)
		{
			std::stringstream ss;
			ss << "<p font_size=" << fontSize << ">" << text << "</p>";

			__subText = ss.str();
		}
		else
		{
			__subText = text;
		}
	}
}

void CalDialogEditTwoTextRemoveIconItem::onSelect()
{
	WENTER();
	if(__selectCb)
	{
		__selectCb();
	}
}

void CalDialogEditTwoTextRemoveIconItem::setSelectCb(std::function<void (void)> selectCb)
{
	__selectCb = selectCb;
}

void CalDialogEditTwoTextRemoveIconItem::setRemoveCb(std::function<void (void)> removeCb)
{
	__removeCb = removeCb;
}

