//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: MemBufInputStream.cpp,v 1.10 2006/10/30 17:45:53 bostic Exp $
//

#include "MemBufInputStream.hpp"
#include "Document.hpp"
#include <xercesc/framework/MemBufInputSource.hpp>

using namespace DbXml;
XERCES_CPP_NAMESPACE_USE

MemBufInputStream::MemBufInputStream(const char * srcDocBytes,
                                     const unsigned int	byteCount,
                                     const char *const bufId,
                                     const bool adoptBuffer)
	: BaseInputStream(0)
{
	InputSource *is = new MemBufInputSource(
		(XMLByte*)srcDocBytes,
		byteCount, bufId, adoptBuffer);
	setStream(is);
}

MemBufInputStream::~MemBufInputStream()
{
}

