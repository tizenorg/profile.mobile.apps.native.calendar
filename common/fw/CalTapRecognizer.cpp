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

#include "CalTapRecognizer.h"
#include "CalCommon.h"

CalTapRecognizer::CalTapRecognizer(std::function<void (Evas_Object* obj)> onTap) :
	__onTap(onTap),
	__pressedPoint({0})
{
}

CalTapRecognizer::~CalTapRecognizer()
{
}

void CalTapRecognizer::onPress(Evas_Object* obj, Evas_Event_Mouse_Down* eventInfo)
{
	__pressedPoint = eventInfo->canvas;
	WDEBUG("(%d, %d)", __pressedPoint.x, __pressedPoint.y);
}

void CalTapRecognizer::onMove(Evas_Object* obj, Evas_Event_Mouse_Move* eventInfo)
{
}

void CalTapRecognizer::onRelease(Evas_Object* obj, Evas_Event_Mouse_Up* eventInfo)
{
	if ((eventInfo->event_flags & EVAS_EVENT_FLAG_ON_HOLD) == EVAS_EVENT_FLAG_ON_HOLD)
	{
		/**
		 * @note
		 * Do not process the TAP event in this case.
		 */
		WDEBUG("On Hold triggered");
	}
	else
	{
		const Evas_Coord_Point& currentPoint = eventInfo->canvas;
		const int dx = currentPoint.x - __pressedPoint.x;
		const int dy = currentPoint.y - __pressedPoint.y;

		WDEBUG("(%d, %d) (%d, %d)", currentPoint.x, currentPoint.y, dx, dy);
		if (claimThisTouchToBeMine())
		{
			__onTap(obj);
		}
	}
}
