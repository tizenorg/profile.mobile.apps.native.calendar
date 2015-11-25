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
#include <Eina.h>

#include "CalCommon.h"

#include "CalDataManager.h"
#include "CalSettingsManager.h"
#include "CalScheduleRepeat.h"

#include "CalNaviframe.h"
#include "CalEditDialogControl.h"

#include "CalEditReminderPopup.h"
#include "CalEditRepeatDailyView.h"
#include "CalEditRepeatWeeklyView.h"
#include "CalEditRepeatMonthlyView.h"
#include "CalEditRepeatYearlyView.h"
#include "CalEditRepeatPopup.h"

#include "CalDialogEditTextFieldItem.h"
#include "CalDialogEditDateTimeItem.h"
#include "CalDialogEditOptionalTextFieldItem.h"
#include "CalDialogEditRepeatRadioItem.h"
#include "CalDialogEditMoreItem.h"
#include "CalDialogEditTwoTextRemoveIconItem.h"
#include "CalDialogEditOneTextRemoveIconItem.h"
#include "CalDialogEditOneTextNoRemoveIconItem.h"

#include "CalBookManager.h"
#include "CalTimePickerPopup.h"

#include "CalEditView.h"
#include <notification.h>
#include <app_control.h>
#include "CalLocaleManager.h"

#include "WPopup.h"
#include "CalEditCustomReminderPopup.h"

#include "CalAppControlLauncher.h"
#include "CalDatePickerPopup.h"
#include "CalEditMoreMenuControl.h"


CalEditView::CalEditView() : CalView("CalEditView")
{
	WENTER();
	__initialize();

	__workingCopy = CalSchedule::makeDefaultSchedule();

	const int bookId = CalSettingsManager::getInstance().getLastUsedCalendar();
	if (CalBookManager::getInstance().getBook(bookId))
	{
		__workingCopy->setBookId(bookId);
	}
}

CalEditView::CalEditView(const CalDate& focusedDate) : CalView("CalEditView")
{
	WENTER();
	__initialize();

	__focusedDate = focusedDate;
	__workingCopy = CalSchedule::makeDefaultSchedule(focusedDate);

	const int bookId = CalSettingsManager::getInstance().getLastUsedCalendar();
	if (CalBookManager::getInstance().getBook(bookId))
	{
		__workingCopy->setBookId(bookId);
	}

	__mode = CREATE;
}

CalEditView::CalEditView(std::shared_ptr<CalSchedule> schedule, CalEditMode mode,
                         std::function<void (int newId)> savedCb, bool setLastUsedCalendar) :
	CalView("CalEditView"),
	__savedCb(savedCb)
{
	WENTER();
	__initialize();

	__inputSchedule = schedule;
	__mode = mode;
	switch(__mode)
	{
		case CREATE:
		case EDIT_EXTERNAL:
			__workingCopy = __inputSchedule;
			break;
		case COPY:
			__workingCopy = CalDataManager::getInstance().getWorkingCopy(CalDataManager::ONLY_THIS, *__inputSchedule);
			break;
		case EDIT:
			__workingCopy = CalDataManager::getInstance().getWorkingCopy(CalDataManager::ALL, *__inputSchedule);
			break;
		case EDIT_INSTANCE:
			__workingCopy = CalDataManager::getInstance().getWorkingCopy(CalDataManager::ONLY_THIS, *__inputSchedule);
			break;
		case EDIT_THIS_AND_FURTURE:
			__workingCopy = CalDataManager::getInstance().getWorkingCopy(CalDataManager::THIS_AND_FUTURE, *__inputSchedule);
			break;
		default:
			WASSERT_EX(0, "invalid type %x", __mode);
			break;
	}

	bool isReadOnly = CalBookManager::getInstance().getBook(__workingCopy->getBookId())->isReadOnly();
	if( (__mode == CREATE && setLastUsedCalendar == true) || (__mode == COPY && isReadOnly))
	{
		const int bookId = CalSettingsManager::getInstance().getLastUsedCalendar();
		if (CalBookManager::getInstance().getBook(bookId))
		{
			__workingCopy->setBookId(bookId);
		}
	}

	WLEAVE();
}

CalEditView::~CalEditView()
{
	WENTER();

	free(__currentParticipantKeyword);

	if (__idler)
	{
		ecore_timer_del(__idler);
		__idler = NULL;
	}

	evas_object_smart_callback_del(__dialog->getEvasObj(), "realized", [](void* data, Evas_Object* obj, void* event_info) {
	 	CalEditView* self = (CalEditView*)data;
		self->__updateMoreItemButtonStatus();/*for location, reminder, repeat 3 buttons*/
		self->__updateMoreButtonStatus(); /*for more button*/
	 });
}

void CalEditView::__initialize()
{
	__workingCopy = NULL;
	__inputSchedule = NULL;
	__mode = CREATE;
	__isChanged = false;
	__isMoreShown = false;
	__dialog = NULL;
	__title = NULL;
	__time = NULL;
	__timezone = NULL;
	__location = NULL;
	__reminderTitleItem = NULL;
	__repeat = NULL;
	__currentParticipantKeyword = NULL;
	__description = NULL;
	__more = NULL;
	__isDiscard = false;
	__reminderNumber = 1;
	__isPrepareExit = false;
	__list = NULL;
	__count = 0;
	__appendNum = 0;
	__idler = NULL;
	memset(__reminderItems, 0, sizeof(__reminderItems));
}

void CalEditView::createRepeatPopup(WNaviframe *naviframe, CalScheduleRepeat &repeat, const std::function<void (const CalScheduleRepeat)>& doneCb)
{
	WENTER();
	const char* timezone = __workingCopy->getTimeZone();
	CalDateTime dateTime;
	__workingCopy->getStart(dateTime);

	CalEditRepeatPopup* popup = new CalEditRepeatPopup(repeat.unitType,
		[naviframe, doneCb, repeat, dateTime, timezone](const CalScheduleRepeat::UnitType repeatUnitType) {
			WDEBUG("DONE CALLBACK!! repeatResult.unitType=%d", repeatUnitType);
			CalView *view = NULL;
			repeat.print();
			CalScheduleRepeat tmpRepeat(repeat);
			tmpRepeat.setDefault(repeatUnitType,dateTime,timezone);
			switch (repeatUnitType)
			{
				case CalScheduleRepeat::DAY:
					view = new CalEditRepeatDailyView(
							tmpRepeat, dateTime, doneCb
					);
					break;
				case CalScheduleRepeat::WEEK:
					view = new CalEditRepeatWeeklyView(
							tmpRepeat, dateTime, timezone, doneCb
					);
					break;
				case CalScheduleRepeat::MONTH:
					view = new CalEditRepeatMonthlyView(
							tmpRepeat, dateTime, timezone, doneCb
					);
					break;
				case CalScheduleRepeat::YEAR:
					view = new CalEditRepeatYearlyView(
							tmpRepeat, dateTime, doneCb
					);
					break;
				case CalScheduleRepeat::ONE_TIME:
					tmpRepeat.setDefault(CalScheduleRepeat::UnitType::ONE_TIME, dateTime, timezone);
					doneCb(tmpRepeat);
					break;
				default:
					break;
			}

			if (!view)
			{
				return;
			}

			naviframe->push(view);
		}
	);

	attachPopup(popup);
	elm_object_part_text_set(popup->getEvasObj(), "title,text", _L_("IDS_CLD_BUTTON_REPEAT_ABB"));
}

