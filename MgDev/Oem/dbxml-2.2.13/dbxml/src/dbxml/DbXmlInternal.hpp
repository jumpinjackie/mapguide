//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlInternal.hpp,v 1.20 2005/11/22 00:57:35 gmf Exp $
//

#ifndef __DBXMLINTERNAL_HPP
#define __DBXMLINTERNAL_HPP

#if defined(_MSC_VER)
// versions check:
// we don't support Visual C++ prior to version 6:
#if (_MSC_VER<1200)
#error "Compiler version not supported"
#endif // _MSC_VER<1200

// turn off deprecated CRT function warnings from Visual Studio 2005
// TBD: consider changing to use secure version
#define _CRT_SECURE_NO_DEPRECATE 1
// this line would automatically use the secure versions
// #define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1

// turn off the warnings before we #include anything
#pragma warning( disable : 4786 ) // ident trunc to '255' chars in debug info
#pragma warning( disable : 4503 ) // warning: decorated name length exceeded
#pragma warning( disable : 4290 ) // warning: C++ exception specification ignored
#pragma warning( disable : 4996 ) // warning: deprecation of strdup() in VS8
//disable warnings on extern before template instantiation
#pragma warning (disable : 4231)

#endif

#include "dbxml_config.h"
#include "dbxml/XmlPortability.hpp"
#include <string>
#include "dbxml/XmlException.hpp"

namespace DbXml
{

void assert_fail(const char *expression, const char *file, int line);

static inline void checkNullPointer(const void *ptr, const char *message)
{
	if (ptr == 0) {
		std::string mesg = "Attempt to use uninitialized object: ";
		mesg += message;
		throw XmlException(XmlException::NULL_POINTER, mesg);
	}
}
	
}

#ifdef ENABLE_ASSERTS
#  define DBXML_ASSERT(expr)((void)((expr) ? 0 : (assert_fail(#expr, __FILE__, __LINE__), 0)))
#else
#  define DBXML_ASSERT(expr)((void)(0))
#endif

#endif
