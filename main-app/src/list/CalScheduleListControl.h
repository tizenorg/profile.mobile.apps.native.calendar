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

/**
 * @brief Calendar schedule list control.
 *
 * @see CalListControl
 */
class CalScheduleListControl : public CalListControl
{
public:
	/**
	 * @brief Create calendar schedule list control.
	 *
	 * @param[in]   fowardModel         The forward model.
	 * @param[in]   backwardModel       The backward model.
	 * @param[in]   onTapScheduleItem   Tap schedule item callback.
	 * @param[in]   onScrollStart       Start scroll callback.
	 * @param[in]   onScrollStop        Stop scroll callback.
	 * @param[in]   isCreateCheck       To create checkbox or not.
	 * @param[in]   isHideCheck         To hide checkbox or not.
	 * @param[in]   searchText          The search text.
	 * @param[in]   looperCount         The looper count.
	 * @param[in]   enableNoEvent       True to enable no event, false otherwise.
	 *
	 * @see ICalListModel
	 * @see CalSchedule
	 * @see CalListControl::Item
	 */
	CalScheduleListControl(ICalListModel *fowardModel, ICalListModel *backwardModel,
			std::function<void (std::shared_ptr<CalSchedule> schedule)> onTapScheduleItem,
			std::function<void (int dir)> onScrollStart,
			std::function<void (int dir, CalListControl::Item *topShowingGroupItem)> onScrollStop,
			bool isCreateCheck = false, bool isHideCheck = false, std::string searchText = std::string(),
			int looperCount = 1, bool enableNoEvent = false);
	virtual ~CalScheduleListControl();

	/**
	 * @brief Get class name.
	 *
	 * @return the class name.
	 *
	 * @see CalListControl::getClassName()
	 */
	virtual const char *getClassName();

	/**
	 * @brief Set maximum limit select callback.
	 *
	 * @param[in]   onMaxLimitSelectCb  The on maximum limit select callback.
	 */
	void setMaxLimitSelectCb(std::function<void (void)> onMaxLimitSelectCb);

	/**
	 * @brief Set maximum limit of the selected items.
	 *
	 * @param[in]   maxCount    Maximum number of selected items.
	 */
	void setMaxLimitCount(int maxCount);

	/**
	 * @brief Set is selected callback.
	 *
	 * @param[in]   isSelectedCb    The is selected callback.
	 */
	void setIsSelectedCb(std::function<bool (std::shared_ptr<CalSchedule> &schedule)> isSelectedCb);

	/**
	 * @brief Get selected schedule item count.
	 *
	 * @return the selected schedule item count.
	 */
	int getSelectScheduleItemCount();

	/**
	 * @brief Get schedule item count.
	 *
	 * @return the schedule item count.
	 */
	int getScheduleItemCount();

	/**
	 * @brief Scroll to specific date.
	 *
	 * @param[in]   date    The date.
	 *
	 * @return true if it succeeds, false if it fails.
	 *
	 * @see CalDate
	 */
	bool scrollTo(const CalDate& date);

	/**
	 * @brief Get selected schedules.
	 *
	 * @param[in]   list    The list.
	 */
	void getSelectedSchedules(std::list<std::shared_ptr<CalSchedule>> &list);

	/**
	 * @brief Select/Deselect all schedule item.
	 *
	 * @param[in]   sellectedAll    Determines whether all schedules items should be selected.
	 */
	void selectAllScheduleItem(Eina_Bool sellectedAll);

	/**
	 * @brief Show/Hide the check box.
	 *
	 * @param[in]   showCheck   Determines whether the checkbox should be shown or hidden.
	 */
	void showCheckBox(Eina_Bool showCheck);

	/**
	 * @brief Query if this object is empty.
	 *
	 * @return true if empty, false if not.
	 */
	bool isEmpty();

	/**
	 * @brief Process the event item tap.
	 *
	 * @param[in]   container   If non-null, the container.
	 * @param[in]   item        If non-null, the item.
	 */
	void processEventItemTap(CalListGroupTitleItem *container, CalEventItem *item);

	/**
	 * @brief Query if this object is scrolling.
	 *
	 * @return true if scrolling, false if not.
	 */
	bool isScrolling() { return __scrollAnimStarted || __scrollIsUserInitiatedByDragging; }

	/**
	 * @brief Lock the drag.
	 *
	 * @param[in]   lock    True to lock, false to unlock.
	 */
	void lockDrag(bool lock);

	/**
	 * @brief Get list height.
	 *
	 * @return the list height.
	 */
	Evas_Coord getListHeight();

	/**
	 * @brief Get shown items height under top.
	 *
	 * @return the shown items height under top.
	 */
	Evas_Coord getShownItemsHeightUnderTop();

	/**
	 * @brief Get hidden items height under bottom.
	 *
	 * @return the hidden items height under bottom.
	 */
	Evas_Coord getHiddenItemsHeightUnderBottom();

	/**
	 * @brief Scroll to last item.
	 *
	 * @param[in]   noAnimation     True to no animation.
	 */
	void scrollToLastItem(bool noAnimation = false);

	/**
	 * @brief Query if this object is scrolled to top item.
	 *
	 * @return true if scrolled to top item, false if not.
	 */
	bool isScrolledToTopItem();

	/**
	 * @brief Get top showing group item.
	 *
	 * @return null if it fails, else the top showing group item.
	 */
	CalListControl::Item *getTopShowingGroupItem();

	/**
	 * @brief Set selected count.
	 *
	 * @param[in]   count   Number of selected item.
	 */
	void setSelectedCount(int count);

