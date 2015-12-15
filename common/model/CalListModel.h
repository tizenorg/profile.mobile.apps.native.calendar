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

#ifndef _CAL_LIST_MODEL_H_
#define _CAL_LIST_MODEL_H_

#include <memory>
#include <vector>

#include "ICalListModel.h"

#include "ICalListProvider.h"

class WAPP_ASSIST_EXPORT CalListModel : public ICalListModel
{
public:
	virtual ~CalListModel();
public:
	virtual void prefetch(bool fillBothBuffers);
	virtual std::shared_ptr<CalSchedule> getNext(bool& dayChanged);
	virtual const CalDateTime& getCurrentDate();
	virtual bool eof();
protected:
	CalListModel(const CalDateTime& base, int dir, bool enableFlowControl = true);
private:
	WDISABLE_COPY_AND_ASSIGN(CalListModel);
protected:
	const int __dir;
private:
	CalDateTime __currentDate;
	std::vector<ICalListProvider*> __providers;
	std::shared_ptr<CalSchedule> __currentSchedule;
	bool __enableFlowControl;
friend class CalListModelFactory;
};

#endif
