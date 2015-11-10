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
#include <notification.h>
#include "CalCommon.h"
#include "CalDataManager.h"
#include "CalInstanceSchedule.h"
#include "CalDialogEditTextFieldItem.h"
#include "CalSearchView.h"
#include "CalDetailView.h"
#include "WMenuPopup.h"
#include "CalListModelFactory.h"

CalSearchView::CalSearchView() : CalView("CalSearchView"),
	__list(nullptr),
	__entryText(strdup("")),
	__entry(nullptr),
	__focusDate(),
	__isNoContents(false),
	__noContents(nullptr),
	__searchBar(nullptr),
	__forwardModel(nullptr),
	__backwardModel(nullptr),
	__notificationTimer(nullptr)
{
}

CalSearchView ::~CalSearchView ()
{
	if (__notificationTimer)
	{
		ecore_timer_del(__notificationTimer);
		__notificationTimer = nullptr;
	}
	if (__entryText)
	{
		free(__entryText);
		__entryText = nullptr;
	}
}

/**
 * Executes the create action.
 *
 * @param [in]	parent		If non-null, the parent.
 * @param [in]	viewParam	If non-null, the view parameter.
 *
 * @return	A CalSearchView.
 */
Evas_Object* CalSearchView ::onCreate(Evas_Object* parent, void* viewParam)
{
	Evas_Object* layout = elm_layout_add(parent);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_file_set(layout, CAL_EDJE, "CalSearchView");

	__searchBar = __createSearchBar(layout);
	evas_object_show(__searchBar);

	Evas_Object *buttonBack = elm_button_add(layout);
	elm_object_style_set(buttonBack, "naviframe/back_btn/default");
	evas_object_size_hint_weight_set(buttonBack, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(buttonBack, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(buttonBack);
	evas_object_smart_callback_add(buttonBack, "clicked",
		[](void *data, Evas_Object *obj, void *event_info) {
			CalSearchView *pCalSearchView = (CalSearchView*) data;
			elm_naviframe_item_pop(pCalSearchView->getNaviframe()->getEvasObj());
		}, this
	);

	elm_object_part_content_set(layout, "elm.search.back", buttonBack);
	elm_object_part_content_set(layout, "elm.swallow.searchbar", __searchBar);

	return layout;
}

/**
 * Executes the create entry
 *
 * @param [in]	parent		If non-null, the parent.
 *
 * @return	A pointer to entry object
 */
Evas_Object* CalSearchView::__createEntry(Evas_Object *parent)
{
	Evas_Object *entry = elm_entry_add(parent);
	elm_entry_single_line_set(entry, EINA_TRUE);
	elm_entry_cnp_mode_set(entry, ELM_CNP_MODE_PLAINTEXT);
	elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_SENTENCE);
	elm_entry_prediction_allow_set(entry, EINA_TRUE);
	elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_SEARCH);
	elm_entry_input_panel_enabled_set(entry, EINA_FALSE);
	elm_entry_text_style_user_push(entry, "DEFAULT='color=#fff align=left'");

	elm_entry_scrollable_set(entry, EINA_TRUE);

	Elm_Entry_Filter_Limit_Size limit_filter_data;
	limit_filter_data.max_char_count = CAL_EVENT_TITLE_MAX_CHAR_LIMIT;

	elm_entry_markup_filter_append(entry, elm_entry_filter_limit_size, &limit_filter_data);
	evas_object_smart_callback_add(entry, "maxlength,reached", [](void *data, Evas_Object *obj, void *event_info)
		{
			CalSearchView *self = (CalSearchView*) data;
			if (self->__notificationTimer) {
				return;
			}

			self->__notificationTimer = ecore_timer_add(3.0, [](void *data)->Eina_Bool{
				WHIT();
				WPRET_VM(NULL == data, ECORE_CALLBACK_CANCEL, "parameter(data) is NULL");
				CalSearchView *self = (CalSearchView*) data;
				self->__notificationTimer = NULL;
				return ECORE_CALLBACK_CANCEL;
			}, self);
			notification_status_message_post(_L_("IDS_CLD_TPOP_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED"));
		},
		this);


	evas_object_smart_callback_add(entry, "changed",__entryChangeCb, this);
	evas_object_smart_callback_add(entry, "preedit,changed",__entryChangeCb, this);

	evas_object_smart_callback_add(entry, "activated", [](void *data, Evas_Object *obj, void *event_info){
			Ecore_IMF_Context *imf_context = (Ecore_IMF_Context*) elm_entry_imf_context_get(obj);
			if (imf_context) {
				ecore_imf_context_input_panel_hide(imf_context);
			}
			elm_object_focus_set(obj, EINA_FALSE);
		}, NULL);
	return entry;
}

