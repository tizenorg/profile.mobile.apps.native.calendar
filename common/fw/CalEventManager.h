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

/**
 * @brief Calendar event manager.
 */
class WAPP_ASSIST_EXPORT CalEventManager
{
SINGLETON_IDIOM(CalEventManager);
public:
	/**
	 * @brief Attach event listener.
	 *
	 * @param[in]   listener   Event listener.
	 *
	 * @see ICalEventListener.
	 */
	void attach(ICalEventListener* listener);

	/**
	 * @brief Detach event listener.
	 *
	 * @param[in]   listener   Event listener.
	 *
	 * @see ICalEventListener.
	 */
	void detach(ICalEventListener* listener);

	/**
	 * @brief Get detach state.
	 *
	 * @param[in]   listener   Event listener.
	 *
	 * @return Whether the event listener is detached.
	 *
	 * @see ICalEventListener.
	 */
	bool isDetached(ICalEventListener* listener);

	/**
	 * @brief Block event listener.
	 *
	 * @param[in]   listener   Event listener.
	 *
	 * @see ICalEventListener.
	 */
	void block(ICalEventListener* listener);

	/**
	 * @brief Unblock event listener.
	 *
	 * @param[in]   listener   Event listener.
	 *
	 * @see ICalEventListener.
	 */
	void unblock(ICalEventListener* listener);

	/**
	 * @brief Notify event listener.
	 *
	 * @param[in]   event   Event.
	 *
	 * @see CalEvent.
	 */
	void notify(const CalEvent& event);

	/**
	 * @brief Clear all event listeners.
	 *
	 */
	void clear();

	/**
	 * @brief Suspend event.
	 *
	 */
	void suspend();

	/**
	 * @brief Resume event.
	 *
	 */
	void resume();

protected:
	CalEventManager();
	virtual ~CalEventManager();

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

	WDISABLE_COPY_AND_ASSIGN(CalEventManager);

	std::map<ICalEventListener*, ListenerNode*> __listenerMap;
	Ecore_Timer* __timerForPeindingEvents;
	int __timeout;
	int __timeoutMax;
	bool __pendingEvents;
	bool __isSuspended;
};

#endif /* _CAL_EVENT_MANAGER_H_ */
