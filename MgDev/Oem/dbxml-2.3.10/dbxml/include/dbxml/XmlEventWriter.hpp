//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: XmlEventWriter.hpp,v 1.8 2006/11/03 17:26:10 gmf Exp $
//

#ifndef __XMLEVENTWRITER_HPP
#define __XMLEVENTWRITER_HPP

#include "DbXmlFwd.hpp"
#include "XmlPortability.hpp"
#include "XmlEventReader.hpp"
#include <string>
 
namespace DbXml
{

class DBXML_EXPORT XmlEventWriter
{
public:
	virtual ~XmlEventWriter();
	virtual void close();

	// attributes (including namespaces)
	virtual void writeAttribute(const unsigned char *localName,
				    const unsigned char *prefix,
				    const unsigned char *uri,
				    const unsigned char *value,
				    bool isSpecified);

	// text, comments, CDATA, ignorable whitespace
	// length does not include trailing null
	virtual void writeText(XmlEventReader::XmlEventType type,
			       const unsigned char *text,
			       int length);

	// processing Instruction
	virtual void writeProcessingInstruction(const unsigned char *target,
						const unsigned char *data);

	// elements
	virtual void writeStartElement(const unsigned char *localName,
				       const unsigned char *prefix,
				       const unsigned char *uri,
				       int numAttributes,
				       bool isEmpty);
	virtual void writeEndElement(const unsigned char *localName,
				     const unsigned char *prefix,
				     const unsigned char *uri);
	
	// DTD
	virtual void writeDTD(const unsigned char *dtd, int length);

	// XML decl and start document
	virtual void writeStartDocument(const unsigned char *version,
					const unsigned char *encoding,
					const unsigned char *standalone);
	virtual void writeEndDocument();

	// note the start of entity expansion
	virtual void writeStartEntity(const unsigned char *name,
				      bool expandedInfoFollows);

	// note the end of entity expansion
	virtual void writeEndEntity(const unsigned char *name);
};
}
	
#endif

