//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: XmlEventReaderToWriter.hpp,v 1.4 2006/10/30 17:45:48 bostic Exp $
//

#ifndef __XMLEVENTREADERTOWRITER_HPP
#define __XMLEVENTREADERTOWRITER_HPP

#include "DbXmlFwd.hpp"
#include "XmlPortability.hpp"

/*
 * XmlEventReaderToWriter
 *
 * Write to XmlEventWriter from XmlEventReader events
 */
namespace DbXml
{

class XmlEventReader;
class XmlEventWriter;
class EventReaderToWriter;
	
class DBXML_EXPORT XmlEventReaderToWriter
{
public:
	XmlEventReaderToWriter();
	XmlEventReaderToWriter(const XmlEventReaderToWriter&);
	XmlEventReaderToWriter &operator=(const XmlEventReaderToWriter &);

	XmlEventReaderToWriter(XmlEventReader &reader,
			       XmlEventWriter &writer,
			       bool ownsReader = true);
	~XmlEventReaderToWriter();
	void start();

	bool isNull() const { return impl_ == 0; }
private:
	EventReaderToWriter *impl_;
};
}
	
#endif

