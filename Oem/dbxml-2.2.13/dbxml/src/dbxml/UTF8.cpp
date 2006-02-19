//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: UTF8.cpp,v 1.13 2005/08/16 13:29:31 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include "UTF8.hpp"
#include "nodeStore/NsUtil.hpp"

using namespace DbXml;

UTF8ToXMLCh::UTF8ToXMLCh(const std::string &s)
{
	size_t l = s.length();
	xmlch_ = new XMLCh[l + 1];
	try {
		// len_ does not get trailing null
		len_ = NsUtil::nsFromUTF8(0, &xmlch_, (const xmlbyte_t *)s.c_str(),
					  l + 1, l + 1) - 1;
	} catch (...) { delete [] xmlch_; xmlch_ = 0; throw; }
}

// string may not be null terminated.
UTF8ToXMLCh::UTF8ToXMLCh(const char *s, size_t l)
{
	xmlch_ = new XMLCh[l + 1];
	try {
		// len_ does not get trailing null, so l must not include it.
		len_ = NsUtil::nsFromUTF8(0, &xmlch_, (const xmlbyte_t *)s, l,
					  l + 1);
		xmlch_[l] = 0; // Add null terminator
	} catch (...) { delete [] xmlch_; xmlch_ = 0; throw; }
}

UTF8ToXMLCh::~UTF8ToXMLCh()
{
	delete [] xmlch_;
}

const XMLCh *UTF8ToXMLCh::str() const
{
	return xmlch_;
}

int UTF8ToXMLCh::len() const
{
	return len_;
}

XMLCh *UTF8ToXMLCh::adopt()
{
	XMLCh *result = xmlch_;
	xmlch_ = 0;
	return result;
}

XMLChToUTF8::XMLChToUTF8(const XMLCh* const toTranscode, int len)
{
	if(toTranscode == 0) {
		p_ = new XMLByte[1];
		*p_ = 0;
		len_ = 0;
	} else {
		if (!len)
			len = NsUtil::nsStringLen(toTranscode);
		// 3 bytes per XMLCh is the worst case, + '\0'
		const unsigned int needed = len * 3 + 1;
		p_ = new XMLByte[needed];
		// len_ does not get trailing null
		len_ = NsUtil::nsToUTF8(0, &p_, toTranscode, len + 1,
					needed) - 1;
	}
}

XMLChToUTF8::~XMLChToUTF8()
{
	if (p_)
		delete [] p_;
}

const char *XMLChToUTF8::str() const
{
	return (const char *)p_;
}

int XMLChToUTF8::len() const
{
	return len_;
}

char *XMLChToUTF8::adopt()
{
	char *result = (char *)p_;
	p_ = 0;
	return result;
}

