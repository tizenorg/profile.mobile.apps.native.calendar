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

#ifndef _CAL_EDIT_DIALOG_CONTROL_H_
#define _CAL_EDIT_DIALOG_CONTROL_H_

#include <set>

#include "CalDialogControl.h"

/**
 * @brief Edit dialog control
 *
 * @see CalDialogControl
 *
 */
class CalEditDialogControl : public CalDialogControl
{
public:
	/**
	 * @brief Constructor
	 */
	CalEditDialogControl();

	/**
	 * @brief Destructor
	 */
	virtual ~CalEditDialogControl();

	/**
	 * @brief Get class name
	 *
	 * @return Class name
	 */
	virtual const char* getClassName();
public:
	/**
	 * @brief Add item
	 *
	 * @param item Dialog control item
	 *
	 * @return Added item
	 */
	virtual Elm_Object_Item* add(CalDialogControl::Item* item);

	/**
	 * @brief Remove item
	 *
	 * @param item Item to remove
	 *
	 */
	void remove(CalDialogControl::Item* item);

	/**
	 * @brief Set null
	 *
	 * @param item Item to set null
	 */
	void setNull(CalDialogControl::Item* item);
private:
	WDISABLE_COPY_AND_ASSIGN(CalEditDialogControl);
private:
	std::set<int> __sortedSet;
};

#endif
