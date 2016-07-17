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
#include "CalNotificationsView.h"
#include "CalSchedule.h"
#include "CalAlertItem.h"
#include "CalNaviframe.h"
#include "CalStatusBarManager.h"
#include "CalNotificationsSelectAllItem.h"
#include "CalAppControlLauncher.h"


CalNotificationsView::CalNotificationsView(std::shared_ptr<CalAlertData> alertData) : CalView("CalNotificationsView"),
	__isAllVisible(false),
	__model(alertData),
	__dialog(nullptr),
	__selectAllItem(nullptr),
	__left_button(nullptr),
	__right_button(nullptr)
{
	WENTER();
}

CalNotificationsView::~CalNotificationsView()
{
	WENTER();
}

void CalNotificationsView::__updateSelectAllCheck(void)
{
	WENTER();
	__selectAllItem->setSelectAllVisibleState(__isAllVisible);
	elm_genlist_item_update(__selectAllItem->getElmObjectItem());
	WLEAVE();
}

void CalNotificationsView::__updateButtonStatus(bool leftStatus, bool rightStatus)
{
	WENTER();
	elm_object_disabled_set(__left_button, leftStatus);
	elm_object_disabled_set(__right_button, rightStatus);
	WLEAVE();
}

void CalNotificationsView::__updateSelectAllItems()
{
	WENTER();
	__isAllVisible = !__isAllVisible;
	__updateSelectAllCheck();

	for (auto it = __itemMap.begin(); it != __itemMap.end(); ++it)
	{
		CalAlertItem* item = it->second;
		elm_check_state_set(item->getCheckObject(), __isAllVisible);
	}
	__updateButtonStatus(!__isAllVisible, !__isAllVisible);
	WLEAVE();
}

void CalNotificationsView::__updateCheckStatus()
{
	WENTER();
	int checkedCount = 0;
	for (auto it = __itemMap.begin(); it != __itemMap.end(); ++it)
	{
		CalAlertItem* item = it->second;
		Evas_Object* obj = item->getCheckObject();
		if (elm_check_state_get(obj) == EINA_TRUE)
		{
			++checkedCount;
		}

	}

	__updateButtonStatus(!checkedCount, !checkedCount);

	if (__model.getCount() > 1)
	{
		__isAllVisible = (__model.getCount() == checkedCount);
		__updateSelectAllCheck();
	}
	WLEAVE();
}

void CalNotificationsView::__launchDetailApp(const std::shared_ptr<CalAlertNotificationItem>& alertItem)
{
	app_control_h req = NULL;
	app_control_create(&req);
	app_control_set_mime(req, APP_CONTROL_MIME_CALENDAR);
	app_control_set_operation(req, APP_CONTROL_OPERATION_VIEW);

	int recordIndex = alertItem->getScheduleId();
	char buf[ID_LENGTH] = {0};
	snprintf(buf, sizeof(buf), "%d", recordIndex);

	app_control_add_extra_data(req, APP_CONTROL_DATA_ID, buf);
	app_control_set_launch_mode(req, APP_CONTROL_LAUNCH_MODE_GROUP);

	CalAppControlLauncher::getInstance().sendLaunchRequest(req, NULL, NULL, __dialog->getEvasObj());
}

void CalNotificationsView::__update()
{
	WENTER();
	if (__dialog->getEvasObj())
	{
		elm_genlist_clear(__dialog->getEvasObj());
	}

	__itemMap.clear();

	int count = __model.getCount();

	if(count > 1)
	{
		__selectAllItem = new CalNotificationsSelectAllItem(__isAllVisible,
			[this] ()
			{
				__updateSelectAllItems();
			});

		__dialog->add(__selectAllItem);
	}
	else
	{
		__selectAllItem = NULL;
	}

	for (int i = 0; i < count; i++)
	{
		std::shared_ptr<CalAlertNotificationItem> alertItem = __model.getAt(i);
		CalAlertItem* item = new CalAlertItem(alertItem);

		item->setSelectCb(
				[this, alertItem, i] (void) {
						__launchDetailApp(alertItem);
						__model.dismiss(i);
				}
		);

		item->setCheckCb(
				[this, item] (void) {
						bool isSelect = elm_check_state_get(item->getCheckObject());
						elm_check_state_set(item->getCheckObject(), !isSelect);
						__updateCheckStatus();
				}
		);

		__dialog->add(item);
		__itemMap.insert(std::pair<int, CalAlertItem*>(i, item));
	}

	if (__itemMap.size() == 1)
	{
		__itemMap[0]->setCheckVisibility(false);
	}

	bool buttonStatus = !(__model.getCount() == 1);
	__updateButtonStatus(buttonStatus, buttonStatus);

	WLEAVE();
}

