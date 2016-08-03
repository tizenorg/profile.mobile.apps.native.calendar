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

#ifndef _WAPP_EVENT_HANDLER_H_
#define _WAPP_EVENT_HANDLER_H_

#include <functional>
#include <app.h>
#include "WDefine.h"

/**
* @class WAppEventHandler.
*
* @brief Application event handler.
*/
class WAPP_ASSIST_EXPORT WAppEventHandler
{
public:
	/**
	 * @brief This is the default constructor for this class.
	 */
	WAppEventHandler();

	/**
	 * @brief This is the destructor for this class.
	 */
	virtual ~WAppEventHandler();

	/**
	 * @brief Add application event handler of event type.
	 *
	 * @param[in]   eventType     Event type to be handled.
	 * @param[in]   handlerFunc   The pointer to handler function.
	 * @param[in]   userData      User data pointer to be used in handler function.
	 *
	 * @return 0 on success, otherwise error.
	 */
	int addEventHandler( app_event_type_e eventType, const std::function<void (app_event_info_h eventInfo, void* userData)>& handlerFunc, void* userData );

	/**
	 * @brief Remove application event handler.
	 */
	void removeEventHandler();

protected:
	/**
	 * @brief Return the handle of added event handler.
	 *
	 * @return The handle.
	 *
	 * @see addEventHandler().
	 */
	app_event_handler_h getHandle();

private:
	WDECLARE_PRIVATE_IMPL(WAppEventHandler);
	WDISABLE_COPY_AND_ASSIGN(WAppEventHandler);

};

#endif /* _WAPP_EVENT_HANDLER_H_ */
