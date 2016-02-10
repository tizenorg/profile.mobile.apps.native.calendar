#ifndef CALPAHT_H
#define CALPAHT_H

#include "WDefine.h"

#include <string>
#include <app.h>

class WAPP_ASSIST_EXPORT CalPath
{
public:

	enum DirType
	{
		RESOURCE = 0,
		EDGE,
		LOCALE,
		IMAGE,
		THEME,
	};

	static const std::string &getEdjeDir();
	static const std::string &getLocaleDir();
	static const std::string &getThemeDir();
	static const std::string &getImageDir();
	static std::string getPath(const std::string &relativePath, DirType type = EDGE);
private:
	CalPath();
	WDISABLE_COPY_AND_ASSIGN(CalPath);

	static const std::string &getResourceDir();
	static std::string __resourcePath;
};

#endif // CALPAHT_H
