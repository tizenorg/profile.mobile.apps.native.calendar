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

#ifndef _CAL_SETTINGS_MANAGER_H_
#define _CAL_SETTINGS_MANAGER_H_

#include <string>
#include "WDefine.h"
#include "CalSingleton.h"
#include "CalScheduleReminder.h"
#include "CalEvent.h"
#include <system_settings.h>


#define CAL_SETTING_NOTIFICATION_SILENT         "silent"
#define CAL_SETTING_NOTIFICATION_SOUND_DEFAULT  "default ringtone"

/**
 * @brief The CalSettingsManager class handle settings.
 *
 */
class WAPP_ASSIST_EXPORT CalSettingsManager
{
SINGLETON_IDIOM(CalSettingsManager);

public:

	/**
	 * @brief First day of week.
	 *
	 */
	enum FirstDayOfWeekType
	{
		SUN = 0, /*!< Sunday    */
		MON = 1, /*!< Monday    */
		SAT = 6, /*!< Saturday  */
		LOCALES  /*!< by locale */
	};

	/**
	 * @brief The type of alert.
	 *
	 */
	enum AlertType
	{
		ALERT = 0,                /*!< Alert view               */
		STATUS_BAR_NOTIFICATION,  /*!< Status bar notification  */
		OFF                       /*!< No alert                 */
	};

	/**
	 * @brief Get first day of week preference.
	 *
	 * @return number of first day.
	 *
	 */
	int getFirstDayOfWeek(void);

	/**
	 * @brief Check whether the time zone locked in Calendar settings.
	 *
	 * @return preference state.
	 *
	 */
	bool getLockTimeZone(void);

	/**
	 * @brief Get device/locked time zone.
	 *
	 * @param[out] timezone    text buffer.
	 *
	 */
	void getTimeZone(std::string& timezone);

	/**
	 * @brief Get current alert type.
	 *
	 * @return alert type, @see AlertType.
	 *
	 */
	AlertType getAlertType(void);

	/**
	 * @brief Get alert sound.
	 *
	 * @return alert sound.
	 *
	 */
	const char* getAlertSound(void);

	/**
	 * @brief Get last used Calendar.
	 *
	 * @return last used Calendar ID.
	 *
	 */
	int getLastUsedCalendar(void);

	/**
	 * @brief Set first day of week
	 *
	 * @param[in] type        first day of week, @see FirstDayOfWeekType.
	 *
	 */
	void setFirstDayOfWeek(const FirstDayOfWeekType type);

	/**
	 * @brief Lock time zone.
	 *
	 * @param[in] isOn        lock state
	 *
	 */
	void setLockTimeZone(const bool isOn);

	/**
	 * @brief Set time zone.
	 *
	 * @param[in] timezone       time zone
	 *
	 */
	void setTimeZone(const std::string &timezone);

	/**
	 * @brief Set alert type.
	 *
	 * @param[in] type       type of alert, @see AlertType.
	 *
	 */
	void setAlertType(const AlertType type);

	/**
	 * @brief Set alert sound.
	 *
	 * @param[in] sound        sound of alert.
	 *
	 */
	void setAlertSound(const char* sound);

	/**
	 * @brief Set last used Calendar
	 *
	 * @param[in] bookId           last used Calendar ID.
	 *
	 */
	void setLastUsedCalendar(const int bookId);

	/**
	 * @brief Check whether 24 hours format is used.
	 *
	 * @return true if used, otherwise false.
	 *
	 */
	bool isHour24(void);

	/**
	 * @brief Set 24 hours format.
	 *
	 */
	void setHour24(void);

	/**
	 * @brief Get Calendar application time zone.
	 *
	 * @param[out] timezone       text buffer
	 *
	 */
	void getCalendarTimeZone(std::string& timezone);

	/**
	 * @brief Update regional settings, use this function after settings change.
	 */
	void updateRegion(void);

protected:
	CalSettingsManager();
	virtual ~CalSettingsManager();

	void getDeviceTimeZone(std::string& timeZone);

private:
	WDISABLE_COPY_AND_ASSIGN(CalSettingsManager);

	static void __changedCb(const char *key, void *userData);
	void __setChangedCb(const char* key, CalEvent::Type type);
	void __notify(CalEvent::Type type, CalEvent::Source source);
	void __setHour24(void);
	void __setLocaleTimeZone(void);
	void __notifyTimeChanged(void);
	static void __systemSettingsChangeCb(system_settings_key_e key, void *user_data);

	std::string __alertSound;
	bool __isHour24;

	friend class CalSchedule;
};

#endif
