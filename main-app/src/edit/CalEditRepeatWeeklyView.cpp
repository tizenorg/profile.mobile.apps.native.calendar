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

#include <functional>
#include "CalCommon.h"
#include "CalView.h"
#include "CalDialogControl.h"
#include "CalScheduleRepeat.h"

#include "CalDialogEditRepeatWeeklyUnitInfoItem.h"
#include "CalDialogEditRepeatUnitItem.h"
#include "CalDialogSimpleTitleItem.h"
#include "CalDialogEditRepeatUntilForeverItem.h"
#include "CalDialogEditRepeatUntilTimesItem.h"
#include "CalDialogEditRepeatUntilDueDateItem.h"
#include "CalDatePickerPopup.h"

#include "CalEditRepeatWeeklyView.h"
#include "CalNumberPickerPopup.h"
#include <notification.h>

CalEditRepeatWeeklyView::CalEditRepeatWeeklyView(const CalScheduleRepeat repeatOri, const CalDateTime& dateTime, const char* timezone, std::function<void (const CalScheduleRepeat repeat)> doneCb) :
	CalView("CalEditRepeatWeeklyView"),
	__doneCb(doneCb),
	__startTime(dateTime),
	__dialog(NULL),
	__doneButton(NULL)
{
	__repeat = CalScheduleRepeat(repeatOri);
	if (timezone)
		__timezone = timezone;
	__genlist = NULL;
}

CalEditRepeatWeeklyView::~CalEditRepeatWeeklyView()
{
}

static bool __isNoDaySelected(CalScheduleRepeat::UnitInfo unitInfo)
{
	for ( auto index = 0; index < 7; index++)
	{
		if (unitInfo.weekly.selected[index])
			return false;
	}

	return true;
}

