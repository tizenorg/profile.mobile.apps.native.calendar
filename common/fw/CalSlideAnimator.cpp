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

#include "CalSlideAnimator.h"
#include "CalCommon.h"
#include <Elementary.h>

#define SLIDE_ANIMATION_DURATION 0.13
#define RECT_REF(_rect_var) &_rect_var.x, &_rect_var.y, &_rect_var.w, &_rect_var.h

CalSlideAnimator::CalSlideAnimator(Evas_Object* obj, std::function<void (int dir)> slideFinishedCb)
	: __obj(obj)
	, __mode(VERTICAL_MOVE)
	, __mAnimator(NULL)
	, __min(0)
	, __max(0)
	, __origin({})
	, __start({})
	, __dir(0)
	, __target(0)
	, __slideFinishedCb(slideFinishedCb)
	, __active(false)
	, __timer(NULL)
{
}

CalSlideAnimator::~CalSlideAnimator()
{
	if(__mAnimator)
	{
		ecore_animator_del(__mAnimator);
	}

	__mAnimator = NULL;

	if(__timer)
	{
		ecore_timer_del(__timer);
	}

	__timer = NULL;
}

void CalSlideAnimator::attachSidekick(Evas_Object* obj)
{
	Sidekick sidekick = {0};
	sidekick.obj = obj;
	__sidekicks.push_back(sidekick);
}

void CalSlideAnimator::begin(Mode mode, Evas_Coord min, Evas_Coord max)
{
	__mode = mode;
	__min = min;
	__max = max;
	__dir = 0;
	evas_object_geometry_get(__obj, RECT_REF(__origin));
	for (auto it = __sidekicks.begin(); it != __sidekicks.end(); it++)
	{
		Evas_Coord_Rectangle rect;
		evas_object_geometry_get(it->obj, RECT_REF(rect));
		it->offset.x = rect.x - __origin.x;
		it->offset.y = rect.y - __origin.y;
	}
	__active = true;
}

void CalSlideAnimator::update(Evas_Coord delta)
{
	if (delta < __min)
	{
		delta = __min;
	}
	else if (delta > __max)
	{
		delta = __max;
	}

	__update(__obj, __origin, delta);
}

void CalSlideAnimator::__update(Evas_Object* obj, const Evas_Coord_Rectangle& origin, Evas_Coord delta)
{
	switch (__mode)
	{
		case VERTICAL_MOVE:
			evas_object_move(obj, origin.x, origin.y + delta);
			break;
		case VERTICAL_GROW_AND_SHRINK_TOP:
			evas_object_move(obj, origin.x, origin.y + delta);
			evas_object_resize(obj, origin.w, origin.h - delta);
			break;
		case VERTICAL_GROW_AND_SHRINK_BOTTOM:
		case HORIZONTAL_MOVE:
		case HORIZONTAL_GROW_AND_SHRINK_LEFT:
		case HORIZONTAL_GROW_AND_SHRINK_RIGHT:
		default:
			WASSERT(0);
			break;
	}

	for (auto it = __sidekicks.begin(); it != __sidekicks.end(); it++)
	{
		evas_object_move(it->obj, origin.x + it->offset.x, origin.y + delta + it->offset.y);
	}
}

void CalSlideAnimator::finish(int dir, bool noAnimation)
{
	WASSERT(dir > 0 || dir < 0);

	__dir = dir;

	if (noAnimation)
	{
		__finalize();
	}
	else if (__dir < 0)
	{
		__slide(__min + __origin.y);
	}
	else
	{
		__slide(__max + __origin.y);
	}
}

void CalSlideAnimator::cancel(bool noAnimation)
{
	WASSERT(__dir == 0);

	if (noAnimation)
	{
		__finalize();
	}
	else
	{
		__slide(__origin.y);
	}
}

void CalSlideAnimator::resetPosition()
{
	evas_object_move(__obj, __origin.x, __origin.y);
}

void CalSlideAnimator::__slide(Evas_Coord target)
{
	evas_object_geometry_get(__obj, RECT_REF(__start));
	__target = target;

	const double remainingRatio = double(__target - __start.y) / __target;
	WDEBUG("%d %d %f", __start.y, target, remainingRatio);

	if(__mAnimator)
	{
		ecore_animator_del(__mAnimator);
	}

	evas_object_freeze_events_set(__obj, EINA_TRUE);
	__mAnimator = ecore_animator_timeline_add(SLIDE_ANIMATION_DURATION,
		[](void* data, double pos) {
			CalSlideAnimator* self = (CalSlideAnimator*)data;
			if (self->__tween(pos)) {
				return ECORE_CALLBACK_RENEW;
			} else {
				self->__mAnimator = NULL;
				return ECORE_CALLBACK_CANCEL;
			}
		},
		this
	);
}

bool CalSlideAnimator::__tween(double pos)
{
	if (pos < 1.0)
	{
		double map = ecore_animator_pos_map(pos, ECORE_POS_MAP_LINEAR, 0, 0);
		__update(__obj, __start, map * (__target - __start.y));

		return true;
	}
	else
	{
		__update(__obj, __start, (__target - __start.y));

		if(__timer)
		{
			ecore_timer_del(__timer);
		}

		__timer = ecore_timer_add(0.0, [](void* data) {
			CalSlideAnimator* self = (CalSlideAnimator*)data;
			self->__finalize();
			self->__timer = NULL;
			return ECORE_CALLBACK_CANCEL;
		}, this);

		return false;
	}
}

void CalSlideAnimator::__finalize()
{
	if (__slideFinishedCb)
	{
		__slideFinishedCb(__dir);
	}
	evas_object_freeze_events_set(__obj, EINA_FALSE);
	__active = false;
}
