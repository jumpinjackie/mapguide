//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: StdInInputStream.cpp,v 1.5 2005/08/16 13:29:31 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include "StdInInputStream.hpp"
#include <xercesc/framework/StdInInputSource.hpp>

using namespace DbXml;

StdInInputStream::StdInInputStream() : BaseInputStream(new XERCES_CPP_NAMESPACE_QUALIFIER StdInInputSource()) {
}

StdInInputStream::~StdInInputStream() {
}