void CalEditView::__removeLocation()
{
	if(!__location)
	{
		return;
	}

	__resetAllEntryState(__location);

	__workingCopy->setLocation(NULL);
	__dialog->remove(__location);
	__location = NULL;

	__more->setButtonDisable(CalDialogEditMoreItem::LOCATION, false);
}

void CalEditView::__onAddLocationField()
{
	WENTER();
	if (__location)
	{
		return;
	}

	__location = new CalDialogEditOptionalTextFieldItem(LOCATION, [this](CalEditField* editField)
	{
		std::string location;
		__workingCopy->getDisplayLocation(-1, location);

		editField->setText(location.c_str());

		int fontSize = __location->getSystemFontSize();
		if(abs(fontSize) > TEXT_ITEM_DEFAULT_SIZE)
		{
			editField->setFontSize(fontSize);
			editField->setGuideText(_L_G_("IDS_CLD_BUTTON_LOCATION_ABB"), fontSize);
		}
		else
		{
			editField->setGuideText(_L_G_("IDS_CLD_BUTTON_LOCATION_ABB"), 0);
		}

		editField->setChangeCb([this](const char* text)
		{
			WDEBUG("Location : %s", text);
			__workingCopy->setLocation(text);
		});

		editField->setEntryCompletedCb([this]()
		{
			if (__description)
			{
				__description->getEditField()->setFocusToEntry();
			}
			else
			{
				elm_object_focus_next(getNaviframe()->getEvasObj(), ELM_FOCUS_NEXT);
			}
		});

		editField->setEntryMaxLenReachCb([this]()
		{
			char warningText[CAL_WARNING_MAX_LENGTH] = {0};
			snprintf(warningText, sizeof(warningText)-1, _L_("IDS_CLD_TPOP_MAXIMUM_NUMBER_OF_CHARACTERS_HPD_REACHED"), CAL_EVENT_TITLE_MAX_CHAR_LIMIT);
			notification_status_message_post(warningText);
		});

		editField->setEntryReturnKeyType(CalEditField::DONE);

		CalUnderlineEditField* titleEditField = __title->getEditField();
		if (titleEditField)
		{
			titleEditField->setEntryReturnKeyType(CalUnderlineEditField::NEXT);
		}

		if (__description == NULL)
		{
			editField->setEntryReturnKeyType(CalEditField::DONE);
		}
		else
		{
			editField->setEntryReturnKeyType(CalEditField::NEXT);
		}
	});

	__dialog->add(__location);

	__location->setRemoveCb([this]() {
			__removeLocation();
		}
	);
}

void CalEditView::__onAddLocationButton()
{
	WENTER();

	__onAddLocationField();
	__setFocusToField(__location);

	if (__more)
	{
		__more->setButtonDisable(CalDialogEditMoreItem::LOCATION, true);
	}
}

void CalEditView::__removeRepeatItem()
{
	if(!__repeat)
	{
		return;
	}

	CalScheduleRepeat repeat;
	repeat.unitType = CalScheduleRepeat::UnitType::ONE_TIME;
	__workingCopy->setRepeat(repeat);

	__dialog->remove(__repeat);
	__repeat = NULL;
	__more->setButtonDisable(CalDialogEditMoreItem::REPEAT, false);
}

void CalEditView::__addRepeat()
{
	WENTER();
	CalScheduleRepeat repeat = __workingCopy->getRepeat();
	std::string repeatString;

	CalScheduleRepeat::Date date;
	CalDateTime start;
	const char* tz = __workingCopy->getTimeZone();
	__workingCopy->getStart(start);
	date.year = start.getYear(tz);
	date.month = start.getMonth(tz);
	date.mday = start.getMday(tz);

	repeat.getRepeatString(date, tz, repeatString);

	__repeat = new CalDialogEditTwoTextRemoveIconItem(REPEAT,
		_L_S_("IDS_CLD_BUTTON_REPEAT_ABB"),
		repeatString.c_str());

	__repeat->setSelectCb([this]() {
			WDEBUG("Repeat selected.");
			CalScheduleRepeat repeat = __workingCopy->getRepeat();

			createRepeatPopup(this->getNaviframe(), repeat,
				[this](const CalScheduleRepeat repeat) {
					WDEBUG("Repeat done.");
					repeat.print();
					__workingCopy->setRepeat(repeat);
					std::string repeatString;
					CalScheduleRepeat::Date date;
					CalDateTime start;
					const char* tz = __workingCopy->getTimeZone();
					__workingCopy->getStart(start);
					date.year = start.getYear(tz);
					date.month = start.getMonth(tz);
					date.mday = start.getMday(tz);
					repeat.getRepeatString(date, tz, repeatString);
					((CalDialogEditTwoTextRemoveIconItem*)__repeat)->setSubText(repeatString.c_str());
					elm_genlist_item_update((Elm_Object_Item*)__repeat->getElmObjectItem());
				}
			);
		}
	);
	__repeat->setRemoveCb([this]() {
			__removeRepeatItem();
		}
	);

	Elm_Object_Item* itemOjbect = __dialog->add(__repeat);
	if (__mode == EDIT_INSTANCE)
	{
		elm_object_item_disabled_set(itemOjbect, EINA_TRUE);
	}

	if (__more)
	{
		__more->setButtonDisable(CalDialogEditMoreItem::REPEAT, true);
	}
}

void CalEditView::__addRepeatOff()
{
	WENTER();

	__repeat = new CalDialogEditTwoTextRemoveIconItem(REPEAT,
		_L_S_("IDS_CLD_BUTTON_REPEAT_ABB"),
		_L_S_("IDS_CLD_OPT_NONE"));

	__repeat->setSelectCb([this]() {
			WDEBUG("Repeat selected.");
			CalScheduleRepeat repeat = __workingCopy->getRepeat();

			createRepeatPopup(this->getNaviframe(), repeat,
				[this](const CalScheduleRepeat repeat) {
					WDEBUG("Repeat done.");
					repeat.print();
					__workingCopy->setRepeat(repeat);
					std::string repeatString;
					CalScheduleRepeat::Date date;
					CalDateTime start;
					const char* tz = __workingCopy->getTimeZone();
					__workingCopy->getStart(start);
					date.year = start.getYear(tz);
					date.month = start.getMonth(tz);
					date.mday = start.getMday(tz);
					repeat.getRepeatString(date, tz, repeatString);
					((CalDialogEditTwoTextRemoveIconItem*)__repeat)->setSubText(repeatString.c_str());
					elm_genlist_item_update((Elm_Object_Item*)__repeat->getElmObjectItem());
				}
			);
		}
	);
	__repeat->setRemoveCb([this]() {
			__removeRepeatItem();
		}
	);

	__dialog->add(__repeat);
}

void CalEditView::__onAddRepeatButton()
{
	WENTER();
	if (__repeat)
		return;

	CalScheduleRepeat repeatOriginal = __workingCopy->getRepeat();

	createRepeatPopup(this->getNaviframe(), repeatOriginal,
		[this](const CalScheduleRepeat repeat) {
			WDEBUG("Repeat done.");
			repeat.print();
			__workingCopy->setRepeat(repeat);

			if (__repeat)
				elm_genlist_item_update((Elm_Object_Item*)__repeat->getElmObjectItem());
			else
				__addRepeat();
		}
	);
}

