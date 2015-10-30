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

#include "CalBookDeleteView.h"
#include "CalCommon.h"
#include "CalDialogBookLocalItem.h"
#include "CalDialogBookGroupTitleItem.h"

#include "CalBookManager.h"
#include "CalEventManager.h"
#include "WPopup.h"

CalBookDeleteView::CalBookDeleteView( ):CalView("CalBookDeleteView")
{
	__checkedCount = 0;
	__allItemsCount = 0;
	__selectAllCheckbox = NULL;
	__deleteBtn = NULL;
	__dialog = NULL;
	__selectAllLayout = NULL;
}

CalBookDeleteView::~CalBookDeleteView()
{

}

void CalBookDeleteView::__onSelect(void *data)
{
	CalDialogBookLocalItem* item = (CalDialogBookLocalItem*)data;

	if (item == NULL) {
		return ;
	}
	const Eina_Bool isSelect = item->getItemCheckState();

	if (isSelect)
		__checkedCount--;
	else
		__checkedCount++;

	__checkedCount = __checkedCount >= 0 ? __checkedCount : 0;

	item->setItemCheckState(!isSelect);

	if (__checkedCount == __allItemsCount) {
		elm_check_state_set(__selectAllCheckbox, EINA_TRUE);
	} else {
		elm_check_state_set(__selectAllCheckbox, EINA_FALSE);
	}

	if (__checkedCount <= 0) {
		elm_object_disabled_set(__deleteBtn, EINA_TRUE);
	} else {
		elm_object_disabled_set(__deleteBtn, EINA_FALSE);
	}

}

void CalBookDeleteView::__onCheckBox(bool state)
{
	Eina_Bool isSelect = state;
	if (isSelect)
		__checkedCount++;
	else
		__checkedCount--;

	__checkedCount = __checkedCount >= 0 ? __checkedCount : 0;

	if (__checkedCount == __allItemsCount) {
		elm_check_state_set(__selectAllCheckbox, EINA_TRUE);
	} else {
		elm_check_state_set(__selectAllCheckbox, EINA_FALSE);
	}

	if (__checkedCount <= 0) {
		elm_object_disabled_set(__deleteBtn, EINA_TRUE);
	} else {
		elm_object_disabled_set(__deleteBtn, EINA_FALSE);
	}
}


void CalBookDeleteView::__onSelectAllSelect(bool is_selectall)
{
	Elm_Object_Item* it = elm_genlist_first_item_get(__dialog->getEvasObj());
	Elm_Object_Item* next = it;

	__checkedCount = 0;

	while(next) {
		CalDialogControl::Item *item = (CalDialogControl::Item *)elm_object_item_data_get(next);
		if (!item->isGroupTitle() && !elm_object_item_disabled_get(item->getElmObjectItem())) {
			if(!elm_object_item_disabled_get(item->getElmObjectItem())){//"My calendar" item disable and undeleteable
				CalDialogBookLocalItem *bookItem =  (CalDialogBookLocalItem *)elm_object_item_data_get(next);
				bookItem ->setItemCheckState(is_selectall);
				if(is_selectall)
					++__checkedCount;
			}
		}
		next = elm_genlist_item_next_get(next);
	}

	if (!is_selectall) {
		elm_object_disabled_set(__deleteBtn, EINA_TRUE);
	} else {
		elm_object_disabled_set(__deleteBtn, EINA_FALSE);
	}
	return;
}



void CalBookDeleteView::__updateSelectInfo()
{
	const int titleBuffer = 100;
	char title[titleBuffer] = {0};

	snprintf(title, titleBuffer, _L_("IDS_CLD_HEADER_PD_SELECTED_ABB3"), __checkedCount);
	setTitle(title);

	if (__checkedCount <= 0)
	{
		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", _L_("IDS_CLD_HEADER_DELETE"));
	}
	else
	{
		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", getTitle());
	}
}


