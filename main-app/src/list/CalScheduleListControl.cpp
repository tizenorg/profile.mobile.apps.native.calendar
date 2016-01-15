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

#include <memory>
#include "WUiTimer.h"
#include "CalScheduleListControl.h"
#include "CalListGroupTitleItem.h"
#include "CalListMonthItem.h"
#include "CalCommon.h"
#include "CalDataManager.h"
#include "CalListModel2.h"
#include "CalListModelFactory.h"

#define ITEM_SHOWING_THRESHOLD_HEIGHT 50
#define LIST_ITEM_MIN_COUNT_TO_FILL_PAGE 1
#define PRELOAD_ITEM_COUNT 50

CalScheduleListControl::CalScheduleListControl(ICalListModel* fowardModel, ICalListModel* backwardModel,
	std::function<void (std::shared_ptr<CalSchedule> schedule)> onTapScheduleItem,
	std::function<void (int dir)> onScrollStart,
	std::function<void (int dir, CalListControl::Item* topShowingGroupItem)> onScrollStop,
	bool isCreateCheck, bool isHideCheck, std::string searchText, int looperCount,
	bool enableNoEvent) :
	__appended(false),
	__isCreateCheck(isCreateCheck),
	__isHideCheck(isHideCheck),
	__selectedCount(0),
	__scheduleItemCount(0),
	__maxLimitSelectCount(-1),
	__scrollAnimStarted(false),
	__scrollIsUserInitiatedByDragging(false),
	__dragLock(false),
	__initialPageDrawComplete(false),
	__searchText(searchText),
	__isMouseDown(false),
	__forwardModel(fowardModel),
	__backwardModel(backwardModel),
	__checkFirstDrawTimer(NULL),
	__forwardPreloader(NULL),
	__backwardPreloader(NULL),
	__backwardModelInitialized(false),
	__peekBackwardSchedule(nullptr),
	__peekForwardSchedule(nullptr),
	__baseMonthIsNotEmpty(false),
	__latestRealizedItem(NULL),
	__onTapScheduleItem(onTapScheduleItem),
	__onScrollStart(onScrollStart),
	__onScrollStop(onScrollStop),
	__onMaxLimitSelectCb(NULL),
	__isSelectedCb(NULL)
{
	WASSERT(__forwardModel);
	WASSERT(__backwardModel);
	__base = __forwardModel->getCurrentDate();
}

CalScheduleListControl::~CalScheduleListControl()
{
	if (__checkFirstDrawTimer)
	{
		ecore_timer_del(__checkFirstDrawTimer);
	}

	if (__forwardPreloader)
	{
		ecore_animator_del(__forwardPreloader);
	}

	if (__backwardPreloader)
	{
		ecore_animator_del(__backwardPreloader);
	}
}

const char* CalScheduleListControl::getClassName()
{
	return "CalScheduleListControl";
}

/**
 * Scroll to.
 *
 * @param	date	The date.
 *
 * @return	true if it succeeds, false if it fails.
 */
bool CalScheduleListControl::scrollTo(const CalDate& date)
{
	if (__scrollAnimStarted)
	{
		WDEBUG("__scrollAnimStarted");
		return true;
	}

	CalListGroupTitleItem* topShowingGroupItem = (CalListGroupTitleItem*)__getTopShowingGroupItem();
	if (topShowingGroupItem == NULL)
	{
		WDEBUG("[mode] topShowingGroupItem == NULL");
		return false;
	}

	auto target = topShowingGroupItem->search(date);
	WDEBUG("%s %s %d %d %d %d", target->getDate().dump().c_str(), date.dump().c_str(), target->getNext(), target->getPrevious(), __forwardModel->eof(), __backwardModel->eof());
	if ((target->getDate() < date && target->getNext() == NULL && !__forwardModel->eof()) ||
		(target->getDate() > date && target->getPrevious() == NULL && !__backwardModel->eof()))
	{
		WDEBUG("[mode] target search fault");
		return false;
	}

	__enableScroll();

	WDEBUG("[mode] bring in");
	elm_genlist_item_bring_in(target->getElmObjectItem(), ELM_GENLIST_ITEM_SCROLLTO_TOP);
	__finalizeScrollIfAnimationDoesntStartAtAll();
	return true;
}

