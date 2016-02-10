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

#include "CalCommon.h"
#include "CalListGroupTitleItem.h"
#include "CalLocaleManager.h"
#include "CalPath.h"

#define ITEM_FULL_HEIGHT        165  // 54 + 44 + 44 + 23 (padding) UI 1.5
#define ITEM_NO_LOCATION_HEIGHT 121  // 54 + 44 + 23 (padding)
#define ITEM_TOP_PADDING        23

CalListGroupTitleItem::CalListGroupTitleItem(CalScheduleListControl* parent, const CalDate& date) :
	__parent(parent)
{
	__isGroupTitle = true;
	__date = date;

	CalLocaleManager::getInstance().getDateText(CalLocaleManager::DATEFORMAT_2, __date, __weekDay);
	CalLocaleManager::getInstance().getDateText(CalLocaleManager::DATEFORMAT_9, __date, __monthDay);

	std::size_t found = __monthDay.find_first_not_of("0123456789");
	if (found != std::string::npos)
	{
		__monthDay = __monthDay.substr(0, found);
	}
}

CalListGroupTitleItem::~CalListGroupTitleItem()
{
	for(auto it: __listEvents)
	{
		delete it;
	}
}

CalListGroupTitleItem* CalListGroupTitleItem::search(const CalDate& date)
{
	auto node = this;
	if (node->__date > date)
	{
		while (true)
		{
			WDEBUG("%d", node);
			WASSERT(node->__date >= date);
			auto prev = (CalListGroupTitleItem*)node->getPrevious();
			if (node->__date == date || prev == NULL || prev->__date < date)
			{
				return node;
			}

			node = prev;
		}

	}
	else
	{
		while (true)
		{
			WDEBUG("%d", node);
			WASSERT(node->__date <= date);
			auto next = (CalListGroupTitleItem*)node->getNext();
			if (node->__date == date || next == NULL)
			{
				return node;
			}
			if (next->__date > date)
			{
				return next;
			}
			node = next;
		}
	}

	return NULL;
}

