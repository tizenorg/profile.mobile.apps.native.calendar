#include "CalPath.h"
#include "CalDefines.h"

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
		resourceDir = CAL_RES_DIR;
	}
	return resourceDir;
}

std::string CalPath::getPath(const std::string &relativePath, DirType type)
{
	switch (type) {
	case EDGE:
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
