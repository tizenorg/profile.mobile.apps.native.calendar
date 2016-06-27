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

#include <app_control.h>
#include <vector>
#include <contacts.h>

#include "CalCommon.h"
#include "CalNaviframe.h"
#include "CalDialogControl.h"
#include "CalDataManager.h"
#include "CalBookManager.h"
#include "CalEditView.h"
#include "WMenuPopup.h"

#include "WPopup.h"
#include "CalRepeatEventModeControl.h"
#include "CalShareAsPopup.h"

#include "CalDialogDetailLocationItem.h"
#include "CalDialogDetailRepeatItem.h"
#include "CalDialogDetailReminderItem.h"
#include "CalDetailView.h"
#include "CalAppControlLauncher.h"
#include "CalCheckboxPopup.h"
#include <notification.h>
#include "CalEditModel.h"
#include "CalEventManager.h"

CalDetailView::CalDetailView(const std::shared_ptr<CalSchedule> schedule, MenuState state, bool pushLeftRightBtns) :
	CalView("CalDetailView"),
	__inputSchedule(schedule),
	__dialog(NULL),
	__title(nullptr),
	__time(nullptr),
	__description(nullptr),
	__radioIndex(1),
	__isCheck(false),
	__menuState(state),
	__pushLeftRightBtns(pushLeftRightBtns)
{
	WASSERT_EX(__inputSchedule, "The input schedule is null");
	if(0 < __inputSchedule->getIndex())
	{
		__workingCopy = CalDataManager::getInstance().getWorkingCopy(CalDataManager::ONLY_THIS, *schedule);
	}
	else
	{
		__workingCopy = schedule;
	}
}

CalDetailView::~CalDetailView()
{
	CalAppControlLauncher::getInstance().terminate();
}

/**
 * Change theme of different view parts, taking their pointers and updating them.
 *
 * @param [in]	data	If non-null, the data.
 */
void CalDetailView::__changeTheme(void *data)
{
	WENTER();
	CalDetailView* self = (CalDetailView*) data;
	if (self)
	{
		if (self->__title)
		{
			Elm_Object_Item* item = self->__title->getElmObjectItem();
			if (item)
			{
				elm_genlist_item_update(item);
			}
		}
		if (self->__time)
		{
			Elm_Object_Item* item = self->__time->getElmObjectItem();
			if (item)
			{
				elm_genlist_item_update(item);
			}
		}
		if (self->__description)
		{
			Elm_Object_Item* item = self->__description->getElmObjectItem();
			if (item)
			{
				elm_genlist_item_update(item);
			}
		}
	}
}

void CalDetailView::__dialogDeleteCallback(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
}

/**
 * Executes the create action. Here our dialog is created.
 *
 * @param [in]	parent   	If non-null, the parent.
 * @param [in]	viewParam	If non-null, the view parameter.
 *
 * @return	null if it fails, else an Evas_Object*.
 */
Evas_Object* CalDetailView::onCreate(Evas_Object* parent, void* viewParam)
{
	__dialog = new CalDialogControl;
	__dialog->create(parent, NULL);
	Evas_Object *genlist = __dialog->getEvasObj();
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_select_mode_set(genlist, ELM_OBJECT_SELECT_MODE_DEFAULT);
	__update();

	return __dialog->getEvasObj();
}

/**
 * Edits this object. Another dialog window with edit contents is called.
 */
void CalDetailView::__edit()
{
	WENTER();
	bool isRepeat = __workingCopy->hasRepeat();
	bool isException = __workingCopy->isException();

	if (isRepeat && !isException)
	{
		WHIT();
		WPopup *popup = new WPopup(_L_("IDS_CLD_OPT_EDIT"), NULL );
		WHIT();
		popup->setContent(new CalRepeatEventModeControl([this](CalDataManager::OperationMode mode) {
				__editRepeat(mode);
			}
		));
		attachPopup(popup);
	}
	else if (isRepeat && isException)
	{
		popOut();
		getNaviframe()->push(new CalEditView(__inputSchedule, CalEditView::EDIT_INSTANCE));
	}
	else
	{
		popOut();
		getNaviframe()->push(new CalEditView(__inputSchedule, CalEditView::EDIT));
	}
}

