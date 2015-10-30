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

#ifndef _CAL_PICK_VIEW_H_
#define _CAL_PICK_VIEW_H_

#include <list>
#include <memory>
#include <functional>
#include "CalSchedule.h"
#include "CalView.h"
#include "CalScheduleListControl.h"
#include "ICalListModel.h"
#include "CalInstanceScheduleSet.h"

class CalPickView : public CalView
{
public:
	enum ResultType
	{
		RESULT_TYPE_ERROR = -1,
		RESULT_TYPE_VCS,
		RESULT_TYPE_TEXT,
		RESULT_TYPE_ID
	};

	CalPickView(int maxLimit, CalPickView::ResultType resultType = RESULT_TYPE_VCS);
	void setSelectCb(std::function<void (const std::list<std::shared_ptr<CalSchedule>>& schedules)> selectCb);

private:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);
	virtual void onDestroy();
	virtual void onPushed(Elm_Object_Item* naviItem);
	virtual void onEvent(const CalEvent& event);
	void __update();
	void __updateTitleInfo();
	void __showContent();
	void __createList(Evas_Object* parent = NULL);
	Evas_Object* __createSearchBar(Evas_Object* parent);
	Evas_Object* __createEntry(Evas_Object* parent);
	Evas_Object* __createNoContent();
	void __reCheck();
protected:
	virtual ~CalPickView();
private:
	WDISABLE_COPY_AND_ASSIGN(CalPickView);
private:
	std::function<void (const std::list<std::shared_ptr<CalSchedule>>& schedules)> __selectCb;
private:
	int __maxLimit;
	ResultType __resultType;
	CalScheduleListControl* __list;
	CalDate __focusDate;
	char* __entryText;
	Evas_Object* __entry;
	Evas_Object* __box;
	Evas_Object* __noContents;
	Evas_Object* __searchBar;
	ICalListModel* __forwardModel;
	ICalListModel* __backwardModel;
	Ecore_Timer *__notificationTimer;
	bool __isNoContents;
	CalInstanceScheduleSet selectedScheduleSet;
};

#endif
