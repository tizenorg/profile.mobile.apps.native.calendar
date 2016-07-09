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

#ifndef _CAL_ALERT_DATA_H_
#define _CAL_ALERT_DATA_H_

#include <vector>
#include "WApp.h"
#include "CalAlertNotificationItem.h"

class CalAlertData
{
public:
	CalAlertData(app_control_h request);
	CalAlertData();
	virtual ~CalAlertData();
	CalAlertData(const CalAlertData&);
	const CalAlertData& operator=(const CalAlertData&);

public:
	// get value
	int getTick(void);
	int getUnit(void);
	int getCount(void);
	std::shared_ptr<CalAlertNotificationItem> getAt(int nth);
	bool isSnoozed(int nth);

	// remove
	void remove(int nth);
	bool removeById(int id);
	void clear();

	// set
	void add(const CalAlertData&);
	void replace(const CalAlertData&);
private:
	void __addRecord(int recordIndex, bool isSnoozed = false);
private:
	// see alarm-data.h
	int __tick;
	int __unit;
	std::vector<std::shared_ptr<CalAlertNotificationItem> > __alerts;
};

#endif
