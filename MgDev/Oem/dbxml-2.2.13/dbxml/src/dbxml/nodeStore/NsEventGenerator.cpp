//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsEventGenerator.cpp,v 1.38 2005/12/07 15:58:17 jsnelson Exp $
//

#include "NsEventGenerator.hpp"
#include "NsEvent.hpp"
#include "NsDocument.hpp"
#include "NsUtil.hpp"
#include "NsConstants.hpp"
#include "NsDocumentDatabase.hpp"
#include <pathan/XPath2MemoryManager.hpp>
#include <xercesc/framework/MemoryManager.hpp>

#define MAKEBOOL(x) ((x)? true : false)

// use 256K buffer for DB_MULTIPLE gets
#define NS_EVENT_DB_BUFSIZE (256 * 1024)

namespace DbXml {
/*
 * NsEventGeneratorBuf
 *
 * Helper class that encapsulates buffer mgmt for the DB bulk
 * retrieval API.  The only tricky part is knowing when all of the
 * DBTs in a multi-buffer are out of scope.  This occurs after child
 * processing is complete.
 */
class NsEventGeneratorBuf
{
public:
	NsEventGeneratorBuf();
	// data
	NsEventGeneratorBuf *reuseNext;
	NsEventGeneratorBuf *freeNext;
	uint32_t nNodes;
	uint32_t nComplete;
	uint32_t nSize;
	bool usedUp;
	DbMultipleDataIterator iter;
};

class NsEventGeneratorNodeList
{
public:
	NsEventGeneratorNodeList(NsEventGeneratorNodeList *parent, NsEventGeneratorBuf *currentBuf)
		: node(NULL), buffer(currentBuf), doXmlDecl(false),
		  childrenDone(0), textDone(0), parent(parent) {
	}

	nsNode_t *node;
	NsEventGeneratorBuf *buffer;
	bool doXmlDecl;
	int childrenDone;
	int textDone;

	NsEventGeneratorNodeList *parent;
};
	
}

using namespace DbXml;

NsEventGenerator::NsEventGenerator(
	Transaction *txn, NsDocumentDatabase *db, const ID &docId,
	u_int32_t flags, XER_NS MemoryManager *memManager, const nid_t *startId)
	: _document(0, memManager),
	  _doInit(true),
	  _current(NULL),
	  _currentBuffer(NULL),
	  _cursor(db->getNodeStorageDatabase(),
		  txn, CURSOR_READ, flags),
	  _cursorFlags(DB_CURSOR_GET_MASK(flags)),
	  _reuseList(NULL),
	  _freeList(NULL)
{
	_document.initDoc(txn, db, docId, flags, /*newDoc*/false);

	docId.setDbtFromThis(_docKey);

	// set starting ID.  If NULL, start at beginning
	if (startId) {
		memset(&_startId, 0, sizeof(_startId));
		_document.copyId(&_startId, startId);
	} else {
		memset(&_startId, 0, sizeof(_startId));
		_document.copyId(&_startId, &Globals::docRootNID);
	}
}

NsEventGenerator::~NsEventGenerator()
{
	while(_current != 0) {
		NsEventGeneratorNodeList *tmp = _current;
		_current = tmp->parent;

		if(tmp->buffer)
			_releaseNode(tmp->buffer);
		if(tmp->node)
			_document.getMemoryManager()->deallocate(tmp->node);
		delete tmp;
	}

	while (_freeList) {
		NsEventGeneratorBuf *cur = _freeList;
		_freeList = cur->freeNext;
		::free(cur);
	}
	_document.freeId(&_startId);
}

