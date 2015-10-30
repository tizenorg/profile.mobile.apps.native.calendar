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

#include "CalWorker.h"
#include "CalCommon.h"

CalWorker::CalWorker(std::function<void ()> job) :
	__job(job)
{
	WENTER();
	__workerThread = g_thread_new("CalWorker", CalWorker::__threadFunc, (gpointer)this);
}

CalWorker::~CalWorker()
{
}

void CalWorker::wait()
{
	WENTER();
	if (__workerThread)
	{
		g_thread_join(__workerThread);
		__workerThread = NULL;
	}

	WLEAVE();
}

gpointer CalWorker::__threadFunc(gpointer data)
{
	WENTER();
	CalWorker* self = (CalWorker*)data;

	self->__job();

	WLEAVE();
	return NULL;
}