Evas_Object* CalBookDeleteView::onCreate(Evas_Object* parent, void* viewParam)
{
	Evas_Object* layout = elm_layout_add(parent);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_file_set(layout, CAL_EDJE, "CalDeleteView");

	Evas_Object* box = elm_box_add(layout);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "sw", box);

	Evas_Object* select_all_layout = elm_layout_add(layout);
	elm_layout_theme_set(select_all_layout, "genlist", "item", "select_all/default");
	__selectAllLayout = select_all_layout;
	evas_object_size_hint_weight_set(select_all_layout, EVAS_HINT_EXPAND, EVAS_HINT_FILL);
	evas_object_size_hint_align_set(select_all_layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_event_callback_add(select_all_layout, EVAS_CALLBACK_MOUSE_UP,
		[](void *data, Evas *e, Evas_Object *obj, void *eventInfo){
			CalBookDeleteView* self = (CalBookDeleteView*)data;
			Evas_Object* check = elm_object_part_content_get(obj, "elm.icon");
			if(!check)
				return;

			Eina_Bool isCheck = elm_check_state_get(check);
			elm_check_state_set(check, !isCheck);
			self->__onSelectAllSelect(elm_check_state_get(check));
			self->__updateSelectInfo();
		},
	this);

	elm_object_part_text_set(select_all_layout, "elm.text.main", _L_("IDS_CLD_OPT_SELECT_ALL"));

	Evas_Object* check = elm_check_add(select_all_layout);

	__selectAllCheckbox = check;
	evas_object_smart_callback_add(check, "changed",
		[](void *data, Evas_Object *obj, void *eventInfo){
				CalBookDeleteView* self = (CalBookDeleteView*)data;
				self->__onSelectAllSelect(elm_check_state_get(obj));
				self->__updateSelectInfo();
		},
	this);

	evas_object_propagate_events_set(check, EINA_FALSE);

	elm_object_part_content_set(select_all_layout, "elm.icon", check);

	elm_box_pack_start(box, select_all_layout);
	evas_object_show(select_all_layout);

	CalDialogControl *dialog = new CalDialogControl;
	dialog->create(layout, NULL);
	__dialog = dialog;
	__update();
	elm_box_pack_end(box, dialog->getEvasObj());
	return layout;
}

void CalBookDeleteView::__deleteBooks()
{
	Elm_Object_Item* it = elm_genlist_first_item_get(__dialog->getEvasObj());
	Elm_Object_Item* next = it;

	int all_check_item = elm_genlist_items_count(__dialog->getEvasObj())-2;
	while(next) {
		CalDialogControl::Item *item = (CalDialogControl::Item *)elm_object_item_data_get(next);
		if (!item->isGroupTitle() && !elm_object_item_disabled_get(item->getElmObjectItem())) {
			CalDialogBookLocalItem *bookItem =  (CalDialogBookLocalItem *)elm_object_item_data_get(next);
			if (bookItem ->getItemCheckState()) {
				CalBookManager::getInstance().deleteBook(bookItem->getLocalBook());
				all_check_item--;
			}
		}
		next = elm_genlist_item_next_get(next);
	}
}

void CalBookDeleteView::onEvent(const CalEvent& event)
{
	WENTER();
	WDEBUG("type = %u, source = %u", event.type, event.source);
	switch (event.type) {
		case CalEvent::BOOK_DB_CHANGED:
		case CalEvent::TIME_CHANGED:
		case CalEvent::LANGUAGE_CHANGED:
		case CalEvent::SETTING_CHANGED:
			{
				destroyPopup();
				__update();
			}
			break;
		default:
			WERROR("invalid type = %u", event.type);
			break;
	}
}


void CalBookDeleteView::__onDeleteButton()
{

	CalEventManager::getInstance().block(this);
	__deleteBooks();
	popOut();
}

