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

#include "CalListModelFactory.h"

#include "CalListModel.h"
#include "CalListModel2.h"

#include "CalSimpleListProvider.h"
#include "CalComplexListProvider.h"

#include "CalSettingsManager.h"
#include "CalLocaleManager.h"

#ifndef CAL_USE_MULTITHREADING
#define CAL_USE_MULTITHREADING 0
#endif

SINGLETON_INSTANCE(CalListModelFactory);

CalListModelFactory::CalListModelFactory() :
	__worker(NULL),
	__forwardModel(NULL),
	__backwardModel(NULL),
	__rangeModel(NULL)
{
}

CalListModelFactory::~CalListModelFactory()
{
	if (__worker) {
		__worker->wait();
		delete __worker;
	}
	if (__forwardModel)
		delete __forwardModel;
	if (__backwardModel)
		delete __backwardModel;
	if (__rangeModel)
		delete __rangeModel;
}

void CalListModelFactory::init()
{
}

#define GRID_ROW_COUNT 6
#define DAYS_PER_WEEK 7
void CalListModelFactory::prepare()
{
	if (__worker)
		return;


	auto job = [this]() {
			const CalDate today;
			__forwardModel = getList(today, 1, true);
			__forwardModel->prefetch(false);
//			__backwardModel = getList(today, -1, true);
//			__backwardModel->prefetch(false);
			CalDate gridStartDate(today.getYear(), today.getMonth(), 1);
			gridStartDate.setToMonthGridStart(__getFirstDayOfWeek());
			CalDate gridEndDate(gridStartDate);
			gridEndDate.addDays(GRID_ROW_COUNT * DAYS_PER_WEEK);
			__rangeModel = getRangeList(gridStartDate, gridEndDate, true);
			__rangeModel->prefetch(false);
		};

#if CAL_USE_MULTITHREADING
		__worker = new CalWorker(job);
#else
		job();
#endif
}

int CalListModelFactory::__getFirstDayOfWeek()
{
	int result = CalSettingsManager::getInstance().getFirstDayOfWeek();
	if (result == CalSettingsManager::LOCALES)
		return CalLocaleManager::getInstance().getLocaleFirstDayOfWeek();

	return result;
}

ICalListModel* CalListModelFactory::getList(const CalDate& base, int dir, bool ignoreWorker)
{
	WDEBUG("%d", dir);

	if (!ignoreWorker) {
		if (__worker) {
			__worker->wait();
			delete __worker;
			__worker = NULL;
		}
	}
	if (dir == 1 && __forwardModel) {
		ICalListModel* listModel = __forwardModel;
		__forwardModel = NULL;
		return listModel;
	} else if (dir == -1 && __backwardModel) {
		ICalListModel* listModel = __backwardModel;
		__backwardModel = NULL;
		return listModel;
	}

	WDEBUG("%s", base.dump(true).c_str());

	CalListModel2* listModel = new CalListModel2(base, dir);

	calendar_filter_h filterAllDay = CalInstanceSchedule::createListFilter(true, true, false, NULL);
	calendar_filter_h filterNormal = CalInstanceSchedule::createListFilter(false, true, false, NULL);

	listModel->__providers.push_back(new CalComplexListProvider(base, dir, true, filterAllDay));
	listModel->__providers.push_back(new CalComplexListProvider(base, dir, false, filterNormal));

	return listModel;
}

ICalListModel* CalListModelFactory::getSearchList(const CalDate& base, int dir, const char* keyword)
{
	WDEBUG("%d, %s", dir, keyword);

	CalListModel* listModel = new CalListModel(base, dir);

	calendar_filter_h filterAllDay = CalInstanceSchedule::createListFilter(true, true, false, keyword);
	calendar_filter_h filterNormal = CalInstanceSchedule::createListFilter(false, true, false, keyword);

	listModel->__providers.push_back(new CalSimpleListProvider(base, dir, true, filterAllDay));
	listModel->__providers.push_back(new CalSimpleListProvider(base, dir, false, filterNormal));

	return listModel;
}

ICalListModel* CalListModelFactory::getDeleteList(const CalDate& base, int dir)
{
	WDEBUG("%d", dir);

	CalListModel* listModel = new CalListModel(base, dir);

	calendar_filter_h filterAllDay = CalInstanceSchedule::createListFilter(true, true, false, NULL);
	calendar_filter_h filterNormal = CalInstanceSchedule::createListFilter(false, true, false, NULL);

	listModel->__providers.push_back(new CalSimpleListProvider(base, dir, true, filterAllDay));
	listModel->__providers.push_back(new CalSimpleListProvider(base, dir, false, filterNormal));

	return listModel;
}

ICalListModel* CalListModelFactory::getRangeList(const CalDate& from, const CalDate& to, bool ignoreWorker)
{
	WDEBUG("");

	if (!ignoreWorker) {
		if (__worker) {
			__worker->wait();
			delete __worker;
			__worker = NULL;
		}
	}
	if (__rangeModel) {
		ICalListModel* listModel = __rangeModel;
		__rangeModel = NULL;
		return listModel;
	}

	CalListModel* listModel = new CalListModel(from, 1, false);

	calendar_filter_h filterAllDay = CalInstanceSchedule::createListFilter(true, true, false, NULL);
	calendar_filter_h filterNormal = CalInstanceSchedule::createListFilter(false, true, false, NULL);

	listModel->__providers.push_back(new CalSimpleListProvider(from, to, true, filterAllDay));
	listModel->__providers.push_back(new CalSimpleListProvider(from, to, false, filterNormal));

	return listModel;
}
