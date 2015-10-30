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

#ifndef _CAL_MIDNIGHT_NOTIFIER_H_
#define _CAL_MIDNIGHT_NOTIFIER_H_

#include <glib.h>
#include <functional>
#include "WDefine.h"
#include "CalSingleton.h"
#include "CalDate.h"
#include "CalEvent.h"
#include "ICalEventListener.h"

class WAPP_ASSIST_EXPORT CalMidnightNotifier : public ICalEventListener
{
SINGLETON_IDIOM(CalMidnightNotifier);
public:
	enum Type
	{
		NONE = 0,
		TIMEOUT,
		ALARM,
	} type;
	int turnOn(Type type);
	int turnOn(Type type, std::function<int (void*)> onMidnight, void* onMidnightData);
	int turnOn(Type type, const CalDate& date, std::function<int (void*)> onMidnight, void* onMidnightData);
	int turnOff();
	bool isOnMidnight(app_control_h request);
	void onMidnight();
protected:
	CalMidnightNotifier();
	virtual ~CalMidnightNotifier();
	virtual void onEvent(const CalEvent& event);
private:
	WDISABLE_COPY_AND_ASSIGN(CalMidnightNotifier);
private:
	int __setTimeout(const CalDate& date);
	int __setAlarm(const CalDate& date);
	int __unsetTimeout();
	int __unsetAlarm();
	static gboolean __onTimeout(gpointer data);
private:
	Type __type;
	std::function<int (void*)> __onMidnight;
	void* __onMidnightData;
	guint __timeout;
	int __alarmId;
	int __date;
};

#endif /* _CAL_MIDNIGHT_NOTIFIER_H_ */
