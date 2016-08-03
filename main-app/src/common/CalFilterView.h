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

/**
 * @brief Select callback.
 *
 * @param[in]   schedule   The shedule.
 *
 * @see CalSchedule.
 */
typedef std::function<void (std::shared_ptr<CalSchedule> schedule)> EVENT_ITEM_ONSELECT_CB;

/**
 * @brief Calendar filter view.
 *
 * @see CalView.
 */
class CalFilterView : public CalView
{
	WDISABLE_COPY_AND_ASSIGN(CalFilterView);
public:
	/**
	 * @brief Create filter view.
	 *
	 * @param[in]   name   The name.
	 */
	CalFilterView(const char *name);

	virtual ~CalFilterView();

	/**
	 * @brief Set select callback.
	 *
	 * @param[in]   selectCb   Select callback.
	 *
	 * @see EVENT_ITEM_ONSELECT_CB
	 */
	void setSelectCb(EVENT_ITEM_ONSELECT_CB selectCb);

	/**
	 * @brief Set back button visibility state.
	 *
	 * @param[in]   isVisible   Back button visibility state.
	 */
	void setBackButtonVisibility(bool isVisible);

	/**
	 * @brief Get back button visibility state.
	 *
	 * @return Whether the  back button is visibile.
	 */
	bool isBackButtonVisibile();

protected:
	void createList(EVENT_ITEM_ONSELECT_CB selectCb, bool useChecks = false);
	Evas_Object *createNoContent();
	void showContent();
	CalUnderlineEditField *createSearchBarEntry(Evas_Object *parent);
	void updateList();
	void setFocusToSearchEntry();

	CalScheduleListControl* __list;

private:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);
	virtual void onPushed(Elm_Object_Item* naviItem);
	virtual void onEvent(const CalEvent& event);
	virtual void onDestroy();

	std::string __searchText;
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
