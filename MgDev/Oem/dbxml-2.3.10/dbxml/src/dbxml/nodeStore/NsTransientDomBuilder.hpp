//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsTransientDomBuilder.hpp,v 1.10 2006/10/30 17:45:57 bostic Exp $
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
 * Implements NsEventHandler16, and derives from NsHandlerBase.
 *
 * Purely transient Xerces parses, for querying.
 *
 */
class NsTransientDomBuilder : public NsEventHandler16, public NsHandlerBase {
public:
	NsTransientDomBuilder(XER_NS MemoryManager *mmgr, NsDocument *doc,
			      NsDomFactory *factory, 
			      bool includeEntityInfo = false);
	virtual ~NsTransientDomBuilder();
	virtual void startDocument(const xmlch_t *sniffedEncoding);
	virtual void xmlDecl(const xmlch_t *xmlDecl,
			     const xmlch_t *encodingStr,
			     const xmlch_t *standaloneStr);
	virtual void endDocument();
	virtual void startElement(const xmlch_t *localName,
				  const xmlch_t *prefix,
				  const xmlch_t *uri,
				  NsEventAttrList16 *attrs,
				  const uint32_t attrCount,
				  bool isEmpty);
	virtual void endElement(const xmlch_t *localName,
				const xmlch_t *prefix,
				const xmlch_t *uri);
	virtual void characters(const xmlch_t *characters,
				uint32_t len, bool isCDATA,
				bool needsEscape);
	virtual void ignorableWhitespace(const xmlch_t *characters,
					 uint32_t len, bool isCDATA);
	virtual void comment(const xmlch_t *comment, uint32_t len);
	virtual void processingInstruction(const xmlch_t *target,
					   const xmlch_t *data);
#ifdef NS_USE_SCHEMATYPES	
	virtual void setTypeInfo(const xmlch_t *name,
				 const xmlch_t *uri) {}
#endif
	virtual void docTypeDecl(const xmlch_t *data, uint32_t len);
	virtual void startEntity(const xmlch_t *name, uint32_t len);
	virtual void endEntity(const xmlch_t *name, uint32_t len);
private:
	NsDomElement *createDomElem(NsNode *node);
private:
	NsDomFactory *_domFactory;
	NsDomElement *_domCurrent;
	NsDomElement *_domPrevious;
	bool _includeEntityInfo;
};

}

#endif