void
NsEventGenerator::textEvent(NsEventHandler8 *handler)
{
	const nsTextEntry_t *entry = nsTextEntry(_current->node, _current->textDone);
	++_current->textDone;

	const xmlbyte_t *ptr = (const xmlbyte_t*)entry->te_text.t_chars;
	uint32_t ptrlen = entry->te_text.t_len;
	uint32_t type = entry->te_type;

	if(nsIsText(type) || nsIsCDATA(type)) {
		if(type & NS_IGNORABLE) {
			handler->ignorableWhitespace(ptr, ptrlen,
				nsIsCDATA(type));
		} else {
			handler->characters(ptr, ptrlen,
				nsIsCDATA(type),
				MAKEBOOL(type & NS_ENTITY_CHK));
		}
	} else if(nsIsComment(type)) {
		handler->comment(ptr, ptrlen);
	} else if(nsIsPI(type)) {
		uint32_t len = NsUtil::nsStringLen((xmlbyte_t*)ptr);
		const xmlbyte_t *data = ptr + len + 1;
		handler->processingInstruction(ptr, data);
	} else if(nsIsEntityStart(type)) {
		handler->startEntity(ptr, ptrlen);
	} else if(nsIsEntityEnd(type)) {
		handler->endEntity(ptr, ptrlen);
	} else if(nsIsSubset(type)) {
		handler->docTypeDecl(ptr, ptrlen);
	} else {
		DBXML_ASSERT(0); // bad text type
	}
}

bool
NsEventGenerator::elementEvent(bool start, NsEventHandler8 *handler)
{
	if(start) {
		// Fetch the next node
		_current = new NsEventGeneratorNodeList(_current, _currentBuffer);

		Dbt data;
		if(_doInit) {
			_doInit = false;
			_nextNode(&_current->buffer, data, &_startId);
			if(_current->buffer == 0) {
				// no-content document
				return false;
			}
		} else {
			_nextNode(&_current->buffer, data);
			DBXML_ASSERT(_current->buffer != 0);
		}
		_currentBuffer = _current->buffer;

		// Because the node buffer will stick around until we've finished,
		// we can set copyStrings to false, meaning that the nsNode_t strings
		// point directly into the node buffer.
		_current->node = NsUtil::nsUnmarshalNode(_document.getMemoryManager(),
			(unsigned char *)data.get_data(), data.get_size(),
			/*adoptBuffer*/false);
	}

	uint32_t flags = _current->node->nd_header.nh_flags;
	if (flags & NS_ISDOCUMENT) {
		if(start) {
			handler->startDocument(_document.getSniffedEncodingStr());
			_current->doXmlDecl = true;
			return true;
		} else {
			handler->endDocument();
			return true;
		}
	} else {
		bool isEmpty =
			!((flags & NS_HASCHILD) || (flags & NS_HASTEXT));
		const xmlbyte_t *name = (const xmlbyte_t*)
			nsName(_current->node)->n_text.t_chars;
		const xmlbyte_t *prefix =
			_document.getPrefix8(nsNamePrefix(_current->node));
		const xmlbyte_t *uri = nsHasUri(_current->node)
			? _document.getUri8(nsUriIndex(_current->node)) : 0;

		if(start) {
			nsAttrList_t *attrs = _current->node->nd_attrs;
			NsEventNodeAttrList8 alist(attrs, _document);

			handler->startElement(name, prefix, uri, -1, &alist,
				attrs ? attrs->al_nattrs : 0, isEmpty,
				MAKEBOOL(flags & NS_ISROOT), _current->node);
			return true;
		} else {
			if(!isEmpty) {
				handler->endElement(name, prefix, uri,
					MAKEBOOL(flags & NS_ISROOT),
					_current->node);
				return true;
			} else {
				return false;
			}
		}
	}
}

bool
NsEventGenerator::nextEvent(NsEventHandler8 *handler)
{
	if(_doInit) {
		return elementEvent(/*start*/true, handler);
	}

	while(_current != NULL) {

		if(_current->doXmlDecl) {
			_current->doXmlDecl = false;
			// xmlDecl event only happens if the document has one
			if(_document.getXmlDecl8()) {
				handler->xmlDecl(_document.getXmlDecl8(),
					_document.getEncodingStr8(),
					_document.getStandaloneStr());
				return true;
			}
		}

		if((u_int32_t)_current->childrenDone < nsNumChildElem(_current->node)) {
			if(nsHasText(_current->node) &&
				(u_int32_t)_current->textDone < nsChildTextIndex(_current->node, _current->childrenDone)) {
				textEvent(handler);
				return true;
			}

			++_current->childrenDone;
			elementEvent(/*start*/true, handler);
			return true;
		}

		if(nsHasText(_current->node) &&
			_current->textDone < nsNumText(_current->node)) {
			textEvent(handler);
			return true;
		}

		// Do the endElement event
		bool endElementSuccess =  elementEvent(/*start*/false, handler);

		NsEventGeneratorNodeList *tmp = _current;
		_current = tmp->parent;

		_releaseNode(tmp->buffer);
		_document.getMemoryManager()->deallocate(tmp->node);
		delete tmp;

		if(endElementSuccess) return true;
	}

	return false;
}