/**
 * Executes the created action.
 */
void CalScheduleListControl::onCreated()
{
	CalListControl::onCreated();

	elm_scroller_policy_set(getEvasObj(), ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);

	evas_object_smart_callback_add(getEvasObj(), "scroll,drag,stop", __onScrollDragStop, this);

	evas_object_smart_callback_add(getEvasObj(), "scroll,drag,start", [](void* data, Evas_Object* obj, void* event_info) {
		WDEBUG("scroll,drag,start");
		CalScheduleListControl* self = (CalScheduleListControl*)data;
		self->__scrollIsUserInitiatedByDragging = true;
	}, this);

	evas_object_smart_callback_add(getEvasObj(), "scroll,anim,start", [](void* data, Evas_Object* obj, void* event_info) {
		WDEBUG("scroll,anim,start");
		CalScheduleListControl* self = (CalScheduleListControl*)data;
		self->__initializeScroll();
	}, this);

	evas_object_smart_callback_add(getEvasObj(), "scroll,anim,stop", __onScrollAnimStop, this);

	evas_object_smart_callback_add(getEvasObj(), "realized", [](void *data, Evas_Object *obj, void *eventInfo) {
		CalScheduleListControl* self = (CalScheduleListControl*)data;
		Elm_Object_Item* it = (Elm_Object_Item *)eventInfo;
		if(self->__isCreateCheck && self->__isHideCheck){
			elm_object_item_signal_emit(it, "elm,state,hide,check", "elm");
		}
		self->__latestRealizedItem = (Item*)elm_object_item_data_get(it);
		if (self->__initialPageDrawComplete && !self->__scrollAnimStarted && !self->__scrollIsUserInitiatedByDragging)
			self->__loadMore();
		WDEBUG("Item(%d) realized", self->__latestRealizedItem->getIndex());
	}, this);

	evas_object_event_callback_add(getEvasObj(), EVAS_CALLBACK_MOUSE_DOWN,
		[](void* data, Evas* e, Evas_Object* obj, void* event_info) {
			CalScheduleListControl* self = (CalScheduleListControl*)data;
			self->__isMouseDown = true;
		}, this);

	evas_object_event_callback_add(getEvasObj(), EVAS_CALLBACK_MOUSE_UP,
		[](void* data, Evas* e, Evas_Object* obj, void* event_info) {
			CalScheduleListControl* self = (CalScheduleListControl*)data;
			self->__isMouseDown = false;
		}, this);

	addInitialBatchOfItems();

	__checkFirstDrawTimer = ecore_timer_add(0.5,
		[](void* data)->Eina_Bool {
			CalScheduleListControl* self = (CalScheduleListControl*)data;
			self->__checkFirstDrawTimer = NULL;
			self->__initialPageDrawComplete = true;
			if (self->__latestRealizedItem && !self->__scrollAnimStarted)
				self->__loadMore();
			return ECORE_CALLBACK_CANCEL;
		},
		this
	);
}

/**
 * Executes the scroll animation stop action.
 *
 * @param [in]	data		If non-null, the data.
 * @param [in]	obj			If non-null, the object.
 * @param [in]	event_info	If non-null, information describing the event.
 */
void CalScheduleListControl::__onScrollAnimStop(void* data, Evas_Object* obj, void* event_info)
{
	WDEBUG("scroll,anim,stop");
	CalScheduleListControl* self = (CalScheduleListControl*)data;
	if(self)
	{
		self->__finalizeScroll();
	}
}