void CalNotificationsView::__createAlertEventGenlist(Evas_Object* parent, void* viewParam)
{
	WENTER();
	Evas_Object* box = elm_box_add(parent);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(parent, "sw", box);

	CalDialogControl *dialog = new CalDialogControl;
	dialog->create(parent, NULL);
	Evas_Object *genlist = dialog->getEvasObj();
	__dialog = dialog;

	elm_genlist_homogeneous_set(genlist, EINA_FALSE);
	elm_genlist_mode_set(genlist, ELM_LIST_SCROLL);

	__update();

	elm_box_pack_end(box, dialog->getEvasObj());
	evas_object_show(box);
	WLEAVE();
}


Evas_Object* CalNotificationsView::onCreate( Evas_Object* parent, void* viewParam )
{
	WENTER();

	Evas_Object* layout = elm_layout_add(parent);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_file_set(layout, CalPath::getPath(CAL_EDJE).c_str(), "Event_notification");

	__createAlertEventGenlist(layout, NULL);

	evas_object_show(layout);

	WLEAVE();
	return layout;
}

void CalNotificationsView::onDestroy()
{
	WENTER();
}

void CalNotificationsView::onBecomeTop()
{
	WENTER();
}

void CalNotificationsView::onExpose()
{
	WENTER();
}

void CalNotificationsView::onPushed(Elm_Object_Item* naviItem)
{
	WENTER();
	elm_object_item_part_text_set(naviItem, "elm.text.title", _L_("IDS_CLD_HEADER_EVENT_NOTIFICATIONS"));

	Evas_Object* layout = elm_layout_add(getNaviframe()->getEvasObj());
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_file_set(layout, CalPath::getPath(CAL_EDJE).c_str(), "toolbar_button");

	Evas_Object* left_button = elm_button_add(layout);
	elm_object_style_set(left_button, "bottom");

	evas_object_smart_callback_add(left_button, "clicked",
		[](void* data, Evas_Object* obj, void* event_info){
			CalNotificationsView* self = (CalNotificationsView*)data;
			if (self->__selectAllItem)
			{
				if(self->__isAllVisible)
				{
					WDEBUG("Dismiss all");
					self->__model.dismissAll();
				}
				else
				{
					WDEBUG("Dismiss selected");
					int i = 0;
					std::vector<int> nths;
					for (auto item : self->__itemMap)
					{
						Evas_Object* obj = item.second->getCheckObject();
						if (elm_check_state_get(obj))
						{
							nths.push_back(i);
						}
						i++;
					}
					self->__model.dismiss(nths);
				}
			}
			else
			{
				WDEBUG("Dismiss one");
				self->__model.dismissAll();
			}
			elm_exit();
		}, this
 	);

	elm_object_part_content_set(layout, "left_button", left_button);
	elm_object_text_set(left_button, _L_("IDS_CLD_BUTTON_DISMISS"));

	Evas_Object* right_button = elm_button_add(layout);
	elm_object_style_set(right_button, "bottom");

	evas_object_smart_callback_add(right_button, "clicked",
		[](void* data, Evas_Object* obj, void* event_info){
			CalNotificationsView* self = (CalNotificationsView*)data;
			if (self->__selectAllItem)
			{
				if(self->__isAllVisible)
				{
					WDEBUG("Snooze all");
					self->__model.snoozeAll();
				}
				else
				{
					WDEBUG("Snooze selected");
					int i = 0;
					for (auto item : self->__itemMap)
					{
						Evas_Object* obj = item.second->getCheckObject();
						if (elm_check_state_get(obj))
						{
							self->__model.snooze(i);
						}
						i++;
					}
				}
			}
			else
			{
				WDEBUG("Snooze one");
				self->__model.snoozeAll();
			}
			elm_exit();
		}, this
	);

	elm_object_part_content_set(layout, "right_button", right_button);
	elm_object_text_set(right_button, _L_("IDS_CLD_BUTTON_SNOOZE"));
	__left_button = left_button;
	__right_button = right_button;

	bool buttonStatus = !(__model.getCount() == 1);
	__updateButtonStatus(buttonStatus, buttonStatus);

	elm_object_item_part_content_set(naviItem, "toolbar", layout);
}

void CalNotificationsView::onEvent(const CalEvent& event)
{
	WENTER();
	WDEBUG("type = %u, source = %u", event.type, event.source);
	switch (event.type) {
		case CalEvent::DB_CHANGED:
		{
			CalStatusBarManager::getInstance().checkDeletedEvent();
			std::shared_ptr<CalAlertData> alertData = std::make_shared<CalAlertData>();
			if (alertData->getCount() == 0)
			{
				elm_exit();
			}
			__model.relaceAlertData(alertData);
			__update();
			break;
		}
		case CalEvent::APP_RESUMED:
			if(__model.getCount() == 0)
			{
				elm_exit();
			} else {
				__update();
			}
			break;
		default:
			WERROR("invalid type = %u", event.type);
			break;
	}
}

void CalNotificationsView::replaceAlertData(std::shared_ptr<CalAlertData> alertData)
{
	WENTER();

	__model.relaceAlertData(alertData);
	__update();

	WLEAVE();
}
