/*
 * Copyright (c) 2009-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef _CAL_EVENT_ITEM_H_
#define _CAL_EVENT_ITEM_H_

#include <Elementary.h>
#include <memory>

#include "CalListControl.h"
#include "CalSchedule.h"

/**
 * @brief Calendar event item.
 */
class CalEventItem
{
public:
	/**
	 * @brief Create event item.
	 *
	 * @param[in]   dayOfGroup          The day of group.
	 * @param[in]   schedule            The schedule.
	 * @param[in]   isCheckboxRequired  Is checkbox required or not.
	 * @param[in]   isCheckboxSelected  Is checkbox selected or not.
	 * @param[in]   searchText          The search text.
	 *
	 * @see CalDate
	 * @see CalSchedule
	 */
	CalEventItem(const CalDate &dayOfGroup, const std::shared_ptr<CalSchedule> schedule,
			bool isCheckboxRequired, bool isCheckboxSelected, const std::string &searchText = NULL);
	~CalEventItem();

	/**
	 * @brief Get title.
	 *
	 * @return title.
	 */
	const char *getTitle();

	/**
	 * @brief Get location.
	 *
	 * @return location.
	 */
	const char *getLocation();

	/**
	 * @brief Get date interval.
	 *
	 * @return date interval.
	 */
	const char *getDateInterval();

	/**
	 * @brief Query if checkbox is required or not.
	 *
	 * @return true if checkbox is required, false otherwise.
	 */
	bool isCheckboxRequired();

	/**
	 * @brief Query if checkbox is selected or not.
	 *
	 * @return true if checkbox is selected, false otherwise.
	 */
	bool isCheckboxSelected();

	/**
	 * @brief Set checkbox selected state.
	 *
	 * @param[in]   isChecked   True - checkbox should be selected, false otherwise.
	 */
	void setCheckboxSelected(bool isChecked);

	/**
	 * @brief Get on tap callback.
	 *
	 * @return on tap callback.
	 *
	 * @see setOnTapCallback()
	 */
	std::function<void (CalEventItem *item)> getOnTapCallback();

	/**
	 * @brief Set on tap callback.
	 *
	 * @param[in]   onTap   On tap callback.
	 *
	 * @see CalEventItem
	 */
	void setOnTapCallback(std::function<void (CalEventItem *item)> onTap);

	/**
	 * @brief Set checkbox object.
	 *
	 * @param[in]   checkbox    The checkbox object.
	 */
	void setCheckObject(Evas_Object *checkbox);

	/**
	 * @brief Get schedule.
	 *
	 * @return schedule.
	 *
	 * @see CalSchedule
	 */
	const std::shared_ptr<CalSchedule> getSchedule();

private:
	WDISABLE_COPY_AND_ASSIGN(CalEventItem);

	void getHilightText(std::string &hilightText);

	std::function<void (CalEventItem *item)> __onTranslateTapToGroup;
	std::shared_ptr<CalSchedule> __schedule;
	bool __isCheckboxRequired;
	bool __isCheckboxSelected;
	Evas_Object *__checkbox;
	std::string __title;
	std::string __location;
	std::string __searchText;
	std::string __dateText;
};

#endif /* _CAL_EVENT_ITEM_H_ */
