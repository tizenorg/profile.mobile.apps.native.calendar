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

#include "CalEvent.h"
#include "CalCommon.h"

CalEvent::CalEvent() :
	type(NONE),
	source(LOCAL)
{
}

CalEvent::~CalEvent()
{
}

CalEvent::CalEvent(Type _type, Source _source) :
	type(_type),
	source(_source)
{
}

CalEvent::CalEvent(const CalEvent& obj)
{
	type = obj.type;
	source = obj.source;
}

const CalEvent& CalEvent::operator=(const CalEvent& obj)
{
	if (this != &obj)
	{
		type = obj.type;
		source = obj.source;
	}

	return *this;
}
