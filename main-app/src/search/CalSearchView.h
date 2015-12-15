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

#ifndef _CAL_SEARCH_VIEW_H_
#define _CAL_SEARCH_VIEW_H_

#include <memory>
#include "CalView.h"
#include "CalSchedule.h"
#include "CalScheduleListControl.h"
#include "ICalListModel.h"

class CalSearchView : public CalView
{
public:
	CalSearchView();
	virtual ~CalSearchView ();

private:

	/**
	 * Executes the create action.
	 *
	 * @param [in]	parent		If non-null, the parent.
	 * @param [in]	viewParam	If non-null, the view parameter.
	 *
	 * @return	null if it fails, else an Evas_Object*.
	 */
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);

	/**
	 * Executes the pushed action.
	 *
	 * @param [in]	naviItem	If non-null, the navi item.
	 */
	virtual void onPushed(Elm_Object_Item* naviItem);

	/**
	 * Executes the destroy action.
	 */
	virtual void onDestroy();

	/**
	 * Executes the event action.
	 *
	 * @param	event	The event.
	 */
	virtual void onEvent(const CalEvent& event);

	/**
	 * Updates this object.
	 */
	void __update();

	/**
	 * Creates the list.
	 *
	 * @return	null if it fails, else the new list.
	 */
	CalScheduleListControl* __createList();

	/**
	 * Executes the create search bar
	 *
	 * @param [in]	parent		If non-null, the parent.
	 *
	 * @return	A pointer to SearchBar object
	 */
	Evas_Object* __createSearchBar(Evas_Object* parent);

	/**
	 * Executes the create entry
	 *
	 * @param [in]	parent		If non-null, the parent.
	 *
	 * @return	A pointer to entry object
	 */
	Evas_Object* __createEntry(Evas_Object* parent);

	/**
	 * Creates no content.
	 *
	 * @return	null if it fails, else the new no content.
	 */
	Evas_Object* __createNoContent();

	/**
	 * Shows the content.
	 */
	void __showContent();

	/**
	 * Transition finished cb.
	 *
	 * @param [in]	data		If non-null, the data.
	 * @param [in]	obj			If non-null, the object.
	 * @param [in]	eventInfo	If non-null, information describing the event.
	 */
	static void __transitionFinishedCb(void* data, Evas_Object* obj, void* eventInfo);

	/**
	 * Entry change cb.
	 *
	 * @param [in]	data		If non-null, the data.
	 * @param [in]	obj			If non-null, the object.
	 * @param [in]	event_info	If non-null, information describing the event.
	 */
	static void __entryChangeCb(void* data, Evas_Object* obj, void* event_info);
	WDISABLE_COPY_AND_ASSIGN(CalSearchView);

private:
	CalScheduleListControl* __list;
	char *__entryText;
	Evas_Object* __entry;
	CalDateTime __focusDate;
	bool __isNoContents;
	Evas_Object* __noContents;
	Evas_Object* __searchBar;
	ICalListModel* __forwardModel;
	ICalListModel* __backwardModel;
	Ecore_Timer *__notificationTimer;
};

#endif /* _CAL_SEARCH_VIEW_H_ */
