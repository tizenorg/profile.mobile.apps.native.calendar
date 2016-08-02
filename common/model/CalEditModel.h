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

#ifndef _CAL_EDIT_MODEL_H_
#define _CAL_EDIT_MODEL_H_

#include <memory>
#include "CalSchedule.h"

/**
 * @brief The CalEditModel class represents default visibility of event fields.
 *
 */
class WAPP_ASSIST_EXPORT CalEditModel
{
public:

	/**
	 * @brief Field type.
	 *
	 */
	enum CalEditExpandField
	{
		LOCATION,
		REMINDER,
		REPEAT,
		DESCRIPTION,
		TIME_ZONE
	};

	CalEditModel();
	virtual ~CalEditModel();

	/**
	 * @brief Check field default visibility.
	 *
	 * @param field    field to check
	 *
	 * @return true if visible, false otherwise.
	 */
	bool isOn(const CalEditExpandField field);

	/**
	 * @brief Set field default visibility.
	 *
	 * @param field    field to set
	 * @param isOn     visibility
	 *
	 */
	void setOn(const CalEditExpandField field, bool isOn);

private:
	WDISABLE_COPY_AND_ASSIGN(CalEditModel);

	int __getInt(const char *key);
	void __setInt(const char *key, int value);
	const char* __getKey(CalEditExpandField field);
};

#endif
