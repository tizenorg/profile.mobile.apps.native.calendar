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

#ifndef _CAL_ASYNC_TASK_H_
#define _CAL_ASYNC_TASK_H_

#include <functional>
#include <Ecore.h>
#include "WDefine.h"


typedef std::function<void (const void *data)> ECORE_IDLER_CB;

class CalAsyncTask
{
	WDISABLE_COPY_AND_ASSIGN(CalAsyncTask);

public:
	CalAsyncTask(ECORE_IDLER_CB cb, const void *data);
	~CalAsyncTask();

	void Invoke();
	void Terminate();

private:
	Ecore_Idler* __idler;
	ECORE_IDLER_CB __idlerCb;
	const void * __data;

};

#endif // _CAL_ASYNC_TASK_H_
