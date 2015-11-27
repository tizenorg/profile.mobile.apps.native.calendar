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

#include <notification.h>
#include "CalCommon.h"
#include "CalPickView.h"
#include "CalDataManager.h"
#include "CalScheduleListControl.h"
#include "CalListModelFactory.h"
#include "WPopup.h"

#define CAL_PICK_TITLE_LENGTH 32

CalPickView::CalPickView(int maxLimit, ResultType resultType) : CalFilterView("CalPickView"),
	__maxLimit(maxLimit),
	__resultType(resultType)
{
}

CalPickView::~CalPickView()
{
}

void CalPickView::onPushed(Elm_Object_Item* naviItem)
{
	WENTER();
	Evas_Object* button = elm_button_add(getNaviframe()->getEvasObj());
	elm_object_style_set(button, "naviframe/title_left");
	elm_object_text_set(button, _L_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));
	evas_object_smart_callback_add(button, "clicked",
		[](void* data, Evas_Object* obj, void* event_info) {
			Evas_Object* naviframe = (Evas_Object*)data;
			elm_naviframe_item_pop(naviframe);
		}, getNaviframe()->getEvasObj()
	);
	elm_object_item_part_content_set(naviItem, "title_left_btn", button);

	button = elm_button_add(getNaviframe()->getEvasObj());
	elm_object_style_set(button, "naviframe/title_right");
	elm_object_text_set(button, _L_("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));
	evas_object_smart_callback_add(button, "clicked",
		[](void* data, Evas_Object* obj, void* event_info) {
			CalPickView* self = (CalPickView*)data;
			std::list<std::shared_ptr<CalSchedule>> selectedScheduleList;
			self->__selectedScheduleSet.getList(selectedScheduleList);
			if (self->__doneCb){
				self->__doneCb(selectedScheduleList);
			}
			Evas_Object* naviframe = self->getNaviframe()->getEvasObj();
			elm_naviframe_item_pop(naviframe);
		}, this
	);
	elm_object_item_part_content_set(naviItem, "title_right_btn", button);

	createList([this](std::shared_ptr<CalSchedule> schedule)
		{
			if(__selectedScheduleSet.exists(schedule))
			{
				WHIT();
				__selectedScheduleSet.remove(schedule);
			}
			else
			{
				WHIT();
				__selectedScheduleSet.add(schedule);
			}
			this->__updateTitleInfo();
		}, true);

	if(__maxLimit > 0)
	{
		__list->setMaxLimitSelectCb([this]()
		{
			char warningText[CAL_WARNING_MAX_LENGTH] = {0};
			if (__resultType == CalPickView::RESULT_TYPE_TEXT || __resultType == CalPickView::RESULT_TYPE_ID)
			{
				snprintf(warningText, sizeof(warningText)-1, _L_("IDS_CLD_TPOP_MAXIMUM_NUMBER_OF_EVENTS_THAT_CAN_BE_INSERTED_HPD_REACHED"), __maxLimit);
			}
			else
			{
				snprintf(warningText, sizeof(warningText)-1, _L_("IDS_CLD_TPOP_MAXIMUM_NUMBER_OF_ATTACHMENTS_HPD_REACHED"), __maxLimit);
			}

			notification_status_message_post(warningText);
		});
		__list->setMaxLimitCount(__maxLimit);
	}

	__list->setIsSelectedCb([this](std::shared_ptr<CalSchedule>& schedule)->bool
		{
			return __selectedScheduleSet.exists(schedule);
		});

	__list->setSelectedCount(__selectedScheduleSet.getCount());

	__updateTitleInfo();
	WLEAVE();
}

void CalPickView::onEvent(const CalEvent& event)
{
	WENTER();
	WDEBUG("type = %u, source = %u", event.type, event.source);
	switch (event.type)
	{
		case CalEvent::DB_CHANGED:
		case CalEvent::TIME_CHANGED:
		case CalEvent::LANGUAGE_CHANGED:
		case CalEvent::SETTING_CHANGED:
			updateList();
			__reCheck();
			break;
		default:
			WERROR("invalid type = %u", event.type);
		break;
	}
}

void CalPickView::setDoneCb(std::function<void (const std::list<std::shared_ptr<CalSchedule>>& schedules)> doneCb)
{
	__doneCb = doneCb;
}

void CalPickView::__updateTitleInfo()
{
	char title[CAL_PICK_TITLE_LENGTH] = {0};
	int selectedItemCount = 0;
	selectedItemCount = __selectedScheduleSet.getCount();

	if (__maxLimit > 0)
	{
		sprintf(title, "%d/%d", selectedItemCount, __maxLimit);
	}
	else
	{
		sprintf(title, _L_("IDS_CLD_HEADER_PD_SELECTED_ABB3"), selectedItemCount);
	}

	setTitle(title);
	if (getNaviItem())
	{
		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", getTitle());
		Evas_Object* button = elm_object_item_part_content_get(getNaviItem(), "title_right_btn");
		elm_object_disabled_set(button, selectedItemCount > 0 ? EINA_FALSE : EINA_TRUE);
	}
}

/*check the event has been deleted or not*/
void CalPickView::__reCheck()
{
	WENTER();
	if(__selectedScheduleSet.getCount() <= 0)
	{
		return;
	}

	std::list<std::shared_ptr<CalSchedule>> selectedScheduleList;
	__selectedScheduleSet.getList(selectedScheduleList);

	auto it = selectedScheduleList.begin();
	for( ; it != selectedScheduleList.end(); ++it)
	{
		int eventId = (*it)->getIndex();
		CalDateTime startTime;
		(*it)->getStart(startTime);
		if(!CalDataManager::getInstance().getInstanceSchedule(eventId, startTime)){
			/*event have been deleted*/
			__selectedScheduleSet.remove(*it);
		}
	}

}