/**
 * Executes the scroll drag stop action.
 *
 * @param [in]	data		If non-null, the data.
 * @param [in]	obj			If non-null, the object.
 * @param [in]	event_info	If non-null, information describing the event.
 */
void CalScheduleListControl::__onScrollDragStop(void* data, Evas_Object* obj, void* event_info)
{
	WDEBUG("scroll,drag,stop");
	CalScheduleListControl* self = (CalScheduleListControl*)data;
	if (!self->__scrollIsUserInitiatedByDragging)
	{
		return;
	}

	self->__finalizeScrollIfAnimationDoesntStartAtAll();
}

/**
 * Adds initial batch of items.
 */
void CalScheduleListControl::addInitialBatchOfItems()
{
	__forwardModel->prefetch(false);
	bool dayChanged = false;
	__peekForwardSchedule = __forwardModel->getNext(dayChanged);

	if (__peekForwardSchedule)
	{
		if (__peekForwardSchedule->getType() == CalSchedule::INVALID)
		{
			__initBackwardModel(true);
		}
		else
		{
			if (CalDate::compareMonth(__forwardModel->getCurrentDate(), __base) == 0)
			{
				__baseMonthIsNotEmpty = true;
			}
		}
	}

	int appendedCount = 0;

	for (int i = 0; i < LIST_ITEM_MIN_COUNT_TO_FILL_PAGE; i++)
	{
		if (__appendNext())
		{
			appendedCount++;
		}
	}

	if (appendedCount < LIST_ITEM_MIN_COUNT_TO_FILL_PAGE)
	{
		__initBackwardModel(__appended);
		for (int i = 0; i < LIST_ITEM_MIN_COUNT_TO_FILL_PAGE; i++)
		{
			if (!__prependNext())
			{
				break;
			}
		}
	}
}

/**
 * Initialises the initialise backward model.
 *
 * @param	enableNoEvent	true to enable, false to disable the no event.
 */
void CalScheduleListControl::__initBackwardModel(bool enableNoEvent)
{
	if (__backwardModelInitialized)
	{
		return;
	}

	__backwardModelInitialized = true;
	__backwardModel->prefetch(false);
	bool dayChanged = false;
	__peekBackwardSchedule = __backwardModel->getNext(dayChanged);

	if (__peekBackwardSchedule)
	{
		if (__peekBackwardSchedule->getType() != CalSchedule::INVALID)
		{
			if (CalDate::compareMonth(__backwardModel->getCurrentDate(), __base) == 0)
			{
				__baseMonthIsNotEmpty = true;
			}
		}
	}
}

/**
 * Finalize scroll if animation doesnt start at all.
 */
void CalScheduleListControl::__finalizeScrollIfAnimationDoesntStartAtAll()
{
	WUiTimer::addTimer(0.1, [](void* data) {
		CalScheduleListControl* self = (CalScheduleListControl*)data;
		if (!self->__scrollAnimStarted) {
			WDEBUG("anim not started");
			self->__finalizeScroll();
		}
		return ECORE_CALLBACK_CANCEL;
	}, this, getWeakPtr());
}

/**
 * Enables the scroll.
 */
void CalScheduleListControl::__enableScroll()
{
	if (__dragLock)
	{
		WDEBUG("[mode] unfreeze scroll and freeze touch");
		elm_object_scroll_freeze_pop(getEvasObj());
	}
}

/**
 * Initializes the initialize scroll.
 */
void CalScheduleListControl::__initializeScroll()
{
	__scrollAnimStarted = true;
	if (__scrollIsUserInitiatedByDragging && __onScrollStart)
	{
		__onScrollStart(__scrollDir);
	}
}

/**
 * Finalize scroll.
 */
