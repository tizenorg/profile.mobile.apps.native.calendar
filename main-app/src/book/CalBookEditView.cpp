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

#include <notification.h>
#include <memory>
#include "CalBookEditView.h"
#include "CalCommon.h"
#include "CalBook.h"
#include "CalDialogControl.h"
#include "CalDialogBookEditItem.h"
#include "CalBookManager.h"
#include "CalDialogBookLocalItem.h"
#include "CalDialogColorSelectorItem.h"
#include "WPopup.h"
#include "CalEventManager.h"

CalBookEditView::CalBookEditView(CalBookMode itemMode, const std::shared_ptr<CalBook>& book)
	: CalView("CalBookEditView")
	, __book(NULL)
	, __editItem(NULL)
	, __saveButton(NULL)
	, __bookName(NULL)
	, __dialog(NULL)
{
	__book = book;
	__itemMode = itemMode;
	__isDiscard = false;
	__isPrepareExit = false;
}

CalBookEditView::~CalBookEditView()
{
	if(__bookName)
		free(__bookName);
}

/**
 * Removes the space in string end described by str.
 *
 * @param [in]	str	If non-null, the string.
 */
void CalBookEditView::__removeSpaceInStrEnd(char *str)
{
	int i = 0;
	int len = 0;

	len = CAL_STRLEN(str);
	if (len) {
		for (i = len - 1; i >= 0; i--)
		{
			if (!isspace(str[i]))
				break;
		}
		str[i + 1] = '\0';
	}
}

/**
 * Gets edit item text.
 */
void CalBookEditView::__getEditItemText()
{
	if(__bookName)
		CAL_FREE(__bookName);

	__bookName = g_strdup(__editItem->getTitle());
	__removeSpaceInStrEnd(__bookName);
}

/**
 * Updates this object.
 */
void CalBookEditView::__update()
{
	int r, g, b, a;
	__editItem->getColor(r, g, b, a);

	__getEditItemText();

	if (!CAL_STRLEN(__bookName)
		|| (__itemMode == CREATE && CalBookManager::getInstance().findBook(__bookName))
		|| (__itemMode == EDIT && CalBookManager::getInstance().findBook(__bookName,r, g, b, a)))
	{
		elm_object_disabled_set(__saveButton, EINA_TRUE);
	} else {
		elm_object_disabled_set(__saveButton, EINA_FALSE);
	}
}

/**
 * Executes the create action.
 *
 * @param [in]	parent   	If non-null, the parent.
 * @param [in]	viewParam	If non-null, the view parameter.
 *
 * @return	null if it fails, else an Evas_Object*.
 */
Evas_Object* CalBookEditView::onCreate(Evas_Object* parent, void* viewParam)
{
	CalDialogControl *dialog = new CalDialogControl;
	dialog->create(parent, NULL);
	Evas_Object *genlist = dialog->getEvasObj();
	elm_genlist_homogeneous_set(genlist, EINA_FALSE);
	elm_genlist_mode_set(genlist, ELM_LIST_SCROLL);

	__dialog = dialog;
	__updateView();

	return genlist;
}

/**
 * Updates the view.
 */
