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
#include "CalCommon.h"
#include "CalPickView.h"
#include "CalDataManager.h"
#include "CalScheduleListControl.h"
#include "CalListModelFactory.h"
#include "WPopup.h"

CalPickView::CalPickView(int maxLimit, ResultType resultType) : CalView("CalPickView"),
	__selectCb(nullptr),
	__maxLimit(maxLimit),
	__resultType(resultType),
	__list(NULL),
	__focusDate(),
	__entryText(strdup("")),
	__entry(NULL),
	__box(NULL),
	__noContents(NULL),
	__searchBar(NULL),
	__forwardModel(NULL),
	__backwardModel(NULL),
	__notificationTimer(NULL),
	__isNoContents(false),
	selectedScheduleSet()
{
}

CalPickView::~CalPickView()
{
	if (__notificationTimer)
	{
		ecore_timer_del(__notificationTimer);
		__notificationTimer = NULL;
	}
	free(__entryText);
}

Evas_Object* CalPickView::onCreate(Evas_Object* parent, void* viewParam)
{
	Evas_Object* layout = elm_layout_add(parent);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_file_set(layout, CAL_EDJE, "CalSearchView");

	__createList(layout);

	if (!__list->isEmpty())
	{
		__searchBar = __createSearchBar(layout);

		Evas_Object *buttonBack = elm_button_add(layout);
		elm_object_style_set(buttonBack, "naviframe/back_btn/default");
		evas_object_size_hint_weight_set(buttonBack, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(buttonBack, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_show(buttonBack);

		evas_object_smart_callback_add(buttonBack, "clicked",
			[](void *data, Evas_Object *obj, void *event_info) {
				CalPickView *self = (CalPickView*) data;
				Evas_Object *naviframe = self->getNaviframe()->getEvasObj();
				elm_naviframe_item_pop(naviframe);
			}, this
		);

		elm_object_part_content_set(layout, "elm.swallow.searchbar", __searchBar);
		elm_object_part_content_set(layout, "elm.search.back", buttonBack);
	}
	else
	{
		elm_object_signal_emit(layout, "elm,state,nosearch", "elm");
	}

	__box = elm_box_add(layout);
	evas_object_size_hint_weight_set(__box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "elm.swallow.content", __box);

	ecore_idler_add([](void *data)->Eina_Bool
	{
		CalPickView *self = (CalPickView*) data;
		if (self->__entry)
		{
			elm_object_focus_set(self->__entry, EINA_TRUE);
		}
		return ECORE_CALLBACK_CANCEL;
	}, this);

	return layout;
}

void CalPickView::__createList(Evas_Object* parent)
{
	if(!parent)
	{
		parent = getEvasObj();
	}

	if (__list)
	{
		evas_object_del(__list->getEvasObj());
		__list = NULL;
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

	__forwardModel = CalListModelFactory::getInstance().getSearchList(__focusDate, 1, __entryText);
	__backwardModel = CalListModelFactory::getInstance().getSearchList(__focusDate, -1, __entryText);

	__list = new CalScheduleListControl(__forwardModel, __backwardModel,
		[this](std::shared_ptr<CalSchedule> schedule)
		{
			if(selectedScheduleSet.exists(schedule))
			{
				WHIT();
				selectedScheduleSet.remove(schedule);
			}
			else
			{
				WHIT();
				selectedScheduleSet.add(schedule);
			}
			this->__updateTitleInfo();
		}, NULL, NULL, true, false, __entryText);

	if(__maxLimit > 0)
	{
		__list->setMaxLimitSelectCb([this]()
		{
			char warningText[CAL_WARNING_MAX_LENGTH] = {0};
			if (__resultType == CalPickView::RESULT_TYPE_TEXT || __resultType == CalPickView::RESULT_TYPE_ID)
			{
				snprintf(warningText, sizeof(warningText)-1, _L_("IDS_CLD_TPOP_MAXIMUM_NUMBER_OF_EVENTS_THAT_CAN_BE_INSERTED_HPD_REACHED"), __maxLimit);
			}
			else
			{
				snprintf(warningText, sizeof(warningText)-1, _L_("IDS_CLD_TPOP_MAXIMUM_NUMBER_OF_ATTACHMENTS_HPD_REACHED"), __maxLimit);
			}

			notification_status_message_post(warningText);
		});
		__list->setMaxLimitCount(__maxLimit);
	}

	__list->setIsSelectedCb( [this](std::shared_ptr<CalSchedule>& schedule)->bool
	{
		return selectedScheduleSet.exists(schedule);
	});
	__list->setSelectedCount(selectedScheduleSet.getCount());

	__list->create(parent, NULL);
}

Evas_Object* CalPickView::__createSearchBar(Evas_Object *parent)
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
	evas_object_smart_callback_add(buttonClear, "clicked", [](void* data, Evas_Object* obj, void* event_info)
		{
			CalPickView *self = (CalPickView*) data;
			elm_object_text_set(self->__entry, "");
		}, this);
	evas_object_show(buttonClear);
	elm_object_part_content_set(sb, "elm.swallow.button", buttonClear);

	return sb;
}

Evas_Object* CalPickView::__createEntry(Evas_Object *parent)
{
	Evas_Object *entry = elm_entry_add(parent);
	elm_entry_single_line_set(entry, EINA_TRUE);
	std::string guideText = "<color=#EFEFEFFF>";
	guideText += _L_("IDS_CLD_OPT_SEARCH");
	guideText += "</color>";
	elm_object_part_text_set(entry, "elm.guide", guideText.c_str());
	elm_entry_cnp_mode_set(entry, ELM_CNP_MODE_PLAINTEXT);
	elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_SENTENCE);
	elm_entry_prediction_allow_set(entry, EINA_TRUE);
	elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_SEARCH);
	elm_entry_input_panel_enabled_set(entry, EINA_FALSE);
	elm_entry_text_style_user_push(entry, "DEFAULT='color=#fff align=left'");
	evas_object_size_hint_weight_set(entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_entry_scrollable_set(entry, EINA_TRUE);
	evas_object_show(entry);

	Elm_Entry_Filter_Limit_Size limit_filter_data;
	limit_filter_data.max_char_count = CAL_EVENT_TITLE_MAX_CHAR_LIMIT;

	elm_entry_markup_filter_append(entry, elm_entry_filter_limit_size, &limit_filter_data);
	evas_object_smart_callback_add(entry, "maxlength,reached", [](void *data, Evas_Object *obj, void *event_info)
	{
		CalPickView *self = (CalPickView*) data;
		if (self->__notificationTimer)
		{
			return;
		}

		self->__notificationTimer = ecore_timer_add(1.0, [](void *data)->Eina_Bool
		{
			CalPickView *self = (CalPickView*) data;
			if(self)
			{
				char warningText[CAL_WARNING_MAX_LENGTH] = {0};
				snprintf(warningText, sizeof(warningText)-1, _L_("IDS_CLD_TPOP_MAXIMUM_NUMBER_OF_CHARACTERS_HPD_REACHED"), CAL_EVENT_TITLE_MAX_CHAR_LIMIT);
				notification_status_message_post(warningText);

				self->__notificationTimer = NULL;
			}

			return ECORE_CALLBACK_CANCEL;
		}, NULL);
	},this);

	auto entryChangeCb = [](void *data, Evas_Object *obj, void *event_info)
	{
		CalPickView *self = (CalPickView*)data;
		char *input = elm_entry_markup_to_utf8(elm_object_text_get(self->__entry));
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
			if(self->__entryText && strlen(self->__entryText))
			{
				elm_object_signal_emit(self->__searchBar, "elm,action,show,button", "");
			}
			else
			{
				elm_object_signal_emit(self->__searchBar, "elm,action,hide,button", "");
			}
		}
	};

	evas_object_smart_callback_add(entry, "changed", entryChangeCb, this);
	evas_object_smart_callback_add(entry, "preedit,changed", entryChangeCb, this);

	evas_object_smart_callback_add(entry, "clicked",
		[](void *data, Evas_Object *obj, void *event_info) {
			CalPickView *self = (CalPickView*)data;
			elm_entry_input_panel_enabled_set(self->__entry, EINA_TRUE);
			elm_object_focus_set(self->__entry, EINA_TRUE);
		},this);

	evas_object_smart_callback_add(entry, "focused",
		[](void *data, Evas_Object *obj, void *event_info) {
			CalPickView *self = (CalPickView*) data;
			elm_entry_input_panel_enabled_set(self->__entry, EINA_TRUE);
		},this);

	return entry;
}

