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

#ifndef _CAL_SCHEDULE_LIST_CONTROL_H_
#define _CAL_SCHEDULE_LIST_CONTROL_H_

#include "CalListControl.h"
#include "CalEventItem.h"
#include "ICalListModel.h"

class CalListGroupTitleItem;

class CalScheduleListControl : public CalListControl
{
public:
	CalScheduleListControl(ICalListModel* fowardModel, ICalListModel* backwardModel,
	std::function<void (std::shared_ptr<CalSchedule> schedule)> onTapScheduleItem,
	std::function<void (int dir)> onScrollStart,
	std::function<void (int dir, CalListControl::Item* topShowingGroupItem)> onScrollStop,
	bool isCreateCheck = false, bool isHideCheck = false, std::string searchText = std::string(),
	int looperCount = 1, bool enableNoEvent = false);
	virtual ~CalScheduleListControl();
	virtual const char* getClassName();

	/**
	 * Sets maximum limit select cb.
	 *
	 * @param	onMaxLimitSelectCb	The on maximum limit select cb.
	 */
	void setMaxLimitSelectCb(std::function<void (void)> onMaxLimitSelectCb);

	/**
	 * Sets maximum limit count.
	 *
	 * @param	maxCount	Number of maximums.
	 */
	void setMaxLimitCount(int maxCount);

	/**
	 * Sets is selected cb.
	 *
	 * @param [in]	isSelectedCb	The is selected cb.
	 */
	void setIsSelectedCb(std::function<bool (std::shared_ptr<CalSchedule>& schedule)> isSelectedCb);

	/**
	 * Gets select schdule item count.
	 *
	 * @return	The select schdule item count.
	 */
	int getSelectSchduleItemCount();

	/**
	 * Gets schdule item count.
	 *
	 * @return	The schdule item count.
	 */
	int getSchduleItemCount();

	/**
	 * Scroll to specific date.
	 *
	 * @param	date	The date.
	 *
	 * @return	true if it succeeds, false if it fails.
	 */
	bool scrollTo(const CalDate& date);

	/**
	 * Gets selected schedules.
	 *
	 * @param [in]	list	The list.
	 */
	void getSelectedSchedules(std::list<std::shared_ptr<CalSchedule>>& list);

	/**
	 * Select all schdule item.
	 *
	 * @param	sellectedAll	The sellected all.
	 */
	void selectAllSchduleItem(Eina_Bool sellectedAll);

	/**
	 * Shows the check box.
	 *
	 * @param	showCheck	The show check.
	 */
	void showCheckBox(Eina_Bool showCheck);

	/**
	 * Query if this object is empty.
	 *
	 * @return	true if empty, false if not.
	 */
	bool isEmpty();

	/**
	 * Process the event item tap.
	 *
	 * @param [in]	container	If non-null, the container.
	 * @param [in]	item	 	If non-null, the item.
	 */
	void processEventItemTap(CalListGroupTitleItem* container, CalEventItem* item);

	/**
	 * Query if this object is scrolling.
	 *
	 * @return	true if scrolling, false if not.
	 */
	bool isScrolling() {return __scrollAnimStarted || __scrollIsUserInitiatedByDragging;}

	/**
	 * Locks the drag.
	 *
	 * @param	lock	true to lock, false to unlock.
	 */
	void lockDrag(bool lock);

	/**
	 * Gets list height.
	 *
	 * @return	The list height.
	 */
	Evas_Coord getListHeight();

	/**
	 * Gets shown items height under top.
	 *
	 * @return	The shown items height under top.
	 */
	Evas_Coord getShownItemsHeightUnderTop();

	/**
	 * Gets hidden items height under bottom.
	 *
	 * @return	The hidden items height under bottom.
	 */
	Evas_Coord getHiddenItemsHeightUnderBottom();

	/**
	 * Scroll to last item.
	 *
	 * @param	noAnimation	true to no animation.
	 */
	void scrollToLastItem(bool noAnimation = false);

	/**
	 * Query if this object is scrolled to top item.
	 *
	 * @return	true if scrolled to top item, false if not.
	 */
	bool isScrolledToTopItem();

	/**
	 * Gets top showing group item.
	 *
	 * @return	null if it fails, else the top showing group item.
	 */
	CalListControl::Item* getTopShowingGroupItem();

	/**
	 * Sets selected count.
	 *
	 * @param	count	Number of.
	 */
	void setSelectedCount(int count);

	/**
	 * Sets list models.
	 *
	 * @param [in]	forwardModel 	If non-null, the forward model.
	 * @param [in]	backwardModel	If non-null, the backward model.
	 */
	void setListModels(ICalListModel* forwardModel, ICalListModel* backwardModel);

	/**
	 * Adds initial batch of items.
	 */
	void addInitialBatchOfItems();

