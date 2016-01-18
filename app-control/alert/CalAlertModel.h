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

#ifndef _CAL_ALERT_MODEL_H_
#define _CAL_ALERT_MODEL_H_

#include "WApp.h"
#include "CalAlertData.h"

class CalAlertModel
{
public:
	CalAlertModel(std::shared_ptr<CalAlertData> alertData);
	virtual ~CalAlertModel();
private:
	WDISABLE_COPY_AND_ASSIGN(CalAlertModel);
public:
	// manage alertData
	void addAlertData(const std::shared_ptr<CalAlertData>& alertData);
	void relaceAlertData(const std::shared_ptr<CalAlertData>& alertData);

	// get alertData
	int getCount(void);

	std::shared_ptr<CalAlertNotificationItem> getAt(int nth);

	void snoozeAll(void);
	void snooze(int nth);
	void dismissAll(void);
	void dismiss(int nth);
	void dismiss(std::vector<int> &nths);
	void remove(int nth);

	// lcd !
	void turnOnLcd(void);
	void turnOffLcd(void);
	static bool isScreenOn(void);

	// snooze time
	void setSnoozeMinute(const int min);
	int getSnoozeMinute(void);
private:
	void __snooze(std::shared_ptr<CalAlertNotificationItem> &item);
	void __dismiss(std::shared_ptr<CalAlertNotificationItem> &item);
private:
	std::shared_ptr<CalAlertData> __alertData;
};

#endif