void CalScheduleListControl::__finalizeScroll()
{
	__scrollAnimStarted = false;
	if (__scrollIsUserInitiatedByDragging && __onScrollStop)
	{
		__onScrollStop(__scrollDir, __getTopShowingGroupItem());
	}

	__scrollIsUserInitiatedByDragging = false;

	if (__dragLock)
	{
		WDEBUG("[mode] freeze scroll and unfreeze touch");
		if (elm_object_scroll_freeze_get(getEvasObj()) == 0)
		{
			elm_object_scroll_freeze_push(getEvasObj());
		}
	}

	if (__initialPageDrawComplete)
	{
		__loadMore();
	}
}

/**
 * Loads the more.
 */
void CalScheduleListControl::__loadMore()
{
	WASSERT(__initialPageDrawComplete);
	WASSERT(__latestRealizedItem);
	WASSERT(!__scrollAnimStarted);

	WDEBUG("%d %d %d", __latestRealizedItem->getIndex(), getLastAppendedIndex(), getLastPrependedIndex());

	if (__isNearTail() && __forwardPreloader == NULL)
	{
		__forwardPreloader = ecore_animator_add(
			[](void* data)->Eina_Bool {
				CalScheduleListControl* self = (CalScheduleListControl*)data;
				if (!self->__isMouseDown && !self->__scrollAnimStarted && self->__isNearTail())
				{
					WDEBUG("[__loadMore] %d %d %d", self->__latestRealizedItem->getIndex(), self->getLastAppendedIndex(), self->getLastPrependedIndex());
					self->__forwardModel->prefetch(true);
					if (self->__appendNext())
					{
						return ECORE_CALLBACK_RENEW;
					}
				}
				self->__forwardPreloader = NULL;
				return ECORE_CALLBACK_CANCEL;
			},
			this
		);
	}

	if (__isNearHead() && __backwardPreloader == NULL)
	{
		__backwardPreloader = ecore_animator_add(
			[](void* data)->Eina_Bool {
				CalScheduleListControl* self = (CalScheduleListControl*)data;
				self->__initBackwardModel(self->__appended);
				if (!self->__isMouseDown && !self->__scrollAnimStarted && self->__isNearHead())
				{
					WDEBUG("[__loadMore] %d %d %d", self->__latestRealizedItem->getIndex(), self->getLastAppendedIndex(), self->getLastPrependedIndex());
					self->__backwardModel->prefetch(true);
					if (self->__prependNext())
						return ECORE_CALLBACK_RENEW;
				}
				self->__backwardPreloader = NULL;
				return ECORE_CALLBACK_CANCEL;
			},
			this
		);
	}
}

/**
 * Determines if we can is near tail.
 *
 * @return	true if near tail, false if not.
 */
bool CalScheduleListControl::__isNearTail()
{
	const int totalForwardPreloadedItemsCount = getLastAppendedIndex() - LIST_ITEM_MIN_COUNT_TO_FILL_PAGE + 1;
	const int distanceUntilTail = getLastAppendedIndex() - __latestRealizedItem->getIndex();
	return totalForwardPreloadedItemsCount < PRELOAD_ITEM_COUNT || distanceUntilTail < PRELOAD_ITEM_COUNT;
}

/**
 * Determines if we can is near head.
 *
 * @return	true if near head, false if not.
 */
bool CalScheduleListControl::__isNearHead()
{
	const int totalBackwardPreloadedItemsCount = -getLastPrependedIndex();
	const int distanceUntilHead = __latestRealizedItem->getIndex() - getLastPrependedIndex();
	return totalBackwardPreloadedItemsCount < PRELOAD_ITEM_COUNT || distanceUntilHead < PRELOAD_ITEM_COUNT;
}

/**
 * Executes the destroy action.
 */
void CalScheduleListControl::onDestroy()
{
	evas_object_smart_callback_del(getEvasObj(), "scroll,anim,stop", __onScrollAnimStop);
	evas_object_smart_callback_del(getEvasObj(), "scroll,drag,stop", __onScrollDragStop);

	CalListControl::onDestroy();
}

/**
 * Query if this object is empty.
 *
 * @return	true if empty, false if not.
 */
