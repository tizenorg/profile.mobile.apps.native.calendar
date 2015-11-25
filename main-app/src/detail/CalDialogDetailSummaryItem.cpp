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

#include <app_control.h>
#include <calendar.h>
#include "CalCommon.h"
#include "CalBook.h"
#include "CalBookManager.h"
#include "CalAppControlLauncher.h"
#include "CalDialogDetailSummaryItem.h"

#define CONTACT_LENGTH 16

CalDialogDetailSummaryItem::CalDialogDetailSummaryItem(int bookId, int personId, const char *summary, const char *image) :
	__bookId(bookId),
	__personId(personId),
	__summary(summary),
	__image(image)
{
}

CalDialogDetailSummaryItem::~CalDialogDetailSummaryItem()
{
}

Evas_Object* CalDialogDetailSummaryItem::__createTitleEntry(Evas_Object *obj)
{
	Evas_Object *entry = elm_entry_add(obj);
	evas_object_size_hint_weight_set(entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_entry_single_line_set(entry, EINA_TRUE);
	elm_entry_scrollable_set(entry, EINA_TRUE);
	elm_entry_editable_set(entry, EINA_FALSE);
	elm_entry_context_menu_disabled_set(entry, EINA_FALSE);
	elm_entry_entry_set(entry, __summary.c_str());
	evas_object_show(entry);

	return entry;
}

Evas_Object* CalDialogDetailSummaryItem::__createIcon(Evas_Object *obj)
{
	Evas_Object* layout = elm_layout_add(obj);

	Evas_Object *image = elm_image_add(layout);
	elm_image_aspect_fixed_set(image, EINA_TRUE);
	elm_image_fill_outside_set(image, EINA_TRUE);
	elm_layout_file_set(layout, CAL_EDJE, "thumbnail_96");
	if(!__image.empty())
	{
		Eina_Bool result = elm_image_file_set(image, __image.c_str(), NULL);
		if(!result)
		{
			elm_image_file_set(image, CAL_IMAGE_DIR "contacts_default.png", NULL);
		}
	}
	else
	{
		elm_image_file_set(image, CAL_IMAGE_DIR "contacts_default.png", NULL);
	}
	elm_object_part_content_set(layout, "swallow.image", image);

	evas_object_show(image);

	if(__bookId == DEFAULT_BIRTHDAY_CALENDAR_BOOK_ID)
	{
		evas_object_smart_callback_add(image, "clicked",
			[](void *data, Evas_Object *obj, void *event_info) {

			app_control_h service = NULL;
			if(app_control_create(&service) == APP_CONTROL_ERROR_NONE)
			{
				CalDialogDetailSummaryItem *self = (CalDialogDetailSummaryItem*) data;

				char contact[CONTACT_LENGTH] = {0};
				snprintf(contact, sizeof(contact), "%d", self->__personId);

				app_control_set_operation(service, APP_CONTROL_OPERATION_VIEW);
				app_control_set_mime(service, APP_CONTROL_MIME_CONTACT);
				app_control_add_extra_data(service, APP_CONTROL_DATA_ID, contact);
				app_control_set_launch_mode(service, APP_CONTROL_LAUNCH_MODE_GROUP);
				app_control_send_launch_request(service, NULL, NULL);
			}
		}, this);
	}

	evas_object_show(layout);

	return layout;
}

Elm_Genlist_Item_Class* CalDialogDetailSummaryItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {}; // Implicitly 0-init in C++
	if(__bookId == DEFAULT_BIRTHDAY_CALENDAR_BOOK_ID)
	{
		itc.item_style = "CalDialogDetailSummaryIcon";
	}
	else
	{
		itc.item_style = "CalDialogDetailSummaryPlain";
	}

	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		return NULL;
	};
	itc.func.content_get = [](void *data, Evas_Object *obj, const char *part)->Evas_Object*
	{
		CalDialogDetailSummaryItem *item = (CalDialogDetailSummaryItem*)data;
		if (!strcmp(part, "elm.details.color"))
		{
			int r,g,b,a;
			CalBookManager& calBookManager = CalBookManager::getInstance();
			std::shared_ptr<CalBook> calBook = calBookManager.getBook(item->__bookId);
			if(calBook)
			{
				calBook->getColor(r,g,b,a);
				Evas_Object *rect = evas_object_rectangle_add(evas_object_evas_get(obj));
				evas_object_color_set(rect, r, g, b, a);

				return rect;
			}
		}
		else if(!strcmp(part, "elm.details.entry"))
		{
			return item->__createTitleEntry(obj);
		}
		else if(!strcmp(part, "elm.details.icon"))
		{
			return item->__createIcon(obj);
		}

		return NULL;
	};
	return &itc;
}
