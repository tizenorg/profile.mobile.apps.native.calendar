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

#include "CalDialogEditTextFieldItem.h"
#include "CalCommon.h"

CalDialogEditTextFieldItem::CalDialogEditTextFieldItem(int sortIndex, std::function<void (CalUnderlineEditField* editField)> onCreateEditField)
	: CalDialogControl::Item(sortIndex),
	__editField(NULL),
	__onCreateEditField(onCreateEditField)
{
}

CalDialogEditTextFieldItem::CalDialogEditTextFieldItem()
{
	__editField = NULL;
	__onCreateEditField = NULL;
}

CalDialogEditTextFieldItem::~CalDialogEditTextFieldItem()
{
}

Elm_Genlist_Item_Class* CalDialogEditTextFieldItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++
	itc.item_style = "entry";
	itc.func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		CalDialogEditTextFieldItem* item = (CalDialogEditTextFieldItem*)data;
		if (strcmp(part, "elm.icon.entry") == 0)
		{
			return item->createEntry(obj);
		}
		else
		{
			return NULL;
		}
	};

	return &itc;
}

Evas_Object* CalDialogEditTextFieldItem::createEntry(Evas_Object* parent)
{
	__editField = new CalUnderlineEditField();
	__editField->create(parent, NULL);

	evas_object_event_callback_add(__editField->getEvasObj(), EVAS_CALLBACK_DEL, onEditFieldDestroyed, this);

	if (__onCreateEditField)
	{
		__onCreateEditField(__editField);
	}

	return __editField->getEvasObj();
}

CalUnderlineEditField* CalDialogEditTextFieldItem::getEditField()
{
	return __editField;
}

void CalDialogEditTextFieldItem::onEditFieldDestroyed(void* data, Evas* evas, Evas_Object* obj, void* event_info)
{
	CalDialogEditTextFieldItem* self = (CalDialogEditTextFieldItem*) data;
	self->__editField = NULL;
}
