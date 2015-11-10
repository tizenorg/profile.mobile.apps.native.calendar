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

#include "CalEventManager.h"
#include "CalCommon.h"

#define CAL_TIMEOUT_MAX             8 // 8 sec

SINGLETON_INSTANCE(CalEventManager);

CalEventManager::CalEventManager() :
	__timerForPeindingEvents(NULL),
	__timeout(0),
	__timeoutMax(CAL_TIMEOUT_MAX),
	__pendingEvents(false),
	__isSuspended(false)
{
	WENTER();
}

CalEventManager::~CalEventManager()
{
	WENTER();
	__removeTimerForPendingEvents();
}

void CalEventManager::init()
{
	WENTER();
	__timeoutMax = CAL_TIMEOUT_MAX;
}

void CalEventManager::attach(ICalEventListener* listener)
{
	WENTER();
	WDEBUG("%p", listener);
	ListenerNode* node = new ListenerNode;
	node->isBlocked = false;
	node->listener = listener;
	__resetEventTable(node);
	__listenerMap.insert(std::pair<ICalEventListener*, ListenerNode*>(listener, node));
}

void CalEventManager::detach(ICalEventListener* listener)
{
	WENTER();
	WDEBUG("%p", listener);
	auto finder = __listenerMap.find(listener);
	if (finder == __listenerMap.end())
	{
		WDEBUG("null");
		return;
	}

	auto node = finder->second;
	__clearEventTable(node);
	delete node;

	__listenerMap.erase(listener);
}

void CalEventManager::block(ICalEventListener* listener)
{
	WENTER();
	WDEBUG("%p", listener);
	auto finder = __listenerMap.find(listener);
	if (finder == __listenerMap.end())
	{
		WDEBUG("null");
		return;
	}
	auto node = finder->second;
	node->isBlocked = true;
}

void CalEventManager::unblock(ICalEventListener* listener)
{
	WENTER();
	WDEBUG("%p", listener);
	auto finder = __listenerMap.find(listener);
	if (finder == __listenerMap.end())
	{
		WDEBUG("null");
		return;
	}
	auto node = finder->second;
	node->isBlocked = false;
	__notifyFromEventTable(node);
}

void CalEventManager::notify(const CalEvent& event)
{
	WENTER();
	if (__isSuspended)
	{
		for (auto it = __listenerMap.begin(); it != __listenerMap.end();)
		{
			auto node = it->second;
			it++;
			__addtoEventTable(node, event);
		}

		return;
	}

	for (auto it = __listenerMap.begin(); it != __listenerMap.end(); ++it)
	{
		auto node = it->second;
		if (node->isBlocked)
		{
			WDEBUG("node->isBlocked");
			__addtoEventTable(node, event);
		}
		else
		{
			if (event.source == CalEvent::REMOTE)
			{
				int timeout = __getTimeoutForTimerForPendingEvents();
				WDEBUG("an event from a remote one, timeout = %d", timeout);
				if (timeout == 0)
				{
					node->listener->onEvent(event);
				}
				else
				{
					__addtoEventTable(node, event);
				}
			}
			else
			{
				node->listener->onEvent(event);
			}
		}
	}

	if (event.source == CalEvent::REMOTE)
	{
		__addTimerForPendingEvents();
	}

	WLEAVE();
}

void CalEventManager::clear()
{
	for (auto it = __listenerMap.begin(); it != __listenerMap.end(); ++it)
	{
		auto node = it->second;
		auto first = it->first;

		__clearEventTable(node);
		__listenerMap.erase(first);
	}
}

void CalEventManager::__resetEventTable(ListenerNode* node)
{
	// reset the event table
	for (int i = 1; i < CalEvent::TYPE_MAX; i++)
	{
		for (int j = 0; j < CalEvent::SOURCE_MAX; j++)
		{
			node->eventTable[i][j] = NULL;
		}
	}
}

void CalEventManager::__addtoEventTable(ListenerNode* node, const CalEvent& event)
{
	// add it to the event table
	if (node->eventTable[event.type][event.source])
	{
		delete node->eventTable[event.type][event.source];
	}

	node->eventTable[event.type][event.source] = new CalEvent(event);
}

void CalEventManager::__notifyFromEventTable(ListenerNode* node)
{
	// notify events from the event table
	for (int i = 1; i < CalEvent::TYPE_MAX; i++)
	{
		for (int j = 0; j < CalEvent::SOURCE_MAX; j++)
		{
			if (node->eventTable[i][j])
			{
				ICalEventListener* listener = node->listener;
				node->listener->onEvent(*(node->eventTable[i][j]));

				auto finder = __listenerMap.find(listener);
				if (finder == __listenerMap.end())
				{
					WDEBUG("null");
					return;
				}

				delete node->eventTable[i][j];
				node->eventTable[i][j] = NULL;
			}
		}
	}
}

