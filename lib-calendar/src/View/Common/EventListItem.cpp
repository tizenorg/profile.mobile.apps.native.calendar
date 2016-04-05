/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "View/Common/EventListItem.h"
#include "common/edje/inc/UiCommon.h"
#include "Common.h"

#include "Utils/Logger.h"

#include <string.h>
#include <app_i18n.h>

using namespace Calendar::View::Common;
using namespace Ui;

EventListItem::EventListItem()
{
}

Elm_Genlist_Item_Class *EventListItem::getItemClass() const
{
	TRACE;
	static Elm_Genlist_Item_Class itc = createItemClass(EVENT_LIST_ITEM);
	return &itc;
}

char *EventListItem::getText(Evas_Object *parent, const char *part)
{
	TRACE;
	DBG("Part: %s", part);
	//TODO: Return text from EventInstancePart

	if (PART_IS(EVENT_ITEM_PART_NAME_TITLE)){
		return strdup(_("IDS_CLD_MBODY_MY_EVENT"));
	} else if (PART_IS(EVENT_ITEM_PART_NAME_DAY)) {
		return strdup("17");
	} else if (PART_IS(EVENT_ITEM_PART_NAME_WEEKDAY)) {
		return strdup("Mon");
	} else if (PART_IS(EVENT_ITEM_PART_NAME_TIME)) {
		return strdup("10:00 - 11:00");
	} else if (PART_IS(EVENT_ITEM_PART_NAME_LOCATION)) {
		return strdup("Kyiv city");
	} else {
		return NULL;
	}
}

void EventListItem::onRealized()
{
	//TODO: If event does not have location, emit signal to collapse location textblock
}

void EventListItem::onUnrealized()
{
	//TODO: Unsubscribe from event data provider update/remove callbacks
}
