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

#include "CalListModel.h"
#include "CalInstanceSchedule.h"
#include "CalCommon.h"

CalListModel::CalListModel(const CalDate& base, int dir, bool enableFlowControl) :
	__dir(dir),
	__currentDate(base),
	__enableFlowControl(enableFlowControl)
{
}

CalListModel::~CalListModel()
{
	WENTER();
	for (auto it = __providers.begin(); it != __providers.end(); ++it)
		delete *it;
}

void CalListModel::prefetch(bool fillBothBuffers)
{
	for (auto it = __providers.begin(); it != __providers.end(); ++it)
		(*it)->prefetch(fillBothBuffers);
}

std::shared_ptr<CalSchedule> CalListModel::getNext(bool& dayChanged)
{
	WENTER();

	dayChanged = false;

	ICalListProvider* selectedProvider = NULL;

	if (__dir > 0) {

		CalDate minDate(CalDate::INIT_UPPER_BOUND);
		for (auto it = __providers.begin(); it != __providers.end(); ++it) {
			ICalListProvider* provider = *it;
			if (!provider->getCurrentSchedule())
				continue;
			const CalDate& date = provider->getCurrentDate();
			if (date < minDate) {
				minDate = date;
				selectedProvider = provider;
			}
		}

	} else {

		CalDate maxDate(CalDate::INIT_LOWER_BOUND);
		for (auto it = __providers.rbegin(); it != __providers.rend(); ++it) {
			ICalListProvider* provider = *it;
			if (!provider->getCurrentSchedule())
				continue;
			const CalDate& date = provider->getCurrentDate();
			if (date > maxDate) {
				maxDate = date;
				selectedProvider = provider;
			}
		}
	}

	if (selectedProvider == NULL)
		return __currentSchedule = nullptr;

	__currentSchedule = selectedProvider->getCurrentSchedule();
	if (__currentDate != selectedProvider->getCurrentDate()) {
		__currentDate = selectedProvider->getCurrentDate();
		dayChanged = true;
	}

	selectedProvider->loadNext();

	return __currentSchedule;
}

const CalDate& CalListModel::getCurrentDate()
{
	return __currentDate;
}

bool CalListModel::eof()
{
	for (auto it = __providers.begin(); it != __providers.end(); ++it)
		if (!(*it)->eof())
			return false;
	return true;
}
