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

class WAPP_ASSIST_EXPORT CalDataManager
{
SINGLETON_IDIOM(CalDataManager);
public:
	enum OperationMode {
		ONLY_THIS = 0,
		THIS_AND_FUTURE,
		ALL,
	};
	int insertSchedule(const CalSchedule& schedule, int* newId = NULL);
	int updateSchedule(OperationMode mode, const CalSchedule& inputSchedule, CalSchedule& workingCopy, int* newId = NULL);
	int deleteSchedule(const CalSchedule& inputSchedule);
	int deleteSchedule(OperationMode mode, const CalSchedule& inputSchedule);
	std::shared_ptr<CalSchedule> getUpdatedWorkingCopy(OperationMode mode, const CalSchedule& inputSchedule);
	std::shared_ptr<CalSchedule> getWorkingCopy(OperationMode mode, const CalSchedule& inputSchedule);
	std::shared_ptr<CalSchedule> getWorkingCopyForForward(OperationMode mode, const CalSchedule& inputSchedule);
	void getSchedulesFromVcs(const char* filePath, std::list<std::shared_ptr<CalSchedule>>& inputSchedules);
	void generateVcsFromSchedule(const CalSchedule& schedule, const char* filePath, bool isOriginalSchedule = false);
	char* generateTextFromSchedule(const CalSchedule& schedule);
	std::shared_ptr<CalSchedule> getSchedule(const int eventIndex);
	std::shared_ptr<CalSchedule> getInstanceSchedule(const int eventIndex, const CalDateTime& dateTime);
	bool isOverlapped(const CalSchedule& schedule);
protected:
	CalDataManager();
	virtual ~CalDataManager();
private:
	WDISABLE_COPY_AND_ASSIGN(CalDataManager);
private:
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
private:
	void __notify(CalEvent::Source source);
	void __notifyAlert(int recordIndex);
	static void __onChanged(const char* uri, void* userData);
private:
	static std::shared_ptr<CalOriginalSchedule> __getOriginalSchedule(const CalSchedule& instance);
	static void __deleteExceptionRecordsOnAndAfter(const CalOriginalSchedule& originalSchedule, const CalDateTime& inputInstanceStartTime);
	static void __deleteAllExceptionRecords(const CalOriginalSchedule& originalSchedule);
private:
	int __localVersion;
	int __version;
};

#endif