void
NsEventGenerator::_releaseNode(NsEventGeneratorBuf *buf)
{
	buf->nComplete++;
	// it's not available until 1) all records have
	// been used up, and 2) all nodes it includes are
	// done processing
	if (buf->usedUp && (buf->nComplete == buf->nNodes)) {
		// done -- put it on free list
		buf->reuseNext = _reuseList;
		_reuseList = buf;
	}
}

//
// if startId is non-null, it's the first time, so
// do a GET vs NEXT cursor op.  *bufp will be NULL.
//
// No-content documents: it's possible to get here with a document
// ID that is not in the database, so silently handle such failures.
//
void
NsEventGenerator::_nextNode(NsEventGeneratorBuf **bufp, Dbt &data, nid_t *startId)
{
	NsEventGeneratorBuf *buf = *bufp;
	DBXML_ASSERT(buf || startId);
	uint32_t bufSize = NS_EVENT_DB_BUFSIZE;
	// are there any items left?
	if(buf) {
		if (!buf->iter.next(data)) {
			buf->usedUp = true;
			if (buf->nComplete == buf->nNodes) {
				buf->reuseNext = _reuseList;
				_reuseList = buf;
			}
			buf = 0;
		}
	}
	
	while (!buf) {
		// "construct" the object plus data
		// pull off free list if available
		if (_reuseList && (_reuseList->nSize >= bufSize)) {
			buf = _reuseList;
			_reuseList = buf->reuseNext;
		} else {
			buf = (NsEventGeneratorBuf *) malloc(bufSize + sizeof(NsEventGeneratorBuf));
			buf->nSize = bufSize;
			buf->freeNext = _freeList;
			_freeList = buf;
		}
		if (!buf) {
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
						 "Malloc failed",
						 __FILE__, __LINE__);
			return;
		}
		buf->reuseNext = 0;
		buf->nNodes = 0;
		buf->nComplete = 0;
		buf->usedUp = false;

		Dbt data1; // does not need to live past this block
		data1.set_flags(DB_DBT_USERMEM);
		data1.set_ulen(buf->nSize);
		data1.set_data(buf + 1);
		int ret = 0;
		if (startId) {
			xmlbyte_t *dptr = (xmlbyte_t *)
				data1.get_data();
			*dptr++ = NS_PROTOCOL_VERSION;
			memcpy(dptr, NsUtil::nsNode(startId),
				startId->idLen);
			ret = _cursor.get(
				&_docKey,
				&data1,
				_cursorFlags|DB_GET_BOTH|DB_MULTIPLE);
		} else {
			ret = _cursor.get(
				&_docKey,
				&data1,
				_cursorFlags|DB_NEXT_DUP|DB_MULTIPLE);
		}
		if (ret != 0) {
			_freeList = buf->freeNext;
			::free(buf);
			if (ret == DB_BUFFER_SMALL) {
				bufSize = data1.get_size() << 1; // 2x
				buf = 0;
				continue;
			} else if(ret != DB_NOTFOUND) {
				throw XmlException(ret, __FILE__, __LINE__);
			}

			if(startId) {
				// no-content document
				buf = 0;
			}
			*bufp = buf; // 0
			return;
		}
		// placement new to initialize multiple iter
		(void) new (&(buf->iter)) DbMultipleDataIterator(data1);
		if (!buf->iter.next(data)) {
			NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
						 "Failed to find node.",
						 __FILE__, __LINE__);
			return;
		}
		*bufp = buf;
	}
	buf->nNodes++;
}
