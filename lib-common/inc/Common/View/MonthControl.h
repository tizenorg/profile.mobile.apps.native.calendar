/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef COMMON_VIEW_MONTH_CONTROL_H
#define COMMON_VIEW_MONTH_CONTROL_H

#include "Ui/Control.h"

namespace Common
{
	namespace View
	{
		/**
		 * @brief Month control
		 */
		class MonthControl : public Ui::Control
		{
		public:
			MonthControl();

			virtual Evas_Object *onCreate(Evas_Object *parent) override;
		};
	}
}


#endif /* COMMON_VIEW_MONTH_CONTROL_H */
