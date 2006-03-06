//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsXercesTranscoder.hpp,v 1.8 2005/09/20 14:35:57 jsnelson Exp $
//

#ifndef __DBXMLNSXERCESTRANSCODER_HPP
#define __DBXMLNSXERCESTRANSCODER_HPP
	
#include "NsEvent.hpp"
#include "NsHandlerBase.hpp"
#include "NsDocument.hpp"

namespace DbXml {
	
/**
 * NsXercesTranscoder
 *
 * Implements NsEventHandler, and derives from NsHandlerBase.
 *
 * This class handles Xerces events, and transcodes to UTF-8,
 * for parsing into persistent nodes.
 *
 */
class NsXercesTranscoder : public NsEventTranslator, public NsHandlerBase {
public:
	NsXercesTranscoder(Transaction *txn, NsDocumentDatabase *db, const ID &docId,
			 u_int32_t flags, XER_NS MemoryManager *mmgr);
	virtual ~NsXercesTranscoder();
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
				 const xmlch_t *uri);
	virtual void docTypeDecl(const xmlch_t *data, uint32_t len);
	virtual void startEntity(const xmlch_t *name, uint32_t len);
	virtual void endEntity(const xmlch_t *name, uint32_t len);

	virtual NsEventHandler8 *getNsEventHandler(void) const {
		return _handler;
	}
	virtual void setNsEventHandler(NsEventHandler8 *handler) {
		_handler = handler;
	}
private:
	void doCharacters(const xmlch_t *characters,
			  uint32_t len, bool isCDATA,
			  bool ignorable);
private:
	NsDocument _document;
};

}

#endif
