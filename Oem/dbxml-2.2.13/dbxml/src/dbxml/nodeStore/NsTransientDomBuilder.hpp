//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsTransientDomBuilder.hpp,v 1.3 2005/09/20 14:35:56 jsnelson Exp $
//

#ifndef __DBXMLNSXERCESHANDLER_HPP
#define __DBXMLNSXERCESHANDLER_HPP

#include "NsEvent.hpp"
#include "NsHandlerBase.hpp"

namespace DbXml {
	
class NsDomElement;
	
/**
 * NsTransientDomBuilder
 *
 * Implements NsEventHandler, and derives from NsHandlerBase.
 *
 * Purely transient Xerces parses, for querying.
 *
 */
class NsTransientDomBuilder : public NsEventHandler, public NsHandlerBase {
public:
	NsTransientDomBuilder(XER_NS MemoryManager *mmgr, NsDocument *doc,
			NsDomFactory *factory);
	virtual ~NsTransientDomBuilder();
	virtual void startDocument(const xmlch_t *sniffedEncoding);
	virtual void xmlDecl(const xmlch_t *xmlDecl,
			     const xmlch_t *encodingStr,
			     const xmlch_t *standaloneStr);
	virtual void endDocument();
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
	virtual void ignorableWhitespace(const xmlch_t *characters,
					 uint32_t len, bool isCDATA);
	virtual void comment(const xmlch_t *comment, uint32_t len);
	virtual void processingInstruction(const xmlch_t *target,
					   const xmlch_t *data);
	virtual void setTypeInfo(const xmlch_t *name,
				 const xmlch_t *uri) {}
	// use default (no-op) implementations for docTypeDecl(),
	// startEntity() and endEntity().  No need to have that information
	// during a query.
private:
	NsDomElement *createDomElem(nsNode_t *node);
private:
	NsDomFactory *_domFactory;
	NsDomElement *_domCurrent;
	NsDomElement *_domPrevious;
};

}

#endif
