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

#ifndef _CAL_DRAG_RECOGNIZER_H_
#define _CAL_DRAG_RECOGNIZER_H_

#include "CalTouchInputRecognizer.h"

#include <functional>

/**
 * @brief Calendar drag recognizer.
 *
 * @see CalTouchInputRecognizer.
 */
class WAPP_ASSIST_EXPORT CalDragRecognizer : public CalTouchInputRecognizer
{
public:
	/**
	 * @brief Create drag recognizer.
	 *
	 * @param[in]   onPressed    Press callback.
	 * @param[in]   onDragged    Drag callback.
	 * @param[in]   onReleased   Release callback.
	 */
	CalDragRecognizer(
		std::function<void (int x, int y)> onPressed,
		std::function<void (int dx, int dy)> onDragged,
		std::function<void (int dx, int dy, bool isValidDrag)> onReleased);
	virtual ~CalDragRecognizer();

private:
	virtual void onPress(Evas_Object* obj, Evas_Event_Mouse_Down* eventInfo);
	virtual void onMove(Evas_Object* obj, Evas_Event_Mouse_Move* eventInfo);
	virtual void onRelease(Evas_Object* obj, Evas_Event_Mouse_Up* eventInfo);

	const std::function<void (int x, int y)> __onPressed;
	const std::function<void (int dx, int dy)> __onDragged;
	const std::function<void (int dx, int dy, bool isValidDrag)> __onReleased;
	Evas_Coord_Point __pressedPoint;
	bool __isValidDrag;
};

#endif
