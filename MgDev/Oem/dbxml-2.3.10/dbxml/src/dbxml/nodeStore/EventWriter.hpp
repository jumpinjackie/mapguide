//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: EventWriter.hpp,v 1.4 2006/11/03 17:26:11 gmf Exp $
//

#ifndef __DBXMLEVENTWRITER_HPP
#define __DBXMLEVENTWRITER_HPP

#include <dbxml/XmlEventWriter.hpp>

/*
 * EventWriter -- internal extension of XmlEventWriter
 *
 * This is an abstract interface, and is the primary "push"
 * interface used inside BDB XML
 */

namespace DbXml
{

// forward
class NsEventAttrList;
class NsNode;
	
//
// EventWriter
//
class EventWriter : public XmlEventWriter
{
public:
	virtual void writeSniffedEncoding(const unsigned char *enc) {}
	virtual void writeStartElementWithAttrs(const unsigned char *localName,
						const unsigned char *prefix,
						const unsigned char *uri,
						int numAttributes,
						NsEventAttrList *attrs,
						const NsNode *node,
						bool isEmpty) = 0;

	virtual void writeEndElementWithNode(const unsigned char *localName,
					     const unsigned char *prefix,
					     const unsigned char *uri,
					     const NsNode *node) = 0;
	virtual void writeTextWithEscape(XmlEventReader::XmlEventType type,
					 const unsigned char *chars,
					 int length,
					 bool needsEscape) = 0;
	// TBD: methods to pass type info for elements?  Add name/uri to
	// StartElement, probably
};
}
	
#endif