static void __setDisable(Evas_Object* object, bool isDisable)
{
	elm_object_disabled_set(object, isDisable ? EINA_TRUE : EINA_FALSE);
}
void CalEditRepeatWeeklyView::__createGenlist()
{
	Evas_Object *radio_main = elm_radio_add(__genlist);
	elm_radio_state_value_set(radio_main, 0);
	elm_radio_value_set(radio_main, 0);
	evas_object_data_set(__genlist, "radio_main", radio_main);

	// repeat interval
	//__dialog->add(new CalDialogSimpleTitleItem(_L_("Interval")));

	CalDialogEditRepeatUnitItem *unitItem = new CalDialogEditRepeatUnitItem(
		[this](CalScheduleRepeat::UnitType unitType, int unitInterval)->void
		{
			__repeat.unitType = unitType;
			__repeat.unitInterval = unitInterval;
		}, __repeat, CalScheduleRepeat::WEEK);
	unitItem->setDateButtonClickedCb( [this, unitItem](){
		CalNumberPickerPopup* popup = new CalNumberPickerPopup(__repeat, false);
		popup->setCancelCb([this, popup](){
			});
		popup->setDoneCb([this, popup, unitItem](const CalScheduleRepeat & repeat) {
				__repeat = repeat;
				unitItem->setDateButtonTime(__repeat);
				unitItem->updateUnitsLabel();
		});
		popup->create(getNaviframe()->getEvasObj(), NULL);
		attachPopup(popup);
	});
	Elm_Object_Item *itemObject = __dialog->add(unitItem);
	if (itemObject)
		elm_genlist_item_select_mode_set(itemObject, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	__dialog->add(new CalDialogEditRepeatWeeklyUnitInfoItem(
		[this](CalScheduleRepeat::UnitInfo unitInfo)->void
		{
			__repeat.unitInfo.weekly = unitInfo.weekly;

			if (__isNoDaySelected(unitInfo))
				__setDisable(__doneButton, true);
			else
				__setDisable(__doneButton, false);
		}, __repeat, __startTime, __timezone)
	);

	// repeat until
	__dialog->add(new CalDialogSimpleTitleItem(_L_S_("IDS_CLD_HEADER_REPEAT_UNTIL")));

	int radioIndex = 1;

	__dialog->add(new CalDialogEditRepeatUntilForeverItem(
		[this](CalScheduleRepeat::UntilType untilType, CalScheduleRepeat::UntilInfo untilInfo)->void
		{
			__repeat.untilType = untilType;
		}, __repeat, radioIndex++)
	);

	CalDialogEditRepeatUntilTimesItem *untilTimesItem = new CalDialogEditRepeatUntilTimesItem(
		[this](CalScheduleRepeat::UntilType untilType, CalScheduleRepeat::UntilInfo untilInfo)->void
		{
			__repeat.untilType = untilType;
			__repeat.untilInfo = untilInfo;
		}, __repeat, radioIndex++);
	untilTimesItem->setDateButtonClickedCb( [this, untilTimesItem](Evas_Object* dateTime){
		CalNumberPickerPopup* popup = new CalNumberPickerPopup(__repeat, true);
		popup->setCancelCb([this, popup](){
			});
		popup->setDoneCb([this, popup, untilTimesItem](const CalScheduleRepeat & repeat) {
				__repeat = repeat;
				untilTimesItem->setDateButtonTime(__repeat);
		});
		popup->create(getNaviframe()->getEvasObj(), NULL);
		attachPopup(popup);
		});
	__dialog->add(untilTimesItem);


	CalDialogEditRepeatUntilDueDateItem *untilDateItem = new CalDialogEditRepeatUntilDueDateItem(
		[this](CalScheduleRepeat::UntilType untilType, CalScheduleRepeat::UntilInfo untilInfo)->void
		{
			__repeat.untilType = untilType;
			__repeat.untilInfo = untilInfo;
		}, __repeat, radioIndex++, __startTime);
	untilDateItem->setDateButtonClickedCb( [this, untilDateItem](Evas_Object* dateTime){

		CalDatePickerPopup* popup = new CalDatePickerPopup(untilDateItem->getUntilDate());
		popup->setChangeCb([this, dateTime, untilDateItem](CalDate& newdate){
			__repeat.untilInfo.date.mday = newdate.getMday();
			__repeat.untilInfo.date.month = newdate.getMonth();
			__repeat.untilInfo.date.year = newdate.getYear();
			__repeat.untilType = CalScheduleRepeat::DUE_DATE;

			untilDateItem->setDateButtonTime(__repeat);
		});

		popup->create(getNaviframe()->getEvasObj(), NULL);
		attachPopup(popup);
	});
	__dialog->add(untilDateItem);
}

Evas_Object* CalEditRepeatWeeklyView::onCreate(Evas_Object* parent, void* viewParam)
{
	__dialog = new CalDialogControl();
	__dialog->create(parent, NULL);

	Evas_Object *genlist = __dialog->getEvasObj();
	elm_genlist_highlight_mode_set(genlist, EINA_FALSE);
	__genlist = genlist;
	__createGenlist();

	return genlist;
}

void CalEditRepeatWeeklyView::onPushed(Elm_Object_Item* naviItem)
{
	elm_object_item_part_text_set(getNaviItem(), "elm.text.title", _L_("IDS_CLD_BUTTON_REPEAT_ABB"));

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
	elm_object_style_set(button, "naviframe/title_right");
	elm_object_text_set(button, _L_("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));
	evas_object_smart_callback_add(button, "clicked",
		[](void* data, Evas_Object* obj, void* event_info){
		CalEditRepeatWeeklyView *pView = (CalEditRepeatWeeklyView *)data;
		//TODO: Need to set data to schedule
		if (pView->__doneCb) {
			CalDateTime dateTime;
			dateTime.set(pView->__repeat.untilInfo.date.year, pView->__repeat.untilInfo.date.month, pView->__repeat.untilInfo.date.mday);
			if (pView->__repeat.untilType == CalScheduleRepeat::UntilType::DUE_DATE && dateTime <= pView->__startTime) {
				notification_status_message_post(_L_("IDS_CLD_TPOP_END_DATE_SHOULD_BE_LATER_THAN_START_DATE"));
			} else {
				pView->__doneCb(pView->__repeat);
			}
		}
		Evas_Object* naviframe = pView->getNaviframe()->getEvasObj();
		elm_naviframe_item_pop(naviframe);
		}, this
	);
	elm_object_item_part_content_set(naviItem, "title_right_btn", button);
	__doneButton = button;

}

void CalEditRepeatWeeklyView::__update()
{
	elm_object_text_set(elm_object_item_part_content_get(getNaviItem(), "title_left_btn"), _L_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));
	elm_object_text_set(elm_object_item_part_content_get(getNaviItem(), "title_right_btn"), _L_("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));
	elm_object_item_part_text_set(getNaviItem(), "elm.text.title", _L_("IDS_CLD_BUTTON_REPEAT_ABB"));

	elm_genlist_clear(__genlist);
	__createGenlist();
}

void CalEditRepeatWeeklyView::onEvent(const CalEvent & event)
{
	WENTER();
	WDEBUG("type = %u, source = %u", event.type, event.source);
	switch (event.type) {
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


