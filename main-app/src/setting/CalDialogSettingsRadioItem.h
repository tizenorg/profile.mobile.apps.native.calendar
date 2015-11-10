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

#ifndef _CAL_DIALOG_SETTINGS_RADIO_ITEM_H_
#define _CAL_DIALOG_SETTINGS_RADIO_ITEM_H_

#include "CalDialogControl.h"

class CalDialogSettingsRadioItem : public CalDialogControl::Item
{
public:
	enum sub_item_type
	{
		SUB_LOCALE_DEFAULT = 0,
		SUB_SATURDAY,
		SUB_SUNDAY,
		SUB_MONDAY,
		SUB_ALERTS,
		SUB_NOTIFICATION,
		SUB_OFF,
	};
	CalDialogSettingsRadioItem(std::function<void (void)> selectCb, sub_item_type type, int radioIndex);
	virtual ~CalDialogSettingsRadioItem();
	void setRadioCb(std::function<void (void)> radioCb);
private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogSettingsRadioItem);
	virtual void onSelect();
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
public:
	sub_item_type getSubItemType();
	Evas_Object *  getItemRadioObject();
private:
	int __radioIndex;
	sub_item_type __type;
	Evas_Object *__radio;
	std::function<void (void)> __selectCb;
	std::function<void (void)> __radioCb;
};

#endif

