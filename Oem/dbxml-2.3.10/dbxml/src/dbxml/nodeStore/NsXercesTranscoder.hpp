//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsXercesTranscoder.hpp,v 1.15 2006/10/30 17:45:58 bostic Exp $
//

#ifndef __DBXMLNSXERCESTRANSCODER_HPP
#define __DBXMLNSXERCESTRANSCODER_HPP
	
#include "NsEvent.hpp"
#include "NsHandlerBase.hpp"
#include "NsDocument.hpp"

namespace DbXml {

class EventWriter;

/**
 * NsXercesTranscoder
 *
 * Implements NsEventTranslator, and derives from NsHandlerBase.
 *
 * This class handles parser events, transcodes to UTF-8
 * for indexing and storing into persistent nodes.
 */
class NsXercesTranscoder : public NsEventTranslator, public NsHandlerBase {
public:
	NsXercesTranscoder(Transaction *txn, Container *container,
			   const DocID &docId,
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
				 const xmlch_t *uri);
#endif
	virtual void docTypeDecl(const xmlch_t *data, uint32_t len);
	virtual void startEntity(const xmlch_t *name, uint32_t len);
	virtual void endEntity(const xmlch_t *name, uint32_t len);

	// NsEventTranslator methods
	virtual EventWriter *getEventWriter(void) const {
		return getBaseEventWriter();
	}
	virtual void setEventWriter(EventWriter *writer) {
		setBaseEventWriter(writer);
	}
private:
	void doCharacters(const xmlch_t *characters,
			  uint32_t len, bool isCDATA,
			  bool ignorable);
	void doStart();
private:
	NsDocument _document;
	bool _needsStart;
};

}

#endif
