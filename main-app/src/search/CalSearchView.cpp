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
#include <notification.h>

#include "CalCommon.h"
#include "CalDataManager.h"
#include "CalInstanceSchedule.h"
#include "CalDialogEditTextFieldItem.h"
#include "CalSearchView.h"
#include "CalDetailView.h"
#include "WMenuPopup.h"

CalSearchView::CalSearchView() : CalFilterView("CalSearchView")
{
}

CalSearchView ::~CalSearchView()
{
}


/**
 * Executes the pushed action.
 *
 * @param [in]	naviItem	If non-null, the navi item.
 *
 * @return	A CalSearchView.
 */
void CalSearchView ::onPushed(Elm_Object_Item* naviItem)
{
	WENTER();
	elm_naviframe_item_title_enabled_set(getNaviItem(), false, EINA_TRUE);
	evas_object_smart_callback_add(getNaviframe()->getEvasObj(), "transition,finished",
		CalSearchView ::__transitionFinishedCb, this);

	createList([this](std::shared_ptr<CalSchedule> schedule) {
			getNaviframe()->push(new CalDetailView(schedule));
		}, false);
}

/**
 * Transition finished cb.
 *
 * @param [in]	data		If non-null, the data.
 * @param [in]	obj			If non-null, the object.
 * @param [in]	eventInfo	If non-null, information describing the event.
 */
void CalSearchView::__transitionFinishedCb(void* data, Evas_Object* obj, void* eventInfo)
{
	WENTER();
	CalSearchView* self = (CalSearchView* )data;
	evas_object_smart_callback_del_full(self->getNaviframe()->getEvasObj(), "transition,finished",
			CalSearchView::__transitionFinishedCb, self);

}

/**
 * Executes the event action.
 *
 * @param	event	The event.
 */
void CalSearchView::onEvent(const CalEvent& event)
{
	WENTER();
	WDEBUG("type = %u, source = %u", event.type, event.source);
	switch (event.type)
	{
		case CalEvent::DB_CHANGED:
		case CalEvent::TIME_CHANGED:
		case CalEvent::LANGUAGE_CHANGED:
		case CalEvent::SETTING_CHANGED:
		case CalEvent::APP_PAUSED:
		case CalEvent::APP_RESUMED:
			updateList();
			break;
		default:
			WERROR("invalid type = %u", event.type);
		break;
	}
}
