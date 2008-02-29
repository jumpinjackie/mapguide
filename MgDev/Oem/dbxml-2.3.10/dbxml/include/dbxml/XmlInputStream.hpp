//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: XmlInputStream.hpp,v 1.12 2006/10/30 17:45:48 bostic Exp $
//

#ifndef __XMLINPUTSTREAM_HPP
#define	__XMLINPUTSTREAM_HPP

#include "DbXmlFwd.hpp"
#include "XmlPortability.hpp"

namespace DbXml
{

class DBXML_EXPORT XmlInputStream
{
public:
	virtual ~XmlInputStream();

	virtual unsigned int curPos () const = 0;
	virtual unsigned int readBytes (char *toFill, 
		const unsigned int maxToRead) = 0;
protected:
	XmlInputStream() {}
private:
	XmlInputStream(const XmlInputStream &);
	XmlInputStream &operator=(const XmlInputStream &);
};

}

#endif

