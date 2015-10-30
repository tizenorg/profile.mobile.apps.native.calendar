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

#ifndef _CAL_MAIN_VIEW_H_
#define _CAL_MAIN_VIEW_H_

#include "CalView.h"
#include "ICalListModel.h"

#include "CalMonthControl2.h"
#include "CalScheduleListControl.h"

#include "CalSettingsManager.h"

#include "CalDragRecognizer.h"
#include "CalSlideAnimator.h"

class CalMainView : public CalView
{
public:
	CalMainView(const CalDate& focusedDate);
	virtual const char* getClassName();
	void focus(const CalDate& date);

protected:
	virtual ~CalMainView();

private:

	/**
	 * Executes the create action.
	 *
	 * @param [in]	parent   	If non-null, the parent.
	 * @param [in]	viewParam	If non-null, the view parameter.
	 *
	 * @return		null if it fails, else an Evas_Object*.
	 */
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);

	/**
	 * Executes the created action.
	 */
	virtual void onCreated();

	/**
	 * Executes the destroy action.
	 */
	virtual void onDestroy();

	/**
	 * Executes the pushed action.
	 *
	 * @param [in]	naviItem	If non-null, the navi item.
	 */
	virtual void onPushed(Elm_Object_Item* naviItem);

	/**
	 * Executes the menu button action.
	 */
	virtual void onMenuButton();

	/**
	 * Executes the event action.
	 *
	 * @param	event	The event.
	 */
	virtual void onEvent(const CalEvent& event);

	/**
	 * Creates button select cb.
	 *
	 * @param [in]	data	  	If non-null, the data.
	 * @param [in]	obj		  	If non-null, the object.
	 * @param [in]	event_info	If non-null, information describing the event.
	 */
	static void __createBtnSelectCb(void* data, Evas_Object* obj, void* event_info);

	/**
	 * Deletes the button select cb.
	 *
	 * @param [in]	data	  	If non-null, the data.
	 * @param [in]	obj		  	If non-null, the object.
	 * @param [in]	event_info	If non-null, information describing the event.
	 */
	static void __deleteBtnSelectCb(void* data, Evas_Object* obj, void* event_info);
	WDISABLE_COPY_AND_ASSIGN(CalMainView);

	/**
	 * Construct month control.
	 *
	 * @param	dir	The dir.
	 *
	 * @return	null if it fails, else a CalMonthControl2*.
	 */
	CalMonthControl2* __constructMonthControl(int dir);

	/**
	 * Creates list control.
	 */
	void __createListControl();

	/**
	 * Gets a month.
	 *
	 * @param	dir	The dir.
	 *
	 * @return	The month.
	 */
	CalMonthControl2& getMonth(int dir);

	/**
	 * Focus month.
	 *
	 * @param	newFocusDate	The new focus date.
	 */
	void __focusMonth(const CalDate& newFocusDate);

	/**
	 * Focus list.
	 */
	void __focusList();

	/**
	 * Shows the only focused row.
	 */
	void __showOnlyFocusedRow();

	/**
	 * Updates the title.
	 */
	void __updateTitle();

	/**
	 * Shows the no content.
	 *
	 * @param	isShow	true if this object is show.
	 */
	void __showNoContent(bool isShow);

	/**
	 * Updates this object.
	 */
	void __update();

	/**
	 * Finalize mode change.
	 *
	 * @param	cancelled	true if cancelled.
	 */
	void __finalizeModeChange(bool cancelled);

	/**
	 * Block touch.
	 *
	 * @param	caller	The caller.
	 */
	void __blockTouch(const char* caller);

	/**
	 * Unblock touch.
	 *
	 * @param	caller	The caller.
	 */
	void __unblockTouch(const char* caller);

	/**
	 * Gets month header.
	 *
	 * @return	null if it fails, else the month header.
	 */
	Evas_Object* __getMonthHeader();

	/**
	 * Resets the month.
	 *
	 * @param [in]	month	The month.
	 * @param		dir		The dir.
	 */
	void __resetMonth(CalMonthControl2& month, int dir);

	/**
	 * Gets the first day of week.
	 *
	 * @return	The first day of week.
	 */
	int __getFirstDayOfWeek();

	/**
	 * Finds the getlistpositionrange of the given arguments.
	 *
	 * @param [in, out]	min	The minimum.
	 * @param [in, out]	max	The maximum.
	 */
	void __getListPositionRange(Evas_Coord& min, Evas_Coord& max);

	/**
	 * Finds the getmonthpositionrange of the given arguments.
	 *
	 * @param [in, out]	min	The minimum.
	 * @param [in, out]	max	The maximum.
	 */
	void __getMonthPositionRange(Evas_Coord& min, Evas_Coord& max);

	/**
	 * Sets list drag recognizer state.
	 *
	 * @param [in]	min	The minimum.
	 */
	void __setListDragRecognizerState(Evas_Coord& min);

	/**
	 * Enables the drag.
	 */
	void __enableDrag();

	/**
	 * Disables the drag.
	 */
	void __disableDrag();

	/**
	 * Determines if we can user interaction in progress.
	 *
	 * @return	true if it succeeds, false if it fails.
	 */
	bool __userInteractionInProgress();

	/**
	 * Terminate user interaction.
	 */
	void __terminateUserInteraction();

private:
	enum ListDragRecognizerState{
		DISABLE = 0,
		HALF_ENABLE, // List will be positioned with origin state after finishing animation.
		ENABLE,
	};

	CalDate __focusedDate;
	CalMonthControl2* __month;
	CalScheduleListControl* __list;
	ICalListModel* __fowardModel;
	ICalListModel* __backwardModel;
	CalSlideAnimator* __monthSlideAnimator;
	CalSlideAnimator* __listSlideAnimator;
	CalDragRecognizer* __monthDragRecognizer;
	CalDragRecognizer* __listDragRecognizer;
	bool __listMode;
	ListDragRecognizerState __listDragRecognizerState;
	bool __listIsBeingDragged;
	bool __monthDragBegan;
	Ecore_Timer* __updateTimer;
	bool __isCutomFocus;
	CalDate __customFocusDate;
	Evas_Object* __mainViewLayout;
};

#endif
