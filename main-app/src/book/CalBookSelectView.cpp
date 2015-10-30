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

#include "CalBookSelectView.h"
#include "CalCommon.h"
#include "CalDialogBookGroupTitleItem.h"
#include "CalDialogBookLocalItem.h"
#include "CalBookSelectView.h"
#include "CalBookManager.h"
#include "CalEventManager.h"
#include "CalBookEditView.h"




CalBookSelectView::CalBookSelectView( ):CalView("CalBookSelectView")
{
	__dialog = NULL;
}

CalBookSelectView::~CalBookSelectView()
{

}


Evas_Object* CalBookSelectView::onCreate(Evas_Object* parent, void* viewParam)
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

void CalBookSelectView::__update()
{
	if (getNaviItem()) {
		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", _L_("IDS_CLD_OPT_EDIT"));
	}

	elm_genlist_clear(__dialog->getEvasObj());

	__bookMap.clear();
	CalBookManager::getInstance().getBooks(__bookMap);

	// My calendar group
	elm_genlist_item_select_mode_set(__dialog->add(new CalDialogBookGroupTitleItem(_L_("IDS_CLD_OPT_MY_CALENDAR_ABB"))), ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	// My calendar books
	for (auto it = __bookMap.begin(); it != __bookMap.end(); it++) {
		auto node = it->second;
		WASSERT(node->getStoreType() == CALENDAR_BOOK_TYPE_EVENT);
		int index = node->getIndex();

		if (index == DEFAULT_BIRTHDAY_CALENDAR_BOOK_ID) {
			continue;
		}

		CalDialogBookLocalItem* item;
		item= new CalDialogBookLocalItem(CalDialogBookLocalItem::LocalBookOnlySelect,node);
		item->setSelectCb(
					[this] (const std::shared_ptr<CalBook>& book) {
						getNaviframe()->push(new CalBookEditView(CalBookEditView::EDIT, book));
					}
					);
		Elm_Object_Item *list_item;
		list_item = __dialog->add(item); // don't worry about memory leak of item

		if (node->getIndex() ==DEFAULT_EVENT_CALENDAR_BOOK_ID || node->isReadOnly()) {
			elm_object_item_disabled_set(list_item, EINA_TRUE);
		}
	}
}

void CalBookSelectView::onEvent(const CalEvent & event)
{
	WENTER();
	WDEBUG("type = %u, source = %u", event.type, event.source);
	switch (event.type) {
		case CalEvent::BOOK_DB_CHANGED:
		case CalEvent::TIME_CHANGED:
		case CalEvent::LANGUAGE_CHANGED:
		case CalEvent::SETTING_CHANGED:
			if (event.source == CalEvent::REMOTE || getNaviItem() != elm_naviframe_top_item_get(getNaviframe()->getEvasObj())) {
				__update();
			}
			break;
		default:
			WERROR("invalid type = %u", event.type);
			break;
	}
}


void CalBookSelectView::onPushed(Elm_Object_Item* naviItem)
{
	elm_object_item_part_text_set(getNaviItem(), "elm.text.title", _L_("IDS_CLD_OPT_EDIT"));
}

void CalBookSelectView::onDestroy()
{

}