/**
 * Processes 'onSave' event.
 */
void CalDetailView::__save()
{
	__inputSchedule->setBookId(DEFAULT_EVENT_CALENDAR_BOOK_ID);
	CalDataManager::getInstance().insertSchedule(*__inputSchedule);

	notification_status_message_post(_L_("IDS_CLD_TPOP_EVENT_SAVED"));

	popOut();
}

/**
 * Edit repeat. Another dialog window is called with repeat contents.
 *
 * @param	mode	The mode.
 */
void CalDetailView::__editRepeat(int mode)
{
	switch (mode)
	{
	case CalDataManager::ONLY_THIS:
		popOut();
		getNaviframe()->push(new CalEditView(__inputSchedule, CalEditView::EDIT_INSTANCE));
		break;
	case CalDataManager::THIS_AND_FUTURE:
		popOut();
		getNaviframe()->push(new CalEditView(__inputSchedule, CalEditView::EDIT_THIS_AND_FURTURE));
		break;
	case CalDataManager::ALL:
		popOut();
		getNaviframe()->push(new CalEditView(__inputSchedule, CalEditView::EDIT));
		break;
	default:
		break;
	}
}

/**
 * Calls a popup with 2 buttons, one for cancelling deletion, and another one - for performing it.
 */
void CalDetailView::__delete()
{
	WENTER();
	if (__workingCopy->hasRepeat())
	{
		bool isException = __workingCopy->isException();
		WPopup* popup = NULL;
		if(isException)
		{
			popup = new WPopup(
				_L_("IDS_CLD_HEADER_DELETE"),
				_L_("IDS_CLD_POP_THIS_EVENT_WILL_BE_DELETED"));
				popup->addButton(_L_("IDS_CLD_BUTTON_CANCEL"), NULL);
				popup->addButton(_L_("IDS_CLD_BUTTON_DELETE_ABB"),
				[this](bool* destroyPopup) {
					CalEventManager::getInstance().block(this);
					CalDataManager::getInstance().deleteSchedule(CalDataManager::ONLY_THIS, *__inputSchedule);
					popOut();
					*destroyPopup = false;
				}
			);
		}
		else
		{
			popup = new WPopup(_L_("IDS_CLD_HEADER_DELETE"), NULL);
			popup->setContent(new CalRepeatEventModeControl(
				[this](CalDataManager::OperationMode mode) {
					CalEventManager::getInstance().block(this);
					CalDataManager::getInstance().deleteSchedule(mode, *__inputSchedule);
					popOut();
				}
			));
		}
		attachPopup(popup);
	}
	else
	{
		WPopup* popup = new WPopup(
			_L_("IDS_CLD_HEADER_DELETE"),
			_L_("IDS_CLD_POP_THIS_EVENT_WILL_BE_DELETED"));
			popup->addButton(_L_("IDS_CLD_BUTTON_CANCEL"), NULL);
			popup->addButton(_L_("IDS_CLD_BUTTON_DELETE_ABB"),
			[this](bool* destroyPopup) {
				CalEventManager::getInstance().block(this);
				CalDataManager::getInstance().deleteSchedule(*__workingCopy);
				popOut();
				*destroyPopup = false;
			}
		);
		attachPopup(popup);
	}
}

/**
 * Updates this event. Sets content for each control.
 */
