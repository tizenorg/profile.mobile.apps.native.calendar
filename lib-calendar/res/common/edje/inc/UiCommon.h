/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef CALENDAR_UI_COMMON_H
#define CALENDAR_UI_COMMON_H

#define UI_BASE_SCALE 2.6

#define PADDING_VIEW_DEFAULT_SIZE 0
#define PADDING_EVENT_ITEM_DEFAULT_SIZE 32

#define CALENDAR_CONTROL_GRID_HEIGHT 70
#define CALENDAR_CONTROL_WEAKDAY_HEADER_HEIGHT 624

#define MONTH_HEIGHT CALENDAR_CONTROL_GRID_HEIGHT+CALENDAR_CONTROL_WEAKDAY_HEADER_HEIGHT

#define AGENDA_PART_NAME_MONTH_CONTROL	"elm.swallow.month"
#define AGENDA_PART_NAME_LIST_CONTROL	"elm.swallow.list"

#define EVENT_ITEM_PART_NAME_DAY		"elm.text.day"
#define EVENT_ITEM_PART_NAME_WEEKDAY	"elm.text.weekday"
#define EVENT_ITEM_PART_NAME_TITLE		"elm.text.title"
#define EVENT_ITEM_PART_NAME_TIME		"elm.text.time"
#define EVENT_ITEM_PART_NAME_LOCATION	"elm.text.location"

#define EVENT_ITEM_PART_NAME_COLOR_BAR	"elm.swallow.color.bar"
#define EVENT_ITEM_PART_NAME_CHECKBOX	"elm.swallow.checkbox"

#define AGENDA_VIEW		"agenda_view"
#define EVENT_LIST_ITEM	"event_list_item"

#define ORANGE_BAR 224 167 79 255
#define PURPLE_BAR 141 17 27 255

#endif /* CALENDAR_UI_COMMON_H */
