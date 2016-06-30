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

#ifndef _CAL_DETAIL_VIEW_H_
#define _CAL_DETAIL_VIEW_H_

#include "CalView.h"
#include "CalSchedule.h"
#include "CalDataManager.h"
#include "CalDialogControl.h"
#include "CalScheduleReminder.h"
#include "CalDialogDetailSummaryItem.h"
#include "CalDialogDetailEventTimeItem.h"
#include "CalDialogDetailDescriptionItem.h"

class CalDetailView : public CalView
{
public:
	enum MenuState
	{
		MENU_ENABLED,
		MENU_DISABLED
	};

public:
	CalDetailView(const std::shared_ptr<CalSchedule> schedule, MenuState state = CalDetailView::MENU_ENABLED, bool isTitleBtnsVisible = false);
	virtual ~CalDetailView();
private:
	WDISABLE_COPY_AND_ASSIGN(CalDetailView);
	struct PersonDetails
	{
		int id;
		std::string image;
	};
private:

	/**
	 * Executes the create action.
	 *
	 * @param [in]	parent   	If non-null, the parent.
	 * @param [in]	viewParam	If non-null, the view parameter.
	 *
	 * @return	null if it fails, else - created Evas_Object*.
	 */
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);

	/**
	 * Executes the pushed action. This function is called when our view was successfully pushed into naviframe container.
	 *
	 * @param [in]	naviItem	If non-null, the navi item.
	 */
	virtual void onPushed(Elm_Object_Item* naviItem);

	/**
	 * Executes the menu button action.
	 */
	virtual void onMenuButton();

	/**
	 * Executes some action, depending on event it receives.
	 *
	 * @param	event	The event.
	 */
	virtual void onEvent(const CalEvent& event);

	/**
	 * Gets reminder value.
	 *
	 * @param	reminder	The reminder.
	 *
	 * @return	The reminder value.
	 */
	int __getReminderValue(CalScheduleReminder reminder);

	/**
	 * Gets person identifier from event identifier.
	 *
	 * @param		eventId	 	Identifier for the event.
	 * @param [in]	image		The image.
	 *
	 * @return		The person identifier from event identifier.
	 */
	int __getPersonIdFromEventId(const int eventId, std::string& image);

	/**
	 * Change theme of the view.
	 *
	 * @param [in]	data	If non-null, the data is sent as paramenter. Data contains information about layout applied to the container.
	 */
	static void __changeTheme(void *data);

private:
	enum CalDetailMode
	{
		REPLY = 0,
		REPLAY_ALL,
		FORWARD,
	};
	enum CalPhonelMode
	{
		CALL = 0,
		SEND_MESSAGE,
		ADD_TO_CONTACTS,
		COPY_TO_CLIPBOARD,
	};

	/**
	 * Edits this object.
	 */
	void __edit();

	/**
	 * Processes 'onSave' event.
	 */
	void __save();

	/**
	 * Edit repeat.
	 *
	 * @param	mode	The mode.
	 */
	void __editRepeat(int mode);

	/**
	 * Deletes this object.
	 */
	void __delete();

	/**
	 * Updates this object.
	 */
	void __update();

	/**
	 * Share by vcs.
	 */
	void __shareByVcs();

	/**
	 * Share by text.
	 */
	void __shareByText();

	/**
	 * Updates the given newId.
	 *
	 * @param	newId	Identifier for the new.
	 */
	void __update(int newId);

	/**
	 * Callback, called when the dialog delete.
	 *
	 * @param [in]	data	  	If non-null, the data.
	 * @param [in]	e		  	If non-null, the Evas to process.
	 * @param [in]	obj		  	If non-null, the object.
	 * @param [in]	event_info	If non-null, information describing the event.
	 */
	static void __dialogDeleteCallback(void* data, Evas* e, Evas_Object* obj, void* event_info);

private:
	std::shared_ptr<CalSchedule> __inputSchedule;
	std::shared_ptr<CalSchedule> __workingCopy;
	CalDialogControl* __dialog;
	CalDialogDetailSummaryItem* __title;
	CalDialogDetailEventTimeItem* __time;
	CalDialogDetailDescriptionItem* __description;
	int __radioIndex;
	bool __isCheck;
	MenuState __menuState;
	bool __isTitleBtnsVisible;
};

#endif

