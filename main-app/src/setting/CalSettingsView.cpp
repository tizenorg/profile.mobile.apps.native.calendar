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

#include <memory>
#include <metadata_extractor.h>
#include <app_control.h>

#include <string.h>
#include "CalCommon.h"
#include "CalDataManager.h"
#include "CalDialogControl.h"
#include "CalDialogSimpleTitleItem.h"
#include "CalDialogSettingsTwoTextItem.h"
#include "CalDialogSettingsMultilSubItem.h"
#include "CalSettingsView.h"
#include "CalSettingsManager.h"
#include "CalDialogSettingsRadioItem.h"
#include "CalSettingFirstDayOfWeekPopup.h"
#include "CalLocaleManager.h"
#include "CalAppControlLauncher.h"


#define CAL_APPSETT_PARAM_MARKED_MODE "marked_mode"
#define CAL_APPSETT_PARAM_MARKED_MODE_DEFAULT "default"
#define CAL_APPSETT_PARAM_PATH "path"
#define CAL_APPSETT_PARAM_PATH_VALUE "/opt/usr/share/settings/Alerts"
#define CAL_APPSETT_PARAM_SILENT "silent"
#define CAL_APPSETT_PARAM_SILENT_VALUE "silent show"
#define CAL_APPSETT_PARAM_DEFAULT "default"
#define CAL_APPSETT_PARAM_DEFAULT_VALUE "default show"


CalSettingsView::CalSettingsView() : CalView("CalSettingsView"),
	__first_day_week_item(nullptr),
	__lock_time_zone_item(nullptr),
	__time_zone_item(nullptr),
	__noti_sound_item(nullptr),
	__dialog(nullptr)
{
}

CalSettingsView::~CalSettingsView()
{
}

/**
 * Callback, called when the dialog delete.
 *
 * @param [in]	data		If non-null, the data.
 * @param [in]	e			If non-null, the Evas to process.
 * @param [in]	obj			If non-null, the object.
 * @param [in]	event_info	If non-null, information describing the event.
 */
void CalSettingsView::__dialogDeleteCallback(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    WENTER();
}

/**
 * Executes the create firstday popup action.
 */
void CalSettingsView::onCreateFirstdayPopup(void)
{
	CalSettingFirstDayOfWeekPopup *popup = new CalSettingFirstDayOfWeekPopup([this]() {
			 elm_genlist_item_update((Elm_Object_Item*)__first_day_week_item->getElmObjectItem());
	});

	attachPopup(popup);
}

/**
 * Executes the lock time zone action.
 */
void CalSettingsView::onLockTimeZone(void)
{
	CalSettingsManager::getInstance().setLockTimeZone(!CalSettingsManager::getInstance().getLockTimeZone());
	elm_object_item_disabled_set((Elm_Object_Item*)__time_zone_item->getElmObjectItem(),
					!CalSettingsManager::getInstance().getLockTimeZone());
}

/**
 * Executes the time zone action.
 */
void CalSettingsView::onTimeZone(void)
{
	app_control_h service;

	app_control_create(&service);
	app_control_set_operation(service, APP_CONTROL_OPERATION_PICK);
	app_control_set_app_id(service, PKG_3DPARTIES_WORLDCLOCK);
	app_control_set_launch_mode(service, APP_CONTROL_LAUNCH_MODE_GROUP);
	CalAppControlLauncher::getInstance().sendLaunchRequest(service,
		[](app_control_h request, app_control_h reply, app_control_result_e result, void* data)
		{
			WHIT();
			CalSettingsView *self = (CalSettingsView *)data;

			char* timezone = NULL;
			app_control_get_extra_data(reply, APP_CONTROL_WORLDCLOCK_KEY_TZPATH, &timezone);
			if (timezone == NULL)
				return;

			std::string tz(timezone);
			std::string dT;

			if (timezone &&  strlen(timezone) > 0) {
				// TODO CALDATETIME: update this after CalDateTime class is ready to use
				//CalLocaleManager::getInstance().getDisplayTextTimeZone(tz, dT);
				self->__time_zone_item->setSubText(dT.c_str());
				CalSettingsManager::getInstance().setTimeZone(tz);
			}

			if (timezone) {
				free(timezone);
				timezone = NULL;
			}
			elm_genlist_item_update((Elm_Object_Item*)self->__time_zone_item->getElmObjectItem());
		},
		this,
		getEvasObj()
	);
}

