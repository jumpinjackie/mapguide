//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlResolver.cpp,v 1.4 2005/04/05 16:44:04 bostic Exp $
//

#include "dbxml/XmlResolver.hpp"

using namespace DbXml;
using namespace std;

bool XmlResolver::resolveDocument(XmlTransaction *txn, XmlManager &mgr,
				  const string &uri, XmlValue &result) const
{
  return false;
}

bool XmlResolver::resolveCollection(XmlTransaction *txn, XmlManager &mgr,
				    const string &uri, XmlResults &result) const
{
  return false;
}

XmlInputStream *XmlResolver::resolveSchema(XmlTransaction *txn, XmlManager &mgr,
					   const std::string &schemaLocation,
					   const std::string &nameSpace) const
{
  return 0;
}

XmlInputStream *XmlResolver::resolveEntity(XmlTransaction *txn, XmlManager &mgr,
					   const std::string &systemId,
					   const std::string &publicId) const
{
  return 0;
}
