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

#ifndef _CAL_TAP_RECOGNIZER_H_
#define _CAL_TAP_RECOGNIZER_H_

#include <functional>
#include "CalTouchInputRecognizer.h"

class WAPP_ASSIST_EXPORT CalTapRecognizer: public CalTouchInputRecognizer
{
public:
	CalTapRecognizer(std::function<void (Evas_Object* obj)> onTap);
	virtual ~CalTapRecognizer();
private:
	virtual void onPress(Evas_Object* obj, Evas_Event_Mouse_Down* eventInfo);
	virtual void onMove(Evas_Object* obj, Evas_Event_Mouse_Move* eventInfo);
	virtual void onRelease(Evas_Object* obj, Evas_Event_Mouse_Up* eventInfo);
private:
	const std::function<void (Evas_Object* obj)> __onTap;
	Evas_Coord_Point __pressedPoint;
};

#endif