void CalEventManager::__notifyFromEventTable(ListenerNode* node, CalEvent::Source source)
{
	// notify events from the event table
	for (int i = 1; i < CalEvent::TYPE_MAX; i++)
	{
		if (node->eventTable[i][source])
		{
			ICalEventListener* listener = node->listener;
			node->listener->onEvent(*(node->eventTable[i][source]));

			auto finder = __listenerMap.find(listener);
			if (finder == __listenerMap.end())
			{
				WDEBUG("null");
				return;
			}

			delete node->eventTable[i][source];
			node->eventTable[i][source] = NULL;
		}
	}
}

void CalEventManager::__clearEventTable(ListenerNode* node)
{
	// clear the event table
	for (int i = 1; i < CalEvent::TYPE_MAX; i++)
	{
		for (int j = 0; j < CalEvent::SOURCE_MAX; j++)
		{
			if (node->eventTable[i][j])
			{
				delete node->eventTable[i][j];
				node->eventTable[i][j] = NULL;
			}
		}
	}
}

void CalEventManager::__processTimerForPendingEvents()
{
	WENTER();
	for (auto it = __listenerMap.begin(); it != __listenerMap.end(); ++it)
	{
		auto node = it->second;
		if (node->isBlocked)
		{
			WDEBUG("node->isBlocked");
			continue;
		}
		else
		{
			__notifyFromEventTable(node, CalEvent::REMOTE);
		}
	}

	__timerForPeindingEvents = NULL;

	if (__pendingEvents)
	{
		__addTimerForPendingEvents();
	}
	else
	{
		WDEBUG("expired");
		__resetTimeoutForTimerForPendingEvents();
	}
}

void CalEventManager::__addTimerForPendingEvents()
{
	WENTER();
	if (__timerForPeindingEvents)
	{
		//a request before timed out, just set the flag
		__pendingEvents = true;
		return;
	}

	__updateTimeoutForTimerForPendingEvents();
	int timeout = __getTimeoutForTimerForPendingEvents();
	WDEBUG("timeout = %d", timeout);
	__timerForPeindingEvents = ecore_timer_add(timeout, [](void* data) {
			CalEventManager* self = (CalEventManager*)data;
			self->__processTimerForPendingEvents();
			return ECORE_CALLBACK_CANCEL;
		} , this);
}

void CalEventManager::__removeTimerForPendingEvents()
{
	WENTER();
	if (__timerForPeindingEvents)
	{
		ecore_timer_del(__timerForPeindingEvents);
	}

	__timerForPeindingEvents = NULL;
	__resetTimeoutForTimerForPendingEvents();
}

int CalEventManager::__getTimeoutForTimerForPendingEvents()
{
	WENTER();
	// get the timeout for update
	return __timeout;
}

void CalEventManager::__updateTimeoutForTimerForPendingEvents()
{
	WENTER();
	// check if there is any request before timed out, if it's true, update the next timeout for update (x2)
	if (__timeout == 0)
	{
		__timeout = 2;
	}
	else
	{
		if (__timeout >= __timeoutMax)
		{
			__timeout = __timeoutMax;
		}
		else
		{
			__timeout *= 2;
		}
	}
	WDEBUG("The next timeout = %d", __timeout);
	__pendingEvents = false;
}

void CalEventManager::__resetTimeoutForTimerForPendingEvents()
{
	WENTER();
	// reset the timeout and the pending flag
	WDEBUG("timeout(%d) will be reset", __timeout);
	__timeout = 0;
	__pendingEvents = false;
}

void CalEventManager::suspend()
{
	WENTER();
	__isSuspended = true;

	__removeTimerForPendingEvents();
}

void CalEventManager::resume()
{
	WENTER();
	__isSuspended = false;

	for (auto it = __listenerMap.begin(); it != __listenerMap.end();)
	{
		auto node = it->second;
		it++;
		if (node->isBlocked)
		{
			WDEBUG("node->isBlocked");
			continue;
		}
		else
		{
			__notifyFromEventTable(node);
		}
	}
}

bool CalEventManager::isDetached(ICalEventListener* listener)
{
	WENTER();
	auto finder = __listenerMap.find(listener);
	if (finder == __listenerMap.end())
	{
		return true;
	}

	return false;
}
