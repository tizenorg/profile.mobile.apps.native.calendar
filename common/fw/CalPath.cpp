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

#include "CalPath.h"
#include "CalDefines.h"

#include <app_common.h>

std::string CalPath::__resourcePath = "";

CalPath::CalPath()
{
}

const std::string &CalPath::getEdjeDir()
{
	static std::string edjeDir;
	if(edjeDir.empty())
	{
		edjeDir = getResourceDir();
		edjeDir.append(CAL_EDJE_DIR);
	}
	return edjeDir;
}

const std::string &CalPath::getLocaleDir()
{
	static std::string localeDir;
	if(localeDir.empty())
	{
		localeDir = getResourceDir();
		localeDir.append(CAL_LOCALE_DIR);
	}
	return localeDir;
}

const std::string &CalPath::getThemeDir()
{
	static std::string themeDir;
	if(themeDir.empty())
	{
		themeDir = getEdjeDir();
		themeDir.append(CAL_THEME_DIR);
	}
	return themeDir;
}

const std::string &CalPath::getImageDir()
{
	static std::string imageDir;
	if(imageDir.empty())
	{
		imageDir = getResourceDir();
		imageDir.append(CAL_IMAGE_DIR);
	}
	return imageDir;
}

const std::string &CalPath::getResourceDir()
{
	static std::string resourceDir;
	if(resourceDir.empty())
	{
#ifdef CAL_DIR
		resourceDir = CAL_DIR "/res/";
#else
		char * resPath = app_get_resource_path();
		resourceDir = resPath;
		free(resPath);
#endif
	}
	return resourceDir;
}

std::string CalPath::getPath(const std::string &relativePath, DirType type)
{
	switch (type) {
	case EDJE:
		return std::string(getEdjeDir()).append(relativePath);
		break;
	case LOCALE:
		return std::string(getLocaleDir()).append(relativePath);
		break;
	case THEME:
		return std::string(getThemeDir()).append(relativePath);
		break;
	case IMAGE:
		return std::string(getImageDir()).append(relativePath);
		break;
	case RESOURCE:
	default:
		break;
	}
	return std::string(getResourceDir()).append(relativePath);
}
