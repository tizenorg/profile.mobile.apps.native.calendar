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

#ifndef _CAL_WORKER_H_
#define _CAL_WORKER_H_

#include <functional>
#include "WDefine.h"
#include <glib.h>

/**
 * @brief Calendar worker.
 */
class WAPP_ASSIST_EXPORT CalWorker {
public:
	/**
	 * @brief Create calendar worker.
	 *
	 * @param[in]   job   Worker job.
	 */
	CalWorker(std::function<void ()> job);

	virtual ~CalWorker();

	/**
	 * @brief Wait till ended running thread.
	 */
	void wait(void);

private:
	static gpointer __threadFunc(gpointer data);

	std::function<void ()> __job;
	GThread* __workerThread;
};

#endif /* _CAL_WORKER_H_ */
