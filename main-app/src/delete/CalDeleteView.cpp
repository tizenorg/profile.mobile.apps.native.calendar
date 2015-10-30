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
#include "CalCommon.h"
#include "CalDataManager.h"
#include "CalInstanceSchedule.h"
#include "CalDeleteView.h"
#include "WPopup.h"
#include "CalListModelFactory.h"

#include "CalEventManager.h"

CalDeleteView ::CalDeleteView(const CalDate& focusDate): CalView("CalDeleteView"),
	__isDeleteMode(NULL),
	__list(NULL),
	__focusDate(focusDate),
	__box(NULL),
	__isNoContents(false),
	__noContents(NULL),
	__fowardModel(NULL),
	__backwardModel(NULL)
{
}

CalDeleteView ::~CalDeleteView ()
{

}

/**
 * Executes the create action. This function is called when CalDeleteView object is created.
 *
 * @param [in]	parent   	If non-null, the parent.
 * @param [in]	viewParam	If non-null, the view parameter.
 *
 * @return	A CalDeleteView.
 */
Evas_Object* CalDeleteView ::onCreate(Evas_Object* parent, void* viewParam)
{
	Evas_Object* layout = elm_layout_add(parent);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_file_set(layout, CAL_EDJE, "CalDeleteView");

	__box = elm_box_add(layout);
	evas_object_size_hint_weight_set(__box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(__box, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_part_content_set(layout, "sw", __box);

	return layout;
}

/**
 * Executes the pushed action. This function is called when our view was successfully pushed into naviframe container.
 *
 * @param [in]	naviItem	If non-null, the navi item.
 *
 * @return	A CalDeleteView.
 */
void CalDeleteView ::onPushed(Elm_Object_Item* naviItem)
{
	Evas_Object* button = elm_button_add(getNaviframe()->getEvasObj());
	elm_object_style_set(button, "naviframe/title_left");
	elm_object_text_set(button, _L_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));
	evas_object_smart_callback_add(button, "clicked",
		[](void* data, Evas_Object* obj, void* eventInfo){
			Evas_Object* naviframe = (Evas_Object*)data;
			elm_naviframe_item_pop(naviframe);
		}, getNaviframe()->getEvasObj()
	);
	elm_object_item_part_content_set(naviItem, "title_left_btn", button);

	button = elm_button_add(getNaviframe()->getEvasObj());
	elm_object_style_set(button, "naviframe/title_right");
	elm_object_text_set(button, _L_("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));
	evas_object_smart_callback_add(button, "clicked",
		[](void* data, Evas_Object* obj, void* event_info){
			CalDeleteView* self = (CalDeleteView*)data;
			self->__delete();
		}, this
	);
	elm_object_item_part_content_set(naviItem, "title_right_btn", button);

	__update();
}

/**
 * Updates this object.
 */
void CalDeleteView ::__update()
{
	WPRET_M(!getEvasObj(), "layout is NULL");

	elm_object_text_set(elm_object_item_part_content_get(getNaviItem(), "title_left_btn"), _L_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));
	elm_object_text_set(elm_object_item_part_content_get(getNaviItem(), "title_right_btn"), _L_("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));

	if(__list){
		evas_object_del(__list->getEvasObj());
		__list = NULL;
	}

	if(__fowardModel){
		delete __fowardModel;
		__fowardModel = NULL;
	}

	if(__backwardModel){
		delete __backwardModel;
		__backwardModel = NULL;
	}

	__fowardModel = CalListModelFactory::getInstance().getDeleteList(__focusDate, 1);
	__backwardModel = CalListModelFactory::getInstance().getDeleteList(__focusDate, -1);

	__list = new CalScheduleListControl(__fowardModel, __backwardModel,
		[this](std::shared_ptr<CalSchedule> schedule) {
				__updateTitleInfo();
		},
		NULL, NULL, true, false
	);

	__list->create(getEvasObj(), NULL);

	if(__list->isEmpty()) {
		evas_object_del(__list->getEvasObj());
		__list = NULL;
		__isNoContents = true;
	} else {
		__isNoContents = false;
		elm_box_pack_end(__box, __list->getEvasObj());
	}

	__showNoContent(__isNoContents);
	__updateTitleInfo();
}

/**
 * Executes the destroy action. This function is called when our view object is being destroyed.
 */
void CalDeleteView::onDestroy()
{

	if(__fowardModel){
		delete __fowardModel;
		__fowardModel = NULL;
	}

	if(__backwardModel){
		delete __backwardModel;
		__backwardModel = NULL;
	}

}

/**
 * Executes some action, depending on event it receives.
 *
 * @param	event	The event.
 */
void CalDeleteView::onEvent(const CalEvent& event)
{
	WENTER();
	WDEBUG("type = %u, source = %u", event.type, event.source);
	switch (event.type) {
		case CalEvent::DB_CHANGED:
		case CalEvent::TIME_CHANGED:
		case CalEvent::LANGUAGE_CHANGED:
		case CalEvent::SETTING_CHANGED:
			destroyPopup();
			__update();
			break;
		default:
			WERROR("invalid type = %u", event.type);
			break;
	}
}

/**
 * Gets repeated schedule count. If calendar service contains some records 'bout events, which have repeats assigned to, they will be returned in 1st parameter.
 *
 * @param [in]	selectedScheduleList	The selected schedule list.
 *
 * @return	The repeated schedule count.
 */
int CalDeleteView ::__getRepeatedScheduleCount(std::list<std::shared_ptr<CalSchedule>>& selectedScheduleList)
{
	int count = 0;
	for (auto it= selectedScheduleList.begin(); it!= selectedScheduleList.end(); ++it) {
		if((*it)->hasRepeat())
			count++;
	}
	return count;
}

/**
 * Deletes this object.
 */
void CalDeleteView ::__delete()
{
	WPRET_M(!__list, "error: __list is NULL");

	int selectedCount = __list->getSelectSchduleItemCount();
	int itemCount = __list->getSchduleItemCount();

	char text[300] = {0};
	if(selectedCount <= 0){
		return;
	}

	std::list<std::shared_ptr<CalSchedule>> selectedScheduleList;
	__list->getSelectedSchedules(selectedScheduleList);

	const int repeatedSchedulesCount = __getRepeatedScheduleCount(selectedScheduleList);

	WPopup *popup = NULL;
	if (repeatedSchedulesCount <= 0){
		if(selectedCount == 1){
			sprintf(text, _L_("IDS_CLD_BODY_1_EVENT_WILL_BE_DELETED"));
		} else if(selectedCount == itemCount){
			sprintf(text, _L_("IDS_CLD_POP_ALL_EVENTS_WILL_BE_DELETED")); // need to change
		} else {
			snprintf(text, sizeof(text)-1, _L_("IDS_CLD_POP_PD_EVENTS_WILL_BE_DELETED"), selectedCount);
		}
		 popup = new WPopup( _L_("IDS_CLD_HEADER_DELETE"), text);
	} else {
		if(repeatedSchedulesCount == 1)
			snprintf(text, sizeof(text)-1, _L_("IDS_CLD_POP_1_OF_THE_EVENTS_YOU_HAVE_SELECTED_TO_DELETE_IS_A_RECURRING_EVENT_DELETE_ALL_OCCURRENCES_OF_THIS_EVENT_Q"));
		else
			snprintf(text, sizeof(text)-1, _L_("IDS_CLD_POP_PD_OF_THE_EVENTS_YOU_HAVE_SELECTED_TO_DELETE_ARE_RECURRING_EVENTS_DELETE_ALL_OCCURRENCES_OF_THESE_EVENTS_Q"),
				repeatedSchedulesCount);
		popup = new WPopup( _L_("IDS_CLD_HEADER_DELETE"), text);
	}

	popup->addButton(_L_("IDS_CLD_BUTTON_CANCEL"), NULL);
	popup->addButton(_L_("IDS_CLD_BUTTON_DELETE_ABB"), [this, selectedScheduleList](bool* destroyPopup){
		CalEventManager::getInstance().block(this);
		CalDataManager::getInstance().deleteSchedules(selectedScheduleList);
		popOut();
		*destroyPopup = false;
	});
	attachPopup( popup );
}

/**
 * Depending on paramater, shows view contents or hids them.
 *
 * @param	isShow	true if this object is show.
 */
void CalDeleteView::__showNoContent(bool isShow)
{
	if(isShow && !__noContents){
		__noContents = elm_layout_add(getEvasObj());
		elm_layout_theme_set(__noContents,"layout","nocontents","search");
		evas_object_size_hint_weight_set(__noContents, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(__noContents, EVAS_HINT_FILL, EVAS_HINT_FILL);
		elm_object_part_text_set(__noContents, "elm.text", _L_("IDS_CLD_BODY_NO_EVENTS"));
		evas_object_show(__noContents);
		elm_box_pack_end(__box, __noContents);
	}else if(!isShow && __noContents){
		evas_object_del(__noContents);
		__noContents = NULL;
	}
}

/**
 * Updates the title information.
 */
void CalDeleteView::__updateTitleInfo()
{
	char title[100] = {0};

	int selectedCount = 0;
	if(__list)
		selectedCount = __list->getSelectSchduleItemCount();

	if(selectedCount > 0){
		snprintf(title, sizeof(title)-1, _L_("IDS_CLD_HEADER_PD_SELECTED_ABB3"), selectedCount);
	}else{
		sprintf(title, "%s", _L_("IDS_CLD_HEADER_DELETE"));
	}

	setTitle(title);
	if(getNaviItem()){
		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", getTitle());
		Evas_Object* button = elm_object_item_part_content_get(getNaviItem(), "title_right_btn");
		elm_object_disabled_set(button, selectedCount > 0 ? EINA_FALSE : EINA_TRUE);
	}
}


