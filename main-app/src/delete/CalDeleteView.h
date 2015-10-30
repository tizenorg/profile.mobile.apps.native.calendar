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

#ifndef _CAL_DELETE_VIEW_H_
#define _CAL_DELETE_VIEW_H_

#include <memory>
#include "CalView.h"
#include "CalSchedule.h"
#include "CalScheduleListControl.h"
#include "ICalListModel.h"

class CalDeleteView : public CalView
{
public:
	CalDeleteView(const CalDate& focusDate);
	virtual ~CalDeleteView();

private:

	/**
	 * Executes the create action. This function is called when CalDeleteView object is created.
	 *
	 * @param [in]	parent   	If non-null, the parent.
	 * @param [in]	viewParam	If non-null, the view parameter.
	 *
	 * @return	null if it fails, else - created Evas_Object*.
	 */
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);

	/**
	 * Executes the pushed action. This function is called when our view was successfully pushed into naviframe container.
	 *
	 * @param [in]	naviItem	If non-null, the navi item.
	 */
	virtual void onPushed(Elm_Object_Item* naviItem);

	/**
	 * Executes the destroy action. This function is called when our view object is being destroyed.
	 */
	virtual void onDestroy();

	/**
	 * Executes some action, depending on event it receives.
	 *
	 * @param	event	The event.
	 */
	virtual void onEvent(const CalEvent& event);

	/**
	 * Updates this object.
	 */
	void __update();

	/**
	 * Gets repeated schedule count. If calendar service contains some records 'bout events, which have repeats assigned to, they will be returned in 1st parameter.
	 *
	 * @param [in]	selectedScheduleList	The selected schedule list.
	 *
	 * @return	The repeated schedule count.
	 */
	int __getRepeatedScheduleCount(std::list<std::shared_ptr<CalSchedule>>& selectedScheduleList);

	/**
	 * Deletes this object.
	 */
	void __delete();

	/**
	 * Updates the title information.
	 */
	void __updateTitleInfo();

	/**
	 * Depending on paramater, shows view contents or hids them.
	 *
	 * @param	isShow	true if this object is show.
	 */
	void __showNoContent(bool isShow);

	WDISABLE_COPY_AND_ASSIGN(CalDeleteView);

private:
	bool __isDeleteMode;
	CalScheduleListControl* __list;
	const CalDate __focusDate;
	Evas_Object* __box;
	bool __isNoContents;
	Evas_Object* __noContents;
	ICalListModel* __fowardModel;
	ICalListModel* __backwardModel;
};
#endif /* _CAL_DELETE_VIEW_H_ */
