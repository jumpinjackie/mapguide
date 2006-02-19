//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsEvent.hpp,v 1.11 2005/11/22 00:57:36 gmf Exp $
//

#ifndef __DBXMLNSEVENT_HPP
#define __DBXMLNSEVENT_HPP

/*
 * NsEventHandler(8)
 * NsEventAttrList(8)
 *
 * These are interface classes that are used for both input
 * and output events for BDB XML.  The methods are similar to those
 * you'd find in a SAX interface, with some differences, specific
 * to the BDB XML environment.
 *
 * There are both UTF-16 and UTF-8 (suffix '8') versions of these
 * interfaces, in order to accomodate Xerces input and output,
 * as well as UTF-8 input/output.
 *
 * Namespace Note (applies to both NsEventAttrList* attributes
 * and the uriId and uri arguments passed to
 * NsEventHandler::startElement()):
 *   1.  The uriId() method is only used for a cached map of
 *   id -> uri string, where the id is provided by the parser.
 *   Id of 0 must be the xml namespace uri, and id 1 must be
 *   xmlns namespace uri.  After that, it's parser-defined.
 *   If a parser does not wish to use this cached mapping,
 *   it *must* return -1 as the uriId.
 *   2.  If namespaces are not in use, or the attribute
 *   has no namespace uri, the uri() and prefix()
 *   methods must return 0.  uriId() will not be called.
 */

#include "NsTypes.hpp"
#include "NsUtil.hpp"

