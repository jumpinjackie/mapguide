//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: URLInputStream.cpp,v 1.7 2006/10/30 17:45:53 bostic Exp $
//

#include "URLInputStream.hpp"
#include "UTF8.hpp"
#include <xercesc/framework/URLInputSource.hpp>

using namespace DbXml;
XERCES_CPP_NAMESPACE_USE

URLInputStream::URLInputStream(const std::string &baseId, 
                               const std::string &systemId, 
                               const std::string &publicId)
	: BaseInputStream(0)
{
	InputSource *is = new URLInputSource(UTF8ToXMLCh(baseId).str(),
					     UTF8ToXMLCh(systemId).str(),
					     UTF8ToXMLCh(publicId).str());
	setStream(is);
	
}

URLInputStream::URLInputStream(const std::string &baseId, 
                               const std::string &systemId)
	: BaseInputStream(0)
{
	InputSource *is = new URLInputSource(
		UTF8ToXMLCh(baseId).str(),
		UTF8ToXMLCh(systemId).str());
	setStream(is);
}

URLInputStream::~URLInputStream() {
}

