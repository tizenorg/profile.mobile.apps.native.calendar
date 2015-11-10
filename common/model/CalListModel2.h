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
	virtual ~CalListModel2();
public:
//	virtual void prefetch(bool fillBothBuffers);
	virtual std::shared_ptr<CalSchedule> getNext(bool& dayChanged);
	virtual const CalDate& getCurrentDate();
	virtual bool eof();
public:
	void setEnableNoEvent(bool enable) {__enableNoEvent = enable;}
	bool getEnableNoEvent() {return __enableNoEvent;}
private:
	CalListModel2(const CalDate& base, int dir);
	WDISABLE_COPY_AND_ASSIGN(CalListModel2);
private:
	bool __fresh;
private:
	CalDate __currentMonth;
	bool __currentMonthItemReturned;
	std::shared_ptr<CalSchedule> __currentSchedule;
	bool __dayChanged;
private:
	bool __enableNoEvent;
friend class CalListModelFactory;
};

#endif
