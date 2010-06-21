//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsHandlerBase.hpp,v 1.16 2006/10/30 17:45:57 bostic Exp $
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
#include "NsNid.hpp"

namespace DbXml
{

class EventWriter;
struct nsTextList;
	
class NsHandlerBase {
public:
	NsHandlerBase(XER_NS MemoryManager *memManager, NsDocument *doc,
		      EventWriter *writer);
	~NsHandlerBase();

	// Configuration
	void setBaseEventWriter(EventWriter *writer) {
		_ewriter = writer;
	}
	EventWriter *getBaseEventWriter() const {
		return _ewriter;
	}

protected:
	void nextId(NsNid *id);

	void addText(void *text, uint32_t len, uint32_t textType,
		     bool isUTF8 = false, bool isDonated = false);
	void addPI(void *target, void *data, bool isUTF8,
		   bool isDonated, uint32_t len);
	// NOTE: uri and prefix are *always* utf-8
	void startElem(NsNode *node,
		       const void *baseName,
		       const char *uri, // may be null
		       const char *prefix,
		       bool isUTF8,
		       bool isDonated);
	void endDoc(void);
	void endElem(void);
protected:
	// parse state
	XER_NS MemoryManager *_memManager;
	NsDocument *_doc;
	EventWriter *_ewriter;
	NsNode *_current;
	NsNode *_previous;
	uint32_t _depth;
	bool _lastWasText;
	struct nsTextList *_textList;

	// nid
	NsNidGen _nidGen;
};

}

#endif
