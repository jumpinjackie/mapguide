//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsXercesIndexer.hpp,v 1.8 2005/04/20 18:31:33 bostic Exp $
//

#ifndef __DBXMLNSXERCESINDEXER_HPP
#define __DBXMLNSXERCESINDEXER_HPP
	
#include "NsEvent.hpp"
#include "NsHandlerBase.hpp"

namespace DbXml {

/**
 * NsXercesIndexer
 *
 * Implements NsEventHandler, and derives from NsHandlerBase.
 *
 * This class handles Xerces events, and transcodes to UTF-8
 * and ONLY generates NsEventHandler8 events to be handled by
 * the DB XML indexer.  This class only exists to streamline
 * indexing of document stored as whole-doc storage.
 * It bypasses creation of nsNode_t objects.
 *
 * It also does not implement interfaces known not to be of
 * interest to the indexer.
 *
 * Indexer only needs start/end element, and character events
 * involving text (no comments, no processing instructions)
 */
class NsXercesIndexer : public NsEventTranslator {
public:
	NsXercesIndexer(NsEventHandler8 *handler = 0);
	virtual ~NsXercesIndexer();
	virtual void startElement(const xmlch_t *localName,
				  const xmlch_t *prefix,
				  const xmlch_t *uri,
				  int32_t uriId,
				  NsEventAttrList *attrs,
				  const uint32_t attrCount,
				  bool isEmpty,
				  bool isRoot);
	virtual void endElement(const xmlch_t *localName,
				const xmlch_t *prefix,
				const xmlch_t *uri,
				bool isRoot);
	virtual void characters(const xmlch_t *characters,
				uint32_t len, bool isCDATA,
				bool needsEscape);
	virtual void comment(const xmlch_t *comment, uint32_t len) {}
	virtual void processingInstruction(const xmlch_t *target,
					   const xmlch_t *data) {}
	virtual NsEventHandler8 *getNsEventHandler(void) const {
		return _handler;
	}
	virtual void setNsEventHandler(NsEventHandler8 *handler) {
		_handler = handler;
	}
private:
	NsEventHandler8 *_handler;
};

/**
 * hold transcoded attribute strings, for deallocation
 */	
struct attrString {
	struct attrString *next;
	xmlbyte_t s[1];
};

/**
 * A Special, extended attribute list handler, with
 * methods tailored for the DB XML Indexer
 */
class NsEventAttrListIndexer : public NsEventAttrList8 {
public:
	// Indexer does not use most of these.
	NsEventAttrListIndexer(NsEventAttrList *alist,
			       const xmlch_t *elemName = 0,
			       const xmlch_t *elemUri = 0)
		: _alist(alist), _toFree(0), _elemName(elemName),
		  _elemUri(elemUri) {}
	virtual ~NsEventAttrListIndexer();
	virtual bool isEmpty(void) const { return true; } // unused
	virtual int numAttributes(void) const { return 0; } // unused
	virtual const xmlbyte_t *prefix(int index) const { return 0; } //unused
	virtual const xmlbyte_t *localName(int index) const;
	virtual const xmlbyte_t *qname(int index) { return 0; } // unused
	virtual const xmlbyte_t *value(int index) const;
	virtual const xmlbyte_t *uri(int index) const;
	virtual bool hasTypeInfo(int index) const {
		return false; // unused
	}
	virtual const xmlbyte_t *typeName(int index) const {
		return 0; //unused
	}
	virtual const xmlbyte_t *typeUri(int index) const {
		return 0; //unused
	}
	// don't call this function unless uri() is non-null
	virtual const int uriId(int index) const {
		return (-1); // unused
	}
	virtual const bool needsEscape(int index) const {
		return true; // unused
	}
	virtual const bool isSpecified(int index) const {
		return true; // unused
	}
private:
	struct attrString *_allocString(const xmlch_t *str,
					int &nchars) const;
private:
	NsEventAttrList *_alist;
	mutable struct attrString *_toFree;
	const xmlch_t *_elemName;
	const xmlch_t *_elemUri;
};

}

#endif