namespace DbXml {

class NsDocument;

/**
 * NsEventAttrList
 *
 * Interface to get attributes from the event
 * generator.  Passed to the handler's startElement()
 * function.
 */
class NsEventAttrList {
public:
	virtual ~NsEventAttrList();
	virtual bool isEmpty(void) const = 0;
	virtual int numAttributes(void) const = 0;
	virtual const xmlch_t *prefix(int index) const = 0;
	virtual const xmlch_t *localName(int index) const =0;
	virtual const xmlch_t *qname(int index) = 0;
	virtual const xmlch_t *value(int index) const = 0;
	virtual const xmlch_t *uri(int index) const = 0;
	virtual bool hasTypeInfo(int index) const = 0;
	virtual const xmlch_t *typeName(int index) const = 0;
	virtual const xmlch_t *typeUri(int index) const = 0;
	virtual const int uriId(int index) const = 0;
	virtual const bool needsEscape(int index) const = 0;
	virtual const bool isSpecified(int index) const = 0;
};

class NsEventAttrList8 {
public:
	virtual ~NsEventAttrList8() {}
	virtual bool isEmpty(void) const = 0;
	virtual int numAttributes(void) const = 0;
	virtual const xmlbyte_t *prefix(int index) const = 0;
	virtual const xmlbyte_t *localName(int index) const =0;
	virtual const xmlbyte_t *qname(int index) = 0;
	virtual const xmlbyte_t *value(int index) const = 0;
	virtual const xmlbyte_t *uri(int index) const = 0;
	virtual bool hasTypeInfo(int index) const = 0;
	virtual const xmlbyte_t *typeName(int index) const = 0;
	virtual const xmlbyte_t *typeUri(int index) const = 0;
	virtual const int uriId(int index) const = 0;
	virtual const bool needsEscape(int index) const = 0;
	virtual const bool isSpecified(int index) const = 0;
};
	
/**
 * NsEventHandler
 *
 * The event interface itself.
 *
 * TBD:
 *  o add index-relevant info, such as node id, parent, etc.
    May need class or structure for that info.  Could also add
 *  an interface class to startElement -- NsEventGenerator -- used to
 *  acquire info.  Can be implemented by the generator (e.g. NsDocument)
 */
class NsEventHandler {
public:
	virtual ~NsEventHandler() {}
	virtual void startDocument(const xmlch_t *sniffedEncoding) {}
	virtual void xmlDecl(const xmlch_t *xmlDecl,
			     const xmlch_t *encodingStr,
			     const xmlch_t *standaloneStr) {}
	virtual void endDocument() {}
	virtual void startElement(const xmlch_t *localName,
				  const xmlch_t *prefix,
				  const xmlch_t *uri,
				  int32_t uriId,
				  NsEventAttrList *attrs,
				  const uint32_t attrCount,
				  bool isEmpty,
				  bool isRoot) = 0;
	virtual void endElement(const xmlch_t *localName,
				const xmlch_t *prefix,
				const xmlch_t *uri,
				bool isRoot) = 0;
	virtual void characters(const xmlch_t *characters,
				uint32_t len, bool isCDATA,
				bool needsEscape) = 0;
	virtual void ignorableWhitespace(const xmlch_t *characters,
					 uint32_t len, bool isCDATA) {}
	virtual void comment(const xmlch_t *comment, uint32_t len) = 0;
	virtual void processingInstruction(const xmlch_t *target,
					   const xmlch_t *data) = 0;
	virtual void setTypeInfo(const xmlch_t *typeName,
				 const xmlch_t *typeUri) {}
	// these next several have default, no-op implementations
	virtual void docTypeDecl(const xmlch_t *data, uint32_t len);
	virtual void startEntity(const xmlch_t *name, uint32_t len);
	virtual void endEntity(const xmlch_t *name, uint32_t len);
};

class NsEventHandler8 {
public:
	virtual ~NsEventHandler8() {}
	virtual void startDocument(const xmlbyte_t *sniffedEncoding) {}
	virtual void xmlDecl(const xmlbyte_t *xmlDecl,
			     const xmlbyte_t *encodingStr,
			     const xmlbyte_t *standaloneStr) {}
	virtual void endDocument() {}
	/// It is valid for node to be null
	virtual void startElement(const xmlbyte_t *localName,
				  const xmlbyte_t *prefix,
				  const xmlbyte_t *uri,
				  int32_t uriId,
				  NsEventAttrList8 *attrs,
				  const uint32_t attrCount,
				  bool isEmpty,
				  bool isRoot,
				  const nsNode_t *node) = 0;
	/// It is valid for node to be null
	virtual void endElement(const xmlbyte_t *localName,
				const xmlbyte_t *prefix,
				const xmlbyte_t *uri,
				bool isRoot,
				const nsNode_t *node) = 0;
	virtual void characters(const xmlbyte_t *characters,
				uint32_t len, bool isCDATA,
				bool needsEscape) = 0;
	virtual void ignorableWhitespace(const xmlbyte_t *characters,
					 uint32_t len, bool isCDATA) {}
	virtual void comment(const xmlbyte_t *comment, uint32_t len) = 0;
	virtual void processingInstruction(const xmlbyte_t *target,
					   const xmlbyte_t *data) = 0;
	virtual void setTypeInfo(const xmlbyte_t *typeName,
				 const xmlbyte_t *typeUri) {}
	// these next several have default, no-op implementations
	virtual void docTypeDecl(const xmlbyte_t *data, uint32_t len);
	virtual void startEntity(const xmlbyte_t *name, uint32_t len);
	virtual void endEntity(const xmlbyte_t *name, uint32_t len);
};

/**
 * NsEventTranslator
 *
 * Translates from NsEventHandler events into NsEventHandler8
 * events.
 */
class NsEventTranslator : public NsEventHandler {
public:
	virtual ~NsEventTranslator() {}
	virtual NsEventHandler8 *getNsEventHandler(void) const = 0;
	virtual void setNsEventHandler(NsEventHandler8 *handler) = 0;
};

/**
 * NsPushEventSource
 *
 * Interface that can be implemented by a parser or other
 * event generator for NsEventHandler.
 */
class NsPushEventSource {
public:
	virtual ~NsPushEventSource() {}
	virtual void start() = 0;
	virtual NsEventHandler *getNsEventHandler(void) const = 0;
	virtual void setNsEventHandler(NsEventHandler *handler) = 0;
};

/**
 * NsPushEventSource8
 *
 * Interface that can be implemented by a parser or other
 * event generator for NsEventHandler8.
 */
class NsPushEventSource8 {
public:
	virtual ~NsPushEventSource8() {}
	virtual void start() = 0;
	virtual NsEventHandler8 *getNsEventHandler(void) const = 0;
	virtual void setNsEventHandler(NsEventHandler8 *handler) = 0;
};

/**
 * NsPullEventSource8
 *
 * Interface that can be implemented by a parser or other
 * pull type event generator for NsEventHandler8.
 */
class NsPullEventSource8 {
public:
	virtual ~NsPullEventSource8() {}
	virtual bool nextEvent(NsEventHandler8 *handler) = 0;
};

/**
 * NsPullToPushConverter8
 *
 * Interface that can be implemented by a parser or other
 * pull type event generator for NsEventHandler8.
 */
class NsPullToPushConverter8 : public NsPushEventSource8 {
public:
	NsPullToPushConverter8(NsPullEventSource8 *pull)
		: pull_(pull), handler_(0) {}
	virtual ~NsPullToPushConverter8() {
		delete pull_;
	}