void CalEditView::__reminderRemoveCb(CalDialogEditOneTextRemoveIconItem* reminderItem)
{
	const int reminderIndex = __getReminderIndexFromInterval(reinterpret_cast<long long>(reminderItem->getCustomData()));
	WDEBUG("Reminder[%d] remove selected", reminderIndex);
	__removeAllReminderItems();
	__workingCopy->removeReminder(reminderIndex);
	__addAllReminderItems();
	__more->setButtonDisable(CalDialogEditMoreItem::REMINDER, false);
}

CalDialogEditOneTextRemoveIconItem* CalEditView::__createReminderItem(const CalScheduleReminder& reminder, int sortIndex)
{
	std::string reminderText;
	reminder.getString(reminderText);

	CalDialogEditOneTextRemoveIconItem* reminderItem =
		new CalDialogEditOneTextRemoveIconItem(sortIndex, reminderText.c_str());

	reminderItem->setSelectCb([this, reminderItem]() {
		const int reminderInterval = reinterpret_cast<long long>(reminderItem->getCustomData());

		CalScheduleReminder reminder;
		const int count = __workingCopy->getRemindersCount();
		for (int i = 0; i < count; i++)
		{
			__workingCopy->getReminder(i, reminder);
			if(reminder.getMinuteValue() == reminderInterval)
			{
				break;
			}
		}

		CalEditReminderPopup* popup = new CalEditReminderPopup(reminder, true);
		popup->setDoneCb(
			[this, reminderItem](const CalScheduleReminder& reminder, int radio_index) {
				__applyChangesToReminder(reminder, reminderItem);
			}
		);
		popup->setCustomSelectedCb(
			[this, reminderItem](const CalScheduleReminder& reminder, int radio_index) {
				CalEditCustomReminderPopup* popup = new CalEditCustomReminderPopup(reminder);
				popup->setDoneCb([this, reminderItem](const CalScheduleReminder& reminder) {
					__applyChangesToReminder(reminder, reminderItem);
				});
				popup->setCancelCb([](){});
				popup->create(getNaviframe()->getEvasObj(), NULL);
				attachPopup(popup);
			}
		);

		attachPopup(popup);
	});

	reminderItem->setRemoveCb([this, reminderItem]() {
		__reminderRemoveCb(reminderItem);
	});

	return reminderItem;
}

CalDialogEditOneTextRemoveIconItem* CalEditView::__createReminderOffItem()
{
	CalDialogEditOneTextRemoveIconItem* reminderItem =
		new CalDialogEditOneTextRemoveIconItem(REMINDER1, _L_("IDS_CLD_OPT_NONE"));

	reminderItem->setSelectCb([this, reminderItem]() {
		__onAddReminderButton();
	});

	reminderItem->setRemoveCb([this, reminderItem]() {
		__removeReminderTitleItem();
		__removeAllReminderItems();
	});

	return reminderItem;
}

void CalEditView::__addReminder(const CalScheduleReminder& reminder)
{
	bool needToAdd = true;
	for(int i = 0; i < MAX_REMINDER_COUNT; i++)
	{
		CalScheduleReminder existReminder;
		__workingCopy->getReminder(i, existReminder);
		if(existReminder.unitType != CalScheduleReminder::NONE && existReminder.getMinuteValue() == reminder.getMinuteValue())
		{
			needToAdd = false;
			break;
		}
	}

	if(needToAdd)
	{
		__removeAllReminderItems();
		__workingCopy->addReminder(reminder);
		__addAllReminderItems();
	}
}

void CalEditView::__addAllReminderItems()
{
	WENTER();
	const int count = __workingCopy->getRemindersCount();
	WDEBUG("Count: %d", count);

	if(!count)
	{
		if(__reminderTitleItem)
		{
			__removeReminderTitleItem();
		}

		return;
	}

	__addReminderTitleItem();

	memset(__reminderItems, 0, sizeof(__reminderItems));

	std::set<CalScheduleReminder> reminders;
	for (int i = 0; i < count; i++)
	{
		CalScheduleReminder reminder;
		__workingCopy->getReminder(i, reminder);
		reminders.insert(reminder);
	}

	int reminderIndex = 0;
	for(auto it = reminders.begin(); it != reminders.end() && reminderIndex < MAX_REMINDER_COUNT; ++it)
	{
		__reminderItems[reminderIndex] = __createReminderItem((*it), reminderIndex + REMINDER1);
		if(__reminderItems[reminderIndex])
		{
			__reminderItems[reminderIndex]->setCustomData((void*)((*it).getMinuteValue()));
			__dialog->add(__reminderItems[reminderIndex]);
			++reminderIndex;
		}
	}

	if (__more)
	{
		__more->setButtonDisable(CalDialogEditMoreItem::REMINDER, count == MAX_REMINDER_COUNT);
	}
}

void CalEditView::__removeAllReminderItems()
{
	WENTER();
	const int count = __workingCopy->getRemindersCount();
	WASSERT(count >= 0);
	WDEBUG("Count: %d", count);

	for (int i = 0; i < MAX_REMINDER_COUNT; i++)
	{
		if (__reminderItems[i] == NULL)
		{
			continue;
		}

		__dialog->remove(__reminderItems[i]);
		__reminderItems[i] = NULL;
	}

	__removeReminderTitleItem();

	__more->setButtonDisable(CalDialogEditMoreItem::REMINDER, false);
}

void CalEditView::__addReminderTitleItem()
{
	if (__reminderTitleItem)
	{
		return;
	}

	__reminderTitleItem = new CalDialogEditOneTextNoRemoveIconItem(REMINDERTITLE, _L_S_("IDS_CLD_BUTTON_REMINDER_ABB"));
	__dialog->add(__reminderTitleItem);
}

void CalEditView::__removeReminderTitleItem()
{
	if (!__reminderTitleItem)
	{
		return;
	}

	__dialog->remove(__reminderTitleItem);
	__reminderTitleItem = NULL;
}

int CalEditView::__getReminderIndexFromInterval(const int reminderInterval)
{
	int reminderIndex = 0;

	CalScheduleReminder reminder;
	const int count = __workingCopy->getRemindersCount();
	for (int i = 0; i < count; i++)
	{
		__workingCopy->getReminder(i, reminder);
		if(reminder.getMinuteValue() == reminderInterval)
		{
			reminderIndex = i;
			break;
		}
	}

	return reminderIndex;
}

void CalEditView::__applyChangesToReminder(const CalScheduleReminder& reminder, CalDialogEditOneTextRemoveIconItem* reminderItem)
{
	if(reminder.unitType == CalScheduleReminder::NONE)
	{
		return; // NONE selected
	}

	const int reminderIndex = __getReminderIndexFromInterval(reinterpret_cast<long long>(reminderItem->getCustomData()));
	__workingCopy->setReminder(reminderIndex, reminder);

	__removeAllReminderItems();
	__addAllReminderItems();
}

