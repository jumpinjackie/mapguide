//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsHandlerBase.hpp,v 1.9 2005/08/31 10:52:10 jsnelson Exp $
//

#ifndef __DBXMLNSHANDLERBASE_HPP
#define __DBXMLNSHANDLERBASE_HPP

/*
 * NsHandlerBase
 *
 * A base class for handling NsEvent input, used for
 * creation of Node Storage nodes, as well as
 * "transient" parsing for whole-document storage
 * input and querying.
 *
 * There are at least two subclasses -- one for
 * purely transient parse events, which leaves input as UTF-16,
 * and one that targets either node storage or whole-document
 * input.  The former creates a Xerces DOM tree for queries, and
 * the latter interfaces with the indexing mechanism and node
 * storage.
 *
 * This class has methods to input both UTF-8 and UTF-16.
 * It has no virtual behavior.
 */

#include "NsTypes.hpp"
#include "NsUtil.hpp"
#include "NsDocument.hpp"

namespace DbXml
{

class NsEventHandler8;

#define NSHANDLERBASE_IDBUF_SIZE 50

class NsHandlerBase {
public:
	NsHandlerBase(XER_NS MemoryManager *memManager, NsDocument *doc,
		      NsEventHandler8 *handler);
	~NsHandlerBase();
protected:
	void nextId(nid_t *id);

	void addText(void *text, uint32_t len, uint32_t textType,
		     bool isUTF8 = false, bool isDonated = false);
	void addPI(void *target, void *data, bool isUTF8,
		   bool isDonated, uint32_t len);
	void startElem(nsNode_t *node,
		const void *baseName,
		uint32_t uriId, // -1 if no map used
		const void *uri, // may be null
		const void *const prefix,
		bool isUTF8,
		bool isDonated,
		bool copyIDs = true);
	void endDoc(void);
	void endElem(void);
protected:
	// parse state
	XER_NS MemoryManager *_memManager;
	NsDocument *_doc;
	NsEventHandler8 *_handler;
	nsNode_t *_current;
	uint32_t _depth;
	int  _nextText;
	bool _lastWasText;

	// id generation
	uint32_t _idDigit;
	uint8_t _idBuf[NSHANDLERBASE_IDBUF_SIZE];
};

}

#endif
