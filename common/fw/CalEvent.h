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

#ifndef _CAL_EVENT_H_
#define _CAL_EVENT_H_

#include "WDefine.h"

class WAPP_ASSIST_EXPORT CalEvent
{
public:
	enum Type
	{
		NONE = 0,
		DB_CHANGED,
		BOOK_DB_CHANGED,
		TIME_CHANGED,
		LANGUAGE_CHANGED,
		SETTING_CHANGED,
		FIRST_DAY_OF_WEEK_CHANGED,
		CONTACTS_EMAIL_CHANGED,
		APP_PAUSED,
		APP_RESUMED,
		TYPE_MAX,
	} type;
	enum Source
	{
		LOCAL = 0,
		REMOTE,
		SOURCE_MAX,
	} source;
public:
	CalEvent();
	CalEvent(Type _type, Source _source);
	virtual ~CalEvent();
	CalEvent(const CalEvent& obj);
	const CalEvent& operator=(const CalEvent& obj);
};

#endif /* _CAL_EVENT_H_ */
