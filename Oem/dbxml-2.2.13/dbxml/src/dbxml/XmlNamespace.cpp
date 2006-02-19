//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlNamespace.cpp,v 1.22 2005/08/16 13:29:32 jsnelson Exp $
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
