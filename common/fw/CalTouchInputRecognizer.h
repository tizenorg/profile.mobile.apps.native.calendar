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

#ifndef _CAL_TOUCH_INPUT_RECOGNIZER_H_
#define _CAL_TOUCH_INPUT_RECOGNIZER_H_

#include <Evas.h>
#include "WDefine.h"

/**
 * @brief Calendar touch input recognizer.
 */
class WAPP_ASSIST_EXPORT CalTouchInputRecognizer
{
public:
	CalTouchInputRecognizer();

	virtual ~CalTouchInputRecognizer();

	/**
	 * @brief Add target object.
	 *
	 * @param[in]   obj   Target object.
	 */
	void addTarget(Evas_Object* obj);

	/**
	 * @brief Remove target object.
	 *
	 * @param[in]   obj   Target object.
	 */
	void removeTarget(Evas_Object* obj);

	/**
	 * @brief Terminate target object.
	 *
	 * @param[in]   obj   Target object.
	 */
	void terminate(Evas_Object* obj);

	/**
	 * @brief Block recognizing
	 */
	void block();

	/**
	 * @brief Unblock recognizing
	 */
	void unblock();

	/**
	 * @brief Get touch input state.
	 *
	 * @return Whether the touch input is active.
	 */
	bool isActive() { return __active; }

protected:
	bool thisTouchIsFreeOrMine();
	bool claimThisTouchToBeMine();

	virtual void onPress(Evas_Object* obj, Evas_Event_Mouse_Down* eventInfo) = 0;
	virtual void onMove(Evas_Object* obj, Evas_Event_Mouse_Move* eventInfo) = 0;
	virtual void onRelease(Evas_Object* obj, Evas_Event_Mouse_Up* eventInfo) = 0;

private:
	static void __onPress(void* data, Evas* evas, Evas_Object* obj, void* event_info);
	static void __onMove(void* data, Evas* evas, Evas_Object* obj, void* event_info);
	static void __onRelease(void* data, Evas* evas, Evas_Object* obj, void* event_info);

	void __waitPress(Evas_Object* obj);
	void __waitMoveAndRelease(Evas_Object* obj);

	static int __nextRecognizerId;
	static int __currentTouchOwnerId;
	int __id;
	bool __blocked;
	bool __active;
};

#endif
