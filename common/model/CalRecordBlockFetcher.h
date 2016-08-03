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

#ifndef _CAL_RECORD_BLOCK_FETCHER_H_
#define _CAL_RECORD_BLOCK_FETCHER_H_

#include <calendar.h>
#include "CalCommon.h"

/**
 * @brief The CalRecordBlockFetcher class is a wrapper
 *        for a calendar_list_h.
 */
class CalRecordBlockFetcher
{
public:
	CalRecordBlockFetcher(calendar_query_h query, bool fetchAllAtOnce = false);
	virtual ~CalRecordBlockFetcher();

	/**
	 * @brief Prefetch events from calendar service.
	 *
	 * @param fillBothBuffers       prefetch current buffer or current and next
	 *
	 */
	void prefetch(bool fillBothBuffers);

	/**
	 * @brief Get next event.
	 *
	 * @param[in] autoFetch     fetch next block automatically if true
	 *
	 * @return next event record.
	 *
	 */
	calendar_record_h getNext(bool autoFetch = false);

	/**
	 * @brief Check whether block is ended.
	 *
	 */
	bool eof();

private:
	void __fetch(calendar_list_h& batch);

	const calendar_query_h __query;
	const bool __fetchAllAtOnce;
	calendar_list_h __currentBlock;
	calendar_list_h __nextBlock;
	int __offset;
	bool __noMoreToFetch;
	bool __noMoreToGive;
};

#endif