void CalDetailView::__update()
{
	destroyPopup();

	Evas_Object *genlist = __dialog->getEvasObj();
	elm_genlist_clear(genlist);
	__description = NULL;

	// Summary
	std::string summary;
	__workingCopy->getDisplaySummary(-1, summary);
	WDEBUG("%s", summary.c_str());

	std::string image;
	int personId = __getPersonIdFromEventId(__workingCopy->getIndex(), image);
	WDEBUG("person id: %i", personId);

	__title = new CalDialogDetailSummaryItem(__workingCopy->getBookId(), personId, summary.c_str(), image.c_str());
	Elm_Object_Item *item = __dialog->add(__title);
	if (item)
	{
		elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}

	// Event Time
	__time = new CalDialogDetailEventTimeItem(__workingCopy);
	item = __dialog->add(__time);
	if (item)
	{
		elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}

	// Location
	std::string location;
	__workingCopy->getDisplayLocation(-1, location);
	if (!location.empty())
	{
		item = __dialog->add(new CalDialogDetailLocationItem(location.c_str()));
		if (item)
		{
			elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
		}
	}

	// Reminders
	const int count = __workingCopy->getRemindersCount();
	if (count)
	{
		std::set<CalScheduleReminder> reminders;
		for (int i = 0; i < count; i++)
		{
			CalScheduleReminder reminder;
			__workingCopy->getReminder(i, reminder);
			if(reminders.find(reminder) == reminders.end())
			{
				reminders.insert(reminder);
			}
		}

		item = __dialog->add(new CalDialogDetailReminderItem(reminders));
		if (item)
		{
			elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
		}
	}

	// Repeats
	if (__workingCopy->hasRepeat())
	{
		CalDateTime start;
		__workingCopy->getStart(start);
		const char* tz = __workingCopy->getTimeZone();
		item = __dialog->add(new CalDialogDetailRepeatItem(__workingCopy->getRepeat(), start, tz));
		if (item)
		{
			elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
		}
	}
	// Description
	std::string description;
	__workingCopy->getDisplayDescription(description);
	if (!description.empty())
	{
		__description = new CalDialogDetailDescriptionItem(description.c_str());
		item = __dialog->add(__description);
		if (item)
		{
			elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
		}
	}
}

/**
 * Gets person identifier from event identifier.
 *
 * @param		eventId		Identifier for the event.
 * @param [in]	image		The image.
 *
 * @return		The person identifier from event identifier.
 */
int CalDetailView::__getPersonIdFromEventId(const int eventId, std::string& image)
{
	int personId = -1;

	int error = calendar_connect();
	if(error != CALENDAR_ERROR_NONE)
	{
		WDEBUG("Cannot connect to Calendar Service, error: %d", error);
		return -1;
	}

	error = contacts_connect();
	if(error != CALENDAR_ERROR_NONE)
	{
		WDEBUG("Cannot connect to Contacts Service, error: %d", error);
		return -1;
	}

	const char* name = __workingCopy->getSummary();

	std::vector<PersonDetails> persons;
	contacts_query_h queryContacts = NULL;
	contacts_query_create(_contacts_contact._uri, &queryContacts);

	contacts_filter_h filterContacts = NULL;
	contacts_filter_create(_contacts_contact._uri, &filterContacts);

	contacts_filter_add_str(filterContacts, _contacts_contact.display_name, CONTACTS_MATCH_CONTAINS, name);

	contacts_query_set_filter(queryContacts, filterContacts);
	contacts_filter_destroy(filterContacts);

	contacts_list_h listContacts = NULL;
	error = contacts_db_get_records_with_query(queryContacts, 0, 0, &listContacts);
	if (error == CONTACTS_ERROR_NONE)
	{
		contacts_list_first(listContacts);

		int id = 0;
		contacts_record_h cursor = NULL;
		contacts_list_get_current_record_p(listContacts, &cursor);
		while (cursor)
		{
			id = 0;
			error = contacts_record_get_int(cursor, _contacts_contact.person_id, &id);
			if (error == CONTACTS_ERROR_NONE)
			{
				PersonDetails person;
				person.id = id;

				char* image = NULL;
				error = contacts_record_get_str_p(cursor, _contacts_contact.image_thumbnail_path, &image);
				if (error == CONTACTS_ERROR_NONE && image)
				{
					person.image = image;
				}

				persons.push_back(person);
			}


			contacts_list_next(listContacts);
			contacts_list_get_current_record_p(listContacts, &cursor);
		}

		contacts_list_destroy(listContacts, true);
	}

	contacts_query_destroy(queryContacts);

	int count = persons.size();
	if(count > 0)
	{
		int foundEventId = 0;
		int countEvents = 0;
		bool findPerson = false;
		for(int i = 0; i < count; i++)
		{
			calendar_filter_h filter = NULL;
			calendar_filter_create(_calendar_event._uri, &filter);

			calendar_filter_add_int(filter, _calendar_event.person_id, CALENDAR_MATCH_EQUAL, persons.at(i).id);

			calendar_query_h query = NULL;
			calendar_query_create(_calendar_event._uri, &query);
			calendar_query_set_filter(query, filter);

			calendar_list_h listEvents = NULL;
			calendar_db_get_records_with_query(query, 0, 0, &listEvents);

			calendar_filter_destroy(filter);
			calendar_query_destroy(query);

			countEvents = 0;
			calendar_list_get_count(listEvents, &countEvents);
			if(countEvents > 0)
			{
				calendar_record_h cursor = NULL;
				calendar_list_get_current_record_p(listEvents, &cursor);
				while (cursor)
				{
					calendar_record_get_int(cursor, _calendar_event.id, &foundEventId);
					if (error == CONTACTS_ERROR_NONE && foundEventId == eventId)
					{
						personId = persons.at(i).id;
						image = persons.at(i).image;
						findPerson = true;
						break;
					}

					calendar_list_next(listEvents);
					calendar_list_get_current_record_p(listEvents, &cursor);
				}
			}

			calendar_list_destroy(listEvents, true);
			if(findPerson)
			{
				break;
			}
		}
	}

	contacts_disconnect();
	calendar_disconnect();

	return personId;
}