bool CalScheduleListControl::isEmpty()
{
	if(__getFirstItem() != NULL)
	{
		return false;
	}

	return true;
}

/**
 * Sets maximum limit select cb.
 *
 * @param	onMaxLimitSelectCb	The on maximum limit select cb.
 */
void CalScheduleListControl::setMaxLimitSelectCb(std::function<void (void)> onMaxLimitSelectCb)
{
	__onMaxLimitSelectCb = onMaxLimitSelectCb;
}

/**
 * Sets maximum limit count.
 *
 * @param	maxCount	Number of maximums.
 */
void CalScheduleListControl::setMaxLimitCount(int maxCount)
{
	__maxLimitSelectCount = maxCount;
}

/**
 * Gets select schdule item count.
 *
 * @return	The select schdule item count.
 */
int CalScheduleListControl::getSelectSchduleItemCount()
{
	return __selectedCount;
}

/**
 * Gets schdule item count.
 *
 * @return	The schdule item count.
 */
int CalScheduleListControl::getSchduleItemCount()
{
	return __scheduleItemCount;
}

/**
 * Sets selected count.
 *
 * @param	count	Number of.
 */
void CalScheduleListControl::setSelectedCount(int count)
{
	__selectedCount = count;
}

/**
 * Sets list models.
 *
 * @param [in]	forwardModel 	If non-null, the forward model.
 * @param [in]	backwardModel	If non-null, the backward model.
 */
void CalScheduleListControl::setListModels(ICalListModel* forwardModel, ICalListModel* backwardModel)
{
	clear();
	__forwardModel = forwardModel;
	__backwardModel = backwardModel;
	__backwardModelInitialized = false;
	addInitialBatchOfItems();
}

/**
 * Gets selected schedules.
 *
 * @param [in]	list	The list.
 */
void CalScheduleListControl::getSelectedSchedules(std::list<std::shared_ptr<CalSchedule>>& list)
{
	Elm_Object_Item* it = __getFirstItem();
	while (it)
	{
		Item* item = (Item*)elm_object_item_data_get(it);
		if (item->isGroupTitle())
		{
			CalListGroupTitleItem* groupItem = (CalListGroupTitleItem*)item;
			if(groupItem && groupItem->getSelectedItemsCount())
			{
				std::list<std::shared_ptr<CalSchedule>> oneDayList;
				groupItem->getSelectedSchedules(oneDayList);
				for(auto schedule: oneDayList)
				{
					list.push_back(schedule);
				}
			}
		}

		it = elm_genlist_item_next_get(it);
	}
}

/**
 * Select all schdule item.
 *
 * @param	sellectedAll	The sellected all.
 */
void CalScheduleListControl::selectAllSchduleItem(Eina_Bool sellectedAll)
{
	__selectedCount = 0;

	Elm_Object_Item* it = __getFirstItem();
	while (it)
	{
		Item* item = (Item*)elm_object_item_data_get(it);
		if (item->isGroupTitle())
		{
			CalListGroupTitleItem* groupItem = (CalListGroupTitleItem*)item;
			if(groupItem)
			{
				groupItem ->selectAllItems(true);

				__selectedCount += groupItem->getSelectedItemsCount();
			}
		}

		it = elm_genlist_item_next_get(it);
	}
}

/**
 * Shows the check box.
 *
 * @param	showCheck	The show check.
 */
void CalScheduleListControl::showCheckBox(Eina_Bool showCheck)
{
	if(!__isCreateCheck)
	{
		return;
	}

	__isHideCheck = !showCheck;
	Elm_Object_Item* it = __getFirstItem();
	Elm_Object_Item* next = it;
	while(next)
	{
		Item* item = (Item*)elm_object_item_data_get(next);
		if (!item->isGroupTitle())
		{
			if(__isHideCheck)
			{
				elm_layout_signal_emit(next, "checkbox/hide", "elm");
			}
			else
			{
				elm_layout_signal_emit(next, "checkbox/show", "elm");
			}
		}
		next = elm_genlist_item_next_get(next);
	}
}

