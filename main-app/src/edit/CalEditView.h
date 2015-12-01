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

#ifndef _CAL_EDIT_VIEW_H_
#define _CAL_EDIT_VIEW_H_

#include <memory>
#include <functional>
#include "CalView.h"
#include "CalSchedule.h"
#include "CalEditDialogControl.h"
#include "CalDialogEditOptionalTextFieldItem.h"
#include "CalDialogEditTwoTextRemoveIconItem.h"
#include "CalDialogEditOneTextRemoveIconItem.h"
#include "CalDialogEditOneTextNoRemoveIconItem.h"
#include "CalEditReminderPopup.h"
#include "CalEditModel.h"
#include "CalDialogEditMoreItem.h"

#define MAX_REMINDER_COUNT 5

class CalDialogEditDateTimeItem;

class CalEditView : public CalView
{
public:
	enum CalEditMode
	{
		CREATE,
		EDIT,
		COPY,
		EDIT_INSTANCE,
		EDIT_THIS_AND_FURTURE,
		EDIT_EXTERNAL,
	};

	enum CalEditSortIndex
	{
		TITLE = 1,
		TIME,
		SAVETO,
		LOCATION,
		REMINDERTITLE,
		REMINDER1,
		REMINDER2,
		REMINDER3,
		REMINDER4,
		REMINDER5,
		REPEAT,
		DESCRIPTION,
		TIMEZONE,
		MORE,
		MORE_DESCRIPTION,
		MORE_TIMEZONE
	};

public:
	CalEditView();
	CalEditView(const CalDate& focusedDate);
	CalEditView(std::shared_ptr<CalSchedule> schedule, CalEditMode mode = CREATE,
	            std::function<void (int newId)> savedCb = nullptr, bool setLastUsedCalendar = true);
	virtual ~CalEditView();

protected:
	virtual bool onPop();

private:
	WDISABLE_COPY_AND_ASSIGN(CalEditView);

	/**
	 * Initializes this object.
	 */
	void __initialize();

	/**
	 * Creates repeat popup content.
	 *
	 * @param [in]	parent	If non-null, the parent.
	 * @param		repeat		  	The repeat.
	 *
	 * @return		null if it fails, else the new repeat popup content.
	 */
	Evas_Object* createRepeatPopupContent(Evas_Object *parent, const CalScheduleRepeat repeat);

	/**
	 * Creates repeat popup.
	 *
	 * @param [in]	naviframe	If non-null, the naviframe.
	 * @param [in]	repeat		The repeat.
	 * @param		doneCb		The done cb.
	 */
	void createRepeatPopup(WNaviframe *naviframe, CalScheduleRepeat &repeat, const std::function<void (const CalScheduleRepeat)>& doneCb);

	/**
	 * Executes the add title field action.
	 */
	void __onAddTitleField();

	/**
	 * Executes the add time field action.
	 */
	void __onAddTimeField();

	/**
	 * Executes the add more field action.
	 */
	void __onAddMoreField();

	/**
	 * Executes the add location field action.
	 */
	void __onAddLocationField();

	/**
	 * Executes the add location button action.
	 */
	void __onAddLocationButton();

	/**
	 * Executes the add repeat button action.
	 */
	void __onAddRepeatButton();

	/**
	 * Executes the add reminder button action.
	 */
	void __onAddReminderButton();

	/**
	 * Executes the add more button action.
	 */
	void __onAddMoreButton();

	/**
	 * Clears the more sub items.
	 */
	void __clearMoreSubItems();

	/**
	 * Executes the add timezone field action.
	 */
	void __onAddTimezoneField();

	/**
	 * Executes the add description field action.
	 */
	void __onAddDescriptionField();

	/**
	 * Adds repeat.
	 */
	void __addRepeat();

	/**
	 * Adds repeat off.
	 */
	void __addRepeatOff();

	/**
	 * Adds a reminder.
	 *
	 * @param	reminder	The reminder.
	 */
	void __addReminder(const CalScheduleReminder& reminder);

	/**
	 * Adds all reminder items.
	 */
	void __addAllReminderItems();

	/**
	 * Removes all reminder items.
	 */
	void __removeAllReminderItems();

	/**
	 * Reminder remove cb.
	 *
	 * @param [in]	reminderItem	If non-null, the reminder item.
	 */
	void __reminderRemoveCb(CalDialogEditOneTextRemoveIconItem* reminderItem);

	/**
	 * Adds reminder title item.
	 */
	void __addReminderTitleItem();

	/**
	 * Removes the reminder title item.
	 */
	void __removeReminderTitleItem();

	/**
	 * Gets reminder index from interval.
	 *
	 * @param	reminderInterval	The reminder interval.
	 *
	 * @return	The reminder index from interval.
	 */
	int  __getReminderIndexFromInterval(const int reminderInterval);

	/**
	 * Toast popup warning.
	 *
	 * @param	isOverlapped	The is overlapped.
	 * @param	ret				The ret.
	 */
	void __toastPopupWarning(const bool isOverlapped, const int ret);