/**
 * Share by vcs. In this case special file, containing distribute info, is created and sent to specific app.
 */
void CalDetailView::__shareByVcs()
{
	WENTER();
	const char *pathFormat = CalPath::getPath(CAL_VCS_FILE_TEMPLATE, CalPath::DATA).c_str();

	std::shared_ptr<CalSchedule> workingCopy = CalDataManager::getInstance().getWorkingCopy(CalDataManager::ALL, *__workingCopy);
	char* filePath = g_strdup_printf(pathFormat, workingCopy->getIndex());
	CalDataManager::getInstance().generateVcsFromSchedule(*workingCopy, filePath, true);

	app_control_h service;
	if (app_control_create(&service) == APP_CONTROL_ERROR_NONE)
	{
		char* uri = g_strdup_printf("%s%s", "file://", filePath);
		app_control_set_uri(service, uri);
		app_control_set_operation(service, APP_CONTROL_OPERATION_SHARE);
		app_control_set_launch_mode(service, APP_CONTROL_LAUNCH_MODE_GROUP);
		CalAppControlLauncher::getInstance().sendLaunchRequest(service, NULL, NULL, getEvasObj());

		g_free(uri);
	}

	g_free(filePath);

	WLEAVE();
}

/**
 * Share by text. Calls APP_CONTROL directly and sends some text info through this service.
 */
void CalDetailView::__shareByText()
{
	WENTER();

	std::shared_ptr<CalSchedule> workingCopy = CalDataManager::getInstance().getWorkingCopy(CalDataManager::ALL, *__workingCopy);

	app_control_h service;
	int r = app_control_create(&service);
	if (r != 0) {
		WERROR("error");
		return;
	}

	char* shareText = NULL;
	shareText = CalDataManager::getInstance().generateTextFromSchedule(*__workingCopy);
	app_control_add_extra_data(service, APP_CONTROL_DATA_TEXT, shareText);
	app_control_set_operation(service, APP_CONTROL_OPERATION_SHARE_TEXT);
	app_control_set_launch_mode(service, APP_CONTROL_LAUNCH_MODE_GROUP);
	CalAppControlLauncher::getInstance().sendLaunchRequest(service, NULL, NULL, getEvasObj());

	if (shareText) {
		g_free(shareText);
	}

	WLEAVE();
	return;
}

/**
 * Executes the menu button action. In this case it contains either "Share as" option or "Edit", "Delete"
 */
