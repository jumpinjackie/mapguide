#include "../config/pathan_config.h"
#include <pathan/internal/utils/ContextUtils.hpp>
#include "DateUtils.hpp"

#include <string.h>

int ContextUtils::getTimezone()
{
	time_t tt;
	time(&tt);

	struct tm tm;
	struct tm *tm_p = DateUtils::threadsafe_gmtime(&tt, &tm);

	return (int)mktime(tm_p) - (int)tt;
}
