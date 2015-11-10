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
#include "CalNaviframe.h"
#include "WMenuPopup.h"

#include "CalMainView.h"

#include "CalEditView.h"
#include "CalDetailView.h"
#include "CalSearchView.h"
#include "CalBookView.h"
#include "CalSettingsView.h"

#include "CalLocaleManager.h"
#include "CalListModelFactory.h"

#ifdef __cplusplus
	extern "C" {
#endif
#include <efl_extension.h>
#ifdef __cplusplus
	}
#endif

#define RECT_REF(_rect_var) &_rect_var.x, &_rect_var.y, &_rect_var.w, &_rect_var.h

#define TRACK_MODE_CHANGE_FLOW(_tag)\
	WDEBUG("[mode] %s: __listMode(%d) __listDragRecognizerState(%d) __listIsBeingDragged(%d) __monthDragBegan(%d)",\
		   _tag, __listMode, __listDragRecognizerState, __listIsBeingDragged, __monthDragBegan);

CalMainView::CalMainView(const CalDate& focusedDate) : CalView("CalMainView"),
	__focusedDate(focusedDate),
	__month(NULL),
	__list(NULL),
	__fowardModel(NULL),
	__backwardModel(NULL),
	__monthSlideAnimator(NULL),
	__listSlideAnimator(NULL),
	__monthDragRecognizer(NULL),
	__listDragRecognizer(NULL),
	__listMode(false),
	__listDragRecognizerState(DISABLE),
	__listIsBeingDragged(false),
	__monthDragBegan(false),
	__updateTimer(NULL),
	__isCutomFocus(false),
	__mainViewLayout(NULL)
{
}

CalMainView::~CalMainView()
{
}

/**
 * Focus the given date.
 *
 * @param	date	The date.
 */
void CalMainView::focus(const CalDate& date)
{
	__focusMonth(date);
	__focusList();
	getMonth(0).load();

	if (!__listMode) {
		__blockTouch(__func__);
		__finalizeModeChange(false);
	}
}

/**
 * Gets class name.
 *
 * @return	null if it fails, else the class name.
 */
const char* CalMainView::getClassName()
{
	return "CalMainView";
}

/**
 * Signals to emit.
 *
 * @param [in]	obj		If non-null, the object.
 * @param		signal	The signal.
 * @param		j		The int to process.
 */
static void __signal(Evas_Object* obj, const char* signal, int j)
{
	char buffer[100];
	sprintf(buffer, signal, j);
	elm_layout_signal_emit(obj, buffer, "");
}

#define DRAG_BUFFER 10

/**
 * Construct month control.
 *
 * @param	dir	The dir.
 *
 * @return	null if it fails, else a CalMonthControl2*.
 */
