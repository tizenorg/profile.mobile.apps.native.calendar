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

#include "CalDragRecognizer.h"
#include "CalCommon.h"

#define THRESHOLD 5

CalDragRecognizer::CalDragRecognizer(
	std::function<void (int x, int y)> onPressed,
	std::function<void (int dx, int dy)> onDragged,
	std::function<void (int dx, int dy, bool isValidDrag)> onReleased) :
	__onPressed(onPressed),
	__onDragged(onDragged),
	__onReleased(onReleased),
	__pressedPoint({0}),
	__isValidDrag(false)
{
}

CalDragRecognizer::~CalDragRecognizer()
{
}

void CalDragRecognizer::onPress(Evas_Object* obj, Evas_Event_Mouse_Down* eventInfo)
{
	__isValidDrag = false;
	__pressedPoint = eventInfo->canvas;
	WDEBUG("%x (%d, %d)", this, __pressedPoint.x, __pressedPoint.y);
	__onPressed(__pressedPoint.x, __pressedPoint.y);
}

void CalDragRecognizer::onMove(Evas_Object* obj, Evas_Event_Mouse_Move* eventInfo)
{
	const Evas_Coord_Point& currentPoint = eventInfo->cur.canvas;
	const int dx = currentPoint.x - __pressedPoint.x;
	const int dy = currentPoint.y - __pressedPoint.y;
	WDEBUG("%x (%d, %d) (%d, %d)", this, currentPoint.x, currentPoint.y, dx, dy);

	if (!__isValidDrag && (abs(dx) > THRESHOLD || abs(dy) > THRESHOLD) && claimThisTouchToBeMine())
	{
		__isValidDrag = true;
	}

	if (__isValidDrag)
	{
		__onDragged(dx, dy);
	}
}

void CalDragRecognizer::onRelease(Evas_Object* obj, Evas_Event_Mouse_Up* eventInfo)
{
	const Evas_Coord_Point& currentPoint = eventInfo->canvas;
	const int dx = currentPoint.x - __pressedPoint.x;
	const int dy = currentPoint.y - __pressedPoint.y;
	WDEBUG("%x (%d, %d) (%d, %d)", this, currentPoint.x, currentPoint.y, dx, dy);
	__onReleased(dx, dy, __isValidDrag);
	__isValidDrag = false;
}
