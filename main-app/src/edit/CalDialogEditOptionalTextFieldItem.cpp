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
#include "cal.h"
#include "CalCommon.h"
#include "CalDialogEditOptionalTextFieldItem.h"

CalDialogEditOptionalTextFieldItem::CalDialogEditOptionalTextFieldItem(int sortIndex, std::function<void (CalEditField* editField)> onCreateEditField, bool multiLine)
	:CalDialogControl::Item(sortIndex),
	__multiLine(multiLine),
	__editField(nullptr),
	__removeCb(nullptr),
	__onCreateEditField(onCreateEditField)
{
}

CalDialogEditOptionalTextFieldItem::CalDialogEditOptionalTextFieldItem():
	__multiLine(false),
	__editField(nullptr),
	__removeCb(nullptr),
	__onCreateEditField(nullptr)
{
}

CalDialogEditOptionalTextFieldItem::~CalDialogEditOptionalTextFieldItem()
{
}

Evas_Object* CalDialogEditOptionalTextFieldItem::createEntry(Evas_Object* parent)
{
	__editField = new CalEditField(__multiLine);
	__editField->create(parent, NULL);

	if (__onCreateEditField)
	{
		__onCreateEditField(__editField);
	}

	return __editField->getEvasObj();
}

Elm_Genlist_Item_Class* CalDialogEditOptionalTextFieldItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {};
	itc.item_style = "EntryIconCustom";
	itc.func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		CalDialogEditOptionalTextFieldItem* item = (CalDialogEditOptionalTextFieldItem*)data;
		if (!strcmp(part, "elm.icon.entry"))
		{
			return item->createEntry(obj);
		}
		else if (!strcmp(part, "elm.icon"))
		{
			return item->createRemoveButton(obj);
		}

		return NULL;
	};

	return &itc;
}

Evas_Object* CalDialogEditOptionalTextFieldItem::createRemoveButton(Evas_Object* parent)
{
	Evas_Object* box = elm_box_add(parent);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_box_homogeneous_set(box, EINA_FALSE);

	Evas_Object* button = elm_button_add(box);
	elm_object_style_set(button, "editfield_clear");
	evas_object_show(button);
	evas_object_smart_callback_add(button, "clicked", [](void* data, Evas_Object* obj, void* event_info) {
		CalDialogEditOptionalTextFieldItem* self = (CalDialogEditOptionalTextFieldItem*)data;
		self->__removeCb();
		}, this);

	elm_box_pack_end(box, button);
	evas_object_show(box);

	return box;
}

void CalDialogEditOptionalTextFieldItem::setRemoveCb(std::function<void ()> removeCb)
{
	__removeCb = removeCb;
}

CalEditField* CalDialogEditOptionalTextFieldItem::getEditField()
{
	return __editField;
}