CalMonthControl2* CalMainView::__constructMonthControl(int dir)
{
	CalDate date(__focusedDate);
	const int firstDayOfMonth = 1;
	if (dir > 0)
		date.incrementMonth();
	else if (dir < 0)
		date.decrementMonth();
	CalMonthControl2* month = new CalMonthControl2(__getFirstDayOfWeek(), date.getYear(), date.getMonth());
	month->setTapCellCb(
		[this](int i, int j) {
			TRACK_MODE_CHANGE_FLOW("month.tapCellCb");
			CalDate newFocusDate;
			getMonth(0).getDate(i, j, newFocusDate);

			WDEBUG("focusdate %s", __focusedDate.getString());
			WDEBUG("newFocusdate %s", newFocusDate.getString());

			if (newFocusDate.getMonth() == __focusedDate.getMonth()) {
				const bool sameDate = (newFocusDate == __focusedDate);
				__focusedDate = newFocusDate;
				getMonth(0).focus(__focusedDate.getMday());
				getMonth(0).setHeaderFocus(__getMonthHeader());
				__focusList();
				if (sameDate) {
					__terminateUserInteraction();
					getNaviframe()->push(new CalEditView(__focusedDate));
				}
			} else {
				if (__listMode) {
					__focusMonth(newFocusDate);
				} else {
					__isCutomFocus = true;
					__customFocusDate = newFocusDate;
					if (!getMonth(0).slide(newFocusDate > __focusedDate ? 1 : -1)) {
						__isCutomFocus = false;
					}
				}
				__focusList();
			}
		}
	);

	month->setScrollCb(
		[this]() {
			__blockTouch("month.scrollStartCb");
		},
		[this](int dir) {
			__unblockTouch("month.scrollFinishCb");
			if (dir == 0)
				return;
			if (dir > 0)
				__focusedDate.incrementMonth();
			else
				__focusedDate.decrementMonth();

			__updateTitle();

			if (__isCutomFocus)
			{
				__isCutomFocus = false;
				__focusedDate = __customFocusDate;
				getMonth(0).focus(__focusedDate.getMday());
			}
			else
			{
				getMonth(0).focus(firstDayOfMonth);
			}

			getMonth(0).setHeaderFocus(__getMonthHeader());
			getMonth(0).load();

			__focusList();
		}
	);

	__monthDragRecognizer = new CalDragRecognizer(
		[this](int x, int y) {
			TRACK_MODE_CHANGE_FLOW("__monthDragRecognizer.onPressed");
			WASSERT(__listMode);
			Evas_Coord min, max;
			__getListPositionRange(min, max);
			__listSlideAnimator->begin(CalSlideAnimator::VERTICAL_GROW_AND_SHRINK_TOP, min, max);
			__getMonthPositionRange(min, max);
			__monthSlideAnimator->begin(CalSlideAnimator::VERTICAL_MOVE, min, max);
			__monthDragBegan = false;
			int sy, sh;
			elm_scroller_region_get(__list->getEvasObj(), NULL, &sy, NULL, &sh);
			elm_scroller_region_show(__list->getEvasObj(), 0, sy, 0, sh);
			elm_layout_signal_emit(__mainViewLayout, "show-all-rows/post", "");
		},
		[this](int dx, int dy) {
			TRACK_MODE_CHANGE_FLOW("__monthDragRecognizer.onMoved");
			WASSERT(__listMode);
			if (!__monthDragBegan) {
				elm_layout_signal_emit(__mainViewLayout, "show-all-rows/pre", "");
				__monthDragBegan = true;
			} else {
				__listSlideAnimator->update(dy);
				__monthSlideAnimator->update(dy);
			}
		},
		[this](int dx, int dy, bool isValidDrag) {
			TRACK_MODE_CHANGE_FLOW("__monthDragRecognizer.onReleased");
			WASSERT(__listMode);
			if (isValidDrag) {
				__blockTouch("__monthDragRecognizer.onReleased");
				if (dy > DRAG_BUFFER) {
					__listSlideAnimator->finish(1);
					__monthSlideAnimator->finish(1);
				} else {
					__listSlideAnimator->cancel();
					__monthSlideAnimator->cancel();
				}
			} else {
				__showOnlyFocusedRow();
			}
		}
	);

	return month;
}

/**
 * Executes the create action.
 *
 * @param [in]	parent		If non-null, the parent.
 * @param [in]	viewParam	If non-null, the view parameter.
 *
 * @return	null if it fails, else an Evas_Object*.
 */
