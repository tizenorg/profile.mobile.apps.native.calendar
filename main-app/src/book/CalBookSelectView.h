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

#ifndef _CAL_BOOK_SELECT_VIEW_H_
#define _CAL_BOOK_SELECT_VIEW_H_

#include <memory>
#include <map>
#include "CalView.h"
#include "CalDialogControl.h"
#include "CalBook.h"

class CalBookSelectView : public CalView
{
public:
	CalBookSelectView();
	virtual ~CalBookSelectView();
private:
	WDISABLE_COPY_AND_ASSIGN(CalBookSelectView);
private:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);
	virtual void onPushed(Elm_Object_Item* naviItem);
	virtual void onDestroy();

	void __update();
	void onEvent(const CalEvent& event);

private:
	CalDialogControl* __dialog;
	std::map<int, std::shared_ptr<CalBook>> __bookMap;
};

#endif /* _CAL_BOOK_SELECT_VIEW_H_ */

