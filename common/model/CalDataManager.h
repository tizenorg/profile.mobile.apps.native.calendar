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

#ifndef _CAL_DATA_MANAGER_H_
#define _CAL_DATA_MANAGER_H_

#include <memory>
#include <list>
#include "WDefine.h"
#include "CalSingleton.h"
#include "CalSchedule.h"
#include "CalOriginalSchedule.h"
#include "CalEvent.h"

/**
* @class     CalDataManager
*
* @brief    This class manage changings of %CalSchedule and translate
*           them to calendar service DB.
*
*/
class WAPP_ASSIST_EXPORT CalDataManager
{
SINGLETON_IDIOM(CalDataManager);

public:

	/**
	 * @brief OperationMode indicate witch events will bechanged/deleted.
	 */
	enum OperationMode
	{
		ONLY_THIS = 0,
		THIS_AND_FUTURE,
		ALL,
	};

	/**
	 * @brief Insert event to DB.
	 *
	 * @param[in]  schedule      new event to insert
	 * @param[out] newId         ID of newly inserted event
	 *
	 * @return operation result.
	 *
	 */
	int insertSchedule(const CalSchedule& schedule, int* newId = NULL);

	/**
	 * @brief Update event on DB.
	 *
	 * @param[in]  mode           operation mode
	 * @param[in]  inputSchedule  destination event
	 * @param[in]  workingCopy    source event
	 * @param[out] newId          ID of newly inserted event
	 *
	 * @return operation result.
	 *
	 */
	int updateSchedule(OperationMode mode, const CalSchedule& inputSchedule, CalSchedule& workingCopy, int* newId = NULL);

	/**
	 * @brief Delete event from DB.
	 *
	 * @param[in]  inputSchedule     event to delete
	 *
	 * @return operation result.
	 *
	 */
	int deleteSchedule(const CalSchedule& inputSchedule);

	/**
	 * @brief Delete event from DB with operation mode.
	 *
	 * @param[in]  mode              operation mode
	 * @param[in]  inputSchedule     event to delete
	 *
	 * @return operation result.
	 *
	 */
	int deleteSchedule(OperationMode mode, const CalSchedule& inputSchedule);

	/**
	 * @brief Get updated working copy of event.
	 *
	 * @param[in]  mode              operation mode
	 * @param[in]  inputSchedule     event to update
	 *
	 * @return updated working copy of event.
	 */
	std::shared_ptr<CalSchedule> getUpdatedWorkingCopy(OperationMode mode, const CalSchedule& inputSchedule);

	/**
	 * @brief Get working copy of event.
	 *
	 * @param[in]  mode              operation mode
	 * @param[in]  inputSchedule     original event
	 *
	 * @return working copy of event.
	 *
	 */
	std::shared_ptr<CalSchedule> getWorkingCopy(OperationMode mode, const CalSchedule& inputSchedule);

	/**
	 * @brief Get working copy of this or this and future events.
	 *
	 * @param[in]  mode              operation mode
	 * @param[in]  inputSchedule     original event
	 *
	 * @return working copy of event.
	 *
	 */
	std::shared_ptr<CalSchedule> getWorkingCopyForForward(OperationMode mode, const CalSchedule& inputSchedule);

	/**
	 * @brief Import events from VCS file.
	 *
	 * @param[in]  filePath         path to file
	 * @param[out] inputSchedules   list of events
	 *
	 */
	void getSchedulesFromVcs(const char* filePath, std::list<std::shared_ptr<CalSchedule>>& inputSchedules);

	/**
	 * @brief Generate VCS from given event.
	 *
	 * @param[in] inputSchedules        list of events
	 * @param[in] filePath              path to file
	 * @param[in] isOriginalSchedule    is instance in list are original
	 *
	 */
	void generateVcsFromSchedule(const CalSchedule& schedule, const char* filePath, bool isOriginalSchedule = false);

	/**
	 * @brief Generate text from given event.
	 *
	 * @param[in]  schedule     event to generation
	 *
	 * @return generated text.
	 *
	 */
	char* generateTextFromSchedule(const CalSchedule& schedule);

	/**
	 * @brief Get event by ID.
	 *
	 * @param[in]  eventIndex     event ID
	 *
	 * @return event.
	 *
	 */
	std::shared_ptr<CalSchedule> getSchedule(const int eventIndex);

	/**
	 * @brief Get event instance by ID and date.
	 *
	 * @param[in]  eventIndex     event ID
	 * @param[in]  dateTime       event instance date
	 *
	 * @return event instance.
	 *
	 */
	std::shared_ptr<CalSchedule> getInstanceSchedule(const int eventIndex, const CalDateTime& dateTime);

	/**
	 * @brief Check whetheer event is overlapped.
	 *
	 * @param schedule        event
	 *
	 * @return true if overlapped, otherwise false.
	 */
	bool isOverlapped(const CalSchedule& schedule);

protected:
	CalDataManager();
	virtual ~CalDataManager();

private:
	WDISABLE_COPY_AND_ASSIGN(CalDataManager);

	void __updateSchedule(CalOriginalSchedule& workingCopy);
	void __updateOnlyThisInstance(const CalSchedule& inputSchedule, CalOriginalSchedule& workingCopy);
	void __updateThisAndFutureInstances(const CalSchedule& inputSchedule, CalOriginalSchedule& workingCopy, int* newId);
	void __updateAllInstances(const CalSchedule& inputSchedule, CalOriginalSchedule& workingCopy);

	int __deleteSchedule(const CalSchedule& inputSchedule);
	void __deleteOnlyThisInstance(const CalSchedule& inputSchedule);
	void __deleteThisAndFutureInstances(const CalSchedule& inputSchedule);
	void __deleteAllInstances(const CalSchedule& inputSchedule);

	std::shared_ptr<CalOriginalSchedule> __getWorkingInstanceCopy(const CalSchedule& inputSchedule);
	std::shared_ptr<CalOriginalSchedule> __getWorkingOriginalCopy(const CalSchedule& inputSchedule);
	void __setThisAndFutureRepeatCount(const CalSchedule& inputSchedule, CalOriginalSchedule& workingCopy);
	std::shared_ptr<CalOriginalSchedule> __getWorkingUpdatedCopy(const CalSchedule& inputSchedule);

	void __notify(CalEvent::Source source);
	void __notifyAlert();

	static void __onChanged(const char* uri, void* userData);
	static std::shared_ptr<CalOriginalSchedule> __getOriginalSchedule(const CalSchedule& instance);
	static void __deleteExceptionRecordsOnAndAfter(const CalOriginalSchedule& originalSchedule, const CalDateTime& inputInstanceStartTime);
	static void __deleteAllExceptionRecords(const CalOriginalSchedule& originalSchedule);

	int __localVersion;
	int __version;
};

#endif
