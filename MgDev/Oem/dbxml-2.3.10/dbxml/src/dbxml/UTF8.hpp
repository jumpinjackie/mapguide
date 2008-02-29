//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: UTF8.hpp,v 1.12 2006/10/30 17:45:53 bostic Exp $
//

#ifndef __UTF8_HPP
#define	__UTF8_HPP

#include <xercesc/util/XMLUTF8Transcoder.hpp>

#include <string>

namespace DbXml
{
//
// UTF8ToXMLCh and XMLChToUTF8 always allocate at least
// one byte for even a null string, so that str() always
// returns a valid pointer (possibly to null).
// This semantic is required by some code that uses them.
//
// The *Null versions allow a null pointer, which is
// more efficient when a real pointer is not required.
//
class UTF8ToXMLCh
{
public:
	UTF8ToXMLCh() : xmlch_(0), len_(0) {}
	UTF8ToXMLCh(const std::string &s);
	UTF8ToXMLCh(const char *s, size_t l);
	~UTF8ToXMLCh();
	const XMLCh *str() const;
	XMLCh *strToModify() { return xmlch_; }
	int len() const;
	XMLCh *adopt();
protected:
	void init(const char *s, size_t l);
	XMLCh *xmlch_;
	int len_;
};

class XMLChToUTF8
{
public :
	XMLChToUTF8() : p_(0), len_(0) {}
	XMLChToUTF8(const XMLCh* const toTranscode, int len = 0);
	~XMLChToUTF8();
	const char *str() const {
		return (const char *)p_;
	}
	char *strToModify() { return (char*) p_; }
	int len() const { return len_; }
	char *adopt();
protected:
	void init(const XMLCh* const toTranscode, int len = 0);
	XMLByte *p_;
	int len_;
};

class UTF8ToXMLChNull : public UTF8ToXMLCh
{
public:
	UTF8ToXMLChNull(const std::string &s) {
		if (s.length() != 0)
			init(s.c_str(), s.length());
	}
	UTF8ToXMLChNull(const char *s, size_t l) {
		if (s)
			init(s, l);
	}
	~UTF8ToXMLChNull() {
		if (xmlch_)
			delete [] xmlch_;
	}
};

class XMLChToUTF8Null : public XMLChToUTF8
{
public:
	XMLChToUTF8Null(const XMLCh* const toTranscode, int len = 0) {
		if (toTranscode)
			init(toTranscode, len);
	}
};
		
}

#endif
