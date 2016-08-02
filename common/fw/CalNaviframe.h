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

#ifndef _CAL_NAVIFRAME_H_
#define _CAL_NAVIFRAME_H_

#include "WNaviframe.h"

/**
 * @brief Naviframe wrapper.
 *
 * @see WNaviframe.
 */
class WAPP_ASSIST_EXPORT CalNaviframe: public WNaviframe
{
public:
	CalNaviframe();
	virtual ~CalNaviframe();

	/**
	 * @brief Get top view.
	 *
	 * @return Top view.
	 *
	 * @see WView.
	 */
	WView* getTopView();

	/**
	 * @brief Destroy view.
	 *
	 * @param[in]   view   View to destroy.
	 * @param[in]   item   Object item.
	 *
	 * @see WView.
	 */
	void destroyView(WView* view, Elm_Object_Item* item);

private:
	void __arrangeEventManager(WView* view, Elm_Object_Item* item);
	virtual bool onPush( WView* view, void* viewParam);
	virtual bool onItemPop( WView* view, Elm_Object_Item* item);
};

#endif
