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

#ifndef _I_CAL_LIST_MODEL_H_
#define _I_CAL_LIST_MODEL_H_

#include <memory>
#include "CalSchedule.h"
#include "CalDate.h"

class WAPP_ASSIST_EXPORT ICalListModel
{
public:
	virtual void prefetch(bool fillBothBuffers) {}
	virtual std::shared_ptr<CalSchedule> getNext(bool& dayChanged) = 0;
	virtual const CalDate& getCurrentDate() = 0;
	virtual bool eof() = 0;
public:
	virtual ~ICalListModel() {}
};

#endif
