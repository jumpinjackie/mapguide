//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: LocalFileInputStream.cpp,v 1.8 2006/10/30 17:45:52 bostic Exp $
//

#include "LocalFileInputStream.hpp"
#include "UTF8.hpp"
#include <xercesc/framework/LocalFileInputSource.hpp>

using namespace DbXml;
XERCES_CPP_NAMESPACE_USE

LocalFileInputStream::LocalFileInputStream(const std::string &filename)
	: BaseInputStream(0)
{
	InputSource *is = new LocalFileInputSource(
		UTF8ToXMLCh(filename).str());
	setStream(is);
}

LocalFileInputStream::~LocalFileInputStream() {
}