void CalBookDeleteView::onPushed(Elm_Object_Item* naviItem)
{
	Evas_Object* button = elm_button_add(getNaviframe()->getEvasObj());
	elm_object_style_set(button, "naviframe/title_left");
	elm_object_text_set(button, _L_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));
	evas_object_smart_callback_add(button, "clicked",
		[](void* data, Evas_Object* obj, void* event_info){
			Evas_Object* naviframe = (Evas_Object*)data;
			elm_naviframe_item_pop(naviframe);
		}, getNaviframe()->getEvasObj()
	);
	elm_object_item_part_content_set(naviItem, "title_left_btn", button);

	button = elm_button_add(getNaviframe()->getEvasObj());
	__deleteBtn = button;
	elm_object_style_set(button, "naviframe/title_right");
	elm_object_text_set(button, _L_("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));
	evas_object_smart_callback_add(button, "clicked",
		[](void* data, Evas_Object* obj, void* event_info) {
			CalBookDeleteView* pCalBookDeleteView = (CalBookDeleteView*)data;
			pCalBookDeleteView->__onDeleteButton();
		}, this
	);
	elm_object_item_part_text_set(getNaviItem(), "elm.text.title", _L_("IDS_CLD_HEADER_DELETE"));
	elm_object_disabled_set(__deleteBtn, EINA_TRUE);
	elm_object_item_part_content_set(naviItem, "title_right_btn", button);
}

void CalBookDeleteView::__update()
{
	__checkedCount = 0;
	__allItemsCount = 0;

	elm_object_text_set(elm_object_item_part_content_get(getNaviItem(), "title_left_btn"), _L_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));
	elm_object_text_set(elm_object_item_part_content_get(getNaviItem(), "title_right_btn"), _L_("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));

	if (__selectAllLayout) {
		elm_object_part_text_set(__selectAllLayout, "elm.text.main", _L_("IDS_CLD_OPT_SELECT_ALL"));
	}

	if (getNaviItem()) {
		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", _L_("IDS_CLD_HEADER_DELETE"));
	}

	elm_genlist_clear(__dialog->getEvasObj());

	__bookMap.clear();
	CalBookManager::getInstance().getBooks(__bookMap);

	// My calendar group
	elm_genlist_item_select_mode_set(__dialog->add(new CalDialogBookGroupTitleItem(_L_("IDS_CLD_OPT_MY_CALENDAR_ABB"))), ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	// My calendar books
	for (auto it = __bookMap.begin(); it != __bookMap.end(); it++) {
		auto node = it->second;

		if (node == nullptr) {
			WERROR("node is null");
			continue;
		}
		int index = node->getIndex();

		if (index == DEFAULT_BIRTHDAY_CALENDAR_BOOK_ID) {
			continue;
		}

		if (index == DEFAULT_EVENT_CALENDAR_BOOK_ID || node->isReadOnly()) {
				CalDialogBookLocalItem* item = new CalDialogBookLocalItem(CalDialogBookLocalItem::LocalBookDisplay, node);
				Elm_Object_Item* itemObj = __dialog->add(item);
				//disable, don't add __allItemsCount
				elm_genlist_item_select_mode_set(itemObj, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
				elm_object_item_disabled_set(itemObj, EINA_TRUE);
		} else if (index != DEFAULT_BIRTHDAY_CALENDAR_BOOK_ID) {
			CalDialogBookLocalItem* item = new CalDialogBookLocalItem(CalDialogBookLocalItem::LocalBookDelete,node);
			item->setDeleteSelectCb(
				[this] (CalDialogBookLocalItem* item) {
					__onSelect(item);
					__updateSelectInfo();
				}
			);
			item->setCheckboxCb(
				[this] (bool state) {
					__onCheckBox(state);
					__updateSelectInfo();
				}
			);
			__dialog->add(item); // don't worry about memory leak of item
			__allItemsCount++;
		}
	}

	__updateSelectInfo();
}

void CalBookDeleteView::onDestroy()
{
}

