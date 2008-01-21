//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: XmlNamespace.hpp,v 1.20 2006/10/30 17:45:48 bostic Exp $
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