void CalBookEditView::__updateView()
{
	destroyPopup();

	if (__isPrepareExit) {
		return; /*refer to edit view solution. otherwise, db changed->genlist will be updated before view is destroyed.*/
	}

	if (getNaviItem()) {
		elm_object_text_set(elm_object_item_part_content_get(getNaviItem(), "title_left_btn"), _L_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));
		elm_object_text_set(elm_object_item_part_content_get(getNaviItem(), "title_right_btn"), _L_("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));

		if(__itemMode == CREATE)
			elm_object_item_part_text_set(getNaviItem(), "elm.text.title", _L_("IDS_CLD_OPT_CREATE"));
		else
			elm_object_item_part_text_set(getNaviItem(), "elm.text.title", _L_("IDS_CLD_OPT_EDIT"));
	}

	elm_genlist_clear(__dialog->getEvasObj());

	__editItem = NULL;
	if (!__book) {
		__itemMode = CREATE;
		__editItem = new CalDialogBookEditItem(NULL, _L_S_("IDS_CLD_BODY_CALENDAR_M_APPLICATION_NAME_ABB"));
	} else {
		__itemMode = EDIT;
		__editItem = new CalDialogBookEditItem(__book->getName(), _L_S_("IDS_CLD_BODY_CALENDAR_M_APPLICATION_NAME_ABB"), __book);
	}

	__editItem->setTitleChangedCb([this] () { __update(); });
	__editItem->setEntryMaxLenReachCb([this]()
	{
		char warningText[CAL_WARNING_MAX_LENGTH] = {0};
		snprintf(warningText, sizeof(warningText)-1, _L_("IDS_CLD_TPOP_MAXIMUM_NUMBER_OF_CHARACTERS_HPD_REACHED"), CAL_EVENT_TITLE_MAX_CHAR_LIMIT);
		notification_status_message_post(warningText);
	});
	__dialog->add(__editItem);


	int r, g, b, a;
	__editItem->getColor(r, g, b, a);
	CalDialogColorSelectorItem* colorSelector =	new CalDialogColorSelectorItem(r, g, b, a);
	colorSelector->setChangedCb([this](int& r, int& g, int& b, int& a){
		__editItem->setColor(r, g, b, a);
		__update();
	});

	Elm_Object_Item *item = __dialog->add(colorSelector);
	if (item)
		elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

}

/**
 * Executes the save action.
 */
void CalBookEditView::__onSave()
{
	CalEventManager::getInstance().block(this);
	int r, g, b, a;
	if (__itemMode == CREATE) {
		std::shared_ptr<CalBook> instance(new CalBook());
		__editItem->getColor(r, g, b, a);
		instance->setColor(r, g, b, a);
		instance->setName(__bookName);
		instance->setAccountId(_CALENDAR_LOCAL_ACCOUNT_ID);
		instance->setStoreType(CALENDAR_BOOK_TYPE_EVENT);
		instance->setVisibility(true);

		CalBookManager::getInstance().insertBook(instance);
	} else {
		__book->getColor(r,g,b,a);
		__editItem->getColor(r, g, b, a);
		__book->setName(__editItem->getTitle());
		__book->setColor(r, g, b, a);
		CalBookManager::getInstance().updateBook(__book);
	}
}

/**
 * Executes the pop action.
 *
 * @return	true if it succeeds, false if it fails.
 */
bool CalBookEditView::onPop()
{
	if (__isDiscard) {
		return true;
	}

	if(elm_object_disabled_get(__saveButton))
		return true;

	WPopup *popup = new WPopup(
			_L_("IDS_CLD_HEADER_DISCARD_CHANGES_ABB"),
			_L_("IDS_CLD_POP_ALL_CHANGES_WILL_BE_DISCARDED"));

	popup->addButton(_L_("IDS_CLD_BUTTON_CANCEL"), NULL);
	popup->addButton(_L_("IDS_CLD_BUTTON_DISCARD_ABB"), [this](bool* destroyPopup){
		__isDiscard = true;
		popOut();
		*destroyPopup = false;
	});
	attachPopup( popup );

	return false;
}

/**
 * Executes the pushed action.
 *
 * @param [in]	naviItem	If non-null, the navi item.
 */
void CalBookEditView::onPushed(Elm_Object_Item* naviItem)
{
	Evas_Object* button = elm_button_add(getNaviframe()->getEvasObj());
	elm_object_style_set(button, "naviframe/title_left");
	elm_object_text_set(button, _L_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));
	evas_object_smart_callback_add(button, "clicked",
		[](void* data, Evas_Object* obj, void* event_info){
			CalBookEditView* pCalCreateCalendarView = (CalBookEditView*)data;
			pCalCreateCalendarView->popOut();
		},this
	);
	elm_object_item_part_content_set(naviItem, "title_left_btn", button);

	button = elm_button_add(getNaviframe()->getEvasObj());
	elm_object_style_set(button, "naviframe/title_right");
	elm_object_text_set(button, _L_("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));
	evas_object_smart_callback_add(button, "clicked",
		[](void* data, Evas_Object* obj, void* event_info){
		CalBookEditView* pCalCreateCalendarView = (CalBookEditView*)data;
		pCalCreateCalendarView->__isPrepareExit = true;
		pCalCreateCalendarView->__onSave();
		pCalCreateCalendarView->__isDiscard = true;
		pCalCreateCalendarView->popOut();
		}, this
	);
	elm_object_item_part_content_set(naviItem, "title_right_btn", button);

	__saveButton = button;
	elm_object_disabled_set(__saveButton, EINA_TRUE);

	if(__itemMode == CREATE)
		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", _L_("IDS_CLD_OPT_CREATE"));
	else
		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", _L_("IDS_CLD_OPT_EDIT"));

	evas_object_smart_callback_add(getNaviframe()->getEvasObj(), "transition,finished",
		CalBookEditView::__transitionFinishedCb, this);
}

/**
 * Executes the event action.
 *
 * @param	event	The event.
 */
void CalBookEditView::onEvent(const CalEvent& event)
{
	WENTER();
	WDEBUG("type = %u, source = %u", event.type, event.source);
	switch (event.type) {
		case CalEvent::LANGUAGE_CHANGED:
		case CalEvent::BOOK_DB_CHANGED:
		case CalEvent::TIME_CHANGED:
		case CalEvent::SETTING_CHANGED:
			{
				__updateView();
			}
			break;
		default:
			WERROR("invalid type = %u", event.type);
			break;
	}
}

/**
 * Transition finished cb.
 *
 * @param [in]	data	 	If non-null, the data.
 * @param [in]	obj		 	If non-null, the object.
 * @param [in]	eventInfo	If non-null, information describing the event.
 */
void CalBookEditView::__transitionFinishedCb(void* data, Evas_Object* obj, void* eventInfo)
{
	CalBookEditView* self = (CalBookEditView* )data;
	self->__editItem->setFocusToEntry();
	evas_object_smart_callback_del_full(self->getNaviframe()->getEvasObj(), "transition,finished",
			CalBookEditView::__transitionFinishedCb, self);
}

/**
 * Executes the destroy action.
 */
void CalBookEditView::onDestroy()
{
}