void CalEditView::__onAddReminderButton()
{
	WENTER();

	const int count = __workingCopy->getRemindersCount();
	if (__more)
	{
		__more->setButtonDisable(CalDialogEditMoreItem::REMINDER, count == MAX_REMINDER_COUNT);
	}

	CalScheduleReminder reminder = CalScheduleReminder();
	reminder.unitType = CalScheduleReminder::UnitType::NONE;
	reminder.unitValue = 0;

	CalEditReminderPopup* popup = new CalEditReminderPopup(reminder, false);
	popup->setDoneCb(
		[this](const CalScheduleReminder& reminder, int radio_index) {
			if(radio_index > 1)
			{
				__addReminder(reminder);
			}
		}
	);
	popup->setCustomSelectedCb(
		[this](const CalScheduleReminder& reminder, int radio_index) {
			CalEditCustomReminderPopup* popup = new CalEditCustomReminderPopup(reminder);
			popup->setDoneCb([this](const CalScheduleReminder& reminder) {
				__addReminder(reminder);
			});
			popup->setCancelCb([](){});
			popup->create(getNaviframe()->getEvasObj(), NULL);
			attachPopup(popup);
		}
	);
	attachPopup(popup);
}

void CalEditView::__clearMoreSubItems()
{
	WENTER();
	__isMoreShown = false;

	if(__more)
	{
		__more->setButtonDisable(CalDialogEditMoreItem::REMINDER, false);
	}

	__removeTimezoneItem();
	__removeDescription();
}

void CalEditView::__resetAllEntryState(CalDialogControl::Item* item)
{
	if (!item)
	{
		return;
	}

	if (item->getSortIndex() == LOCATION)
	{
		if (__description)
		{
			return;
		}
		else if (__title && __title->getEditField())
		{
			__title->getEditField()->setEntryReturnKeyType(CalUnderlineEditField::DONE);
		}
	}
	else if (item->getSortIndex() == DESCRIPTION)
	{
		if (__location)
		{
			__location->getEditField()->setEntryReturnKeyType(CalEditField::DONE);
		}
		else if (__title && __title->getEditField())
		{
			__title->getEditField()->setEntryReturnKeyType(CalUnderlineEditField::DONE);
		}
	}
}

void CalEditView::__setTimeZone(const std::string& tz)
{
	// get previous tz
	const char* oldTz = __workingCopy->getTimeZone();
	CalDateTime startDateTime, endDateTime;
	struct tm startTm = {};
	struct tm endTm = {};
	__workingCopy->getStart(startDateTime);
	__workingCopy->getEnd(endDateTime);

	// get previous start/end for floating time
	startDateTime.getTm(oldTz, &startTm);
	endDateTime.getTm(oldTz, &endTm);

	// update tz
	__workingCopy->setTimeZone(tz.c_str());

	// change start/end for floating time
	startDateTime.set(startTm, tz.c_str());
	endDateTime.set(endTm, tz.c_str());

	// update floating time
	__workingCopy->setStart(startDateTime);
	__workingCopy->setEnd(endDateTime);
}

gboolean CalEditView::__onIdlerToFreeItem(gpointer data)
{
	WENTER();
	CalDialogControl::Item* item = (CalDialogControl::Item*)data;

	delete item;

	return false;
}

void CalEditView::__removeTimezoneItem()
{
	if(!__timezone)
	{
		return;
	}

	__dialog->remove(__timezone);
	__timezone = NULL;

	__updateMoreButtonStatus();
}

void CalEditView::__onAddTimezoneField()
{
	if (__timezone)
	{
		return;
	}

	const char* defaultTz = __workingCopy->getTimeZone();
	std::string tz;
	std::string dT;

	if (!defaultTz || strlen(defaultTz) == 0)
	{
		tz.clear();
		CalSettingsManager::getInstance().getCalendarTimeZone(tz);
	}
	else
	{
		tz = defaultTz;
	}

	CalLocaleManager::getInstance().getDisplayTextTimeZone(tz, dT);
	__timezone = new CalDialogEditTwoTextRemoveIconItem(TIMEZONE, _L_S_("IDS_CLD_HEADER_TIME_ZONE_ABB"), dT.c_str());
	__timezone->setSelectCb(
		[this]()
		{
			app_control_h service;
			app_control_create(&service);
			app_control_set_operation(service, APP_CONTROL_OPERATION_PICK);
			app_control_set_app_id(service, PKG_3DPARTIES_WORLDCLOCK);
			app_control_set_launch_mode(service, APP_CONTROL_LAUNCH_MODE_GROUP);

			CalAppControlLauncher::getInstance().sendLaunchRequest(service,
				[](app_control_h request, app_control_h reply, app_control_result_e result, void *data)
				{
					CalEditView *self = (CalEditView *)data;

					char* timezone = NULL;
					app_control_get_extra_data(reply, APP_CONTROL_WORLDCLOCK_KEY_TZPATH, &timezone);
					if (timezone == NULL)
						return;

					std::string tz(timezone);
					std::string dT;

					if (timezone &&  strlen(timezone) > 0) {
						CalLocaleManager::getInstance().getDisplayTextTimeZone(tz, dT);
						self->__timezone->setSubText(dT.c_str());
						self->__timezoneString = tz;
					}

					if (timezone) {
						free(timezone);
						timezone = NULL;
					}
					elm_genlist_item_update((Elm_Object_Item*)self->__timezone->getElmObjectItem());
				},
				this,
				getEvasObj()
			);
		}
	);

	__timezone->setRemoveCb([this]() {
			__removeTimezoneItem();
		}
	);

	__dialog->add(__timezone);
}

void CalEditView::__removeDescription()
{
	if(!__description)
	{
		return;
	}

	__resetAllEntryState(__description);

	__workingCopy->setDescription(NULL);
	__dialog->remove(__description);
	__description = NULL;;

	__updateMoreButtonStatus();
}

void CalEditView::__onAddDescriptionField()
{
	WENTER();
	if (__description)
	{
		return;
	}

	__description = new CalDialogEditOptionalTextFieldItem(DESCRIPTION,
		[this](CalEditField* editField)
	{
		std::string description;
		__workingCopy->getDisplayDescription(description);

		editField->setText(description.c_str());

		int fontSize = __description->getSystemFontSize();
		if(abs(fontSize) > TEXT_ITEM_DEFAULT_SIZE)
		{
			editField->setFontSize(fontSize);
			editField->setGuideText(_L_G_("IDS_CLD_BUTTON_NOTES_ABB"), fontSize);
		}
		else
		{
			editField->setGuideText(_L_G_("IDS_CLD_BUTTON_NOTES_ABB"), 0);
		}

		editField->setChangeCb([this](const char* text)
		{
			__workingCopy->setDescription(text);
		});

		editField->setEntryMaxLenReachCb([this]()
		{
			char warningText[CAL_WARNING_MAX_LENGTH] = {0};
			snprintf(warningText, sizeof(warningText)-1, _L_("IDS_CLD_TPOP_MAXIMUM_NUMBER_OF_CHARACTERS_HPD_REACHED"), CAL_EVENT_TITLE_MAX_CHAR_LIMIT);
			notification_status_message_post(warningText);
		});

		editField->setEntryReturnKeyType(CalEditField::DONE);

		if (__location)
		{
			__location->getEditField()->setEntryReturnKeyType(CalEditField::NEXT);
		}
		else if(__title && __title->getEditField())
		{
			__title->getEditField()->setEntryReturnKeyType(CalUnderlineEditField::NEXT);
		}
	},
	true);

	__dialog->add(__description);

	__description->setRemoveCb([this]() {
			__removeDescription();
		});
}

