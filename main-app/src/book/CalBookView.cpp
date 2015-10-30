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

#include <memory>
#include "CalBookView.h"
#include "CalBookEditView.h"
#include "CalBookDeleteView.h"
#include "CalBookSelectView.h"
#include "CalCommon.h"
#include "WMenuPopup.h"

#include "CalBookManager.h"
#include "CalEventManager.h"

#include "CalDialogBookSelectAllItem.h"
#include "CalDialogBookGroupTitleItem.h"

#include "CalAppControlLauncher.h"

#include <app_control.h>
#ifdef GBS_BUILD
#include <app_control_internal.h>
#endif


CalBookView::CalBookView() : CalView("CalBookView"),
	__dialog(nullptr)
{
}

CalBookView::~CalBookView()
{
	__localBookMap.clear();
	__localBookItems.clear();
}

/**
 * Executes the create action. This function is called when CalBookView object is created.
 *
 * @param [in]	parent   	The parent object.
 * @param [in]	viewParam	Parameters for view creation.
 *
 * @return	null if it fails, else an created Evas_Object*.
 */
Evas_Object* CalBookView::onCreate(Evas_Object* parent, void* viewParam)
{
	WENTER();

	__dialog = new CalDialogControl;
	__dialog->create(parent, NULL);
	Evas_Object *genlist = __dialog->getEvasObj();

	elm_genlist_homogeneous_set(genlist, EINA_FALSE);
	elm_genlist_mode_set(genlist, ELM_LIST_SCROLL);

	__update();

	return genlist;
}

/**
 * Executes the pushed action. This function is called when our view was successfully pushed into naviframe container.
 *
 * @param [in]	naviItem	If non-null, the navi item.
 */
void CalBookView::onPushed(Elm_Object_Item* naviItem)
{
	elm_object_item_part_text_set(getNaviItem(), "elm.text.title", _L_("IDS_CLD_OPT_MANAGE_CALENDARS_ABB"));
	activateMenuButton(naviItem);
}

/**
 * Executes the destroy action. This function is called when our view object is being destroyed.
 */
void CalBookView::onDestroy( )
{
}

/**
 * Updates this object.
 */
void CalBookView::__update()
{
	destroyPopup();

	if(getNaviItem())
	{
		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", _L_("IDS_CLD_OPT_MANAGE_CALENDARS_ABB"));
	}

	elm_genlist_clear(__dialog->getEvasObj());

	__localBookMap.clear();
	CalBookManager::getInstance().getBooks(__localBookMap);

	// My calendar books
	for(auto it = __localBookMap.begin(); it != __localBookMap.end(); ++it)
	{
		auto node = it->second;
		if(node && node->getStoreType() != CALENDAR_BOOK_TYPE_EVENT)
		{
			continue;
		}

		if(node->getIndex() != DEFAULT_BIRTHDAY_CALENDAR_BOOK_ID)
		{
			CalDialogBookLocalItem* item = new CalDialogBookLocalItem(CalDialogBookLocalItem::LocalBookChange, node);
			item->setSelectCb([this] (const std::shared_ptr<CalBook>& book) {
					CalBookManager::getInstance().updateBook(book);
				});

			Elm_Object_Item *obj = __dialog->add(item);
			item->setCustomData(obj);

			__localBookItems.push_back(item);
		}
	}

	// My birthday books
	for(auto it = __localBookMap.begin(); it != __localBookMap.end(); ++it)
	{
		auto node = it->second;
		WASSERT(node->getStoreType() == CALENDAR_BOOK_TYPE_EVENT);
		if(node->getIndex() == DEFAULT_BIRTHDAY_CALENDAR_BOOK_ID)
		{
			CalDialogBookLocalItem* item = new CalDialogBookLocalItem(CalDialogBookLocalItem::LocalBookChange,node);
			item->setSelectCb([this] (const std::shared_ptr<CalBook>& book) {
					CalBookManager::getInstance().updateBook(book);
				});

			Elm_Object_Item *obj = __dialog->add(item);
			item->setCustomData(obj);

			__localBookItems.push_back(item);
		}
	}
}

/**
 * Executes some action, depending on event it receives.
 *
 * @param	event	The event.
 */
void CalBookView::onEvent(const CalEvent & event)
{
	WENTER();
	WDEBUG("type = %u, source = %u", event.type, event.source);
	switch (event.type)
	{
		case CalEvent::APP_PAUSED:
		case CalEvent::APP_RESUMED:
		case CalEvent::DB_CHANGED:
			break;
		case CalEvent::LANGUAGE_CHANGED:
		case CalEvent::BOOK_DB_CHANGED:
		case CalEvent::TIME_CHANGED:
		case CalEvent::SETTING_CHANGED:
			if(event.source == CalEvent::REMOTE || getNaviItem() != elm_naviframe_top_item_get(getNaviframe()->getEvasObj()))
			{
				__update();
			}
			break;
		default:
			WERROR("invalid type = %u", event.type);
			break;
	}
}
