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

class WAPP_ASSIST_EXPORT CalSlideAnimator
{
public:
	CalSlideAnimator(Evas_Object* obj, std::function<void (int dir)> slideFinishedCb);
	virtual ~CalSlideAnimator();
public:
	enum Mode
	{
		VERTICAL_MOVE = 0,
		VERTICAL_GROW_AND_SHRINK_TOP,
		VERTICAL_GROW_AND_SHRINK_BOTTOM,
		HORIZONTAL_MOVE,
		HORIZONTAL_GROW_AND_SHRINK_LEFT,
		HORIZONTAL_GROW_AND_SHRINK_RIGHT
	};
public:
	void attachSidekick(Evas_Object* obj);
public:
	void begin(Mode mode, Evas_Coord min, Evas_Coord max);
	void update(Evas_Coord delta);
	void finish(int dir, bool noAnimation = false);
	void cancel(bool noAnimation = false);
public:
	void resetPosition();
public:
	bool isActive() const { return __active; }
public:
	Evas_Coord getMin() const {return __min;}
	Evas_Coord getMax() const {return __max;}
private:
	void __update(Evas_Object* obj, const Evas_Coord_Rectangle& origin, Evas_Coord delta);
private:
	void __slide(Evas_Coord target);
	bool __tween(double pos);
	void __finalize();
private:
	struct Sidekick
	{
		Evas_Object* obj;
		Evas_Coord_Point offset;
	};
private:
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
