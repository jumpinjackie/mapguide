//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlNamespace.hpp,v 1.18 2005/04/05 16:43:55 bostic Exp $
//

#ifndef __XMLNAMESPACE_HPP
#define	__XMLNAMESPACE_HPP

#include "XmlPortability.hpp"

namespace DbXml
{
extern const DBXML_EXPORT char * metaDataNamespace_uri; // = "http://www.sleepycat.com/2002/dbxml";
extern const DBXML_EXPORT char * metaDataNamespace_prefix; // = "dbxml";

extern const DBXML_EXPORT char * metaDataName_name; // = "name";
extern const DBXML_EXPORT char * metaDataName_root; // = "root";
}

#endif