	/**
	 * Updates the search text described by searchText.
	 *
	 * @param	searchText	The search text.
	 */
	void updateSearchText(const char* searchText);

private:
	virtual void onCreated();
	virtual void onDestroy();
	WDISABLE_COPY_AND_ASSIGN(CalScheduleListControl);

	/**
	 * Initialises the initialise backward model.
	 *
	 * @param	enableNoEvent	true to enable, false to disable the no event.
	 */
	void __initBackwardModel(bool enableNoEvent);

	/**
	 * Finalize scroll if animation doesnt start at all.
	 */
	void __finalizeScrollIfAnimationDoesntStartAtAll();

	/**
	 * Enables the scroll.
	 */
	void __enableScroll();

	/**
	 * Initializes the initialize scroll.
	 */
	void __initializeScroll();

	/**
	 * Finalize scroll.
	 */
	void __finalizeScroll();

	/**
	 * Loads the more.
	 */
	void __loadMore();

	/**
	 * Determines if we can is near tail.
	 *
	 * @return	true if near tail, false if not.
	 */
	bool __isNearTail();

	/**
	 * Determines if we can is near head.
	 *
	 * @return	true if near head, false if not.
	 */
	bool __isNearHead();

	/**
	 * Appends the next.
	 *
	 * @return	true if it succeeds, false if it fails.
	 */
	bool __appendNext();

	/**
	 * Prepends the next.
	 *
	 * @return	true if it succeeds, false if it fails.
	 */
	bool __prependNext();
private:

	/**
	 * Adds a group item to 'dir'.
	 *
	 * @param	date	The date.
	 * @param	dir 	The dir.
	 *
	 * @return	null if it fails, else an Item*.
	 */
	Item* __addGroupItem(const CalDate& date, int dir);

	/**
	 * Inserts an item.
	 *
	 * @param	schedule  	The schedule.
	 * @param	date	  	The date.
	 * @param	isSelected	true if this object is selected.
	 * @param	dir		  	The dir.
	 */
	void __insertItem(const std::shared_ptr<CalSchedule>& schedule, CalDate date, bool isSelected, int dir);

	/**
	 * Inserts a month item if needed.
	 *
	 * @param	date	The date.
	 * @param	dir 	The dir.
	 */
	void __insertMonthItemIfNeeded(CalDate date, int dir);
private:

	/**
	 * Executes the select group item action.
	 *
	 * @param [in]	data	  	If non-null, the data.
	 * @param [in]	obj		  	If non-null, the object.
	 * @param [in]	event_info	If non-null, information describing the event.
	 */
	static void __onSelectGroupItem(void* data, Evas_Object* obj, void* event_info);

	/**
	 * Executes the scroll animation stop action.
	 *
	 * @param [in]	data	  	If non-null, the data.
	 * @param [in]	obj		  	If non-null, the object.
	 * @param [in]	event_info	If non-null, information describing the event.
	 */
	static void __onScrollAnimStop(void* data, Evas_Object* obj, void* event_info);

	/**
	 * Executes the scroll drag stop action.
	 *
	 * @param [in]	data	  	If non-null, the data.
	 * @param [in]	obj		  	If non-null, the object.
	 * @param [in]	event_info	If non-null, information describing the event.
	 */
	static void __onScrollDragStop(void* data, Evas_Object* obj, void* event_info);

	/**
	 * Is on hold.
	 *
	 * @param [in]	obj	If non-null, the object.
	 *
	 * @return	An int.
	 */
	int __isOnHold(Evas_Object *obj);
private:
	CalDate __base;
	CalDate __focusDate;
	bool __appended;
	bool __prepended;
	bool __isCreateCheck;
	bool __isHideCheck;
	int __selectedCount;
	int __scheduleItemCount;
	int __maxLimitSelectCount;
	bool __scrollAnimStarted;
	bool __scrollIsUserInitiatedByDragging;
	bool __dragLock;
	bool __initialPageDrawComplete;
	std::string __searchText;
	bool __isMouseDown;
	ICalListModel* __forwardModel;
	ICalListModel* __backwardModel;
	Ecore_Timer* __checkFirstDrawTimer;
	Ecore_Animator* __forwardPreloader;
	Ecore_Animator* __backwardPreloader;
	bool __backwardModelInitialized;
	std::shared_ptr<CalSchedule> __peekBackwardSchedule;
	std::shared_ptr<CalSchedule> __peekForwardSchedule;
	bool __baseMonthIsNotEmpty;
	const Item* __latestRealizedItem;
	CalSearchChain __groupItemChain;
	std::function<void (std::shared_ptr<CalSchedule> schedule)> __onTapScheduleItem;
	std::function<void (int dir)> __onScrollStart;
	std::function<void (int dir, CalListControl::Item* topShowingGroupItem)> __onScrollStop;
	std::function<void (void)> __onMaxLimitSelectCb;
	std::function<bool (std::shared_ptr<CalSchedule>& schedule)> __isSelectedCb;
};

#endif
