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

#ifndef _CAL_LIST_MODEL_FACTORY_H_
#define _CAL_LIST_MODEL_FACTORY_H_

#include "CalCommon.h"
#include "CalSingleton.h"
#include "ICalListModel.h"
#include "CalDate.h"
#include "CalWorker.h"

/**
 * @brief The CalListModelFactory class is a factory for list model.
 *
 */

class WAPP_ASSIST_EXPORT CalListModelFactory
{

	SINGLETON_IDIOM(CalListModelFactory);

public:

	/**
	 * @brief Create list model.
	 *
	 * @param base           focus date
	 * @param dir            fetching direction
	 * @param ignoreWorker   wait for async or not.
	 *
	 * @return list model.
	 *
	 */
	ICalListModel* getList(const CalDate& base, int dir, bool ignoreWorker = false);

	/**
	 * @brief Create search list model.
	 *
	 * @param base           focus date
	 * @param dir            fetching direction
	 * @param keyword        keyword for searching
	 *
	 * @return list model.
	 *
	 */
	ICalListModel* getSearchList(const CalDate& base, int dir, const char* keyword);

	/**
	 * @brief Create delete list model.
	 *
	 * @param base           focus date
	 * @param dir            fetching direction
	 *
	 * @return list model.
	 *
	 */
	ICalListModel* getDeleteList(const CalDate& base, int dir);

	/**
	 * @brief Create range list model.
	 *
	 * @param from            start date of the range
	 * @param to              end date of the range
	 * @param ignoreWorker    wait for async or not
	 *
	 * @return list model.
	 *
	 */
	ICalListModel* getRangeList(const CalDate& from, const CalDate& to, bool ignoreWorker = false);

	/**
	 * @brief Prepare async job.
	 *
	 */
	void prepare();

private:
	WDISABLE_COPY_AND_ASSIGN(CalListModelFactory);

	CalListModelFactory();

	int __getFirstDayOfWeek();
	virtual ~CalListModelFactory();

	CalWorker* __worker;
	ICalListModel* __forwardModel;
	ICalListModel* __backwardModel;
	ICalListModel* __rangeModel;
};

#endif
