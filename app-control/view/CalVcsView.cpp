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
#include "CalVcsView.h"
#include "WMenuPopup.h"
#include "CalDataManager.h"
#include "CalDetailView.h"
#include "CalCustomListModel.h"
#include <notification.h>

#define TITLE_SIZE 128

CalVcsView::CalVcsView(const std::list<std::shared_ptr<CalSchedule>>& schedules) : CalView("CalVcsView"),
	__schedules(schedules),
	__isSelectMode(false),
	__box(NULL),
	__selectAllLayout(NULL),
	__focusDate(),
	__list(NULL),
	__fowardModel(NULL),
	__emptyModel(NULL)
{
}

CalVcsView::~CalVcsView()
{
}

Evas_Object* CalVcsView::onCreate(Evas_Object* parent, void* viewParam)
{
	Evas_Object* layout = elm_layout_add(parent);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_theme_set(layout, "layout", "application", "default");

	__box = elm_box_add(layout);
	evas_object_size_hint_weight_set(__box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "elm.swallow.content", __box);

	__selectAllLayout = elm_layout_add(__box);
	elm_layout_theme_set(__selectAllLayout, "genlist", "item", "select_all/default");
	evas_object_size_hint_weight_set(__selectAllLayout, EVAS_HINT_EXPAND, EVAS_HINT_FILL);
	evas_object_size_hint_align_set(__selectAllLayout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_event_callback_add(__selectAllLayout, EVAS_CALLBACK_MOUSE_UP,
		[](void *data, Evas *e, Evas_Object *obj, void *eventInfo){
			CalVcsView* self = (CalVcsView*)data;
			Evas_Object* check = elm_object_part_content_get(obj, "elm.icon");
			if(!check)
				return;

			Eina_Bool isCheck = elm_check_state_get(check);
			elm_check_state_set(check, !isCheck);
			self->__list->selectAllSchduleItem(!isCheck);
			self->__updateTitleInfo();
		}, this);

	elm_object_part_text_set(__selectAllLayout, "elm.text.main", _L_("IDS_CLD_OPT_SELECT_ALL"));

	Evas_Object* check = elm_check_add(__selectAllLayout);
	evas_object_smart_callback_add(check, "changed",
		[](void *data, Evas_Object *obj, void *eventInfo){
				CalVcsView* self = (CalVcsView*)data;
				self->__list->selectAllSchduleItem(elm_check_state_get(obj));
				self->__updateTitleInfo();
			}, this);

	evas_object_propagate_events_set(check, EINA_FALSE);

	elm_object_part_content_set(__selectAllLayout, "elm.icon", check);

	if(__isSelectMode){
		elm_box_pack_start(__box, __selectAllLayout);
		evas_object_show(__selectAllLayout);
	} else {
		elm_box_unpack(__box, __selectAllLayout);
		evas_object_hide(__selectAllLayout);
	}

	__fowardModel = new CalCustomListModel(__schedules);

	std::list<std::shared_ptr<CalSchedule>> emptySchedules;
	__emptyModel = new CalCustomListModel(emptySchedules);
	__list = new CalScheduleListControl(__fowardModel, __emptyModel,
		[this](std::shared_ptr<CalSchedule> schedule) {
			if(__isSelectMode)
				__updateTitleInfo();
			else
				getNaviframe()->push(new CalDetailView(schedule, CalDetailView::MENU_SAVEONLY));
		}, NULL, NULL, true, !__isSelectMode);

	__list->create(layout, NULL);
	if(__list->isEmpty()){
		Evas_Object* noContents = elm_layout_add(layout);
		elm_layout_theme_set(noContents,"layout","nocontents","search");
		evas_object_size_hint_weight_set(noContents, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(noContents, EVAS_HINT_FILL, EVAS_HINT_FILL);
		elm_object_part_text_set(noContents, "elm.text", _L_("IDS_CLD_BODY_NO_EVENTS"));
		evas_object_show(noContents);
		elm_box_pack_end(__box, noContents);
	}else{
		elm_box_pack_end(__box, __list->getEvasObj());
	}
	return layout;
}

void CalVcsView::onDestroy()
{
	if(__selectAllLayout){
		evas_object_del(__selectAllLayout);
		__selectAllLayout = NULL;
	}

	if(__fowardModel){
		delete __fowardModel;
		__fowardModel = NULL;
	}

	if(__emptyModel){
		delete __emptyModel;
		__emptyModel = NULL;
	}
}

void CalVcsView::onBecomeTop()
{
}

void CalVcsView::onExpose()
{
}

void CalVcsView::onPushed(Elm_Object_Item* naviItem)
{
	if(__isSelectMode){
		Evas_Object* button = elm_button_add(getNaviframe()->getEvasObj());
		elm_object_style_set(button, "naviframe/title_left");
		elm_object_text_set(button, _L_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));
		evas_object_smart_callback_add(button, "clicked",
			[](void* data, Evas_Object* obj, void* eventInfo){
				CalVcsView* self = (CalVcsView*)data;
				self->__switchMode(false);
			}, this
		);

		Evas_Object* button1 = elm_button_add(getNaviframe()->getEvasObj());
		elm_object_style_set(button1, "naviframe/title_right");
		elm_object_text_set(button, _L_("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));
		evas_object_smart_callback_add(button1, "clicked",
			[](void* data, Evas_Object* obj, void* eventInfo){
				CalVcsView* self = (CalVcsView*)data;
				self->__save();
			}, this
		);
		elm_object_item_part_content_set(naviItem, "title_left_btn", button);
		elm_object_item_part_content_set(naviItem, "title_right_btn", button1);
		evas_object_show(button);
		evas_object_show(button1);
	}else{
		evas_object_del(elm_object_item_part_content_unset(naviItem, "title_left_btn"));
		evas_object_del(elm_object_item_part_content_unset(naviItem, "title_right_btn"));
	}

	if(!__list->isEmpty())
		activateMenuButton(naviItem);

	__updateTitleInfo();
}

void CalVcsView::__save()
{
	std::list<std::shared_ptr<CalSchedule>> selectedScheduleList;
	__list->getSelectedSchedules(selectedScheduleList);

	if(selectedScheduleList.empty())
		return;

	WDEBUG("Todo, save to calendar");
	notification_status_message_post(_L_("IDS_CLD_TPOP_EVENT_SAVED"));

	for (auto it= selectedScheduleList.begin(); it != selectedScheduleList.end(); ++it) {
		(*it)->setBookId(DEFAULT_EVENT_CALENDAR_BOOK_ID);
		CalDataManager::getInstance().insertSchedule(**it);
	}
	__switchMode(false);
}

void CalVcsView::onEvent(const CalEvent& event)
{
	switch (event.type) {
		case CalEvent::LANGUAGE_CHANGED:
			elm_object_text_set(elm_object_item_part_content_get(getNaviItem(), "title_left_btn"), _L_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));
			elm_object_text_set(elm_object_item_part_content_get(getNaviItem(), "title_right_btn"), _L_("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));
			break;
		default:
			WERROR("invalid type = %u", event.type);
			break;
	}
}

void CalVcsView::onMenuButton()
{
	WENTER();
	if(__list->isEmpty() || __isSelectMode)
		return;

	WMenuPopup* popup = new WMenuPopup();
	popup->prepare(getWindow()->getEvasObj(), getNaviframe()->getEvasObj());
	popup->appendItem(_L_("IDS_CLD_OPT_SAVE_IN_CALENDAR_ABB"), [this]() {
		__switchMode(true);
	});
	attachPopup( popup );
}

bool CalVcsView::onPop()
{
	WENTER();
	if(__isSelectMode){
		__switchMode(false);
		return false;
	}

	WLEAVE();
	return true;
}

void CalVcsView::__switchMode(bool isSelectMode)
{
	__isSelectMode = isSelectMode;
	__list->showCheckBox(__isSelectMode);

	if(__isSelectMode){
		__list->selectAllSchduleItem(false);
		elm_box_pack_start(__box, __selectAllLayout);
		evas_object_show(__selectAllLayout);
	} else {
		elm_box_unpack(__box, __selectAllLayout);
		evas_object_hide(__selectAllLayout);
	}
	if(__isSelectMode){
		Evas_Object* button = elm_button_add(getNaviframe()->getEvasObj());
		elm_object_style_set(button, "naviframe/title_left");
		elm_object_text_set(button, _L_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));
		evas_object_smart_callback_add(button, "clicked",
			[](void* data, Evas_Object* obj, void* eventInfo){
				CalVcsView* self = (CalVcsView*)data;
				self->__switchMode(false);
			}, this
		);
		Evas_Object* button1 = elm_button_add(getNaviframe()->getEvasObj());
		elm_object_style_set(button1, "naviframe/title_right");
		elm_object_text_set(button, _L_("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));
		evas_object_smart_callback_add(button1, "clicked",
			[](void* data, Evas_Object* obj, void* eventInfo){
				CalVcsView* self = (CalVcsView*)data;
				self->__save();
			}, this
		);
		elm_object_item_part_content_set(getNaviItem(), "title_left_btn", button);
		elm_object_item_part_content_set(getNaviItem(), "title_right_btn", button1);
		evas_object_show(button);
		evas_object_show(button1);
	}else{
		evas_object_del(elm_object_item_part_content_unset(getNaviItem(), "title_left_btn"));
		evas_object_del(elm_object_item_part_content_unset(getNaviItem(), "title_right_btn"));
	}
	__updateTitleInfo();
}

void CalVcsView::__updateTitleInfo()
{
	char title[TITLE_SIZE] = {0};

	int selectedItemCount = __list->getSelectSchduleItemCount();
	int itemCount = __list->getSchduleItemCount();

	if(__isSelectMode){
		snprintf(title, sizeof(title), _L_("IDS_CLD_HEADER_PD_SELECTED_ABB3"), selectedItemCount);
	}else{
		snprintf(title, sizeof(title), _L_("IDS_CLD_HEADER_EVENTS"));
	}
	setTitle(title);

	elm_object_item_part_text_set(getNaviItem(), "elm.text.title", getTitle());
	if(getNaviItem() && __isSelectMode){
		Evas_Object* button = elm_object_item_part_content_get(getNaviItem(), "title_right_btn");
		elm_object_disabled_set(button, selectedItemCount > 0 ? EINA_FALSE : EINA_TRUE);
	}

	Evas_Object* check = elm_object_part_content_get(__selectAllLayout, "elm.icon");

	if(!check)
		return;

	if(itemCount == selectedItemCount && itemCount > 0){
		elm_check_state_set(check, EINA_TRUE);
	}else{
		elm_check_state_set(check, EINA_FALSE);
	}
}
