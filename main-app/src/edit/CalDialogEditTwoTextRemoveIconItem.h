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

#ifndef _CAL_DIALOG_TWO_TEXT_REMOVE_ICON_ITEM_H_
#define _CAL_DIALOG_TWO_TEXT_REMOVE_ICON_ITEM_H_

#include <functional>
#include <string>


#include "CalDialogControl.h"

class CalDialogEditTwoTextRemoveIconItem : public CalDialogControl::Item
{
public:
	CalDialogEditTwoTextRemoveIconItem(int sortIndex, const char* mainText, const char* subText);
	virtual ~CalDialogEditTwoTextRemoveIconItem();
public:
	void setSelectCb(std::function<void (void)> selectCb);
	void setRemoveCb(std::function<void (void)> removeCb);
public:
	void setMainText(const char* text);
	void setSubText(const char* text);
private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogEditTwoTextRemoveIconItem);
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
	virtual void onSelect();
private:
	std::function<void (void)> __removeCb;
	std::string __mainText;
	std::string __subText;
	std::function<void (void)> __selectCb;
};

#endif /* _CAL_DIALOG_TWO_TEXT_REMOVE_ICON_ITEM_H_ */
