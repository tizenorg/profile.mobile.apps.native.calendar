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

#ifndef _CAL_LIST_MODEL2_H_
#define _CAL_LIST_MODEL2_H_

#include "CalListModel.h"

class WAPP_ASSIST_EXPORT CalListModel2 : public CalListModel
{
public:

	/**
	 * @brief Get next event.
	 *
	 * @param[out] dayChanged    indicate whether the date has changed
	 *
	 * @return next event object.
	 *
	 */
	virtual std::shared_ptr<CalSchedule> getNext(bool &dayChanged);

	/**
	 * @brief Get date of currently provided event.
	 *
	 * @return event date.
	 *
	 */
	virtual const CalDate& getCurrentDate();

	/**
	 * @brief Check whether list is ended.
	 *
	 */
	virtual bool eof();

	/**
	 * @brief Set EnableNoEvent flag.
	 *
	 * @param enable    flag value
	 *
	 */
	void setEnableNoEvent(bool enable) {__enableNoEvent = enable;}

	/**
	 * @brief Check whether EnableNoEvent flag is enabled.
	 *
	 * @return EnableNoEvent flag state.
	 *
	 */
	bool getEnableNoEvent() {return __enableNoEvent;}

private:
	WDISABLE_COPY_AND_ASSIGN(CalListModel2);

	CalListModel2(const CalDate& base, int dir);

	bool __fresh;
	CalDate __currentMonth;
	bool __currentMonthItemReturned;
	std::shared_ptr<CalSchedule> __currentSchedule;
	bool __dayChanged;
	bool __enableNoEvent;

	friend class CalListModelFactory;
};

#endif