	/**
	 * @brief Set list models.
	 *
	 * @param[in]   forwardModel    If non-null, the forward model.
	 * @param[in]   backwardModel   If non-null, the backward model.
	 */
	void setListModels(ICalListModel *forwardModel, ICalListModel *backwardModel);

	/**
	 * @brief Add initial batch of items.
	 */
	void addInitialBatchOfItems();

	/**
	 * @brief Update the search text described by @p searchText.
	 *
	 * @param[in]   searchText  The search text.
	 */
	void updateSearchText(const char *searchText);

private:
	/**
	 * @brief Execute the created action.
	 *
	 * @see CalListControl::onCreated()
	 */
	virtual void onCreated();

	/**
	 * @brief Execute the destroy action.
	 *
	 * @see CalListControl::onDestroy()
	 */
	virtual void onDestroy();

	WDISABLE_COPY_AND_ASSIGN(CalScheduleListControl);

	/**
	 * @brief Initialise the initialise backward model.
	 *
	 * @param[in]   enableNoEvent   True to enable, false to disable the no event.
	 */
	void __initBackwardModel(bool enableNoEvent);

	/**
	 * @brief Finalize scroll if animation doesn't start at all.
	 */
	void __finalizeScrollIfAnimationDoesntStartAtAll();

	/**
	 * @brief Enable the scroll.
	 */
	void __enableScroll();

	/**
	 * @brief Initialize the initialize scroll.
	 */
	void __initializeScroll();

	/**
	 * @brief Finalize scroll.
	 */
	void __finalizeScroll();

	/**
	 * @brief Load the more.
	 */
	void __loadMore();

	/**
	 * @brief Determine if we are near tail.
	 *
	 * @return true if near tail, false if not.
	 */
	bool __isNearTail();

	/**
	 * @brief Determine if we are near head.
	 *
	 * @return true if near head, false if not.
	 */
	bool __isNearHead();

	/**
	 * @brief Append the next.
	 *
	 * @return	true if it succeeds, false if it fails.
	 */
	bool __appendNext();

	/**
	 * @brief Prepend the next.
	 *
	 * @return true if it succeeds, false if it fails.
	 */
	bool __prependNext();

private:
	/**
	 * @brief Add a group item to 'dir'.
	 *
	 * @param[in]   date    The date.
	 * @param[in]   dir     The dir.
	 *
	 * @return null if it fails, else an Item*.
	 *
	 * @see CalDate
	 */
	Item *__addGroupItem(const CalDate &date, int dir);

	/**
	 * @brief Insert an item.
	 *
	 * @param[in]   schedule    The schedule.
	 * @param[in]   date        The date.
	 * @param[in]   isSelected  True if this object is selected.
	 * @param[in]   dir         The dir.
	 *
	 * @see CalSchedule
	 * @see CalDate
	 */
	void __insertItem(const std::shared_ptr<CalSchedule>& schedule, CalDate date,
			bool isSelected, int dir);

	/**
	 * @brief Insert a month item if needed.
	 *
	 * @param[in]   date    The date.
	 * @param[in]   dir     The dir.
	 *
	 * @see CalDate
	 */
	void __insertMonthItemIfNeeded(CalDate date, int dir);

	/**
	 * @brief Execute the select group item action.
	 *
	 * @param[in]   data        If non-null, the data.
	 * @param[in]   obj         If non-null, the object.
	 * @param[in]   event_info  If non-null, information describing the event.
	 */
	static void __onSelectGroupItem(void *data, Evas_Object *obj, void *event_info);

	/**
	 * @brief Execute the scroll animation stop action.
	 *
	 * @param[in]   data        If non-null, the data.
	 * @param[in]   obj         If non-null, the object.
	 * @param[in]   event_info  If non-null, information describing the event.
	 */
	static void __onScrollAnimStop(void *data, Evas_Object *obj, void *event_info);

	/**
	 * @brief Execute the scroll drag stop action.
	 *
	 * @param[in]   data        If non-null, the data.
	 * @param[in]   obj         If non-null, the object.
	 * @param[in]   event_info  If non-null, information describing the event.
	 */
	static void __onScrollDragStop(void *data, Evas_Object *obj, void *event_info);

	/**
	 * @brief Is on hold.
	 *
	 * @param[in]   obj     If non-null, the object.
	 *
	 * @return true if is in hold, false otherwise.
	 */
	bool __isOnHold(Evas_Object *obj);

	CalDate __base;
	CalDate __focusDate;
	bool __appended;
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
	ICalListModel *__forwardModel;
	ICalListModel *__backwardModel;
	Ecore_Timer *__checkFirstDrawTimer;
	Ecore_Animator *__forwardPreloader;
	Ecore_Animator *__backwardPreloader;
	bool __backwardModelInitialized;
	std::shared_ptr<CalSchedule> __peekBackwardSchedule;
	std::shared_ptr<CalSchedule> __peekForwardSchedule;
	bool __baseMonthIsNotEmpty;
	const Item *__latestRealizedItem;
	CalSearchChain __groupItemChain;
	std::function<void (std::shared_ptr<CalSchedule> schedule)> __onTapScheduleItem;
	std::function<void (int dir)> __onScrollStart;
	std::function<void (int dir, CalListControl::Item *topShowingGroupItem)> __onScrollStop;
	std::function<void (void)> __onMaxLimitSelectCb;
	std::function<bool (std::shared_ptr<CalSchedule> &schedule)> __isSelectedCb;
};

#endif
