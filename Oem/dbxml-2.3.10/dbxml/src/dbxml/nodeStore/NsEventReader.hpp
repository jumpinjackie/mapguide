//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsEventReader.hpp,v 1.7 2006/10/31 23:39:52 jsnelson Exp $
//

#ifndef __DBXMLNSEVENTREADER_HPP
#define __DBXMLNSEVENTREADER_HPP
	
#include <db_cxx.h>
#include "EventReader.hpp"
#include "NsNid.hpp"
#include "NsEvent.hpp"
#include "NsDocument.hpp"
#include "../ScopedDbt.hpp"
#include "../Cursor.hpp"

namespace DbXml
{

// forwards
class NsEventReaderBuf;
class NsEventReaderNodeList;
class NsDocumentDatabase;

/**
 * NsEventReader
 *
 * This class implements pull methods to walk all or part of a
 * node storage document, using the XmlEventReader interface,
 * which it implements.
 *
 * Usage:
 *   NsEventReader(txn, database, id, flags, memoryManager [, NsNid])
 *
 * There is no reset method to restart the cursor iteration;
 * it's cheap to recreate the object.
 */
class NsEventReader : public EventReader {
public:
	NsEventReader(Transaction *txn, NsDocumentDatabase *db,
		      DictionaryDatabase *ddb, const DocID &docId,
		      u_int32_t flags, XER_NS MemoryManager *memManager,
		      const NsNid *startId = 0);
	virtual ~NsEventReader();

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

	// start_document only -- see EventReader

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
	bool doText();
	bool doElement(bool start);
	void endElement();
	void nextNode(NsEventReaderBuf **bufp, Dbt &key, Dbt &data, NsNid *startId = 0);
	void releaseNode(NsEventReaderBuf *buf);
	void getNode(NsEventReaderNodeList *parent);
	
	// name info
	const unsigned char * localName_;
	
	// text event info
	uint32_t textType_;

	// element/attribute info
	NsNode *node_;
	bool emptyElement_;
	int nattrs_;

	// data
	mutable NsDocument document_;
	XER_NS MemoryManager *memMgr_;
	bool doInit_;
	bool popElement_;
	int entityCount_;

	mutable NsEventReaderNodeList *current_;
	NsEventReaderBuf *currentBuffer_;

	DbtOut docKey_;
        Cursor cursor_;
	u_int32_t cursorFlags_;
	NsNid startId_;
	// memory mgmt
	NsEventReaderBuf *reuseList_;
	NsEventReaderBuf *freeList_;
};

}

#endif