Elm_Genlist_Item_Class* CalListGroupTitleItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {};

	itc.item_style = "groupindex.day";
	itc.func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CalListGroupTitleItem* item = (CalListGroupTitleItem*)data;
		if (!strcmp(part, "elm.text.monthday"))
		{
			return strdup(item->__monthDay.c_str());
		}
		else if (!strcmp(part, "elm.text.weekday"))
		{
			return strdup(item->__weekDay.c_str());
		}

		return strdup("");
	};
	itc.func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		CalListGroupTitleItem* item = (CalListGroupTitleItem*)data;
		if (strcmp(part, "elm.swallow.content") == 0)
		{
			Evas_Object* box = elm_box_add(obj);
			elm_box_horizontal_set(box, EINA_FALSE);
			elm_box_padding_set(box, 0, ELM_SCALE_SIZE(10));
			evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
			elm_box_homogeneous_set(box, EINA_FALSE);
			evas_object_show(box);

			int height = 0;
			for(auto it: item->__listEvents)
			{
				CalEventItem* eventItem = (it);

				Evas_Object* button = elm_button_add(obj);
				elm_object_style_set(button, "transparent");
				evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
				evas_object_show(button);

				Evas_Object* layout = elm_layout_add(obj);
				const char* location = eventItem->getLocation();
				if(location && strlen(location))
				{
					elm_layout_file_set(layout, CalPath::getPath(CAL_EDJE).c_str(), "CalEventItem");
					elm_object_part_text_set(layout, "elm.text.location", location);
					height += ITEM_FULL_HEIGHT;
				}
				else
				{
					if(eventItem->getSchedule()->getBookId() == DEFAULT_BIRTHDAY_CALENDAR_BOOK_ID)
					{
						elm_layout_file_set(layout, CalPath::getPath(CAL_EDJE).c_str(), "CalEventItem.Purple");
					}
					else
					{
						elm_layout_file_set(layout, CalPath::getPath(CAL_EDJE).c_str(), "CalEventItem.NoLocation");
					}

					height += ITEM_NO_LOCATION_HEIGHT;
				}

				evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
				evas_object_show(layout);

				evas_object_smart_callback_add(button, "pressed",
					[](void* data, Evas_Object* obj, void* event_info){
						elm_object_signal_emit((Evas_Object*)data, "elm,button,pressed", "*");
					},layout
				);
				evas_object_smart_callback_add(button, "unpressed",
					[](void* data, Evas_Object* obj, void* event_info){
						elm_object_signal_emit((Evas_Object*)data, "elm,button,unpressed", "*");
					},layout
				);
				evas_object_smart_callback_add(button, "mouse,out",
					[](void* data, Evas_Object* obj, void* event_info){
						elm_object_signal_emit((Evas_Object*)data, "elm,button,unpressed", "*");
					},layout
				);

				evas_object_smart_callback_add(button, "clicked",
					[](void* data, Evas_Object* obj, void* event_info)
					{
						CalEventItem* eventItem = (CalEventItem*)data;
						if(eventItem)
						{
							if(eventItem->isCheckboxRequired())
							{
								Evas_Object* layout = elm_object_content_get(obj);
								if(layout)
								{
									Evas_Object* check = elm_object_part_content_get(layout, "elm.swallow.checkbox");
									if(check)
									{
										Eina_Bool isChecked = elm_check_state_get(check);
										elm_check_state_set(check, !isChecked);

										eventItem->setCheckboxSelected(!isChecked);
									}
								}
							}

							if(eventItem->getOnTapCallback())
							{
								eventItem->getOnTapCallback()(eventItem);
							}
						}
					},eventItem
				);

				elm_object_part_text_set(layout, "elm.text.title", eventItem->getTitle());
				elm_object_part_text_set(layout, "elm.text.time", eventItem->getDateInterval());

				if(eventItem->isCheckboxRequired())
				{
					Evas_Object* check = elm_check_add(obj);
					evas_object_size_hint_align_set(check, EVAS_HINT_FILL, EVAS_HINT_FILL);
					elm_object_part_content_set(layout, "elm.swallow.checkbox", check);
					evas_object_repeat_events_set(check, EINA_TRUE);
					evas_object_propagate_events_set(check, EINA_FALSE);
					evas_object_freeze_events_set(check, EINA_TRUE);

					eventItem->setCheckObject(check);
					if(eventItem->isCheckboxSelected())
					{
						elm_check_state_set(check, EINA_TRUE);
					}
				}

				elm_object_content_set(button, layout);
				elm_box_pack_end(box, button);
			}

			evas_object_size_hint_min_set(box, -1, ELM_SCALE_SIZE(height + ITEM_TOP_PADDING));

			return box;
		}

		return NULL;
	};

	return &itc;
}

void CalListGroupTitleItem::addItem(const std::shared_ptr<CalSchedule>& schedule, bool isCheckboxRequired, bool isCheckboxSelected, const std::string& searchText, int dir)
{
	CalEventItem* eventItem = new CalEventItem(__date, schedule, isCheckboxRequired, isCheckboxSelected, searchText);
	eventItem->setOnTapCallback([this](CalEventItem* item)
	{
		if(__parent)
		{
			__parent->processEventItemTap(this, item);
		}
	});

	if(dir > 0)
	{
		__listEvents.push_back(eventItem);
	}
	else
	{
		__listEvents.push_front(eventItem);
	}
}

int CalListGroupTitleItem::getSelectedItemsCount()
{
	int selectedItemsCount = 0;

	for(auto it: __listEvents)
	{
		if(it->isCheckboxSelected())
		{
			selectedItemsCount++;
		}
	}

	return selectedItemsCount;
}

void CalListGroupTitleItem::selectAllItems(bool select)
{
	for(auto it: __listEvents)
	{
		it->setCheckboxSelected(select);
	}
}

void CalListGroupTitleItem::getSelectedSchedules(std::list<std::shared_ptr<CalSchedule>>& list)
{
	for(auto it: __listEvents)
	{
		if(it->isCheckboxSelected())
		{
			list.push_back(it->getSchedule());
		}
	}
}