/**
 * Executes the notification sound action.
 */
void CalSettingsView::onNotificationSound(void)
{
	app_control_h service;
	app_control_create(&service);

	app_control_add_extra_data(service, CAL_APPSETT_PARAM_PATH, CAL_APPSETT_PARAM_PATH_VALUE);
	/* if need show silent item */
	app_control_add_extra_data(service, CAL_APPSETT_PARAM_SILENT, CAL_APPSETT_PARAM_SILENT_VALUE);
	app_control_add_extra_data(service, CAL_APPSETT_PARAM_DEFAULT, CAL_APPSETT_PARAM_DEFAULT_VALUE);

	const char *ringtone_file = CalSettingsManager::getInstance().getAlertSound();

	if(!strcmp(ringtone_file, CAL_SETTING_NOTIFICATION_SOUND_DEFAULT))
	{
		app_control_add_extra_data(service, CAL_APPSETT_PARAM_MARKED_MODE, CAL_APPSETT_PARAM_MARKED_MODE_DEFAULT);
	}
	else
	{
		app_control_add_extra_data(service, CAL_APPSETT_PARAM_MARKED_MODE, ringtone_file);
	}

	app_control_set_app_id(service, PKG_3DPARTIES_SETTING_RINGTONE);
	app_control_set_launch_mode(service, APP_CONTROL_LAUNCH_MODE_GROUP);

	CalAppControlLauncher::getInstance().sendLaunchRequest(service,
		[](app_control_h request, app_control_h reply, app_control_result_e result, void* data)
		{
			CalSettingsView *self = (CalSettingsView *)data;

			char* ringtone = NULL;
			int error = 0;
			error = app_control_get_extra_data(reply, APP_CONTROL_SETTING_RINGTONE_KEY_RESULT, &ringtone);
			if (error != APP_CONTROL_ERROR_NONE)
			{
				WERROR("get extra data by key result is failed(%x)", error);
				return;
			}
			if (ringtone == NULL)
			{
				WERROR("get extra data by key result return rington NULL");
				return;
			}
			std::string displayText;
			self->__getAlertDisplayText(ringtone, displayText);

			if (!strcmp(ringtone, CAL_SETTING_NOTIFICATION_SILENT))
			{
				self->__noti_sound_item->setSubText(_L_S_("IDS_CLD_OPT_NONE"));
				CalSettingsManager::getInstance().setAlertSound(CAL_SETTING_NOTIFICATION_SILENT);
			}
			else if (!strcmp(ringtone, CAL_SETTING_NOTIFICATION_SOUND_DEFAULT))
			{
				self->__noti_sound_item->setSubText(_L_S_("IDS_COM_BODY_DEFAULT"));
				CalSettingsManager::getInstance().setAlertSound(CAL_SETTING_NOTIFICATION_SOUND_DEFAULT);
			}
			else
			{
				self->__noti_sound_item->setSubText(displayText.c_str());
				CalSettingsManager::getInstance().setAlertSound(ringtone);
			}

			free(ringtone);
			ringtone = NULL;

			elm_genlist_item_update((Elm_Object_Item*)self->__noti_sound_item->getElmObjectItem());
		},
		this,
		getEvasObj()
	);
}

/**
 * Executes the select action.
 *
 * @param	settingType	Type of the setting.
 */
void CalSettingsView::onSelect(CalSettingsView::SettingSelectType settingType)
{
	switch(settingType)
	{
		case SETTING_FIRST_DAY_OF_WEEK_SELECT://first day of week
			onCreateFirstdayPopup();
			break;
		case SETTING_LOCK_TIME_ZONE_SELECT://Lock time zone
			onLockTimeZone();
			break;
		case SETTING_TIME_ZONE_SELECT:// Time zone
			onTimeZone();
			break;
		case SETTING_NOTIF_SOUND_SELECT://// Notification sound
			onNotificationSound();
			break;
		default:
			break;
	}

}

/**
 * Radio on lock time zone.
 */