	/**
	 * Executes the save action.
	 */
	void __onSave();

	/**
	 * Adds items for no empty field.
	 */
	void __addItemsForNoEmptyField();

	/**
	 * Updates this object.
	 */
	void __update();

	/**
	 * Sets the time.
	 */
	void __setTime();

	/**
	 * Sets focus to field.
	 *
	 * @param	field	Text field.
	 */
	void __setFocusToField(CalDialogControl::Item *field);

	/**
	 * Creates reminder item.
	 *
	 * @param	reminder 	The reminder.
	 * @param	sortIndex	Zero-based index of the sort.
	 *
	 * @return	null if it fails, else the new reminder item.
	 */
	CalDialogEditOneTextRemoveIconItem* __createReminderItem(const CalScheduleReminder& reminder, int sortIndex);

	/**
	 * Creates reminder off item.
	 *
	 * @return	null if it fails, else the new reminder off item.
	 */
	CalDialogEditOneTextRemoveIconItem* __createReminderOffItem();

	/**
	 * Creates date picker popup.
	 *
	 * @param [in]	dateTime	If non-null, the date time.
	 */
	void __createDatePickerPopup(Evas_Object* dateTime);

	/**
	 * Creates time picker popup.
	 *
	 * @param [in]	dateTime	If non-null, the date time.
	 */
	void __createTimePickerPopup(Evas_Object* dateTime);

	/**
	 * Applies the changes to reminder.
	 *
	 * @param			reminder		The reminder.
	 * @param	[in]	reminderItem	If non-null, the reminder item.
	 */
	void __applyChangesToReminder(const CalScheduleReminder& reminder, CalDialogEditOneTextRemoveIconItem* reminderItem);

	/**
	 * Returns true if the title is changed
	 *
	 * @return	true if title changed, false if not.
	 */
	bool __isTitleChanged();

	/**
	 * Resets all entry state described by item.
	 *
	 * @param [in]	item	If non-null, the item.
	 */
	void __resetAllEntryState(CalDialogControl::Item* item);

	/**
	 * Sets time zone.
	 *
	 * @param	tz	The tz.
	 */
	void __setTimeZone(const std::string& tz);

	/**
	 * Removes the location.
	 */
	void __removeLocation();

	/**
	 * Removes the repeat item.
	 */
	void __removeRepeatItem();

	/**
	 * Removes the description.
	 */
	void __removeDescription();

	/**
	 * Removes the timezone item.
	 */
	void __removeTimezoneItem();

	/**
	 * Pre process add buttons.
	 */
	void __preProcessAddButtons();

	/**
	 * Posts the process add buttons.
	 */
	void __postProcessAddButtons();

	/**
	 * Updates the more button status.
	 */
	void __updateMoreButtonStatus();

	/**
	 * Updates the more item button status.
	 */
	void __updateMoreItemButtonStatus();

	/**
	 * Executes the idler to free item action.
	 *
	 * @param	data	The data.
	 *
	 * @return	A gboolean.
	 */
	static gboolean __onIdlerToFreeItem(gpointer data);

	/**
	 * Executes the create action.
	 *
	 * @param [in]	parent   	If non-null, the parent.
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
	 * Executes the event action.
	 *
	 * @param	event	The event.
	 */
	virtual void onEvent(const CalEvent& event);

	/**
	 * Executes the naviframe transition finished action.
	 *
	 * @param [in]	data	 	If non-null, the data.
	 * @param [in]	obj		 	If non-null, the object.
	 * @param [in]	eventInfo	If non-null, information describing the event.
	 */
	static void __onNaviframeTransitionFinished(void* data, Evas_Object* obj, void* eventInfo);

private:
	CalEditModel __model;
	std::shared_ptr<CalSchedule> __workingCopy;
	std::shared_ptr<CalSchedule> __inputSchedule;
	CalEditMode __mode;
	bool __isChanged;
	bool __isMoreShown;
	const std::function<void (int newId)> __savedCb;
	CalEditDialogControl* __dialog;
	CalDialogEditTextFieldItem* __title;
	CalDialogEditDateTimeItem * __time;
	CalDialogEditTwoTextRemoveIconItem* __timezone;
	CalDialogEditOptionalTextFieldItem* __location;
	CalDialogEditOneTextNoRemoveIconItem* __reminderTitleItem;
	CalDialogEditOneTextRemoveIconItem* __reminderItems[MAX_REMINDER_COUNT];
	CalDialogEditTwoTextRemoveIconItem* __repeat;
	char* __currentParticipantKeyword;
	CalDialogEditOptionalTextFieldItem* __description;
	CalDialogEditMoreItem* __more;
	CalDate  __focusedDate;
	bool __isDiscard;
	int __reminderNumber;
	bool __isPrepareExit;
	int* __list;
	int __count;
	int __appendNum;
	std::string __timezoneString;
	Ecore_Timer* __idler;/*for append list item to floating box*/
};

#endif
