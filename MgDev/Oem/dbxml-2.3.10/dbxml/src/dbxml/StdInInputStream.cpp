//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: StdInInputStream.cpp,v 1.7 2006/10/30 17:45:53 bostic Exp $
//

#include "DbXmlInternal.hpp"
#include "StdInInputStream.hpp"
#include <xercesc/framework/StdInInputSource.hpp>

using namespace DbXml;

StdInInputStream::StdInInputStream() : BaseInputStream(new XERCES_CPP_NAMESPACE_QUALIFIER StdInInputSource()) {
}

StdInInputStream::~StdInInputStream() {
}