/**
 * Executes the create search bar
 *
 * @param [in]	parent		If non-null, the parent.
 *
 * @return	A pointer to SearchBar object
 */
Evas_Object* CalSearchView::__createSearchBar(Evas_Object *parent)
{
	Evas_Object *sb = elm_layout_add(parent);
	elm_layout_theme_set(sb, "layout", "editfield", "singleline");
	evas_object_size_hint_weight_set(sb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(sb, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(sb);

	__entry = __createEntry(sb);
	elm_object_part_content_set(sb, "elm.swallow.content", __entry);

	Evas_Object *buttonClear = elm_button_add(sb);
	elm_object_style_set(buttonClear, "editfield_clear");
	evas_object_show(buttonClear);
	evas_object_smart_callback_add(buttonClear, "clicked", [](void *data, Evas_Object *obj, void *event_info)
	{
			CalSearchView *self = (CalSearchView*) data;
			elm_object_text_set(self->__entry, "");
		}, this
	);
	elm_object_part_content_set(sb, "elm.swallow.button", buttonClear);

	return sb;
}

/**
 * Executes the pushed action.
 *
 * @param [in]	naviItem	If non-null, the navi item.
 *
 * @return	A CalSearchView.
 */
void CalSearchView ::onPushed(Elm_Object_Item* naviItem)
{
	__update();
	elm_naviframe_item_title_enabled_set(getNaviItem(), false, EINA_TRUE);
	evas_object_smart_callback_add(getNaviframe()->getEvasObj(), "transition,finished",
		CalSearchView ::__transitionFinishedCb, this);
}

/**
 * Transition finished cb.
 *
 * @param [in]	data		If non-null, the data.
 * @param [in]	obj			If non-null, the object.
 * @param [in]	eventInfo	If non-null, information describing the event.
 */
void CalSearchView::__transitionFinishedCb(void* data, Evas_Object* obj, void* eventInfo)
{
	WENTER();
	CalSearchView* self = (CalSearchView* )data;
	elm_entry_input_panel_enabled_set(self->__entry, EINA_TRUE);
	evas_object_show(self->__entry);
	elm_object_focus_set(self->__entry, EINA_TRUE);
	elm_entry_cursor_end_set(self->__entry);
	elm_entry_input_panel_show(self->__entry);

	evas_object_smart_callback_del_full(self->getNaviframe()->getEvasObj(), "transition,finished",
			CalSearchView::__transitionFinishedCb, self);
}

/**
 * Entry change cb.
 *
 * @param [in]	data		If non-null, the data.
 * @param [in]	obj			If non-null, the object.
 * @param [in]	event_info	If non-null, information describing the event.
 *
 * @return	A CalSearchView.
 */
void CalSearchView ::__entryChangeCb(void* data, Evas_Object* obj, void* event_info)
{
	CalSearchView* self = (CalSearchView*)data;
	char* input = elm_entry_markup_to_utf8(elm_object_text_get(self->__entry));
	WDEBUG("Search text %s %s", self->__entryText, input);
	if(self->__entryText && input && strcasecmp(self->__entryText, input))
	{
		free(self->__entryText);

		self->__entryText = input;
		self->__update();
	}
	else
	{
		free(input);
	}

	if (self->__searchBar)
	{
		if (elm_entry_is_empty(obj))
		{
			elm_object_signal_emit(self->__searchBar, "elm,action,hide,button", "");
		}
		else
		{
			elm_object_signal_emit(self->__searchBar, "elm,action,show,button", "");
		}
	}
}

/**
 * Updates this object.
 *
 * @return	A CalSearchView.
 */
void CalSearchView ::__update()
{
	WPRET_M(!getEvasObj(), "layout is NULL");

	if(__entry)
	{
		std::string guideText = "<color=#EFEFEFFF>";
		guideText += _L_("IDS_CLD_OPT_SEARCH");
		guideText += "</color>";
		elm_object_part_text_set(__entry, "elm.guide", guideText.c_str());
	}

	if(__list)
	{
		__list->updateSearchText(__entryText);
		__forwardModel = CalListModelFactory::getInstance().getSearchList(__focusDate, 1, __entryText);
		__backwardModel = CalListModelFactory::getInstance().getSearchList(__focusDate, -1, __entryText);
		__list->setListModels(__forwardModel, __backwardModel);
	}
	else
	{
		__list = __createList();
		__list->create(getEvasObj(), NULL);
	}

	__showContent();
}

/**
 * Executes the event action.
 *
 * @param	event	The event.
 */
void CalSearchView::onEvent(const CalEvent& event)
{
	WENTER();
	WDEBUG("type = %u, source = %u", event.type, event.source);
	switch (event.type)
	{
		case CalEvent::DB_CHANGED:
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
 * Executes the destroy action.
 */
void CalSearchView::onDestroy()
{
	if(__entry)
	{
		evas_object_del(__entry);
		__entry = NULL;
	}

	if(__forwardModel)
	{
		delete __forwardModel;
		__forwardModel = NULL;
	}

	if(__backwardModel)
	{
		delete __backwardModel;
		__backwardModel = NULL;
	}

	free(__entryText);
	__entryText = NULL;

	if (!__isNoContents)
	{
		evas_object_del(__noContents);
	}
}

/**
 * Creates the list.
 *
 * @return	null if it fails, else the new list.
 */
CalScheduleListControl* CalSearchView::__createList()
{
	__forwardModel = CalListModelFactory::getInstance().getSearchList(__focusDate, 1, __entryText);
	__backwardModel = CalListModelFactory::getInstance().getSearchList(__focusDate, -1, __entryText);
	CalScheduleListControl *list = new CalScheduleListControl(__forwardModel, __backwardModel,
		[this](std::shared_ptr<CalSchedule> schedule) {
			getNaviframe()->push(new CalDetailView(schedule));
		},
		NULL, NULL,
		false, false, __entryText, 10
	);
	return list;
}

/**
 * Creates no content.
 *
 * @return	null if it fails, else the new no content.
 */
Evas_Object* CalSearchView::__createNoContent()
{
	Evas_Object *noContent = elm_layout_add(getEvasObj());
	elm_layout_theme_set(noContent, "layout", "nocontents", "default");
	evas_object_size_hint_weight_set(noContent, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(noContent, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_part_text_set(noContent, "elm.text", _L_("IDS_CLD_NPBODY_NO_RESULTS_FOUND"));
	evas_object_show(noContent);
	return noContent;
}

/**
 * Shows the content.
 */
void CalSearchView::__showContent()
{
	if (__list->isEmpty())
	{
		if (!__isNoContents)
		{
			if (!__noContents)
			{
				__noContents = __createNoContent();
			}
			elm_object_part_content_unset(getEvasObj(), "elm.swallow.content");
			elm_object_part_content_set(getEvasObj(), "elm.swallow.content", __noContents);
			evas_object_hide(__list->getEvasObj());
			WDEBUG("set noContent");
			__isNoContents = true;
		}
	}
	else
	{
		if (__isNoContents)
		{
			elm_object_part_content_unset(getEvasObj(), "elm.swallow.content");
			WDEBUG("unset ");
		}

		elm_object_part_content_set(getEvasObj(), "elm.swallow.content", __list->getEvasObj());
		evas_object_hide(__noContents);
		WDEBUG("set list");
		__isNoContents = false;
	}
}