/**
 * Locks the drag.
 *
 * @param	lock	true to lock, false to unlock.
 */
void CalScheduleListControl::lockDrag(bool lock)
{
	WDEBUG("%d", lock);

	if (__dragLock == lock)
	{
		return;
	}

	if (lock)
	{
		elm_object_scroll_freeze_push(getEvasObj());
	}
	else
	{
		elm_object_scroll_freeze_pop(getEvasObj());
	}

	__dragLock = lock;
}

/**
 * Adds a group item to 'dir'.
 *
 * @param	date	The date.
 * @param	dir 	The dir.
 *
 * @return	null if it fails, else a list of.
 */
CalScheduleListControl::Item* CalScheduleListControl::__addGroupItem(const CalDate& date, int dir)
{
	Item* item = (CalListGroupTitleItem*)__findItem(date);
	if(!item)
	{
		item = new CalListGroupTitleItem(this, date);
		__addItem(item, dir, __onSelectGroupItem);
		if (dir > 0)
		{
			__groupItemChain.append(item);
		}
		else
		{
			__groupItemChain.prepend(item);
		}
	}

	return item;
}

/**
 * Appends the next.
 *
 * @return	true if it succeeds, false if it fails.
 */
bool CalScheduleListControl::__appendNext()
{
	WENTER();
	if (!__forwardModel)
	{
		return false;
	}

	bool dayChanged = false;
	auto schedule = __peekForwardSchedule ? __peekForwardSchedule : __forwardModel->getNext(dayChanged);
	__peekForwardSchedule = nullptr;

	if (!schedule)
	{
		WDEBUG("!schedule");
		return false;
	}

	if (schedule->getType() != CalSchedule::INVALID)
	{
		bool isSelected = false;
		if(__isCreateCheck && __isSelectedCb)
		{
			isSelected = __isSelectedCb(schedule);
		}

		__insertItem(schedule, __forwardModel->getCurrentDate(), isSelected, 1);
	}

	__appended = true;
	__scheduleItemCount++;

	WLEAVE();
	return true;
}

/**
 * Prepends the next.
 *
 * @return	true if it succeeds, false if it fails.
 */
bool CalScheduleListControl::__prependNext()
{
	WENTER();
	if (!__backwardModel)
	{
		return false;
	}

	bool dayChanged = false;
	auto schedule = __peekBackwardSchedule ? __peekBackwardSchedule : __backwardModel->getNext(dayChanged);
	__peekBackwardSchedule = nullptr;

	if (!schedule)
	{
		WDEBUG("!schedule");
		return false;
	}

	if (schedule->getType() != CalSchedule::INVALID)
	{
		bool isSelected = false;
		if(__isCreateCheck && __isSelectedCb)
		{
			isSelected = __isSelectedCb(schedule);
		}

		__insertItem(schedule, __backwardModel->getCurrentDate(), isSelected, -1);
	}

	__scheduleItemCount++;

	WLEAVE();
	return true;
}

/**
 * Inserts an item.
 *
 * @param	schedule	The schedule.
 * @param	date		The date.
 * @param	isSelected	True if this object is selected.
 * @param	dir			The dir.
 */
void CalScheduleListControl::__insertItem(const std::shared_ptr<CalSchedule>& schedule, CalDate date, bool isSelected, int dir)
{
	if(dir < 0) // prepend
	{
		CalDateTime startDateTime, endDateTime;
		schedule->getStart(startDateTime);
		schedule->getEnd(endDateTime);

		startDateTime.addDays(1);
		if(endDateTime >= startDateTime) // all day event
		{
			CalDate thisDate(endDateTime.getYear(), endDateTime.getMonth(), endDateTime.getMday());
			if(thisDate == date
					&& !endDateTime.getHour()
					&& !endDateTime.getMinute()
					&& !endDateTime.getSecond()) // all day event this day
			{
				date.addDays(-1);
			}
		}
	}

	__insertMonthItemIfNeeded(date, dir);

	CalListControl::Item* item = __findItem(date);
	if(item && item->isGroupTitle())
	{
		CalListGroupTitleItem* dayItem = (CalListGroupTitleItem*)item;
		if(dayItem)
		{
			dayItem->addItem(schedule, __isCreateCheck, isSelected, __searchText, dir);
		}
	}
	else
	{
		CalListGroupTitleItem* dayItem = (CalListGroupTitleItem*)__addGroupItem(date, dir);
		if(dayItem)
		{
			dayItem->addItem(schedule, __isCreateCheck, isSelected, __searchText, dir);
		}
	}
}

