#include "CalTimeZone.h"
#include "CalDebugInternal.h"
#include <utils_i18n.h>
#include <stdexcept>
#include <sqlite3.h>

#define TIME_MINUTES_IN_HOUR   60
#define TIME_SECONDS_IN_MINUTE 60
#define TIME_SECONDS_IN_HOUR   (TIME_MINUTES_IN_HOUR*TIME_SECONDS_IN_MINUTE)

#define CAL_WORLDCLOCK_DB_TIMEZONE_QUERY \
	"SELECT tz_path FROM city_table where timezone=\"%s\" limit 1"

#define CAL_WORLDCLOCK_DB_FILE "/usr/dbspace/.worldclock.db"

#define DATETIME_BUFFER 64
#define TIMEZONE_LABEL_BUFFER 128
#define TIMEZONE_LABEL_PREFIX_FORMAT "GMT %+d:%02d"
#define QUERY_DB_BUFFER (64 * 4)



class __CalTimeZoneImpl {
public:
	__CalTimeZoneImpl(const std::string& tz);
	~__CalTimeZoneImpl();

	int getOffset() const;
	std::string getScreenName() const;
	std::string getTzDatabaseName() const;

private:
	std::string __getTzByOffsetFromWordclock(int offset) const;
	std::string __getTzEasName() const;
	const std::string __getTzOffset(int offset) const;

	i18n_timezone_h __timezone;
	std::string __tz;
	int __offset;
};

__CalTimeZoneImpl::__CalTimeZoneImpl(const std::string& tz)
{
	int error = I18N_ERROR_NONE;
	error = i18n_timezone_create(&__timezone, tz.c_str());
	if (error != I18N_ERROR_NONE)
	{
		WERROR("i18n_timezone_create error");
		throw std::runtime_error("i18n_timezone_create error");
	}
	__tz = tz;
	int32_t offset_milliseconds = -1;
	i18n_timezone_get_raw_offset(__timezone, &offset_milliseconds);
	__offset = offset_milliseconds / 1000;
}

__CalTimeZoneImpl::~__CalTimeZoneImpl()
{
	i18n_timezone_destroy(__timezone);
}

int __CalTimeZoneImpl::getOffset() const
{
	return __offset;
}

std::string __CalTimeZoneImpl::__getTzEasName() const
{
	std::string tzEasName;
	char *tzname = NULL;
	int error = I18N_ERROR_NONE;
	error = i18n_timezone_get_display_name(__timezone, &tzname);
	if (error != I18N_ERROR_NONE)
	{
		WERROR("i18n_timezone_get_display_name error");
		free(tzname);
	}
	else
	{
		tzEasName = tzname;
	}

	return tzEasName;
}

std::string __CalTimeZoneImpl::getScreenName() const
{
	std::string tz;
	char timezone[TIMEZONE_LABEL_BUFFER] = {0};
	std::string tz_offset = __getTzOffset(__offset);
	std::string tz_name = __getTzEasName();

	if (__tz.find(TIMEZONE_UNKNOWN) == std::string::npos)
	{
		snprintf(timezone, TIMEZONE_LABEL_BUFFER, "(%s) %s", tz_offset.c_str(), tz_name.c_str());
	}
	else
	{
		snprintf(timezone, TIMEZONE_LABEL_BUFFER, "(%s)", tz_offset.c_str());
	}
	return timezone;
}

const std::string __CalTimeZoneImpl::__getTzOffset(int offset) const
{
	int gmtoffset_hour = offset / TIME_SECONDS_IN_HOUR;
	int gmtoffset_min = (offset % TIME_SECONDS_IN_HOUR) / TIME_SECONDS_IN_MINUTE;
	gmtoffset_min *= gmtoffset_min < 0 ? -1 : 1;

	std::string tzOffset;

	char buffer[DATETIME_BUFFER] = {0};
	snprintf(buffer, DATETIME_BUFFER, TIMEZONE_LABEL_PREFIX_FORMAT, gmtoffset_hour, gmtoffset_min);
	tzOffset = buffer;
	return tzOffset;
}

std::string __CalTimeZoneImpl::getTzDatabaseName() const
{
	return __tz;
}

std::string __CalTimeZoneImpl::__getTzByOffsetFromWordclock(int offset) const
{
	WENTER();
	std::string timeZone;

	const int index_buffer = 8;
	const int offset_buffer = 16;

	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret = 0;

	do
	{
		ret = sqlite3_open(CAL_WORLDCLOCK_DB_FILE, &db);
		if (ret != SQLITE_OK) {
			WERROR("sqlite3_open(%s) is failed(%d)", CAL_WORLDCLOCK_DB_FILE, ret);
			break;
		}

		char offset_hour[index_buffer];
		int timezone_offset = offset / TIME_SECONDS_IN_MINUTE;
		int timezone_offset_hour = timezone_offset / TIME_MINUTES_IN_HOUR;
		int timezone_offset_minute = timezone_offset % TIME_MINUTES_IN_HOUR;

		if (0 <= timezone_offset_hour)
		{
			snprintf(offset_hour, sizeof(offset_hour), "+%d", timezone_offset_hour);
		}
		else
		{
			snprintf(offset_hour, sizeof(offset_hour), "%d", timezone_offset_hour);
		}

		char offset[offset_buffer];
		if (timezone_offset_minute)
		{
			snprintf(offset, sizeof(offset), "GMT%s:%d", offset_hour, timezone_offset_minute);
		}
		else
		{
			snprintf(offset, sizeof(offset), "GMT%s", offset_hour);
		}

		char query[QUERY_DB_BUFFER];
		snprintf(query, QUERY_DB_BUFFER, CAL_WORLDCLOCK_DB_TIMEZONE_QUERY, offset);

		ret = sqlite3_prepare_v2(db, query, strlen(query), &stmt, NULL);
		if (ret != SQLITE_OK)
		{
			WERROR("sqlite3_prepare_v2(%s) is failed(%d)", query, ret);
			break;
		}

		ret = sqlite3_step(stmt);
		if (ret != SQLITE_ROW
			&& ret != SQLITE_OK
			&& ret != SQLITE_DONE)
		{
			WERROR("sqlite3_step is failed(%d) : %s", ret, sqlite3_errmsg(db));
			break;
		}

		timeZone = (char*) sqlite3_column_text(stmt, 0);
	}
	while (0);

	if (stmt)
	{
		sqlite3_finalize(stmt);
	}
	if (db)
	{
		sqlite3_close(db);
	}
	return timeZone;
}


/* Implementaion of CalTimeZone methods */
CalTimeZone::CalTimeZone(const char *tz)
{
	__pv = std::make_shared<__CalTimeZoneImpl>(tz ? tz :  TIMEZONE_UNKNOWN);
}

CalTimeZone::CalTimeZone(const CalTimeZone& obj)
{
	__pv = obj.__pv;
}

const CalTimeZone& CalTimeZone::operator=(const CalTimeZone& obj)
{
	if (this != &obj)
	{
		__pv = obj.__pv;
	}
	return *this;
}

CalTimeZone::~CalTimeZone()
{
}

std::string CalTimeZone::getString(LabelType typeFormat) const
{
	switch (typeFormat)
	{
	case TIMEZONE_TIZEN_FORMAT:
		return __pv->getTzDatabaseName();
		break;
	case TIMEZONE_EAS_FORMAT:
		return __pv->getScreenName();
		break;
	default:
		WASSERT_EX(false, "unknown timezone type");
		break;
	}
}

int CalTimeZone::getOffset() const
{
	return __pv->getOffset();
}
