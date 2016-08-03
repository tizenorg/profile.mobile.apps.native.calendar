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

#ifndef _CAL_VCS_VIEW_H_
#define _CAL_VCS_VIEW_H_

#include <list>
#include <memory>
#include <functional>
#include "CalSchedule.h"
#include "CalView.h"
#include "CalScheduleListControl.h"
#include "CalCustomListModel.h"

/**
 * @brief Calendar VCS view.
 *
 * @see CalView.
 */
class CalVcsView : public CalView
{
public:
	/**
	 * @brief Create VCS view.
	 *
	 * @param[in]   schedules   The list of shedules.
	 *
	 * @see CalSchedule
	 */
	CalVcsView(const std::list<std::shared_ptr<CalSchedule>>& schedules);

protected:
	virtual ~CalVcsView();

private:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);
	virtual void onDestroy();
	virtual void onBecomeTop();
	virtual void onExpose();
	virtual void onPushed(Elm_Object_Item* naviItem);
	virtual void onEvent(const CalEvent& event);
	void __updateTitleInfo();
	virtual void  onMenuButton();
	virtual bool onPop();
	void __switchMode(bool isSelectMode);
	void __save();

	WDISABLE_COPY_AND_ASSIGN(CalVcsView);

	std::list<std::shared_ptr<CalSchedule>> __schedules;
	bool __isSelectMode;
	Evas_Object* __box;
	Evas_Object* __selectAllLayout;
	CalDate __focusDate;
	CalScheduleListControl* __list;
	CalCustomListModel* __fowardModel;
	CalCustomListModel* __emptyModel;
};

#endif