bool CalEditView::__isTitleChanged()
{
	switch(__mode)
	{
		case EDIT_INSTANCE:
		case EDIT_THIS_AND_FURTURE:
		case EDIT:
		{
			const char* before = __inputSchedule->getSummary();
			const char* after = __workingCopy->getSummary();
			if (before && after && !strcmp(before, after))
			{
				return false;
			}
			else if (before == NULL && after == NULL)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		case COPY:
			return true;
		case CREATE:
		{
			char *summary = elm_entry_utf8_to_markup(__workingCopy->getSummary());
			if (!summary)
			{
				return false;
			}
			else if (!strcmp(summary, ""))
			{
				free(summary);
				return false;
			}
			else
			{
				free(summary);
				return true;
			}
		}
		default:
			WASSERT_EX(0, "invalid type %x", __mode);
			return false;
	}
}

void CalEditView::__updateMoreButtonStatus()
{
	CalDateTime startTime;
	__workingCopy->getStart(startTime);
	if(__more != NULL)
	{
		bool is_disabled = false;

		if (__description && (__timezone || (startTime.isAllDay() || (__time && __time->isAllDay())) ) )
		{
			is_disabled = true;
		}

		__more->setButtonDisable(CalDialogEditMoreItem::MORE, is_disabled);
	}
}

void CalEditView::__updateMoreItemButtonStatus()
{
	//update location
	if (__location)
	{
		__more->setButtonDisable(CalDialogEditMoreItem::LOCATION, true);
	}
	else
	{
		__more->setButtonDisable(CalDialogEditMoreItem::LOCATION, false);
	}

	//update reminder
	int count = __workingCopy->getRemindersCount();
	__more->setButtonDisable(CalDialogEditMoreItem::REMINDER, count == MAX_REMINDER_COUNT);

	//update repeat
	if (__repeat)
	{
		__more->setButtonDisable(CalDialogEditMoreItem::REPEAT, true);
	}
	else
	{
		__more->setButtonDisable(CalDialogEditMoreItem::REPEAT, false);
	}
}

void CalEditView::__onAddMoreButton()
{
	WENTER();

	bool showDescription = !__description;
	bool showTimezone = true;

	CalDateTime startTime;
	__workingCopy->getStart(startTime);

	if (__timezone || startTime.isAllDay() || __time->isAllDay())
	{
		showTimezone = false;
	}

	WPopup *popup = new WPopup(_L_("IDS_CLD_HEADER_ADD_FIELD"), NULL );
	CalEditMoreMenuControl *list =  new CalEditMoreMenuControl(showDescription, showTimezone);
	list->setSelectCb([this](CalDialogEditMoreMenuItem::TYPE type){
			if (type == CalDialogEditMoreMenuItem::DESCRIPTION) {
				__onAddDescriptionField();
				elm_genlist_item_bring_in((Elm_Object_Item*)__description->getElmObjectItem(), ELM_GENLIST_ITEM_SCROLLTO_TOP);
				if (__description && __description->getEditField())
				{
					 __setFocusToField(__description);
				}
			} else if (type == CalDialogEditMoreMenuItem::TIMEZONE) {
				__onAddTimezoneField();
				elm_genlist_item_bring_in((Elm_Object_Item*)__timezone->getElmObjectItem(), ELM_GENLIST_ITEM_SCROLLTO_TOP);
				/* To avoid set focus to description field, more button will be unfocused after disabled, then, focus can be set to description after adding timezone. */
				elm_object_focus_set(__dialog->getEvasObj(), EINA_TRUE);
			}
			__updateMoreButtonStatus();

			if (type != CalDialogEditMoreMenuItem::DESCRIPTION)
			{
				Ecore_IMF_Context *ctx = ecore_imf_context_add(ecore_imf_context_default_id_get());
				Ecore_IMF_Input_Panel_State imf_state = ecore_imf_context_input_panel_state_get(ctx);
				ecore_imf_context_del(ctx);

				if (imf_state != ECORE_IMF_INPUT_PANEL_STATE_HIDE)
				{
					ecore_imf_input_panel_hide();
				}
			}
		}
	);
	popup->setContent(list);
	attachPopup(popup);

}

void CalEditView::__addItemsForNoEmptyField()
{
	const char* loc = __workingCopy->getLocation();
	if (loc && strlen(loc))
	{
		__onAddLocationButton();
	}

	CalScheduleRepeat repeat = __workingCopy->getRepeat();
	if (repeat.unitType != CalScheduleRepeat::ONE_TIME)
	{
		__addRepeat();
	}

	const char* desc = __workingCopy->getDescription();
	if (desc && strlen(desc))
	{
		__onAddDescriptionField();
	}

	const char* defaultTz = __workingCopy->getTimeZone();
	std::string tz;
	if (defaultTz)
	{
		CalSettingsManager::getInstance().getTimeZone(tz);
		if (tz.compare(defaultTz) != 0)
		{
			__onAddTimezoneField();
		}
	}
}

void CalEditView::__update()
{
	destroyPopup();

	if (__isPrepareExit) // if tap save button, don't update view, otherwise, it will lead to many issues.
	{
		return;
	}

	//first, to check if the record is deleted in db;
	std::shared_ptr<CalBook> book;
	switch(__mode)
	{
		case EDIT_INSTANCE:
		case EDIT_THIS_AND_FURTURE:
		case EDIT:
		case COPY:
			book = CalBookManager::getInstance().getBook(__inputSchedule->getBookId());
			if (book == NULL) // It is edited schedule is deleted, treat it as default ,and mode is changed to create
			{
				__workingCopy = CalSchedule::makeDefaultSchedule();
				__mode = CREATE;
			}
			break;
		case CREATE:
			book = CalBookManager::getInstance().getBook(__workingCopy->getBookId());
			if (book == NULL)
			{
				__workingCopy = CalSchedule::makeDefaultSchedule(__focusedDate);
			}
			break;
		default:
			WASSERT_EX(0, "invalid type %x", __mode);
			break;
	}

	//update "title"
	elm_genlist_item_update((Elm_Object_Item*)__title->getElmObjectItem());

	//update "start time, end time, all day"
	if (__time)
	{
		__time->update();
	}

	//update more item
	if (__more)
	{
		bool disableLocation = false;
		bool disableReminder = false;
		bool disableRepeat = false;
		bool disableMore = false;

		if (__location)
		{
			disableLocation= true;
		}

		int count = __workingCopy->getRemindersCount();
		if (count == MAX_REMINDER_COUNT)
		{
			disableReminder = true;
		}

		if (__repeat)
		{
			disableRepeat = true;
		}

		if (__description && __timezone)
		{
			disableMore = true;
		}

		__more->setButtonStatus(disableLocation, disableReminder, disableRepeat, disableMore);

		elm_genlist_item_update((Elm_Object_Item*)__more->getElmObjectItem());
	}

	//update location
	if (__location)
	{
		elm_genlist_item_update((Elm_Object_Item*)__location->getElmObjectItem());
	}

	//update reminder title
	if (__reminderTitleItem)
	{
		elm_genlist_item_update((Elm_Object_Item*)__reminderTitleItem->getElmObjectItem());
		for (int i = 0; i< MAX_REMINDER_COUNT; i++)
		{
			if (__reminderItems[i])
			{
				CalScheduleReminder reminder;
				__workingCopy->getReminder(i, reminder);
				std::string reminderText;
				reminder.getString(reminderText);
				 __reminderItems[i]->setSubText(reminderText.c_str());
				elm_genlist_item_update((Elm_Object_Item*)__reminderItems[i]->getElmObjectItem());
			}
		}
	}

	//update repeat
	if (__repeat)
	{
		CalScheduleRepeat repeat = __workingCopy->getRepeat();
		std::string repeatString;

		CalScheduleRepeat::Date date;
		CalDateTime start;
		const char* tz = __workingCopy->getTimeZone();
		__workingCopy->getStart(start);
		date.year = start.getYear(tz);
		date.month = start.getMonth(tz);
		date.mday = start.getMday(tz);

		repeat.getRepeatString(date, tz, repeatString);
		 __repeat->setSubText(repeatString.c_str());
		elm_genlist_item_update((Elm_Object_Item*)__repeat->getElmObjectItem());
	}

	//update timezone
	if (__timezone)
	{
		__timezone->setSubText(__timezoneString.c_str());
		elm_genlist_item_update((Elm_Object_Item*)__timezone->getElmObjectItem());
	}

	//update description
	if (__description)
	{
		elm_genlist_item_update((Elm_Object_Item*)__description->getElmObjectItem());
	}
}

void CalEditView::__onAddTitleField()
{
	__title = new CalDialogEditTextFieldItem(TITLE, [this](CalUnderlineEditField* editField)
	{
		char *summary = NULL;
		summary = elm_entry_utf8_to_markup(__workingCopy->getSummary());

		editField->setText(summary);
		free(summary);
		summary = NULL;

		int fontSize = __title->getSystemFontSize();
		if(abs(fontSize) > TEXT_ITEM_DEFAULT_SIZE)
		{
			editField->setFontSize(fontSize);
			editField->setGuideText(_L_G_("IDS_CLD_BODY_TITLE"), fontSize);
		}
		else
		{
			editField->setGuideText(_L_G_("IDS_CLD_BODY_TITLE"), 0);
		}

		editField->setChangeCallback([this](const char* text)
		{
			__workingCopy->setSummary(text);
		});

		editField->setEntryMaxLenReachCallback([this]()
		{
			char warningText[CAL_WARNING_MAX_LENGTH] = {0};
			snprintf(warningText, sizeof(warningText)-1, _L_("IDS_CLD_TPOP_MAXIMUM_NUMBER_OF_CHARACTERS_HPD_REACHED"), CAL_EVENT_TITLE_MAX_CHAR_LIMIT);
			notification_status_message_post(warningText);
		});

		editField->setCompleteCallback([this]()
		{
			if (__location)
			{
				__location->getEditField()->setFocusToEntry();
			}
			else if (__description)
			{
				__description->getEditField()->setFocusToEntry();
			}
		});

		if (__location || __description)
		{
			editField->setEntryReturnKeyType(CalUnderlineEditField::NEXT);
		}
		else
		{
			editField->setEntryReturnKeyType(CalUnderlineEditField::DONE);
		}

		if (__mode == CREATE)
		{
			editField->setFocusToEntry();
		}
	});

	__dialog->add(__title);
}

void CalEditView::__setTime()
{
	CalDateTime startDateTime;
	CalDateTime endDateTime;
	Eina_Bool allDay = EINA_TRUE;
	__time->getDateTime(&startDateTime, &endDateTime, &allDay);

	//adjustment time: keep same UI concept as android S5
	if(allDay != __tempStartDateTime.isAllDay()) // allday checkbox changed
	{
		WDEBUG("Allday checkbox");

		__tempStartDateTime = startDateTime;
		__tempEndDateTime = endDateTime;
		if(__tempStartDateTime > __tempEndDateTime)
		{
			notification_status_message_post(_L_("IDS_CLD_TPOP_END_TIME_SHOULD_BE_LATER_THAN_START_TIME"));
			__tempEndDateTime = __tempStartDateTime;
			__tempEndDateTime.addHours(1);
		}
	}
	else if(endDateTime == __tempEndDateTime)
	{
		WDEBUG("Edit start date time");

		auto setSameInterval = [this, allDay, &startDateTime, &endDateTime](){
			long long int start = __tempStartDateTime.getUtime();
			long long int end = __tempEndDateTime.getUtime();
			long long int interval = end - start;
			WDEBUG("Edit interval %ld",interval);
			if(allDay)
			{
				int intervalDay = interval / (24 * 60 * 60);
				if(interval % (24 * 60 * 60) != 0)
					intervalDay++;

				WDEBUG("Edit intervalDay %d",intervalDay);
				endDateTime = startDateTime;
				endDateTime.addDays(intervalDay);
			}
			else
			{
				endDateTime = startDateTime;
				endDateTime.addSeconds(interval);
			}
		};

		if (startDateTime >= endDateTime)
		{
			notification_status_message_post(_L_("IDS_CLD_TPOP_END_TIME_SHOULD_BE_LATER_THAN_START_TIME"));
			if(!allDay)
			{
				setSameInterval();
			}
			else
			{
				endDateTime = startDateTime;
			}
		}
		else
		{
			setSameInterval();
		}

		__tempStartDateTime = startDateTime;
		__tempEndDateTime = endDateTime;

	}
	else if(startDateTime == __tempStartDateTime)
	{
		WDEBUG("Edit end date time");

		if (endDateTime <= startDateTime)
		{
			notification_status_message_post(_L_("IDS_CLD_TPOP_END_TIME_SHOULD_BE_LATER_THAN_START_TIME"));
			if(!allDay)
			{
				endDateTime = startDateTime;
				endDateTime.addHours(1);
			}
			else
			{
				endDateTime = startDateTime;
			}
		}

		__tempStartDateTime = startDateTime;
		__tempEndDateTime = endDateTime;
	}
	else
	{
		__tempStartDateTime = startDateTime;
		__tempEndDateTime = endDateTime;
	}

	__time->updateStartAndEndTime(__tempStartDateTime, __tempEndDateTime);
	__workingCopy->setStart(__tempStartDateTime);
	__workingCopy->setEnd(__tempEndDateTime);
	__isChanged = true;

	__updateMoreButtonStatus();
}

void CalEditView::__setFocusToField(CalDialogControl::Item *field)
{
	if(field == __location || field == __description)
	{
	__idler = ecore_idler_add([](void* data)
		{
			CalDialogEditOptionalTextFieldItem *self = (CalDialogEditOptionalTextFieldItem*) data;
			self->getEditField()->setFocusToEntry();
			return ECORE_CALLBACK_CANCEL;
		}, field);
	}
	else if(field == __title)
	{
		__idler = ecore_idler_add([](void* data)
			{
				CalDialogEditTextFieldItem *self = (CalDialogEditTextFieldItem*) data;
				self->getEditField()->setFocusToEntry();
				return ECORE_CALLBACK_CANCEL;
			}, field);
	}
}

void CalEditView::__onAddTimeField()
{
	CalDateTime startDateTime, endDateTime;
	__workingCopy->getStart(startDateTime);
	__workingCopy->getEnd(endDateTime);
	__tempStartDateTime = startDateTime;
	__tempEndDateTime = endDateTime;

	std::string st, en;
	startDateTime.getString(st);
	endDateTime.getString(en);
	WDEBUG("startDateTime : %s; endDateTime : %s", st.c_str(), en.c_str());

	__time = new CalDialogEditDateTimeItem(TIME, startDateTime, endDateTime);
	__time->setChangeCb([this](CalDateTime &startDateTime, CalDateTime &endDateTime){
		Eina_Bool isAllDay = EINA_TRUE;
		__time->getDateTime(NULL, NULL, &isAllDay);
		if (__timezone && isAllDay)
		{
			WDEBUG("Remove timezone");
			__dialog->remove(__timezone);
			__timezone = NULL;
		}

		__setTime();
	});

	__time->setDateButtonClickedCb([this](Evas_Object* dateTime) {
		__createDatePickerPopup(dateTime);
		__isChanged = true;
	});

	__time->setTimeButtonClickedCb([this](Evas_Object* dateTime) {
		__createTimePickerPopup(dateTime);
		__isChanged = true;
	});

	const char* tz = __workingCopy->getTimeZone();
	if (tz)
	{
		__time->setTimeZone(tz);
	}

	__dialog->add(__time);
}

void CalEditView::__onAddMoreField()
{
	__more = new CalDialogEditMoreItem(
		[this](CalDialogEditMoreItem::ButtonType buttonType, Evas_Object* button) {
			if (buttonType == CalDialogEditMoreItem::LOCATION) {
				__onAddLocationButton();
				if(__location)
					elm_genlist_item_bring_in((Elm_Object_Item*)__location->getElmObjectItem(), ELM_GENLIST_ITEM_SCROLLTO_TOP);
			} else if (buttonType == CalDialogEditMoreItem::REMINDER) {
				__onAddReminderButton();
			} else if (buttonType == CalDialogEditMoreItem::REPEAT) {
				__onAddRepeatButton();
			} else if (buttonType == CalDialogEditMoreItem::MORE) {
				__onAddMoreButton();
			} else {
				WERROR("Invalid button is clicked. : %d", buttonType);
			}
			__isChanged = true;
		},
		MORE,
		__location != NULL,
		__repeat != NULL,
		__workingCopy->getRemindersCount() >= MAX_REMINDER_COUNT
	);

	if(__more)
	{
		__dialog->add(__more);
	}
}

Evas_Object* CalEditView::onCreate(Evas_Object* parent, void* viewParam)
{
	WENTER();

	CalEditDialogControl* dialog = new CalEditDialogControl;
	__dialog = dialog;
	dialog->create(parent, NULL);
	Evas_Object *genlist = dialog->getEvasObj();
	elm_genlist_highlight_mode_set(genlist, EINA_FALSE);

	evas_object_smart_callback_add(genlist, "realized", [](void* data, Evas_Object* obj, void* event_info) {
	 	CalEditView* self = (CalEditView*)data;
		self->__updateMoreItemButtonStatus();/*for location, reminder, repeat 3 buttons*/
		self->__updateMoreButtonStatus(); /*for more button*/
	 }, this);

	//add title item;
	__onAddTitleField();

	//add start/end time and all day field;
	__onAddTimeField();

	// add location and other fields on edit mode or create mode with extra params from appcontrol
	__addItemsForNoEmptyField();

	if (__workingCopy->hasReminder())
	{
		__addAllReminderItems();
	}

	// add more button;
	__onAddMoreField();

	__preProcessAddButtons();

	__setFocusToField(__title);

	return genlist;
}

void CalEditView::onPushed(Elm_Object_Item* naviItem)
{
	WENTER();

	if (__mode == CREATE || __mode == COPY)
	{
		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", _L_("IDS_CLD_OPT_CREATE"));
	}
	else
	{
		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", _L_("IDS_CLD_OPT_EDIT"));
	}

	Evas_Object* button = elm_button_add(getNaviframe()->getEvasObj());
	elm_object_style_set(button, "naviframe/title_left");
	evas_object_smart_callback_add(button, "clicked",
		[](void* data, Evas_Object* obj, void* event_info) {
			CalEditView* pCalEditView = (CalEditView*)data;
			pCalEditView->popOut();
		}, this
	);
	elm_object_item_part_content_set(naviItem, "title_left_btn", button);

	button = elm_button_add(getNaviframe()->getEvasObj());
	elm_object_style_set(button, "naviframe/title_right");
	evas_object_smart_callback_add(button, "clicked",
		[](void* data, Evas_Object* obj, void* event_info) {
			CalEditView* pCalEditView = (CalEditView*)data;
			pCalEditView->__isPrepareExit = true;
			pCalEditView->__onSave();
			pCalEditView->__isDiscard = true;
			if (((CalNaviframe*)pCalEditView->getNaviframe())->getTopView() == pCalEditView)
			{
				pCalEditView->popOut();
			}
			else
			{
				pCalEditView->destroy();
			}
		}, this
	);
	elm_object_item_part_content_set(naviItem, "title_right_btn", button);

	evas_object_smart_callback_add(getNaviframe()->getEvasObj(), "transition,finished", __onNaviframeTransitionFinished, this);
}

void CalEditView::__onNaviframeTransitionFinished(void* data, Evas_Object* obj, void* eventInfo)
{
	CalEditView* self = (CalEditView* )data;
	self->__updateMoreItemButtonStatus();/*for location, reminder, repeat 3 buttons*/
	self->__updateMoreButtonStatus(); /*for more button*/
	if (self->__title && self->__title->getEditField())
	{
		self->__title->getEditField()->setFocusToEntry();
	}
	evas_object_smart_callback_del(self->getNaviframe()->getEvasObj(), "transition,finished", __onNaviframeTransitionFinished);
}

void CalEditView::onDestroy()
{
	CalAppControlLauncher::getInstance().terminate();
}

void CalEditView::onEvent(const CalEvent& event)
{
	WENTER();
	WDEBUG("type = %u, source = %u", event.type, event.source);
	switch (event.type)
	{
		case CalEvent::DB_CHANGED:
			break;
		case CalEvent::TIME_CHANGED:
		case CalEvent::LANGUAGE_CHANGED:
		case CalEvent::SETTING_CHANGED:
		case CalEvent::CONTACTS_EMAIL_CHANGED:
			if (__mode == CREATE || __mode == COPY)
			{
				elm_object_item_part_text_set(getNaviItem(), "elm.text.title", _L_("IDS_CLD_OPT_CREATE"));
			}
			else
			{
				elm_object_item_part_text_set(getNaviItem(), "elm.text.title", _L_("IDS_CLD_OPT_EDIT"));
			}
			elm_object_text_set(elm_object_item_part_content_get(getNaviItem(), "title_left_btn"), _L_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));
			elm_object_text_set(elm_object_item_part_content_get(getNaviItem(), "title_right_btn"), _L_("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));
			__update();
			break;
		case CalEvent::APP_RESUMED:
			CalEditField::resetFocusPolicy();
			break;
		case CalEvent::APP_PAUSED:
			break;
		case CalEvent::BOOK_DB_CHANGED:
			destroyPopup();
			break;
		default:
			WERROR("invalid type = %u", event.type);
			break;
	}
}

void CalEditView::__toastPopupWarning(const bool isOverlapped, const int ret)
{
	if (ret != 0)
	{
		notification_status_message_post(_L_("IDS_CLD_TPOP_FAILED_TO_SAVE_EVENT_UNKNOWN_ERROR_OCCURRED"));
		return;
	}

	switch (__mode)
	{
		case EDIT:
		case EDIT_INSTANCE:
		case EDIT_THIS_AND_FURTURE:
		case EDIT_EXTERNAL:
			notification_status_message_post(_L_("IDS_CLD_TPOP_EVENT_SAVED"));
			break;
		case CREATE:
		case COPY:
			if (isOverlapped == true)
			{
				notification_status_message_post(_L_("IDS_CLD_TPOP_EVENT_OVERLAPS_WITH_ANOTHER_EVENT"));
			}
			else
			{
				notification_status_message_post(_L_("IDS_CLD_TPOP_EVENT_SAVED"));
			}
			break;
	}
}

void CalEditView::__onSave()
{
	WENTER();
	int ret = 0;
	bool isOverlapped = false;
	int eventId = 0;

	if (__more)
	{
		elm_object_focus_set(__more->getButton(CalDialogEditMoreItem::MORE), true);/*to avoid keyboard is shown again*/
	}

	const char *summary = __workingCopy->getSummary();
	if (!(summary && strlen(summary)))
	{
		__workingCopy->setSummary(_L_("IDS_CLD_MBODY_MY_EVENT"));
	}

	CalDateTime dateTime;
	__workingCopy->getStart(dateTime);
	if(__time->isAllDay() &&
			(__mode == CREATE || (__mode != COPY && __time->isDateChanged())))
	{
		CalDateTime endDateTime;
		__workingCopy->getEnd(endDateTime);
		endDateTime.addDays(1);
		__workingCopy->setEnd(endDateTime);
	}

	switch(__mode)
	{
		case CREATE:
		case COPY:
			isOverlapped = CalDataManager::getInstance().isOverlapped(*__workingCopy);
			ret = CalDataManager::getInstance().insertSchedule(*__workingCopy, &eventId);
			break;
		case EDIT:
		case EDIT_EXTERNAL:
			ret = CalDataManager::getInstance().updateSchedule(CalDataManager::ALL, *__inputSchedule, *__workingCopy, &eventId);
			break;
		case EDIT_INSTANCE:
			ret = CalDataManager::getInstance().updateSchedule(CalDataManager::ONLY_THIS, *__inputSchedule, *__workingCopy, &eventId);
			break;
		case EDIT_THIS_AND_FURTURE:
			ret = CalDataManager::getInstance().updateSchedule(CalDataManager::THIS_AND_FUTURE, *__inputSchedule, *__workingCopy, &eventId);
			break;
	}

	if (!ret)
	{
		CalSettingsManager::getInstance().setLastUsedCalendar(__workingCopy->getBookId());
		__postProcessAddButtons();
	}

	__toastPopupWarning(isOverlapped, ret);

	if (__savedCb)
	{
		__savedCb(eventId);
	}
}

bool CalEditView::onPop()
{
	Ecore_IMF_Context *ctx = ecore_imf_context_add(ecore_imf_context_default_id_get());
	Ecore_IMF_Input_Panel_State imf_state = ecore_imf_context_input_panel_state_get(ctx);
	ecore_imf_context_del(ctx);

	if (imf_state != ECORE_IMF_INPUT_PANEL_STATE_HIDE)
	{
		// Hide IME if it exists.
		// When pop naviframe with the keypad shown, naviframe pop effect can be cracked. (the animation works for only upper side of composer)
		// It's because keypad process and calendar process works asynchronously.
		ecore_imf_input_panel_hide();
	}

	if (__isDiscard)
	{
		return true;
	}

	if(__isChanged || __isTitleChanged())
	{
		WPopup *popup = new WPopup(
				_L_("IDS_CLD_HEADER_DISCARD_CHANGES_ABB"),
				_L_("IDS_CLD_POP_ALL_CHANGES_WILL_BE_DISCARDED"));

		popup->addButton(_L_("IDS_CLD_BUTTON_CANCEL"), NULL);
		popup->addButton(_L_("IDS_CLD_BUTTON_DISCARD_ABB"), [this](bool* destroyPopup){
			elm_object_focus_set(__more->getButton(CalDialogEditMoreItem::MORE), true);
			__isDiscard = true;
			popOut();
			*destroyPopup = false;
		});
		attachPopup(popup);
		return false;
	}

	return true;
}

void CalEditView::__createDatePickerPopup(Evas_Object* dateTime)
{
	Eina_Bool allDay = false;
	CalDateTime startInitialDate, startDateTime, endDateTime;
	__time->getDateTime(&startDateTime, &endDateTime, &allDay);

	if(__time->isStartSelected())
	{
		startInitialDate = startDateTime;
	}
	else
	{
		startInitialDate = endDateTime;
	}

	CalDatePickerPopup* popup = new CalDatePickerPopup(startInitialDate);
	popup->setChangeCb([this, dateTime](CalDate& newdate){
		CalDateTime currentDate(newdate.getYear(), newdate.getMonth(), newdate.getMday());
		__time->setDate(currentDate);
		__setTime();
	});

	popup->create(getNaviframe()->getEvasObj(), NULL);
	attachPopup(popup);
}

void CalEditView::__createTimePickerPopup(Evas_Object* dateTime)
{
	CalDateTime startInitialTime;

	if(__time->isStartSelected())
	{
		__workingCopy->getStart(startInitialTime);
	}
	else
	{
		__workingCopy->getEnd(startInitialTime);
	}

	CalTimePickerPopup* popup = new CalTimePickerPopup(dateTime, _L_("IDS_CLD_HEADER_SET_TIME"), startInitialTime, false);
	popup->setChangeCb([this, dateTime](CalDateTime& newDateTime){
		__time->setTime(newDateTime);
		__setTime();
	});

	popup->create(getNaviframe()->getEvasObj(), NULL);
	attachPopup(popup);
}

void CalEditView::__preProcessAddButtons()
{
	WENTER();
	if (__mode != CREATE)
	{
		return;
	}

	if (__model.isOn(CalEditModel::LOCATION))
	{
		__onAddLocationField();
		__setFocusToField(__location);
	}

	if (__model.isOn(CalEditModel::REMINDER))
	{
		__addReminderTitleItem();
		__reminderItems[0] = __createReminderOffItem();
		__dialog->add(__reminderItems[0]);
	}

	if (__model.isOn(CalEditModel::REPEAT))
	{
		__addRepeatOff();
	}

	if (__model.isOn(CalEditModel::DESCRIPTION))
	{
		__onAddDescriptionField();
	}

	if (__model.isOn(CalEditModel::TIME_ZONE))
	{
		__onAddTimezoneField();
	}
}

void CalEditView::__postProcessAddButtons()
{
	if (__mode != CREATE)
	{
		return;
	}

	// location
	__model.setOn(CalEditModel::LOCATION, (__location && __workingCopy->getLocation() && strlen(__workingCopy->getLocation())));

	// reminder !
	__model.setOn(CalEditModel::REMINDER, (__reminderTitleItem && __workingCopy->hasReminder()));

	// repeat
	__model.setOn(CalEditModel::REPEAT, (__repeat && __workingCopy->hasRepeat()));

	// timezone
	std::string timeZone;
	CalLocaleManager::getInstance().getTimeZone(timeZone);
	__model.setOn(CalEditModel::TIME_ZONE, (__timezone && __workingCopy->getTimeZone() && strcmp(__workingCopy->getTimeZone(), timeZone.c_str())));
}
