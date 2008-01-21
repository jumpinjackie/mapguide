//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsDomReader.hpp,v 1.5 2006/10/30 17:45:57 bostic Exp $
//

#ifndef __DBXMLNSDOMREADER_HPP
#define __DBXMLNSDOMREADER_HPP
	
#include "EventReader.hpp"

namespace DbXml
{

class NsNode;
class NsDocument;
class NsDomElement;
class NsDomReaderNodeList;

	
/**
 * NsDomReader
 *
 */
class NsDomReader : public EventReader {
public:
	NsDomReader(NsDomElement *node);
	virtual ~NsDomReader();
	// the public XmlEventReader interface

	// iterators
	virtual XmlEventType next();

	// naming
	virtual const unsigned char *getNamespaceURI() const;
	// getlocalName returns processing instruction target
	virtual const unsigned char *getLocalName() const;
	virtual const unsigned char *getPrefix() const;
	
	// value -- text values and processing instruction data
	virtual const unsigned char *getValue(int &len) const;

	// attribute access (includes namespace attributes)
	virtual int getAttributeCount() const;
	virtual bool isAttributeSpecified(int index) const;
	virtual const unsigned char *getAttributeLocalName(int index) const;
	virtual const unsigned char *getAttributeNamespaceURI(int index) const;
	virtual const unsigned char *getAttributePrefix(int index) const;
	virtual const unsigned char *getAttributeValue(int index) const;

	// start_document methods -- see EventReader

	// BDB XML "extensions" in XmlEventReader
	virtual bool needsEntityEscape(int index = 0) const;
	virtual bool isEmptyElement() const;

	// required by EventReader
	const NsNode *getNsNode() const;
	const unsigned char *getVersion() const;
	const unsigned char *getEncoding() const;
	bool standaloneSet() const;
	bool encodingSet() const;
	bool isStandalone() const;

private:
	bool startEvent();
	bool endEvent();
	bool childEvent();
	void popElement();
private:
	NsDomReaderNodeList *current_;
	NsNode *node_;
	bool popElement_;
	const xmlbyte_t *piData_;
	bool deallocValue_;
	bool needsEscape_;
	int entityCount_;
	bool savedEntityText_;
	NsDocument *doc_;
};

}

#endif
