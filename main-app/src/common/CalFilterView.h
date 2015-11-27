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

#ifndef _CAL_FILTERVIEW_H_
#define _CAL_FILTERVIEW_H_

#include <memory>
#include <Ecore.h>

#include "CalView.h"
#include "CalSchedule.h"
#include "CalScheduleListControl.h"
#include "ICalListModel.h"
#include "CalUnderlineEditField.h"

typedef std::function<void (std::shared_ptr<CalSchedule> schedule)> EVENT_ITEM_ONSELECT_CB;

class CalFilterView : public CalView
{
	WDISABLE_COPY_AND_ASSIGN(CalFilterView);
public:
	CalFilterView(const char *name);
	virtual ~CalFilterView();

	void setSelectCb(EVENT_ITEM_ONSELECT_CB selectCb);
	void setBackButtonVisibility(bool isVisible);
	bool isBackButtonVisibile();

protected:
	void createList(EVENT_ITEM_ONSELECT_CB selectCb, bool useChecks = false);
	Evas_Object *createNoContent();
	void showContent();
	CalUnderlineEditField *createSearchBarEntry(Evas_Object *parent);
	void updateList();

	CalScheduleListControl* __list;

private:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);
	virtual void onPushed(Elm_Object_Item* naviItem);
	virtual void onEvent(const CalEvent& event);
	virtual void onDestroy();

	const char *__searchText;
	Evas_Object* __noContents;
	CalUnderlineEditField* __searchBarEntry;
	ICalListModel* __forwardModel;
	ICalListModel* __backwardModel;
	bool __isNoContents;
	CalDate __focusDate;
	Ecore_Timer *__notificationTimer;
	EVENT_ITEM_ONSELECT_CB __selectCb;
	bool __backButtonVisibility;

	void __deleteListModels();


};

#endif /* _CAL_FILTERVIEW_H_ */
