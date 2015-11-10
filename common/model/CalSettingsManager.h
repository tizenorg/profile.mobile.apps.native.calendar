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

class WAPP_ASSIST_EXPORT CalSettingsManager
{
SINGLETON_IDIOM(CalSettingsManager);
protected:
	CalSettingsManager();
	virtual ~CalSettingsManager();
private:
	WDISABLE_COPY_AND_ASSIGN(CalSettingsManager);
public:
	enum FirstDayOfWeekType
	{
		SUN = 0,
		MON = 1,
		SAT = 6,
		LOCALES
	};
	enum AlertType
	{
		ALERT = 0,
		STATUS_BAR_NOTIFICATION,
		OFF
	};

	// get
	int getFirstDayOfWeek(void);
	bool getLockTimeZone(void);
	void getLockTimeZone(std::string& timezone);
	AlertType getAlertType(void);
	const char* getAlertSound(void);
	int getLastUsedCalendar(void);

	// set
	void setFirstDayOfWeek(const FirstDayOfWeekType type);
	void setLockTimeZone(const bool isOn);
	void setLockTimeZone(const std::string& timezone);
	void setAlertType(const AlertType type);
	void setAlertSound(const char* sound);
	void setLastUsedCalendar(const int bookId);

	// get date format
	bool isHour24(void);
	void setHour24(void);
	void getCalendarTimeZone(std::string& timezone);

	// update
	void updateRegion(void);
protected:
	void getDeviceTimeZone(std::string& timeZone);
private:
	static void __changedCb(const char *key, void *userData);
	void __setChangedCb(const char* key, CalEvent::Type type);
	void __notify(CalEvent::Type type, CalEvent::Source source);
	void __setHour24(void);
	void __setLocaleTimeZone(void);
	void __notifyTimeChanged(void);
	static void __systemSettingsChangeCb(system_settings_key_e key, void *user_data);
private:
	std::string __alertSound;
	bool __isHour24;

friend class CalSchedule;
};

#endif
