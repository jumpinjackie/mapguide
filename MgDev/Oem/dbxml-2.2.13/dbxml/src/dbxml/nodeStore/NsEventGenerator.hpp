//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsEventGenerator.hpp,v 1.14 2005/08/25 14:39:38 jsnelson Exp $
//

#ifndef __DBXMLNSEVENTGENERATOR_HPP
#define __DBXMLNSEVENTGENERATOR_HPP
	
#include <db_cxx.h>
#include "NsTypes.hpp"
#include "NsEvent.hpp"
#include "NsDocument.hpp"
#include "../ScopedDbt.hpp"
#include "../Cursor.hpp"

namespace DbXml
{

// forwards
class NsEventGeneratorBuf;
class NsEventGeneratorNodeList;
class NsEventHandler8;
class NsDocumentDatabase;

/**
 * NsEventGenerator
 *
 * This class generates events from a persistent NsDocument instance.
 *
 * Usage:
 *   NsEventGenerator ev(NsEventHandler8&, NsDocument &, ...);
 *   ev.start();
 *
 * There is no reset method to restart the cursor iteration.
 * It'd be easy to add.
 */
class NsEventGenerator : public NsPullEventSource8 {
public:
	NsEventGenerator(Transaction *txn, NsDocumentDatabase *db, const ID &docId,
			 u_int32_t flags, XER_NS MemoryManager *memManager,
			 const nid_t *startId = 0);
	virtual ~NsEventGenerator();

	virtual bool nextEvent(NsEventHandler8 *handler);

private:
	void textEvent(NsEventHandler8 *handler);
	bool elementEvent(bool start, NsEventHandler8 *handler);

	void _nextNode(NsEventGeneratorBuf **bufp, Dbt &data, nid_t *startId = 0);
	void _releaseNode(NsEventGeneratorBuf *buf);

	// data
	NsDocument _document;
	bool _doInit;

	NsEventGeneratorNodeList *_current;
	NsEventGeneratorBuf *_currentBuffer;

	DbtOut _docKey;
        Cursor _cursor;
	u_int32_t _cursorFlags;
	nid_t _startId;
	// memory mgmt
	NsEventGeneratorBuf *_reuseList;
	NsEventGeneratorBuf *_freeList;
};

}

#endif
