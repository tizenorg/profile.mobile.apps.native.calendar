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

#include "CalTouchInputRecognizer.h"
#include "CalCommon.h"

int CalTouchInputRecognizer::__nextRecognizerId = 0;
int CalTouchInputRecognizer::__currentTouchOwnerId = -1;

CalTouchInputRecognizer::CalTouchInputRecognizer() :
	__id(__nextRecognizerId++),
	__blocked(false),
	__active(false)
{
}

CalTouchInputRecognizer::~CalTouchInputRecognizer()
{
}

void CalTouchInputRecognizer::addTarget(Evas_Object* obj)
{
	evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_DOWN, __onPress, this);
}

void CalTouchInputRecognizer::removeTarget(Evas_Object* obj)
{
	evas_object_event_callback_del_full(obj, EVAS_CALLBACK_MOUSE_DOWN, __onPress,  this);
	evas_object_event_callback_del_full(obj, EVAS_CALLBACK_MOUSE_MOVE, __onMove,  this);
	evas_object_event_callback_del_full(obj, EVAS_CALLBACK_MOUSE_UP,   __onRelease, this);
}

void CalTouchInputRecognizer::terminate(Evas_Object* obj)
{
	if(obj)
	{
		__waitPress(obj);
	}
}

void CalTouchInputRecognizer::block()
{
	__blocked = true;
}

void CalTouchInputRecognizer::unblock()
{
	__blocked = false;
}

void CalTouchInputRecognizer::__onPress(void* data, Evas* evas, Evas_Object* obj, void* event_info)
{
	CalTouchInputRecognizer* self = (CalTouchInputRecognizer*)data;
	if (self->__blocked)
	{
		WDEBUG("blocked");
		return;
	}

	Evas_Event_Mouse_Down* ev = (Evas_Event_Mouse_Down*)event_info;
	if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
	{
		WDEBUG("EVAS_EVENT_FLAG_ON_HOLD");
		return;
	}

	self->__currentTouchOwnerId = -1;
	self->onPress(obj, ev);

	self->__waitMoveAndRelease(obj);
}

void CalTouchInputRecognizer::__onMove(void* data, Evas* evas, Evas_Object* obj, void* event_info)
{
	CalTouchInputRecognizer* self = (CalTouchInputRecognizer*)data;
	if (self->__blocked)
	{
		WDEBUG("blocked");
		return;
	}

	Evas_Event_Mouse_Move* ev = (Evas_Event_Mouse_Move*)event_info;
	if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
	{
		WDEBUG("EVAS_EVENT_FLAG_ON_HOLD");
		return;
	}

	if (self->thisTouchIsFreeOrMine())
	{
		self->onMove(obj, ev);
	}
}

void CalTouchInputRecognizer::__onRelease(void* data, Evas* evas, Evas_Object* obj, void* event_info)
{
	CalTouchInputRecognizer* self = (CalTouchInputRecognizer*)data;
	if (self->__blocked)
	{
		WDEBUG("blocked");
		self->__waitPress(obj);
		return;
	}

	Evas_Event_Mouse_Up* ev = (Evas_Event_Mouse_Up*)event_info;

	self->onRelease(obj, ev);
	self->__waitPress(obj);
}

void CalTouchInputRecognizer::__waitMoveAndRelease(Evas_Object* obj)
{
	evas_object_event_callback_del_full(obj, EVAS_CALLBACK_MOUSE_DOWN, __onPress,  this);
	evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_MOVE, __onMove,  this);
	evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_UP,   __onRelease, this);
	__active = true;
}

void CalTouchInputRecognizer::__waitPress(Evas_Object* obj)
{
	evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_DOWN, __onPress,  this);
	evas_object_event_callback_del_full(obj, EVAS_CALLBACK_MOUSE_MOVE, __onMove,  this);
	evas_object_event_callback_del_full(obj, EVAS_CALLBACK_MOUSE_UP,   __onRelease, this);
	__active = false;
}

bool CalTouchInputRecognizer::thisTouchIsFreeOrMine()
{
	return __currentTouchOwnerId == -1 || __currentTouchOwnerId == __id;
}

bool CalTouchInputRecognizer::claimThisTouchToBeMine()
{
	if (!thisTouchIsFreeOrMine())
	{
		return false;
	}

	__currentTouchOwnerId = __id;
	return true;
}
