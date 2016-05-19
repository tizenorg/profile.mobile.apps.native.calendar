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

#include "CalFilterView.h"
#include "CalListModelFactory.h"

CalFilterView::CalFilterView(const char *name) : CalView(name),
	__list(NULL),
	__searchText(""),
	__noContents(NULL),
	__searchBarEntry(NULL),
	__forwardModel(NULL),
	__backwardModel(NULL),
	__isNoContents(false),
	__focusDate(),
	__notificationTimer(NULL),
	__backButtonVisibility(true)
{
	WENTER();
}

CalFilterView::~CalFilterView()
{
	if (__notificationTimer)
	{
		ecore_timer_del(__notificationTimer);
		__notificationTimer = nullptr;
	}
}

Evas_Object *CalFilterView::onCreate(Evas_Object *parent, void *viewParam)
{
	WENTER();

	Evas_Object* layout = elm_layout_add(parent);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_file_set(layout, CalPath::getPath(CAL_EDJE).c_str(), "CalFilterView");

	if(isBackButtonVisibile())
	{
		Evas_Object *buttonBack = elm_button_add(layout);
		elm_object_style_set(buttonBack, "naviframe/back_btn/default");
		evas_object_size_hint_weight_set(buttonBack, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(buttonBack, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_show(buttonBack);
		evas_object_smart_callback_add(buttonBack, "clicked",
			[](void *data, Evas_Object *obj, void *event_info) {
				CalFilterView *pCalFilterView = (CalFilterView*) data;
				elm_naviframe_item_pop(pCalFilterView->getNaviframe()->getEvasObj());
			}, this
		);

		elm_object_part_content_set(layout, "elm.search.back", buttonBack);
	}

	__searchBarEntry = createSearchBarEntry(layout);
	elm_object_part_content_set(layout, "elm.swallow.searchbar", __searchBarEntry->getEvasObj());

	return layout;
}

void CalFilterView::onPushed(Elm_Object_Item *naviItem)
{
	WENTER();
	if(!__backButtonVisibility)
	{
		WDEBUG("Hide backButton");
		elm_layout_signal_emit(getEvasObj(), "searchbar,hide", "");
	}
}

void CalFilterView::onEvent(const CalEvent &event)
{
	WENTER();
	WDEBUG("type = %u, source = %u", event.type, event.source);
	switch (event.type)
	{
		case CalEvent::DB_CHANGED:
		case CalEvent::TIME_CHANGED:
		case CalEvent::LANGUAGE_CHANGED:
		case CalEvent::SETTING_CHANGED:
			updateList();
			break;
		default:
			WERROR("invalid type = %u", event.type);
			break;
	}
}

void CalFilterView::onDestroy()
{
	WENTER();

	__deleteListModels();

	if (!__isNoContents)
	{
		evas_object_del(__noContents);
	}
}

void CalFilterView::setSelectCb(EVENT_ITEM_ONSELECT_CB selectCb)
{
	__selectCb = selectCb;
}

void CalFilterView::setBackButtonVisibility(bool isVisible)
{
	__backButtonVisibility = isVisible;
}

bool CalFilterView::isBackButtonVisibile()
{
	return __backButtonVisibility;
}

void CalFilterView::createList(EVENT_ITEM_ONSELECT_CB selectCb, bool useChecks)
{
	WENTER();

	if (__list)
	{
		delete __list;
		__list = NULL;
	}

	__deleteListModels();

	__forwardModel = CalListModelFactory::getInstance().getSearchList(__focusDate, 1, __searchText.c_str());
	__backwardModel = CalListModelFactory::getInstance().getSearchList(__focusDate, -1, __searchText.c_str());

	__list = new CalScheduleListControl(__forwardModel, __backwardModel, selectCb, NULL, NULL, useChecks, !useChecks, __searchText.c_str());
	__list->create(getEvasObj(), NULL);

	showContent();
}

Evas_Object *CalFilterView::createNoContent()
{
	WENTER();
	Evas_Object *noContent = elm_layout_add(getEvasObj());
	elm_layout_theme_set(noContent, "layout", "nocontents", "default");
	evas_object_size_hint_weight_set(noContent, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(noContent, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_part_text_set(noContent, "elm.text", _L_("IDS_CLD_NPBODY_NO_RESULTS_FOUND"));
	evas_object_show(noContent);
	return noContent;
}

void CalFilterView::showContent()
{
	WENTER();
	if (__list->isEmpty())
	{
		if (!__isNoContents)
		{
			if (!__noContents)
			{
				__noContents = createNoContent();
			}
			elm_object_part_content_unset(getEvasObj(), "elm.swallow.content");
			elm_object_part_content_set(getEvasObj(), "elm.swallow.content", __noContents);
			evas_object_hide(__list->getEvasObj());
			__isNoContents = true;
		}
	}
	else
	{
		if (__isNoContents)
		{
			elm_object_part_content_unset(getEvasObj(), "elm.swallow.content");
		}
		elm_object_part_content_set(getEvasObj(), "elm.swallow.content", __list->getEvasObj());
		evas_object_hide(__noContents);
		__isNoContents = false;
	}
}

CalUnderlineEditField *CalFilterView::createSearchBarEntry(Evas_Object *parent)
{
	WENTER();
	CalUnderlineEditField *searchBarEntry = new CalUnderlineEditField();

	searchBarEntry->create(parent, NULL);

	std::string guideText = "<color=#EFEFEFFF>";
	guideText += _L_("IDS_CLD_OPT_SEARCH");
	guideText += "</color>";

	searchBarEntry->setGuideText(guideText.c_str());

	searchBarEntry->setChangeCallback([this] (const char* text)->void
		{
			WDEBUG("Search text: [%p] [%s]", text, text);
			if(text && strcasecmp(text, __searchText.c_str()))
			{
				__searchText = text;
				WDEBUG("Update list");
				updateList();
			}
		});

	searchBarEntry->setEntryMaxLenReachCallback([this]
		{
			if (__notificationTimer)
			{
				return;
			}

			__notificationTimer = ecore_timer_add(3.0, [](void *data)->Eina_Bool
				{
					WHIT();
					WPRET_VM(NULL == data, ECORE_CALLBACK_CANCEL, "parameter(data) is NULL");
					CalFilterView *self = (CalFilterView*) data;
					self->__notificationTimer = NULL;
					return ECORE_CALLBACK_CANCEL;
				}, this);

			notification_status_message_post(_L_("IDS_CLD_TPOP_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED"));
		});

	return searchBarEntry;
}

void CalFilterView::updateList()
{
	WENTER();
	if(__list)
	{
		__deleteListModels();

		__list->updateSearchText(__searchText.c_str());
		__forwardModel = CalListModelFactory::getInstance().getSearchList(__focusDate, 1, __searchText.c_str());
		__backwardModel = CalListModelFactory::getInstance().getSearchList(__focusDate, -1, __searchText.c_str());
		__list->setListModels(__forwardModel, __backwardModel);

		showContent();
	}
	WLEAVE();
}

void CalFilterView::setFocusToSearchEntry()
{
	WENTER();
	if(__searchBarEntry)
	{
		__searchBarEntry->setFocusToEntry();
	}
	WLEAVE();
}

void CalFilterView::__deleteListModels()
{
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
}
