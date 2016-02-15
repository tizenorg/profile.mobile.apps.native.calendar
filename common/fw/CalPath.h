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

#ifndef _CAL_PATH_H_
#define _CAL_PATH_H_

#include "WDefine.h"

#include <string>

class WAPP_ASSIST_EXPORT CalPath
{
public:

	/**
	 * @brief Resource type
	 */
	enum DirType
	{
		RESOURCE = 0,
		EDJE,
		LOCALE,
		IMAGE,
		THEME,
		DATA,
	};

	/**
	 * @return EDJE files directory path.
	 */
	static const std::string &getEdjeDir();


	/**
	 * @return Localization files directory path.
	 */
	static const std::string &getLocaleDir();


	/**
	 * @return Theme files directory path.
	 */
	static const std::string &getThemeDir();


	/**
	 * @return Image files directory path.
	 */
	static const std::string &getImageDir();


	/**
	 * @return Data files directory path.
	 */
	static const std::string &getDataDir();


	/**
	 * @brief Get resource file path.
	 * @param[in]	relativePath	File path relative to resource directory
	 * @param[in]	type	Resource type such as IMAGES, EDJE, etc.
	 * @return Full file path
	 */
	static std::string getPath(const std::string &relativePath, DirType type = EDJE);

private:
	CalPath();
	WDISABLE_COPY_AND_ASSIGN(CalPath);

	static const std::string &getResourceDir();
	static std::string __resourcePath;
};

#endif // _CAL_PATH_H_
