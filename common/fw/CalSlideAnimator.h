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

#ifndef _CAL_SLIDE_ANIMATOR_H_
#define _CAL_SLIDE_ANIMATOR_H_

#include <functional>
#include <list>
#include <Ecore.h>
#include <Evas.h>

#include "WDefine.h"

/**
 * @brief Calendar slide animator.
 */
class WAPP_ASSIST_EXPORT CalSlideAnimator
{
public:
	/**
	 * @brief Slide animator modes.
	 */
	enum Mode
	{
		VERTICAL_MOVE = 0,
		VERTICAL_GROW_AND_SHRINK_TOP,
		VERTICAL_GROW_AND_SHRINK_BOTTOM,
		HORIZONTAL_MOVE,
		HORIZONTAL_GROW_AND_SHRINK_LEFT,
		HORIZONTAL_GROW_AND_SHRINK_RIGHT
	};

	/**
	 * @brief Create slide animator.
	 *
	 * @param[in]   obj   Evas object.
	 * @param[in]   slideFinishedCb   Slide finished callback.
	 */
	CalSlideAnimator(Evas_Object* obj, std::function<void (int dir)> slideFinishedCb);

	virtual ~CalSlideAnimator();

	/**
	 * @brief Attach side kick.
	 *
	 * @param[in]   obj   Evas object.
	 */
	void attachSidekick(Evas_Object* obj);

	/**
	 * @brief Begin slide animation.
	 *
	 * @param[in]   mode   Slide animator mode.
	 * @param[in]   min    Minimum coordinate.
	 * @param[in]   max    Maximum coordinate.
	 *
	 * @see Mode.
	 */
	void begin(Mode mode, Evas_Coord min, Evas_Coord max);

	/**
	 * @brief Update animation.
	 *
	 * @param[in]   delta   Coordinate differences.
	 *
	 * @see Mode.
	 */
	void update(Evas_Coord delta);

	/**
	 * @brief Finish slide animation.
	 *
	 * @param[in]   dir           Direction.
	 * @param[in]   noAnimation   Animation state.
	 */
	void finish(int dir, bool noAnimation = false);

	/**
	 * @brief Cancel slide animation.
	 *
	 * @param[in]   noAnimation   Animation state.
	 */
	void cancel(bool noAnimation = false);

	/**
	 * @brief Reset current position.
	 */
	void resetPosition();

	/**
	 * @brief Get slide animation state.
	 *
	 * @return Whether the slide animation is active.
	 */
	bool isActive() const { return __active; }

	/**
	 * @brief Get minimum coordinate.
	 *
	 * @return Minimum coordinate.
	 */
	Evas_Coord getMin() const {return __min;}

	/**
	 * @brief Get maximum coordinate.
	 *
	 * @return Maximum coordinate.
	 */
	Evas_Coord getMax() const {return __max;}

private:
	struct Sidekick
	{
		Evas_Object* obj;
		Evas_Coord_Point offset;
	};

	void __update(Evas_Object* obj, const Evas_Coord_Rectangle& origin, Evas_Coord delta);
	void __slide(Evas_Coord target);
	bool __tween(double pos);
	void __finalize();

	Evas_Object* __obj;
	std::list<Sidekick> __sidekicks;
	Mode __mode;
	Ecore_Animator *__mAnimator;
	Evas_Coord __min;
	Evas_Coord __max;
	Evas_Coord_Rectangle __origin;
	Evas_Coord_Rectangle __start;
	int __dir;
	int __target;
	std::function<void (int dir)> __slideFinishedCb;
	bool __active;
	Ecore_Timer * __timer;
};

#endif
