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

#ifndef _CAL_EDIT_MORE_MENU_CONTROL_H_
#define _CAL_EDIT_MORE_MENU_CONTROL_H_

#include "CalDialogControl.h"
#include "CalDataManager.h"

#include <memory>
#include <Evas.h>
#include <glib.h>

#include "WDefine.h"
#include "CalDialogEditMoreMenuItem.h"

class CalEditMoreMenuControl : public CalControl
{
public:
	CalEditMoreMenuControl(bool showDescription, bool showTimezone);
	virtual ~CalEditMoreMenuControl();
	virtual const char* getClassName();
private:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* param);
	virtual void onDestroy();
public:
	void setSelectCb(std::function<void (CalDialogEditMoreMenuItem::TYPE type)> selectCb);
	void onPopupListItemSelect(CalDialogEditMoreMenuItem::TYPE selectType);
private:
	WDISABLE_COPY_AND_ASSIGN(CalEditMoreMenuControl);
	void __addDescriptionItem();
	void __addTimezoneItem();
private:
	Evas_Object* __parentPopup;
	std::function<void (CalDialogEditMoreMenuItem::TYPE type)> __selectCb;
	CalDialogControl *__dialog;
	bool __showDescription;
	bool __showTimezone;
};

#endif /* _CAL_EDIT_MORE_MENU_CONTROL_H_ */

