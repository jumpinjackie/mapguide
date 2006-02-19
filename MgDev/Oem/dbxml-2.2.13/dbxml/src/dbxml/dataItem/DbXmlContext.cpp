//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlContext.cpp,v 1.4 2005/04/05 16:44:04 bostic Exp $
//

#include "DbXmlContext.hpp"
#include "../UTF8.hpp"
#include "../Manager.hpp"
#include <sstream>

#if defined(XERCES_HAS_CPP_NAMESPACE)
  XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

void DbXmlContext::trace(const XMLCh* message1, const XMLCh* message2) {
	XMLChToUTF8 m1(message1);
	XMLChToUTF8 m2(message2);
	ostringstream oss;
	oss << m1.str() << " " << m2.str();
	QueryContext &qc = getQueryContext();
	((Manager &)qc.getManager()).log(Log::C_QUERY, Log::L_INFO,
					 oss.str());
}

