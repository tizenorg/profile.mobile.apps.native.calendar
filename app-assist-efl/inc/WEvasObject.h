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

#ifndef _IWEVAS_OBJECT_H_
#define _IWEVAS_OBJECT_H_

#include <memory>
#include <Evas.h>
#include "WDefine.h"

/**
 * @brief Interface for evas object.
 */
class WAPP_ASSIST_EXPORT IWEvasObject
{
public:
	/**
	 * @brief Get evas object.
	 *
	 * @return Evas object.
	 */
	virtual Evas_Object* getEvasObj() = 0;

protected:
	virtual ~IWEvasObject(){};
};

/**
 * @brief Weak pointer to IWEvasObject.
 *
 * @see IWEvasObject.
 */
typedef std::weak_ptr<IWEvasObject> WEvasObjectPtr;

/**
 * @brief Get the IWEvasObject instance.
 *
 * @param[in]   obj   Evas object.
 *
 * @return The weak pointer to IWEvasObject class instance.
 *
 * @see WEvasObjectPtr.
 */
WAPP_ASSIST_EXPORT WEvasObjectPtr WEvasObject_getWeakPtr( Evas_Object* obj );



#endif // _IWEVAS_OBJECT_H_
