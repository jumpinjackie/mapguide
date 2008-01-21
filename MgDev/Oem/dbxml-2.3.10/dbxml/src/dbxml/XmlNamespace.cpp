//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: XmlNamespace.cpp,v 1.24 2006/10/30 17:45:54 bostic Exp $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlNamespace.hpp"

namespace DbXml
{
// NOTE: if these change value, or number, or in any way, dist/swig/dbxml.i
// must be modified for non-C++ language access
const char *metaDataNamespace_uri = "http://www.sleepycat.com/2002/dbxml";
const char *metaDataNamespace_prefix = "dbxml";

const char *metaDataName_name = "name";
const char *metaDataName_default = "default";
const char *metaDataName_root = "root";

}