/**
 * Inserts a month item if needed.
 *
 * @param	date	The date.
 * @param	dir		The dir.
 */
void CalScheduleListControl::__insertMonthItemIfNeeded(CalDate date, int dir)
{
	bool found = false;

	Elm_Object_Item* it = NULL;
	if(dir > 0)
	{
		it = elm_genlist_first_item_get(getEvasObj());
	}
	else
	{
		it = elm_genlist_last_item_get(getEvasObj());
	}

	while (it)
	{
		Item* item = (Item*)elm_object_item_data_get(it);
		if (item && !item->isGroupTitle())
		{
			CalListMonthItem* monthItem = (CalListMonthItem*)item;
			if(monthItem && date.isSameMonth(monthItem->getDate()))
			{
				found = true;
				break;
			}
		}

		if(dir > 0)
		{
			it = elm_genlist_item_next_get(it);
		}
		else
		{
			it = elm_genlist_item_prev_get(it);
		}
	}

	if(!found)
	{
		CalListMonthItem* newItem = new CalListMonthItem(date);
		__addItem(newItem, dir, NULL);
	}
}

/**
 * Is on hold.
 *
 * @param [in]	obj	If non-null, the object.
 *
 * @return		An int.
 */
int CalScheduleListControl::__isOnHold(Evas_Object *obj)
{
	if(!obj)
	{
		return 0;
	}

	Evas *e = evas_object_evas_get(obj);
	if (e)
	{
		int flags = evas_event_default_flags_get(e);
		if ((flags & EVAS_EVENT_FLAG_ON_HOLD) == EVAS_EVENT_FLAG_ON_HOLD)
		{
			return 1;
		}
	}

	return 0;
}

/**
 * Process the event item tap.
 *
 * @param [in]	container	If non-null, the container.
 * @param [in]	item		If non-null, the item.
 */
void CalScheduleListControl::processEventItemTap(CalListGroupTitleItem* container, CalEventItem* item)
{
	if(__isOnHold(getEvasObj()))
	{
		return;
	}

	if(item->isCheckboxRequired() && __isCreateCheck && !__isHideCheck)
	{
		__selectedCount = 0;

		Elm_Object_Item* it = __getFirstItem();
		while (it)
		{
			Item* item = (Item*)elm_object_item_data_get(it);
			if (item->isGroupTitle())
			{
				CalListGroupTitleItem* groupItem = (CalListGroupTitleItem*)item;
				if(groupItem)
				{
					__selectedCount += groupItem->getSelectedItemsCount();
				}
			}

			it = elm_genlist_item_next_get(it);
		}

		if(__maxLimitSelectCount != -1 && __maxLimitSelectCount < __selectedCount)
		{
			item->setCheckboxSelected(false);
			__selectedCount--;

			if(__onMaxLimitSelectCb)
			{
				__onMaxLimitSelectCb();
			}

			return;
		}
	}

	if(__onTapScheduleItem)
	{
		__onTapScheduleItem(item->getSchedule());
	}
}

/**
 * Executes the select group item action.
 *
 * @param [in]	data		If non-null, the data.
 * @param [in]	obj			If non-null, the object.
 * @param [in]	event_info	If non-null, information describing the event.
 */