void CalDetailView::onMenuButton()
{
	WENTER();

	if (__menuState == MENU_DISABLED)
	{
		return;
	}

	WMenuPopup* popup = new WMenuPopup();
	popup->prepare(getWindow()->getEvasObj(), getNaviframe()->getEvasObj());

	if (__menuState == MENU_SAVEONLY)
	{
		popup->appendItem(_L_("IDS_CLD_OPT_SAVE_IN_CALENDAR_ABB"), [this]() {
			this->__save();
		});

		attachPopup(popup);
		return;
	}

	std::shared_ptr<CalBook> book = CalBookManager::getInstance().getBook(__workingCopy->getBookId());

	if (!(book && book->getIndex() == DEFAULT_BIRTHDAY_CALENDAR_BOOK_ID))
	{
		popup->appendItem(_L_("IDS_CLD_OPT_EDIT"), [this]() {
			this->__edit();
		});

		popup->appendItem(_L_("IDS_CLD_OPT_DELETE"), [this]() {
			this->__delete();
		});
	}

	popup->appendItem(_L_("IDS_CLD_HEADER_SHARE_AS"), [this]() {
		CalShareAsPopup* shareAsPop =  new CalShareAsPopup(_L_("IDS_CLD_HEADER_SHARE_AS"));
		shareAsPop->setSelectCb([this](CalShareAsPopup::shareAsType type){
			if(type == CalShareAsPopup::BY_VCS)
				__shareByVcs();
			else if(type == CalShareAsPopup::BY_TEXT)
				__shareByText();
			else
				WDEBUG("Error type");
		});
		attachPopup(shareAsPop);
	});

	attachPopup(popup);
}

/**
 * Executes the 'pushed' action.
 *
 * @param [in]	naviItem	If non-null, the navi item.
 */
void CalDetailView::onPushed(Elm_Object_Item* naviItem)
{
	WENTER();
	std::shared_ptr<CalBook>book = CalBookManager::getInstance().getBook(__workingCopy->getBookId());

	if (__pushLeftRightBtns) {
		Evas_Object* button = elm_button_add(getNaviframe()->getEvasObj());
		elm_object_style_set(button, "naviframe/title_left");
		elm_object_text_set(button, _L_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));
		evas_object_smart_callback_add(button, "clicked",
			[](void* data, Evas_Object* obj, void* eventInfo){
			CalDetailView* self = (CalDetailView*)data;
				self->popOut();
			}, this
		);
		elm_object_item_part_content_set(naviItem, "title_left_btn", button);

		button = elm_button_add(getNaviframe()->getEvasObj());
		elm_object_style_set(button, "naviframe/title_right");
		elm_object_text_set(button, _L_("IDS_CLD_ACBUTTON_SAVE"));
		evas_object_smart_callback_add(button, "clicked",
			[](void* data, Evas_Object* obj, void* eventInfo){
			CalDetailView* self = (CalDetailView*)data;
				self->__save();
			}, this
		);
		elm_object_item_part_content_set(naviItem, "title_right_btn", button);
	}
	activateMenuButton(naviItem);
}

/**
 * Processes event, received from queue, and does corresponding actions.
 *
 * @param	event	The event.
 */
void CalDetailView::onEvent(const CalEvent& event)
{
	WENTER();
	WDEBUG("type = %u, source = %u", event.type, event.source);

	switch (event.type) {
		case CalEvent::DB_CHANGED:
		{
			//first, to check if the record is deleted in db;
			std::shared_ptr<CalBook> book;
			book = CalBookManager::getInstance().getBook(__inputSchedule->getBookId());
			if (book == NULL)  // It is edited schedule is deleted, treat it as default ,and mode is changed to create
			{
				popOut();
				return;
			}
			else
			{
				__workingCopy = CalDataManager::getInstance().getUpdatedWorkingCopy(CalDataManager::ONLY_THIS, *__inputSchedule);
				if (__workingCopy)
				{
					__update();
				}
				else
				{
					if (((CalNaviframe*)getNaviframe())->getTopView() == this)
					{
						popOut();
					}
					else
					{
						destroy();
					}
				}
			}
			break;
		}
		case CalEvent::LANGUAGE_CHANGED:
		case CalEvent::TIME_CHANGED:
		case CalEvent::SETTING_CHANGED:
		case CalEvent::CONTACTS_EMAIL_CHANGED:
			__update();
			break;
		case CalEvent::APP_PAUSED:
		case CalEvent::APP_RESUMED:
			break;
		default:
			WERROR("invalid type = %u", event.type);
			break;
	}
}
