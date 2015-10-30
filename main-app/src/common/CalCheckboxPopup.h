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

#ifndef _CAL_DETAIL_CHECKBOX_POPUP_H_
#define _CAL_DETAIL_CHECKBOX_POPUP_H_

#include <Evas.h>
#include <string>

#include "WPopup.h"
#include "WDefine.h"

class CalCheckboxPopup : public WPopup
{
public:
	CalCheckboxPopup(const char* title, const char* message, const char* checkboxMessage);
	virtual ~CalCheckboxPopup();
	virtual const char* getClassName();
private:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* param);
	virtual void onDestroy();
	Evas_Object* __createContent(Evas_Object* parent);
public:
    void setCheckboxMessage(const char* checkboxMessage);
    bool getCheckboxState();
private:
	WDISABLE_COPY_AND_ASSIGN(CalCheckboxPopup);
private:
    std::string __message;
    std::string __checkboxMessage;
    Evas_Object* __checkbox;
};

#endif /* _CAL_DETAIL_CHECKBOX_POPUP_H_ */