Evas_Object* CalMainView::onCreate(Evas_Object* parent, void* viewParam)
{
	Evas_Object* baseLayout = elm_layout_add(parent);
	evas_object_size_hint_weight_set(baseLayout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_theme_set(baseLayout, "layout", "application", "default");

	Evas_Object* layout = elm_layout_add(baseLayout);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_file_set(layout, CAL_EDJE, "CalMainView");
	elm_object_signal_callback_add(layout, "show-all-rows/done/cpp", "",
			[](void *data, Evas_Object* obj, const char* emission, const char* source) {
				CalMainView* view = (CalMainView*) data;
				view->__blockTouch(__func__);
				view->__finalizeModeChange(false);
			}, this);
	elm_object_part_content_set(baseLayout, "elm.swallow.content", layout);
	evas_object_show(layout);

	__mainViewLayout = layout;

	return baseLayout;
}

/**
 * Executes the created action.
 */
void CalMainView::onCreated()
{
	__month = __constructMonthControl(0);
	__month->create(__mainViewLayout, NULL);
	elm_object_part_content_set(__mainViewLayout, "month/sw", __month->getEvasObj());
	elm_object_part_content_set(__mainViewLayout, "header/sw", getMonth(0).createHeader(__mainViewLayout));

	__updateTitle();
	__month->focus(__focusedDate.getMday());
	__month->setHeaderFocus(__getMonthHeader());
	__month->load();

	__createListControl();

	__monthSlideAnimator = new CalSlideAnimator(
		getMonth(0).getEvasObj(),
		[this](int dir) {
			TRACK_MODE_CHANGE_FLOW("__monthSlideAnimator.slideFinishedCb");
			if (!__listSlideAnimator->isActive())
				__finalizeModeChange(dir == 0);
		}
	);

	__monthDragRecognizer->addTarget((Evas_Object*)edje_object_part_object_get(elm_layout_edje_get(__mainViewLayout), "month/touch"));
	__monthDragRecognizer->block();

	Evas_Object *floatingButton = eext_floatingbutton_add(getEvasObj());
	elm_object_part_content_set(getEvasObj(), "elm.swallow.floatingbutton", floatingButton);
	evas_object_repeat_events_set(floatingButton, EINA_FALSE);

	Evas_Object *button= elm_button_add(floatingButton);
	elm_object_part_content_set(floatingButton, "button1", button);

	Evas_Object *icon = elm_image_add(floatingButton);
	elm_image_file_set(icon, CAL_IMAGE_DIR "core_floating_button_icon.png", NULL);
	elm_object_part_content_set(button, "icon", icon);

	evas_object_smart_callback_add(button, "clicked",
		[](void* data, Evas_Object* obj, void* event_info){
			CalMainView* self = (CalMainView*)data;
			self->getNaviframe()->push(new CalEditView(self->__focusedDate));
		},this
	);
	evas_object_show(floatingButton);
}

/**
 * Executes the destroy action.
 */
void CalMainView::onDestroy()
{
	if(__fowardModel){
		delete __fowardModel;
		__fowardModel = NULL;
	}

	if(__backwardModel){
		delete __backwardModel;
		__backwardModel = NULL;
	}

	if(__monthSlideAnimator){
		delete __monthSlideAnimator;
		__monthSlideAnimator = NULL;
	}

	if(__listDragRecognizer){
		delete __listDragRecognizer;
		__listDragRecognizer = NULL;
	}

	if(__monthDragRecognizer){
		delete __monthDragRecognizer;
		__monthDragRecognizer = NULL;
	}

	if(__listSlideAnimator){
		delete __listSlideAnimator;
		__listSlideAnimator = NULL;
	}
}

/**
 * Executes the menu button action.
 */
void CalMainView::onMenuButton()
{
	WENTER();
	WMenuPopup* popup = new WMenuPopup();
	popup->prepare(getWindow()->getEvasObj(), getNaviframe()->getEvasObj());

	popup->appendItem(_L_("IDS_CLD_OPT_GO_TO_TODAY"),
		[this]() {
			__focusMonth(CalDate());
			__focusList();
		});
	if (__list && !__list->isEmpty())
		popup->appendItem(_L_("IDS_CLD_OPT_SEARCH"), [this]() {getNaviframe()->push(new CalSearchView());});
	popup->appendItem(_L_("IDS_CLD_OPT_MANAGE_CALENDARS_ABB"), [this]() {getNaviframe()->push(new CalBookView);});
	popup->appendItem(_L_("IDS_CLD_OPT_SETTINGS"), [this]() {getNaviframe()->push(new CalSettingsView);});
	attachPopup( popup );
}

/**
 * Executes the event action.
 *
 * @param	event	The event.
 */
void CalMainView::onEvent(const CalEvent& event)
{
	WENTER();
	WDEBUG("type = %u, source = %u", event.type, event.source);
	switch (event.type)
	{
		case CalEvent::LANGUAGE_CHANGED:
		case CalEvent::DB_CHANGED:
		case CalEvent::SETTING_CHANGED:
		case CalEvent::TIME_CHANGED:
		case CalEvent::FIRST_DAY_OF_WEEK_CHANGED:
			if (__userInteractionInProgress())
			{
				if (!__updateTimer)
				{
					__updateTimer = ecore_timer_add(1.0, [](void* data)->Eina_Bool {
						CalMainView* self = (CalMainView*)data;
						if (self->__userInteractionInProgress())
						{
							WDEBUG("Defer again!");
							return ECORE_CALLBACK_RENEW;
						}

						self->__update();
						self->__updateTimer = NULL;
						return ECORE_CALLBACK_CANCEL;
					}, this);
				}
			}
			else
			{
				if (__updateTimer)
				{
					ecore_timer_del(__updateTimer);
					__updateTimer = NULL;
				}
				__update();
			}
			break;
		case CalEvent::APP_PAUSED:
		case CalEvent::APP_RESUMED:
			break;
		default:
			WERROR("invalid type = %u", event.type);
			break;
	}
}

/**
 * Updates this object.
 */
void CalMainView::__update()
{
	WDEBUG("");

	destroyPopup();
	__resetMonth(getMonth(0), 0);

	elm_object_part_content_set(__mainViewLayout, "header/sw", getMonth(0).createHeader(__mainViewLayout));

	getMonth(0).focus(__focusedDate.getMday());
	getMonth(0).setHeaderFocus(__getMonthHeader());
	getMonth(0).load();

	if (__listMode)
		__showOnlyFocusedRow();

	__createListControl();

	__updateTitle();
}

/**
 * Executes the pushed action.
 *
 * @param [in]	naviItem	If non-null, the navi item.
 */
void CalMainView::onPushed(Elm_Object_Item* naviItem)
{
	WENTER();
	activateMenuButton(naviItem);
}

/**
 * Enables the drag.
 */
void CalMainView::__enableDrag()
{
	__monthDragRecognizer->unblock();
	__listDragRecognizer->unblock();
}

/**
 * Disables the drag.
 */
void CalMainView::__disableDrag()
{
	__monthDragRecognizer->block();
	__listDragRecognizer->block();
}

/**
 * Determines if we can user interaction in progress.
 *
 * @return	true if it succeeds, false if it fails.
 */
bool CalMainView::__userInteractionInProgress()
{
	return __monthSlideAnimator->isActive() || __listSlideAnimator->isActive() ||
		   __monthDragRecognizer->isActive() || __listDragRecognizer->isActive() ||
		   __list->isScrolling();
}

/**
 * Terminate user interaction.
 */
void CalMainView::__terminateUserInteraction()
{
	if (__monthSlideAnimator && __monthSlideAnimator->isActive())
	{
		__monthSlideAnimator->cancel(true);
	}

	if (__listSlideAnimator && __listSlideAnimator->isActive())
	{
		__listSlideAnimator->cancel(true);
	}

	if (__monthDragRecognizer && __monthDragRecognizer->isActive())
	{
		__monthDragRecognizer->terminate((Evas_Object*)edje_object_part_object_get(elm_layout_edje_get(__mainViewLayout), "month/touch"));
	}

	if (__listDragRecognizer && __listDragRecognizer->isActive())
	{
		__listDragRecognizer->terminate((Evas_Object*)edje_object_part_object_get(elm_layout_edje_get(__mainViewLayout), "list/touch"));
	}
}

/**
 * Creates list control.
 */
void CalMainView::__createListControl()
{
	if(__fowardModel) delete __fowardModel;
	if(__backwardModel) delete __backwardModel;
	__fowardModel = CalListModelFactory::getInstance().getList(__focusedDate, 1);
	__backwardModel = CalListModelFactory::getInstance().getList(__focusedDate, -1);

	if (__list) evas_object_del(__list->getEvasObj());
	__list = new CalScheduleListControl(__fowardModel, __backwardModel,
		[this](std::shared_ptr<CalSchedule> schedule) {
			WDEBUG("__list.onTapScheduleItem");
			if (!__listSlideAnimator->isActive())
				getNaviframe()->push(new CalDetailView(schedule));
		},
		[this](int dir) {
			TRACK_MODE_CHANGE_FLOW("__list.onScrollStart");
//			__blockTouch("__list.onScrollStart");
		},
		[this](int dir, CalListControl::Item* topShowingGroupItem) {
			TRACK_MODE_CHANGE_FLOW("__list.onScrollStop");
			if (__listIsBeingDragged) {
				if (__list->getHiddenItemsHeightUnderBottom() == 0)
					__list->scrollToLastItem(true);
				__blockTouch(__func__);
				__listSlideAnimator->cancel();
				__monthSlideAnimator->cancel();
				__listIsBeingDragged = false;
			}
			__unblockTouch("__list.onScrollStop");
			if(topShowingGroupItem)
			{
				const CalDate& topShowingDate = topShowingGroupItem->getDate();
				__focusMonth(topShowingDate);
			}
			edje_message_signal_process();

			if (!__listMode)
				__list->lockDrag(true);
		},
		false, false, "", 1
	);
	__list->create(__mainViewLayout, NULL);

	elm_object_part_content_set(__mainViewLayout, "list/sw", __list->getEvasObj());

	Evas_Object* noContent = NULL;
	if (__list->isEmpty()) {
		__showNoContent(true);
		elm_layout_signal_emit(__mainViewLayout, "hide,list", "");
		noContent = elm_object_part_content_get(__mainViewLayout, "list/no-events");
	} else {
		__showNoContent(false);
		elm_layout_signal_emit(__mainViewLayout, "show,list", "");
	}

	if (__listSlideAnimator) delete __listSlideAnimator;
	__listSlideAnimator = new CalSlideAnimator(
		noContent ? noContent : __list->getEvasObj(),
		[this](int dir) {
			TRACK_MODE_CHANGE_FLOW("__listSlideAnimator.slideFinishedCb");
			if (!__monthSlideAnimator->isActive())
				__finalizeModeChange(dir == 0);
		}
	);
	__listSlideAnimator->attachSidekick((Evas_Object*)edje_object_part_object_get(elm_layout_edje_get(__mainViewLayout), "month/divider"));

	if (__listDragRecognizer) {
		__listDragRecognizer->removeTarget((Evas_Object*)edje_object_part_object_get(elm_layout_edje_get(__mainViewLayout), "list/touch"));
		delete __listDragRecognizer;
	}
	__listDragRecognizer = new CalDragRecognizer(
		[this](int x, int y) {
			TRACK_MODE_CHANGE_FLOW("__listDragRecognizer.onPressed");
			Evas_Coord min, max;
			__getListPositionRange(min, max);
			__setListDragRecognizerState(min);

			__listIsBeingDragged = false;

			if (__listDragRecognizerState == DISABLE) {
				__list->lockDrag(false);
			} else {
				__listSlideAnimator->begin(CalSlideAnimator::VERTICAL_GROW_AND_SHRINK_TOP, min, max);
				__getMonthPositionRange(min, max);
				__monthSlideAnimator->begin(CalSlideAnimator::VERTICAL_MOVE, min, max);
			}
		},
		[this](int dx, int dy) {
			TRACK_MODE_CHANGE_FLOW("__listDragRecognizer.onDragged");
			if (dy == 0)
				return;

			if (__listDragRecognizerState == DISABLE)
				return;

			if (!__listIsBeingDragged && dy > 0) {
				__listSlideAnimator->cancel(true);
				__monthSlideAnimator->cancel(true);
				__listIsBeingDragged = false;
				__listDragRecognizerState = DISABLE;
				__list->lockDrag(false);
				return;
			}

			__listIsBeingDragged = true;

			if (dy <= __listSlideAnimator->getMin()) {
				if (__listDragRecognizerState == HALF_ENABLE) {
					__list->lockDrag(false);
				} else {
					__blockTouch(__func__);
					__listSlideAnimator->finish(-1, true);
					__monthSlideAnimator->finish(-1, true);
					__listIsBeingDragged = false;
				}
			} else {
				__list->lockDrag(true);
				__listSlideAnimator->update(dy);
				__monthSlideAnimator->update(dy);
			}
		},
		[this](int dx, int dy, bool isValidDrag) {
			TRACK_MODE_CHANGE_FLOW("__listDragRecognizer.onReleased");

			if (__listDragRecognizerState == DISABLE) {
				WASSERT(!__listIsBeingDragged);
				return;
			}

			__listIsBeingDragged = false;

			if (!isValidDrag) {
				__listSlideAnimator->cancel(true);
				__monthSlideAnimator->cancel(true);
				return;
			}

			__blockTouch("__listDragRecognizer.onReleased");

			if (dy > 0) {
				__listSlideAnimator->cancel();
				__monthSlideAnimator->cancel();
				return;
			}

			if (isValidDrag && __listDragRecognizerState == ENABLE && dy < DRAG_BUFFER) {
				__listSlideAnimator->finish(-1);
				__monthSlideAnimator->finish(-1);
			} else {
				__listSlideAnimator->cancel();
				__monthSlideAnimator->cancel();
			}
		}
	);

	__listDragRecognizer->addTarget((Evas_Object*)edje_object_part_object_get(elm_layout_edje_get(__mainViewLayout), "list/touch"));

	if (__listMode)
		__listDragRecognizer->block();
	else
		__list->lockDrag(true);
}

/**
 * Gets a month.
 *
 * @param	dir	The dir.
 *
 * @return	The month.
 */
CalMonthControl2& CalMainView::getMonth(int dir)
{
	return *__month;
}

/**
 * Focus month.
 *
 * @param	newFocusDate	The new focus date.
 */
void CalMainView::__focusMonth(const CalDate& newFocusDate)
{
	WDEBUG("%s", __focusedDate.getString());
	WDEBUG("%s", newFocusDate.getString());
	WDEBUG("===============");
	const bool sameMonth = (CalDate::compareMonth(__focusedDate, newFocusDate) == 0);
	__focusedDate = newFocusDate;
	if (!sameMonth) {
		__updateTitle();
		__resetMonth(getMonth(0), 0);
		getMonth(0).load();
	}
	getMonth(0).focus(__focusedDate.getMday());
	getMonth(0).setHeaderFocus(__getMonthHeader());
	if (__listMode)
		__showOnlyFocusedRow();
	edje_message_signal_process();
}

/**
 * Shows the only focused row.
 */
void CalMainView::__showOnlyFocusedRow()
{
	const int focusedRow = getMonth(0).getFocusedRow();
	WASSERT(0 <= focusedRow && focusedRow < GRID_ROW_COUNT);
	__signal(__mainViewLayout, "show-one-row/no-anim/%d", focusedRow);
}

/**
 * Focus list.
 */
void CalMainView::__focusList()
{
	TRACK_MODE_CHANGE_FLOW(__func__);
	if (!__list->scrollTo(__focusedDate))
	{
		__createListControl();
	}
	else
	{
		__terminateUserInteraction();
	}
}

/**
 * Updates the title.
 */
void CalMainView::__updateTitle()
{
	setTitle(__focusedDate.getMonthString());
	if (getNaviItem())
	{
		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", getTitle());
	}
}

/**
 * Finalize mode change.
 *
 * @param	cancelled	true if cancelled.
 */
void CalMainView::__finalizeModeChange(bool cancelled)
{
	TRACK_MODE_CHANGE_FLOW(__func__);
	if (!cancelled) {
		if (__listMode)
			elm_layout_signal_emit(__mainViewLayout, "show-all-rows/post", "");
		else
			__showOnlyFocusedRow();
		__listMode = !__listMode;
	} else {
		if (__listMode)
			__showOnlyFocusedRow();
	}
	__unblockTouch(__func__);
}

/**
 * Block touch.
 *
 * @param	caller	The caller.
 */
void CalMainView::__blockTouch(const char* caller)
{
	WDEBUG("%s", caller);
	evas_object_freeze_events_set(getMonth(0).getEvasObj(), EINA_TRUE);
	if (__listMode) {
		__monthDragRecognizer->block();
		__list->lockDrag(true);
	} else {
		__listDragRecognizer->block();
		getMonth(0).freezeScroll(true);
	}
}

/**
 * Unblock touch.
 *
 * @param	caller	The caller.
 */
void CalMainView::__unblockTouch(const char* caller)
{
	WDEBUG("%s", caller);
	evas_object_freeze_events_set(getMonth(0).getEvasObj(), EINA_FALSE);
	if (__listMode) {
		__monthDragRecognizer->unblock();
		__list->lockDrag(false);
	} else {
		__listDragRecognizer->unblock();
		getMonth(0).freezeScroll(false);
	}
}

/**
 * Gets month header.
 *
 * @return	null if it fails, else the month header.
 */
Evas_Object* CalMainView::__getMonthHeader()
{
	return elm_object_part_content_get(__mainViewLayout, "header/sw");
}

/**
 * Gets the first day of week.
 *
 * @return	The first day of week.
 */
int CalMainView::__getFirstDayOfWeek()
{
	int result = CalSettingsManager::getInstance().getFirstDayOfWeek();
	if (result == CalSettingsManager::LOCALES)
		return CalLocaleManager::getInstance().getLocaleFirstDayOfWeek();

	return result;
}

/**
 * Resets the month.
 *
 * @param [in]	month	The month.
 * @param		dir		The dir.
 */
void CalMainView::__resetMonth(CalMonthControl2& month, int dir)
{
	CalDate date(__focusedDate);
	if (dir > 0)
		date.incrementMonth();
	else if (dir < 0)
		date.decrementMonth();
	month.reset(__getFirstDayOfWeek(), date.getYear(), date.getMonth());
}

/**
 * Shows the no content.
 *
 * @param	isShow	true if this object is show.
 */
void CalMainView::__showNoContent(bool isShow)
{
	if (isShow)
	{
		Evas_Object* noContents = elm_layout_add(__mainViewLayout);
		elm_layout_theme_set(noContents, "layout", "nocontents", "default");
		evas_object_size_hint_weight_set(noContents, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(noContents, EVAS_HINT_FILL, EVAS_HINT_FILL);
		elm_object_part_text_set(noContents, "elm.text", _L_("IDS_CLD_BODY_NO_EVENTS"));
		elm_object_part_text_set(noContents, "elm.help.text",
			_L_("IDS_CLD_BODY_AFTER_YOU_CREATE_OR_ACCEPT_INVITATIONS_TO_EVENTS_THEY_WILL_BE_SHOWN_HERE"));
		elm_layout_signal_emit(noContents, "align.center", "elm");
		evas_object_show(noContents);
		elm_object_part_content_set(__mainViewLayout, "list/no-events", noContents);
	}
	else
	{
		evas_object_del(elm_object_part_content_unset(__mainViewLayout, "list/no-events"));
	}
}

/**
 * Sets list drag recognizer state.
 *
 * @param [in]	min	The minimum.
 */
void CalMainView::__setListDragRecognizerState(Evas_Coord& min)
{
	if (min) {
		Evas_Coord listHeight = __list->getListHeight();
		Evas_Coord shownHeight = __list->getShownItemsHeightUnderTop();
		Evas_Coord hiddenHeight = __list->getHiddenItemsHeightUnderBottom();
		bool isScrolledToTopItem = __list->isScrolledToTopItem();

		if (shownHeight < listHeight || hiddenHeight == 0) {
			__listDragRecognizerState = DISABLE;
			return;
		}

		if (hiddenHeight < std::abs(min) && !isScrolledToTopItem) {
			min = -hiddenHeight;
			__listDragRecognizerState = HALF_ENABLE;
			return;
		}

		__listDragRecognizerState = ENABLE;
	}
}

/**
 * Finds the getlistpositionrange of the given arguments.
 *
 * @param [in, out]	min	The minimum.
 * @param [in, out]	max	The maximum.
 */
void CalMainView::__getListPositionRange(Evas_Coord& min, Evas_Coord& max)
{
	const Evas_Object* noContent = elm_object_part_content_get(__mainViewLayout, "list/no-events");

	Evas_Coord curTopPos;
	evas_object_geometry_get(noContent ? noContent : __list->getEvasObj(), NULL, &curTopPos, NULL, NULL);

	Evas_Coord_Rectangle rect;

	evas_object_geometry_get(edje_object_part_object_get(elm_layout_edje_get(__mainViewLayout), "week/bottom-padding"), RECT_REF(rect));
	const Evas_Coord minPos = rect.y + rect.h;

	evas_object_geometry_get(edje_object_part_object_get(elm_layout_edje_get(__mainViewLayout), "month/original"), RECT_REF(rect));
	const Evas_Coord maxPos = rect.y + rect.h;

	min = minPos - curTopPos;
	max = maxPos - curTopPos;

	WDEBUG("min(%s%d) max(%s%d)", min > 0 ? "+" : "", min, max > 0 ? "+" : "", max);
}

/**
 * Finds the getmonthpositionrange of the given arguments.
 *
 * @param [in, out]	min	The minimum.
 * @param [in, out]	max	The maximum.
 */
void CalMainView::__getMonthPositionRange(Evas_Coord& min, Evas_Coord& max)
{
	Evas_Coord curTopPos;
	evas_object_geometry_get(getMonth(0).getEvasObj(), NULL, &curTopPos, NULL, NULL);

	Evas_Coord_Rectangle rect;

	// Get the position of month at which bottom of focused row aligns with top of bottom margin of week area
	getMonth(0).getRowPosition(getMonth(0).getFocusedRow(), RECT_REF(rect));
	const Evas_Coord focusedRowBottomOffset = (rect.y + rect.h) - curTopPos;
	evas_object_geometry_get(edje_object_part_object_get(elm_layout_edje_get(__mainViewLayout), "week/bottom-padding"), RECT_REF(rect));
	const Evas_Coord minPos = rect.y - focusedRowBottomOffset - 1;

	// Get the position of month at which top of this month aligns with bottom of top margin of month area
	getMonth(0).getRowPosition(0, RECT_REF(rect));
	const Evas_Coord thisMonthTopOffset = rect.y - curTopPos;
	evas_object_geometry_get(edje_object_part_object_get(elm_layout_edje_get(__mainViewLayout), "month/top-padding/original"), RECT_REF(rect));
	const Evas_Coord maxPos = (rect.y + rect.h) - thisMonthTopOffset;

	min = minPos - curTopPos;
	max = maxPos - curTopPos;

	WDEBUG("min(%s%d) max(%s%d)", min > 0 ? "+" : "", min, max > 0 ? "+" : "", max);
}
