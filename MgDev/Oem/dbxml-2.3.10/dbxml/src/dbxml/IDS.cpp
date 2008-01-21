//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: IDS.cpp,v 1.28 2006/10/30 17:45:52 bostic Exp $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlException.hpp"
#include "IDS.hpp"
#include "TypeConversions.hpp"
#include "OperationContext.hpp"
#include "Container.hpp"

using namespace std;
using namespace DbXml;

string IDS::toString() const
{
	string s;
	s += "[";
	s += DbXml::toString((u_int32_t)ids.size());
	s += "]";
	IDS::const_iterator p;
	for (p = ids.begin();p != ids.end();++p) {
		s += " ";
		s += (*p).asString();
	}
	return s;
}
