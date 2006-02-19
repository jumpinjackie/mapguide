//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: UTF8.hpp,v 1.9 2005/04/05 16:44:03 bostic Exp $
//

#ifndef __UTF8_HPP
#define	__UTF8_HPP

#include <xercesc/util/XMLUTF8Transcoder.hpp>

#include <string>

namespace DbXml
{

class UTF8ToXMLCh
{
public:
	UTF8ToXMLCh(const std::string &s);
	UTF8ToXMLCh(const char *s, size_t l);
	~UTF8ToXMLCh();
	const XMLCh *str() const;
	XMLCh *strToModify() { return xmlch_; }
	int len() const;
	XMLCh *adopt();
private:
	XMLCh *xmlch_;
	int len_;
};

class XMLChToUTF8
{
public :
	XMLChToUTF8(const XMLCh* const toTranscode, int len = 0);
	~XMLChToUTF8();
	const char *str() const;
	char *strToModify() { return (char*) p_; }
	int len() const;
	char *adopt();
private :
	XMLByte *p_;
	int len_;
};

}

#endif
