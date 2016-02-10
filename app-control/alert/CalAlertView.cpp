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
#include "CalAlertView.h"
#include "CalSchedule.h"
#include "CalDateTime.h"
#include "CalView.h"
#include <sstream>
#include "CalSettingsManager.h"


#define ALERT_TIMER       60.0
#define CENTER_X_POSITION 400
#define DISTANCE_X        79
#define DISTANCE_Y        672
#define ACTIVATE_DISTANCE 198

CalAlertView::CalAlertView(std::shared_ptr<CalAlertData> alertData) : CalView("CalAlertView"),
	__model(alertData),
	__dismissDown(false),
	__snoozeDown(false),
	__actionLock(false),
	__timer(NULL),
	__dismissImageLine(nullptr),
	__snoozeImageLine(nullptr),
	__startXPostion(0),
	__startYPostion(0),
	__dismiss(nullptr),
	__snooze(nullptr)
{
	WENTER();
}

CalAlertView::~CalAlertView()
{
	WENTER();
	__model.turnOffLcd();

	if(__timer)
	{
		ecore_timer_del(__timer);
		__timer = NULL;
	}
}

Evas_Object* CalAlertView::onCreate( Evas_Object* parent, void* viewParam )
{
	WENTER();

	Evas_Object* layout = elm_layout_add(parent);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_file_set(layout, CalPath::getPath(CAL_EDJE).c_str(), "CalAlertView");

	////  add title
	auto alertItem = __model.getAt(0);
	auto schedule = alertItem->getSchedule();
	std::string summary;
	schedule->getDisplaySummary(-1, summary);
	elm_object_part_text_set(layout, "event_title", summary.c_str());

	////  add time
	CalDateTime startTime;
	schedule->getStart(startTime);
	std::string startText;
	startTime.getTimeString(startText);

	std::string buf;
	std::stringstream ss(startText);

	int iteration = 0;
	while (ss >> buf)
	{
		switch (iteration)
		{
		case 0:
			elm_object_part_text_set(layout, "event_time", buf.c_str());
			break;
		case 1:
			elm_object_part_text_set(layout, "event_am_pm", buf.c_str());
			break;
		default:
			break;
		}
		iteration++;
	}

	//// add date
	CalDate date;
	schedule->getStart(startTime);
	date.set(startTime.getYear(), startTime.getMonth(), startTime.getMday());
	elm_object_part_text_set(layout, "event_date", date.getString());

	//// add location
	std::string location;
	schedule->getDisplayLocation(-1, location);
	elm_object_part_text_set(layout, "event_location", location.c_str());

	//// add button callback
	Evas_Object* dismissImage = (Evas_Object *) edje_object_part_object_get(elm_layout_edje_get(layout), "alert_icon_bg_left");
	Evas_Object* snoozeImage = (Evas_Object *) edje_object_part_object_get(elm_layout_edje_get(layout), "alert_icon_bg_right");
	__dismissImageLine = (Evas_Object *) edje_object_part_object_get(elm_layout_edje_get(layout), "alert_icon_circle_line_left");
	__snoozeImageLine = (Evas_Object *) edje_object_part_object_get(elm_layout_edje_get(layout), "alert_icon_circle_line_right");

	__dismiss = elm_layout_add(layout);
	elm_layout_file_set(__dismiss, CalPath::getPath(CAL_EDJE).c_str(), "CalAlertDismissView");
	evas_object_size_hint_weight_set(__dismiss, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(__dismiss, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_part_content_set(layout, "alert_bg_icon_sw_left", __dismiss);
	evas_object_show(__dismiss);

	__snooze = elm_layout_add(layout);
	elm_layout_file_set(__snooze, CalPath::getPath(CAL_EDJE).c_str(), "CalAlertSnoozeView");
	evas_object_size_hint_weight_set(__snooze, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(__snooze, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_part_content_set(layout, "alert_bg_icon_sw_right", __snooze);
	evas_object_show(__snooze);

	evas_object_event_callback_add(dismissImage, EVAS_CALLBACK_MOUSE_DOWN,
		[](void *data, Evas *e, Evas_Object *obj, void *eventInfo){
			CalAlertView* self = (CalAlertView*)data;
			self->__dismissDown = true;
			Evas_Event_Mouse_Up* ev = (Evas_Event_Mouse_Up*)eventInfo;
			if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
			{
				return;
			}
			Evas_Coord_Point currentPoint = ev->canvas;
			self->__startXPostion = currentPoint.x;
			self->__startYPostion = currentPoint.y;
		}, this);
	evas_object_event_callback_add(dismissImage, EVAS_CALLBACK_MOUSE_MOVE,
		[](void *data, Evas *e, Evas_Object *obj, void *eventInfo){
			CalAlertView* self = (CalAlertView*)data;
			if(!self->__dismissDown)
			{
				return;
			}
			Evas_Event_Mouse_Up* ev = (Evas_Event_Mouse_Up*)eventInfo;
			if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
			{
				return;
			}
			Evas_Coord_Point currentPoint = ev->canvas;
			Evas_Coord_Rectangle rect;
			evas_object_geometry_get(self->__dismissImageLine, &rect.x, &rect.y, &rect.w, &rect.h);
			int distanceCentreX = abs(currentPoint.x - DISTANCE_X);
			int distanceCentreY = abs(currentPoint.y - DISTANCE_Y);
			int distanceForActivate =  sqrt(distanceCentreX * distanceCentreX + distanceCentreY * distanceCentreY);

			if(distanceForActivate > ACTIVATE_DISTANCE)
			{
				self->__actionLock = true;
			}
			else
			{
				self->__actionLock = false;
				int centerX = DISTANCE_X;
				int centerY = DISTANCE_Y;
				evas_object_resize(self->__dismiss, 2*distanceForActivate, 2*distanceForActivate);
				evas_object_move(self->__dismiss, centerX - distanceForActivate, centerY-distanceForActivate);
			}
		}, this);

	evas_object_event_callback_add(dismissImage, EVAS_CALLBACK_MOUSE_UP,
		[](void *data, Evas *e, Evas_Object *obj, void *eventInfo){
			CalAlertView* self = (CalAlertView*)data;
			if (self->__actionLock && self->__dismissDown)
			{
				self->__actionLock = false;
				self->__dismissDown = false;
				self->__model.dismiss(0);
				elm_exit();
			}
		}, this);

	evas_object_event_callback_add(snoozeImage, EVAS_CALLBACK_MOUSE_DOWN,
		[](void *data, Evas *e, Evas_Object *obj, void *eventInfo){
			CalAlertView* self = (CalAlertView*)data;
			self->__snoozeDown = true;
			Evas_Event_Mouse_Up* ev = (Evas_Event_Mouse_Up*)eventInfo;
			if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
			{
				return;
			}
			Evas_Coord_Point currentPoint = ev->canvas;
			self->__startXPostion = currentPoint.x;
			self->__startYPostion = currentPoint.y;
		}, this);
	evas_object_event_callback_add(snoozeImage, EVAS_CALLBACK_MOUSE_MOVE,
		[](void *data, Evas *e, Evas_Object *obj, void *eventInfo){
			CalAlertView* self = (CalAlertView*)data;
			if(!self->__snoozeDown)
			{
				return;
			}
			Evas_Event_Mouse_Up* ev = (Evas_Event_Mouse_Up*)eventInfo;
			if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
			{
				return;
			}
			Evas_Coord_Point currentPoint = ev->canvas;
			Evas_Coord_Rectangle rect;
			evas_object_geometry_get(self->__snoozeImageLine, &rect.x, &rect.y, &rect.w, &rect.h);
			int distanceCentreX = abs(currentPoint.x - CENTER_X_POSITION);
			int distanceCentreY = abs(currentPoint.y - DISTANCE_Y);
			int distanceForActivate =  sqrt(distanceCentreX * distanceCentreX + distanceCentreY * distanceCentreY);

			if(distanceForActivate > ACTIVATE_DISTANCE)
			{
				self->__actionLock = true;
			}
			else
			{
				self->__actionLock = false;
				int centerX = CENTER_X_POSITION;
				int centerY = DISTANCE_Y;
				evas_object_resize(self->__snooze, 2*distanceForActivate, 2*distanceForActivate);
				evas_object_move(self->__snooze, centerX - distanceForActivate, centerY-distanceForActivate);
			}
		}, this);
	evas_object_event_callback_add(snoozeImage, EVAS_CALLBACK_MOUSE_UP,
		[](void *data, Evas *e, Evas_Object *obj, void *eventInfo){
			CalAlertView* self = (CalAlertView*)data;
			if (self->__actionLock && self->__snoozeDown)
			{
				self->__actionLock = false;
				self->__snoozeDown = false;
				self->__model.snooze(0);
				elm_exit();
			}
		}, this);

	evas_object_show(layout);

	__model.turnOnLcd();

	__timer = ecore_timer_add(ALERT_TIMER, [](void* data)->Eina_Bool {
			CalAlertView* self = (CalAlertView*)data;
			self->__model.snooze(0);
			self->__snoozeDown = false;
			self->__timer = NULL;
			elm_exit();
			return ECORE_CALLBACK_CANCEL;
		},
		this
	);

	return layout;
}

void CalAlertView::onDestroy()
{
	WENTER();
}

void CalAlertView::onBecomeTop()
{
	WENTER();
}

void CalAlertView::onExpose()
{
	WENTER();
}

void CalAlertView::onPushed(Elm_Object_Item* naviItem)
{
	WENTER();
	elm_naviframe_item_title_enabled_set(naviItem, EINA_FALSE, EINA_TRUE);
	activateMenuButton(naviItem);
}

void CalAlertView::onEvent(const CalEvent& event)
{
	WENTER();
}

void CalAlertView::replaceAlertData(std::shared_ptr<CalAlertData> alertData)
{
	WENTER();
	__model.relaceAlertData(alertData);

	WLEAVE();
}

void CalAlertView::onMenuButton()
{
	WENTER();
}

