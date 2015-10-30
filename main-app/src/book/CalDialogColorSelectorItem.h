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

#ifndef _CAL_DIALOG_COLOR_SELECTOR_ITEM_H_
#define _CAL_DIALOG_COLOR_SELECTOR_ITEM_H_


class CalDialogColorSelectorItem : public CalDialogControl::Item
{
public:
	CalDialogColorSelectorItem(	int r = 123, int g = 0, int b = 0, int a = 123);
	virtual ~CalDialogColorSelectorItem();
	void setChangedCb(std::function<void (int& r, int& g, int& b, int& a)> changedCb);
private:
	WDISABLE_COPY_AND_ASSIGN(CalDialogColorSelectorItem);
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
	void __setColorSelector(Evas_Object* colorSelector);
	Evas_Object* __createColorSelector(Evas_Object* parent);
private:
	std::function<void (int& r, int& g, int& b, int& a)> __changedCb;
	int __r;
	int __g;
	int __b;
	int __a;

};

#endif


