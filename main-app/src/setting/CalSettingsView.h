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

#ifndef _CAL_SETTINGS_VIEW_H_
#define _CAL_SETTINGS_VIEW_H_

#include <string>
#include "CalView.h"
#include "CalSchedule.h"
#include "CalDialogControl.h"
#include "CalDialogSettingsTwoTextItem.h"
#include "CalDialogSettingsOneTextOneIconItem.h"

/**
 * @brief Calendar settings view
 */
class CalSettingsView  : public CalView
{
public:
	/**
	 * @brief Create calendar settings view
	 */
	CalSettingsView();

	/**
	 * @brief Destructor
	 */
	virtual ~CalSettingsView();

private:
	WDISABLE_COPY_AND_ASSIGN(CalSettingsView);

private:

	/**
	 * @brief Setting select type
	 */
	enum SettingSelectType
	{
		SETTING_FIRST_DAY_OF_WEEK_SELECT = 0,
		SETTING_LOCK_TIME_ZONE_SELECT,
		SETTING_TIME_ZONE_SELECT,
		SETTING_NOTIF_SOUND_SELECT
	};

	/**
	 * Executes the create action.
	 *
	 * @param [in]	parent		If non-null, the parent.
	 * @param [in]	viewParam	If non-null, the view parameter.
	 *
	 * @return	null if it fails, else an Evas_Object*.
	 */
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);

	/**
	 * Executes the pushed action.
	 *
	 * @param [in]	naviItem	If non-null, the navi item.
	 */
	virtual void onPushed(Elm_Object_Item* naviItem);

	/**
	 * Executes the destroy action.
	 */
	virtual void onDestroy();

	/**
	 * Executes the select action.
	 *
	 * @param	settingType	Type of the setting.
	 */
	void onSelect(SettingSelectType settingType);

	/**
	 * Executes the create firstday popup action.
	 */
	void onCreateFirstdayPopup(void);

	/**
	 * Executes the lock time zone action.
	 */
	void onLockTimeZone(void);

	/**
	 * Executes the time zone action.
	 */
	void onTimeZone(void);

	/**
	 * Executes the notification sound action.
	 */
	void onNotificationSound(void);

	/**
	 * Radio on lock time zone.
	 */
	void radioOnLockTimeZone(void);

	/**
	 * Callback, called when the dialog delete.
	 *
	 * @param [in]	data		If non-null, the data.
	 * @param [in]	e			If non-null, the Evas to process.
	 * @param [in]	obj			If non-null, the object.
	 * @param [in]	event_info	If non-null, information describing the event.
	 */
	static void __dialogDeleteCallback(void *data, Evas *e, Evas_Object *obj, void *event_info);

	/**
	 * Gets alert display text.
	 *
	 * @param		ringtone		The ringtone.
	 * @param [in]	displayText		The display text.
	 */
	void __getAlertDisplayText(const char* ringtone, std::string& displayText);

	/**
	 * Executes the event action.
	 *
	 * @param	event	The event.
	 */
	virtual void onEvent(const CalEvent& event);

	/**
	 * Updates this object.
	 */
	void __update();

private:
	const std::shared_ptr<CalSchedule> __schedule;

	CalDialogControl::Item *__first_day_week_item;
	CalDialogSettingsOneTextOneIconItem*__lock_time_zone_item;
	CalDialogSettingsTwoTextItem*__time_zone_item;
	CalDialogSettingsTwoTextItem*__noti_sound_item;
	CalDialogControl *__dialog;
};

#endif /* CalSettingsView_H_ */
