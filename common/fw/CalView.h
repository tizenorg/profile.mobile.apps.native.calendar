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

#ifndef _CAL_VIEW_H_
#define _CAL_VIEW_H_

#include "WView.h"
#include "CalEvent.h"
#include "CalPath.h"
#include "ICalEventListener.h"
#include <Elementary.h>

class WAPP_ASSIST_EXPORT CalView: public WView, public ICalEventListener
{
public:
	CalView(const char* name);
public:
	void activateMenuButton(Elm_Object_Item* naviItem);
protected:
	virtual ~CalView();
	virtual void onMenuButton();
	virtual void onEvent(const CalEvent& event);
private:
	Evas_Object* __createMenuBtn(Evas_Object* parent, Evas_Smart_Cb func, void* data);
	static void __menuBtnSelectCb(void* data, Evas_Object* obj, void* event_info);
private:
	WDISABLE_COPY_AND_ASSIGN(CalView);
	friend class CalNaviframe;
};

#endif /* _CAL_VIEW_H_ */