Evas_Object *CalPickView::__createNoContent()
{
	Evas_Object *noContent = elm_layout_add(getEvasObj());
	elm_layout_theme_set(noContent, "layout", "nocontents", "default");
	evas_object_size_hint_weight_set(noContent, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(noContent, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_part_text_set(noContent, "elm.text", _L_("IDS_CLD_NPBODY_NO_RESULTS_FOUND"));
	evas_object_show(noContent);
	return noContent;
}

void CalPickView::__update()
{
	WPRET_M(!getEvasObj(), "layout is NULL");

	if(__list)
	{
		__list->updateSearchText(__entryText);
		__forwardModel = CalListModelFactory::getInstance().getSearchList(__focusDate, 1, __entryText);
		__backwardModel = CalListModelFactory::getInstance().getSearchList(__focusDate, -1, __entryText);
		__list->setListModels(__forwardModel, __backwardModel);
	}
	else
	{
		__createList(getEvasObj());
	}

	__showContent();
	__updateTitleInfo();
}

void CalPickView::__showContent()
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

void CalPickView::onDestroy()
{
	if (__entryText)
	{
		free(__entryText);
		__entryText = NULL;
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

	if (!__isNoContents)
	{
		evas_object_del(__noContents);
	}
}

void CalPickView::onPushed(Elm_Object_Item* naviItem)
{
	Evas_Object* button = elm_button_add(getNaviframe()->getEvasObj());
	elm_object_style_set(button, "naviframe/title_left");
	elm_object_text_set(button, _L_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));
	evas_object_smart_callback_add(button, "clicked",
		[](void* data, Evas_Object* obj, void* event_info) {
			Evas_Object* naviframe = (Evas_Object*)data;
			elm_naviframe_item_pop(naviframe);
		}, getNaviframe()->getEvasObj()
	);
	elm_object_item_part_content_set(naviItem, "title_left_btn", button);

	button = elm_button_add(getNaviframe()->getEvasObj());
	elm_object_style_set(button, "naviframe/title_right");
	elm_object_text_set(button, _L_("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));
	evas_object_smart_callback_add(button, "clicked",
		[](void* data, Evas_Object* obj, void* event_info) {
			CalPickView* self = (CalPickView*)data;
			std::list<std::shared_ptr<CalSchedule>> selectedScheduleList;
			self->selectedScheduleSet.getList(selectedScheduleList);
			if (self->__selectCb){
				self->__selectCb(selectedScheduleList);
			}
			Evas_Object* naviframe = self->getNaviframe()->getEvasObj();
			elm_naviframe_item_pop(naviframe);
		}, this
	);
	elm_object_item_part_content_set(naviItem, "title_right_btn", button);

	__update();
}

void CalPickView::onEvent(const CalEvent& event)
{
	WENTER();
	WDEBUG("type = %u, source = %u", event.type, event.source);
	switch (event.type)
	{
		case CalEvent::DB_CHANGED:
		case CalEvent::TIME_CHANGED:
		case CalEvent::LANGUAGE_CHANGED:
		case CalEvent::SETTING_CHANGED:
			if (__entryText)
			{
				free(__entryText);
				__entryText = strdup("");
				elm_entry_entry_set(__entry, __entryText);
			}
			__reCheck();
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

void CalPickView::setSelectCb(std::function<void (const std::list<std::shared_ptr<CalSchedule>>& schedules)> selectCb)
{
	__selectCb = selectCb;
}

void CalPickView::__updateTitleInfo()
{
	char title[100] = {0};

	int selectedItemCount = 0;

	selectedItemCount = selectedScheduleSet.getCount();

	if (__maxLimit > 0)
	{
		sprintf(title, "%d/%d", selectedItemCount, __maxLimit);
	}
	else
	{
		sprintf(title, _L_("IDS_CLD_HEADER_PD_SELECTED_ABB3"), selectedItemCount);
	}

	setTitle(title);
	if (getNaviItem())
	{
		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", getTitle());
		Evas_Object* button = elm_object_item_part_content_get(getNaviItem(), "title_right_btn");
		elm_object_disabled_set(button, selectedItemCount > 0 ? EINA_FALSE : EINA_TRUE);
	}
}

/*check the event has been deleted or not*/
void CalPickView::__reCheck()
{
	WENTER();
	if(selectedScheduleSet.getCount() <= 0)
	{
		return;
	}

	std::list<std::shared_ptr<CalSchedule>> selectedScheduleList;
	selectedScheduleSet.getList(selectedScheduleList);

	auto it = selectedScheduleList.begin();
	for( ; it != selectedScheduleList.end(); ++it)
	{
		int eventId = (*it)->getIndex();
		CalDateTime startTime;
		(*it)->getStart(startTime);
		if(!CalDataManager::getInstance().getInstanceSchedule(eventId, startTime)){
			/*event have been deleted*/
			selectedScheduleSet.remove(*it);
		}
	}

}
