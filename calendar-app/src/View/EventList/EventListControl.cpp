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

#include "View/EventList/EventListControl.h"
#include "event-list-item/edje/inc/EventListItemLayout.h"

#include "App/Path.h"
#include "Utils/Logger.h"
#include "Ui/Genlist.h"

using namespace View::EventList;

EventListControl::EventListControl()
{
}

void EventListControl::onCreated()
{
	elm_theme_extension_add(NULL, App::getResourcePath(EVENT_LIST_ITEM_LAYOUT_PATH).c_str());
}