void CalSettingsView::radioOnLockTimeZone(void)
{
	elm_object_item_disabled_set((Elm_Object_Item*)__time_zone_item->getElmObjectItem(),
					!CalSettingsManager::getInstance().getLockTimeZone());
}

/**
 * Executes the create action.
 *
 * @param [in]	parent		If non-null, the parent.
 * @param [in]	viewParam	If non-null, the view parameter.
 *
 * @return	null if it fails, else an Evas_Object*.
 */
Evas_Object* CalSettingsView::onCreate(Evas_Object* parent, void* viewParam)
{
	WENTER();

	__dialog = new CalDialogControl;
	__dialog->create(parent, NULL);

	Evas_Object *genlist = __dialog->getEvasObj();

	elm_genlist_homogeneous_set(genlist, EINA_TRUE);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	evas_object_event_callback_add(genlist, EVAS_CALLBACK_DEL, __dialogDeleteCallback, this);

	// First day of week
	__first_day_week_item = new CalDialogSettingsTwoTextItem(
		[this]() {onSelect(SETTING_FIRST_DAY_OF_WEEK_SELECT);},
		_L_S_("IDS_CLD_MBODY_WEEK_STARTS_ON"),
		_L_S_("IDS_CLD_OPT_LOCALE_DEFAULT_ABB"),
		CalDialogSettingsTwoTextItem::SUB_FIRST_DAY_OF_WEEK
	);
	__dialog->add(__first_day_week_item);

	// Notification sound
	std::string displayText = _L_S_("IDS_CLD_MBODY_NOTIFICATION_SOUND");
	displayText += " 1";

	__noti_sound_item = new CalDialogSettingsTwoTextItem(
		[this]() {
			onSelect(SETTING_NOTIF_SOUND_SELECT);
		},
		_L_S_("IDS_CLD_MBODY_NOTIFICATION_SOUND"),
		displayText.c_str(), CalDialogSettingsTwoTextItem::SUB_NOTIFICATION_SOUND
	);

	const char *ringtone_file = CalSettingsManager::getInstance().getAlertSound();
	if (ringtone_file)
	{
		displayText.clear();
		if (!strcmp(ringtone_file, CAL_SETTING_NOTIFICATION_SILENT))
		{
			displayText = _L_S_("IDS_CLD_OPT_NONE");
		}
		else if (!strcmp(ringtone_file, CAL_SETTING_NOTIFICATION_SOUND_DEFAULT))
		{
			displayText = _L_S_("IDS_COM_BODY_DEFAULT");
		}
		else
		{
			__getAlertDisplayText(ringtone_file, displayText);
		}
	}
	else
	{
		WERROR("fail");
		displayText = _L_S_("IDS_CLD_OPT_NONE");
	}
	__noti_sound_item->setSubText(displayText.c_str());
	__dialog->add(__noti_sound_item);

	// Group index -Time Zone
	__dialog->add(new CalDialogSimpleTitleItem(_L_S_("IDS_CLD_HEADER_TIME_ZONE_ABB")));

	// Lock time zone
	__lock_time_zone_item = new CalDialogSettingsOneTextOneIconItem(
		[this]() {onSelect(SETTING_LOCK_TIME_ZONE_SELECT);},
		_L_S_("IDS_CLD_MBODY_LOCK_TIME_ZONE")
	);
	__lock_time_zone_item->setRadioCb([this](void) {radioOnLockTimeZone();});
	__dialog->add(__lock_time_zone_item);

	// Lock time zone description
	CalDialogSettingsMultilSubItem* lock_time_zone_description_item = new CalDialogSettingsMultilSubItem(
		[this]() {},
		_L_S_("IDS_CLD_BODY_LOCK_EVENT_TIMES_AND_DATES_TO_YOUR_SELECTED_TIME_ZONE_THEY_WILL_NOT_CHANGE_EVEN_IF_YOU_MOVE_TO_ANOTHER_TIME_ZONE")
	);

	__dialog->add(lock_time_zone_description_item);
	Elm_Object_Item* itemObj =  lock_time_zone_description_item->getElmObjectItem();
	if (itemObj)
		elm_genlist_item_select_mode_set(itemObj, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	// Time zone
	std::string tz;
	std::string dT;
	CalSettingsManager::getInstance().getTimeZone(tz);

	// TODO CALDATETIME: update this after CalDateTime class is ready to use
	//CalLocaleManager::getInstance().getDisplayTextTimeZone(tz, dT);

	__time_zone_item = new CalDialogSettingsTwoTextItem(
		[this]() {
			onSelect(SETTING_TIME_ZONE_SELECT);
		},
		_L_S_("IDS_CLD_HEADER_TIME_ZONE_ABB"),
		dT.c_str(), CalDialogSettingsTwoTextItem::SUB_TIME_ZONE
	);

	__dialog->add(__time_zone_item);
	elm_object_item_disabled_set((Elm_Object_Item*)__time_zone_item->getElmObjectItem(),
					!CalSettingsManager::getInstance().getLockTimeZone());

	return genlist;
}

/**
 * Updates this object.
 */
void CalSettingsView::__update()
{
	destroyPopup();
	elm_object_item_part_text_set(getNaviItem(), "elm.text.title", _L_("IDS_CLD_OPT_SETTINGS"));

	std::string tz;
	std::string dT;
	CalSettingsManager::getInstance().getTimeZone(tz);

	// TODO CALDATETIME: update this after CalDateTime class is ready to use
	//CalLocaleManager::getInstance().getDisplayTextTimeZone(tz, dT);
	if(__time_zone_item)
	{
		__time_zone_item->setSubText(dT.c_str());
	}

	elm_genlist_realized_items_update(__dialog->getEvasObj());
}

/**
 * Executes the event action.
 *
 * @param	event	The event.
 */
void CalSettingsView::onEvent(const CalEvent& event)
{
	WENTER();
	WDEBUG("type = %u, source = %u", event.type, event.source);
	switch (event.type)
	{
		case CalEvent::DB_CHANGED:
		case CalEvent::APP_PAUSED:
		case CalEvent::APP_RESUMED:
		case CalEvent::FIRST_DAY_OF_WEEK_CHANGED:
			break;
		case CalEvent::TIME_CHANGED:
		case CalEvent::LANGUAGE_CHANGED:
		case CalEvent::SETTING_CHANGED:
			__update();
			break;
		default:
			WERROR("invalid type = %u", event.type);
			break;
	}
}

/**
 * Executes the pushed action.
 *
 * @param [in]	naviItem	If non-null, the navi item.
 */
void CalSettingsView::onPushed(Elm_Object_Item* naviItem)
{
	elm_object_item_part_text_set(getNaviItem(), "elm.text.title", _L_("IDS_CLD_OPT_SETTINGS"));
}

/**
 * Executes the destroy action.
 */
void CalSettingsView::onDestroy()
{
	CalAppControlLauncher::getInstance().terminate();
}

/**
 * Gets alert display text.
 *
 * @param		ringtone		The ringtone.
 * @param [in]	displayText		The display text.
 */
void CalSettingsView::__getAlertDisplayText(const char* ringtone, std::string& displayText)
{
	metadata_extractor_h metadata = NULL;
	int ret = metadata_extractor_create(&metadata);
	if(ret == METADATA_EXTRACTOR_ERROR_NONE && metadata)
	{
		ret = metadata_extractor_set_path(metadata, ringtone);
		if(ret == METADATA_EXTRACTOR_ERROR_NONE)
		{
			char *title = NULL;
			ret = metadata_extractor_get_metadata(metadata, METADATA_TITLE, &title);
			metadata_extractor_destroy(metadata);
			if(title)
			{
				displayText = title;
				free(title);
				return ;
			}
			else
				WDEBUG("metadata title is null");
		}
		else
		{
			WDEBUG("metadata title get NULL");
			metadata_extractor_destroy(metadata);
		}
	}
	else
	{
		WDEBUG("metadata title get NULL");
	}

	std::string tmp = ringtone;

	unsigned foundDot = tmp.find_last_of(".");
	unsigned foundName = tmp.find_last_of("/\\");

	if (foundDot != tmp.npos && foundName != tmp.npos)
	{
		displayText = tmp.substr(foundName+1, (foundDot-foundName-1));
	}
	else
	{
		displayText = tmp;
	}
}
