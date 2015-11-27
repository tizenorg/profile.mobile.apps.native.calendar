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

#include "CalAsyncTask.h"

CalAsyncTask::CalAsyncTask(ECORE_IDLER_CB cb, const void *data) :
	__idler(NULL),
	__idlerCb(cb),
	__data(data)
{

}

CalAsyncTask::~CalAsyncTask()
{

}

void CalAsyncTask::Invoke()
{
	__idler = ecore_idler_add(
	[](void* data) {
			CalAsyncTask *self = (CalAsyncTask*)data;
			self->__idlerCb(self->__data);
			return ECORE_CALLBACK_CANCEL;
	}, this);
}

void CalAsyncTask::Terminate()
{
	ecore_idler_del(__idler);
}