	virtual void start() {
		if(handler_ != 0) {
			while(pull_->nextEvent(handler_)) {}
		}
	}

	virtual NsEventHandler8 *getNsEventHandler(void) const {
		return handler_;
	}
	virtual void setNsEventHandler(NsEventHandler8 *handler) {
		handler_ = handler;
	}
private:
	NsPullEventSource8 *pull_;
	NsEventHandler8 *handler_;
};

/**
 * NsPushEventSourceTranslator8
 *
 * Converts an NsPushEventSource into an NsPushEventSource8 using an
 * NsEventTranslator. Adopts and deletes both of it's
 * constructor arguments.
 */
class NsPushEventSourceTranslator8 : public NsPushEventSource8
{
public:
	NsPushEventSourceTranslator8(NsPushEventSource *source,
				 NsEventTranslator *translator);
	virtual ~NsPushEventSourceTranslator8();

	virtual void start();
	virtual NsEventHandler8 *getNsEventHandler(void) const;
	virtual void setNsEventHandler(NsEventHandler8 *handler);
private:
	NsPushEventSource *source_;
	NsEventTranslator *translator_;
};

/**
 * NsEventNodeAttrList8
 *
 * Instance of NsEventAttrList8 that is constructed using
 * a nsNode_t.  NOTE: the nsNode_t MUST have utf-8 strings
 * in it, which means it's either the result of a transcoded
 * parse (See NsXercesTranscoder) or read from a database (see
 * NsEventGenerator).
 *
 */
class NsEventNodeAttrList8 : public NsEventAttrList8 {
public:
	NsEventNodeAttrList8(nsAttrList_t *list, NsDocument &doc, bool isUTF16 = false);
	virtual ~NsEventNodeAttrList8();
	
	virtual bool isEmpty(void) const { return (_list == 0); }
	virtual int numAttributes(void) const;
	virtual const xmlbyte_t *prefix(int index) const;
	virtual const xmlbyte_t *localName(int index) const;
	virtual const xmlbyte_t *qname(int index);
	virtual const xmlbyte_t *value(int index) const;
	virtual const xmlbyte_t *uri(int index) const;
	virtual bool hasTypeInfo(int index) const {
		return (_list->al_attrs[index].a_type != NS_NOTYPE);
	}
	virtual const xmlbyte_t *typeName(int index) const;
	virtual const xmlbyte_t *typeUri(int index) const;
	virtual const int uriId(int index) const { return -1; }
	virtual const bool needsEscape(int index) const;
	virtual const bool isSpecified(int index) const;
private:
	const xmlbyte_t *qnameBuf(int index, xmlbyte_t *destination,
				  int *destLen) const;
private:
	nsAttrList_t *_list;
	NsDocument &_doc;
	mutable xmlbyte_t **_qnames;

	bool _isUTF16;
	mutable xmlbyte_t **_localnames;
	mutable xmlbyte_t **_values;
};

}

#endif
