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

#ifndef _CAL_EVENT_MANAGER_H_
#define _CAL_EVENT_MANAGER_H_

#include <memory>
#include <map>
#include "WDefine.h"
#include "CalSingleton.h"
#include "CalEvent.h"
#include "ICalEventListener.h"
#include <Ecore.h>

class WAPP_ASSIST_EXPORT CalEventManager
{
SINGLETON_IDIOM(CalEventManager);
public:
	void attach(ICalEventListener* listener);
	void detach(ICalEventListener* listener);
	bool isDetached(ICalEventListener* listener);
	void block(ICalEventListener* listener);
	void unblock(ICalEventListener* listener);
	void notify(const CalEvent& event);
	void clear(); /*clear all listener*/
	void suspend();
	void resume();
protected:
	CalEventManager();
	virtual ~CalEventManager();
private:
	WDISABLE_COPY_AND_ASSIGN(CalEventManager);
private:
	struct ListenerNode
	{
		bool isBlocked;
		CalEvent* eventTable[CalEvent::TYPE_MAX][CalEvent::SOURCE_MAX];
		ICalEventListener* listener;
	};

	void __resetEventTable(ListenerNode* node);
	void __addtoEventTable(ListenerNode* node, const CalEvent& event);
	void __notifyFromEventTable(ListenerNode* node);
	void __notifyFromEventTable(ListenerNode* node, CalEvent::Source soruce);
	void __clearEventTable(ListenerNode* node);
	void __processTimerForPendingEvents();
	void __addTimerForPendingEvents();
	void __removeTimerForPendingEvents();
	int __getTimeoutForTimerForPendingEvents();
	void __updateTimeoutForTimerForPendingEvents();
	void __resetTimeoutForTimerForPendingEvents();
private:
	std::map<ICalEventListener*, ListenerNode*> __listenerMap;
	Ecore_Timer* __timerForPeindingEvents;
	int __timeout;
	int __timeoutMax;
	bool __pendingEvents;
	bool __isSuspended;
};

#endif /* _CAL_EVENT_MANAGER_H_ */
