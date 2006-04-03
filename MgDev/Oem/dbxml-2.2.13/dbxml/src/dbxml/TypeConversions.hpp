//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: TypeConversions.hpp,v 1.7 2005/04/05 16:44:03 bostic Exp $
//

#ifndef __TYPECONVERSIONS_HPP
#define	__TYPECONVERSIONS_HPP

#include <string>
#include <db_cxx.h>

#if defined(WIN32) && !defined(__CYGWIN__)
#define snprintf _snprintf
#endif

namespace DbXml
{

inline std::string toString(int i)
{
	char buf[64];
	::snprintf(buf, 64, "%d", i);
	return std::string(buf);
}

inline std::string toString(u_int32_t i)
{
	char buf[64];
	::snprintf(buf, 64, "%u", i);
	return std::string(buf);
}

inline std::string toString(double d)
{
	char buf[128];
	::snprintf(buf, 128, "%0.17g", d);
	return std::string(buf);
}

inline int stringToInt(const std::string &s)
{
	return ::atoi(s.c_str());
}

}

#endif

