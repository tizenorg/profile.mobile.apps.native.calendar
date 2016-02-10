#ifndef CALPATH_H
#define CALPATH_H

#include "WDefine.h"

#include <string>
#include <app.h>

class WAPP_ASSIST_EXPORT CalPath
{
public:

	/**
	 * @brief Resource type
	 */
	enum DirType
	{
		RESOURCE = 0,
		EDGE,
		LOCALE,
		IMAGE,
		THEME,
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
	 * @brief Get resource file path.
	 * @param[in]	relativePath	File path relative to resource directory
	 * @param[in]	type	Resource type such as IMAGES, EDJE, etc.
	 * @return Full file path
	 */
	static std::string getPath(const std::string &relativePath, DirType type = EDGE);

private:
	CalPath();
	WDISABLE_COPY_AND_ASSIGN(CalPath);

	static const std::string &getResourceDir();
	static std::string __resourcePath;
};

#endif // CALPATH_H