void CalScheduleListControl::__onSelectGroupItem(void* data, Evas_Object* obj, void* event_info)
{
	Elm_Object_Item* it = elm_genlist_selected_item_get(obj);
	elm_genlist_item_selected_set(it, EINA_FALSE);
}

/**
 * Gets list height.
 *
 * @return	The list height.
 */
Evas_Coord CalScheduleListControl::getListHeight()
{
	Evas_Coord h = 0;

	evas_object_geometry_get(getEvasObj(), NULL, NULL, NULL, &h);

	return h;
}

/**
 * Query if this object is scrolled to top item.
 *
 * @return	true if scrolled to top item, false if not.
 */
bool CalScheduleListControl::isScrolledToTopItem()
{
	Evas_Coord regionY = 0;

	elm_scroller_region_get(getEvasObj(), NULL, &regionY, NULL, NULL);

	return regionY == 0 ? true : false;
}

/**
 * Gets shown items height under top.
 *
 * @return	The shown items height under top.
 */
Evas_Coord CalScheduleListControl::getShownItemsHeightUnderTop()
{
	Eina_List* realized_items = elm_genlist_realized_items_get(getEvasObj());
	if (!realized_items)
	{
		return 0;
	}

	eina_list_free(realized_items);

	Evas_Coord listY = 0;
	evas_object_geometry_get(getEvasObj(), NULL, &listY, NULL, NULL);

	Elm_Object_Item* lastItem = elm_genlist_last_item_get(getEvasObj());

	Evas_Object* track = elm_object_item_track(lastItem);
	if (!track)
	{
		Evas_Coord regionH = 0;
		elm_scroller_region_get(getEvasObj(), NULL, NULL, NULL, &regionH);
		return regionH;
	}

	Evas_Coord itemY = 0;
	Evas_Coord itemH = 0;
	evas_object_geometry_get(track, NULL, &itemY, NULL, &itemH);

	elm_object_item_untrack(lastItem);

	return itemY + itemH - listY;
}

/**
 * Gets hidden items height under bottom.
 *
 * @return	The hidden items height under bottom.
 */
Evas_Coord CalScheduleListControl::getHiddenItemsHeightUnderBottom()
{
	Evas_Coord regionY = 0;
	Evas_Coord regionH = 0;
	elm_scroller_region_get(getEvasObj(), NULL, &regionY, NULL, &regionH);

	Evas_Coord sizeH = 0;
	elm_scroller_child_size_get(getEvasObj(), NULL, &sizeH);

	return sizeH - regionY - regionH;
}

/**
 * Scroll to last item.
 *
 * @param	noAnimation	true to no animation.
 */
void CalScheduleListControl::scrollToLastItem(bool noAnimation)
{
	__enableScroll();

	Elm_Object_Item* lastItem = elm_genlist_last_item_get(getEvasObj());
	if (noAnimation)
	{
		elm_genlist_item_show(lastItem, ELM_GENLIST_ITEM_SCROLLTO_TOP);
	}
	else
	{
		elm_genlist_item_bring_in(lastItem, ELM_GENLIST_ITEM_SCROLLTO_TOP);
	}
}

/**
 * Gets top showing group item.
 *
 * @return	null if it fails, else the top showing group item.
 */
CalListControl::Item* CalScheduleListControl::getTopShowingGroupItem()
{
	return __getTopShowingGroupItem();
}

/**
 * Sets is selected cb.
 *
 * @param [in]	isSelectedCb	The is selected cb.
 */
void CalScheduleListControl::setIsSelectedCb(std::function<bool (std::shared_ptr<CalSchedule>& schedule)> isSelectedCb)
{
	__isSelectedCb = isSelectedCb;
}

/**
 * Updates the search text described by searchText.
 *
 * @param	searchText	The search text.
 */
void CalScheduleListControl::updateSearchText(const char* searchText)
{
	if (searchText != NULL)
	{
		__searchText = searchText;
	}
}
