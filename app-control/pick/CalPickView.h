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
#include "CalFilterView.h"
#include "CalScheduleListControl.h"
#include "ICalListModel.h"
#include "CalInstanceScheduleSet.h"

/**
 * @brief Calendar pick view.
 *
 * @see CalFilterView.
 */
class CalPickView : public CalFilterView
{
public:
	/**
	 * @brief Result types.
	 */
	enum ResultType
	{
		RESULT_TYPE_ERROR = -1,
		RESULT_TYPE_VCS,
		RESULT_TYPE_TEXT,
		RESULT_TYPE_ID
	};

	/**
	 * @brief Create pick view.
	 *
	 * @param[in]   maxLimit     The maximum limit.
	 * @param[in]   resultType   Result type
	 *
	 * @see ResultType
	 */
	CalPickView(int maxLimit, CalPickView::ResultType resultType = RESULT_TYPE_VCS);

	/**
	 * @brief Set done callback.
	 *
	 * @param[in]   selectCb   Select callback.
	 *
	 * @see CalSchedule
	 */
	void setDoneCb(std::function<void (const std::list<std::shared_ptr<CalSchedule>>& schedules)> selectCb);

protected:
	virtual ~CalPickView();

private:
	WDISABLE_COPY_AND_ASSIGN(CalPickView);

	virtual void onPushed(Elm_Object_Item* naviItem);
	virtual void onEvent(const CalEvent& event);

	void __updateTitleInfo();
	void __reCheck();

	std::function<void (const std::list<std::shared_ptr<CalSchedule>>& schedules)> __doneCb;

	int __maxLimit;
	ResultType __resultType;
	CalInstanceScheduleSet __selectedScheduleSet;
};

#endif
