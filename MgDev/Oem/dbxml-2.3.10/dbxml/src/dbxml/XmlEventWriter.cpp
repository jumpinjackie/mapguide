//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: XmlEventWriter.cpp,v 1.8 2006/11/03 17:26:10 gmf Exp $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlEventWriter.hpp"
#include "dbxml/XmlException.hpp"

using namespace DbXml;
using namespace std;

/*
 * XmlEventWriter
 *
 * Default (no-op) implementation
 */

XmlEventWriter::~XmlEventWriter() {}

void XmlEventWriter::close() {
	delete this;
}

void XmlEventWriter::writeAttribute(const unsigned char *localName,
				    const unsigned char *prefix,
				    const unsigned char *uri,
				    const unsigned char *value,
				    bool isSpecified)
{

}

void XmlEventWriter::writeText(XmlEventReader::XmlEventType type,
			       const unsigned char *text,
			       int length)
{

}

void XmlEventWriter::writeProcessingInstruction(const unsigned char *target,
						const unsigned char *data)
{

}

void XmlEventWriter::writeStartElement(const unsigned char *localName,
				       const unsigned char *prefix,
				       const unsigned char *uri,
				       int numAttributes,
				       bool isEmpty)
{

}

void XmlEventWriter::writeEndElement(const unsigned char *localName,
				     const unsigned char *prefix,
				     const unsigned char *uri)
{

}
	
void XmlEventWriter::writeDTD(const unsigned char *dtd, int length)
{
}

void XmlEventWriter::writeStartDocument(const unsigned char *version,
					const unsigned char *encoding,
					const unsigned char *standalone)
{

}

void XmlEventWriter::writeEndDocument()
{

}

void XmlEventWriter::writeStartEntity(const unsigned char *name,
				      bool expandedInfoFollows)
{
}

void XmlEventWriter::writeEndEntity(const unsigned char *name)
{
}
