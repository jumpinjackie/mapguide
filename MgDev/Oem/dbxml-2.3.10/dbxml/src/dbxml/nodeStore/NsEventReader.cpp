//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsEventReader.cpp,v 1.14.2.1 2007/01/02 21:01:14 gmf Exp $
//

#include "../DbXmlInternal.hpp"
#include "NsEventReader.hpp"
#include "NsDocument.hpp"
#include "NsUtil.hpp"
#include "NsConstants.hpp"
#include "NsFormat.hpp"
#include "NsDocumentDatabase.hpp"
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xercesc/framework/MemoryManager.hpp>

/*
 * NsEventReader
 *
 * A pull interface to walk node storage documents.
 * This is based on the original NsEventGenerator code,
 * and will replace it when working, and necessary
 * adapter classes are built.
 */

#define MAKEBOOL(x) ((x)? true : false)

// use 256K buffer for DB_MULTIPLE gets
#define NS_EVENT_DB_BUFSIZE (256 * 1024)

namespace DbXml {

/*
 * NsEventReaderBuf
 *
 * Helper class that encapsulates buffer mgmt for the DB bulk
 * retrieval API.  The only tricky part is knowing when all of the
 * DBTs in a multi-buffer are out of scope.  This occurs after child
 * processing is complete.
 */
class NsEventReaderBuf
{
public:
	NsEventReaderBuf();
	// data
	NsEventReaderBuf *reuseNext;
	NsEventReaderBuf *freeNext;
	uint32_t nNodes;
	uint32_t nComplete;
	uint32_t nSize;
	bool usedUp;
	DbMultipleKeyDataIterator iter;
};

enum state {
	TEXT,
	START_ELEMENT,
	CHILD_ELEMENT,
	END_ELEMENT,
	POP
};
	
class NsEventReaderNodeList
{
public:
	NsEventReaderNodeList(NsEventReaderNodeList *parent,
			      NsEventReaderBuf *currentBuf)
		: node(NULL), buffer(currentBuf),
		textDone(0), state(0), started(false), parent(parent) {}

	int getTextIndex() const { return textDone; }
	void incrText() {
		// transition state when done with either
		// leading text or child text
		++textDone;
		int ntext = node->getNumText();
		if (!node->hasTextChild()) {
			if (textDone == ntext)
				state = START_ELEMENT;
		} else {
			if (textDone == ntext)
				state = END_ELEMENT;
			else if (textDone ==
				 (ntext - node->getNumChildText()))
				state = START_ELEMENT;
		}
	}
	void setStartElemDone() {
		started = true;
		if (node->hasChildElem())
			state = CHILD_ELEMENT;
		else if (node->hasTextChild())
			state = TEXT;
		else
			state = END_ELEMENT;
	}
	void setEndElemDone() {
		state = POP;
	}
	void setChildElemDone() {
		if (node->hasTextChild())
			state = TEXT;
		else
			state = END_ELEMENT;
	}
	bool hasSibling() {
		return (node->hasNext());
	}
	int getState() const { return state; }
	void setState(bool init);
	bool getStarted() const { return started; }
public:
	NsNode *node;
	NsEventReaderBuf *buffer;
	int textDone;
	int state;
	bool started;
	NsEventReaderNodeList *parent;
	DbtOut curUri;
	DbtOut curPrefix;
	DbtOut attrUri;
	DbtOut attrPrefix;
};

void NsEventReaderNodeList::setState(bool init)
{
	DBXML_ASSERT(node);
	if (node->hasText()) {
		if (init) {
			// skip leading text if this is
			// the starting node
			textDone +=
				(node->getNumText() -
				 node->getNumChildText());
		} else {
			if (node->getNumText() >
			    node->getNumChildText()) {
				state = TEXT;
				return;
			}
		}
	}
	state = START_ELEMENT; 
}
	
}

using namespace DbXml;
using namespace std;

NsEventReader::NsEventReader(
	Transaction *txn, NsDocumentDatabase *db, DictionaryDatabase *ddb,
	const DocID &docId,
	u_int32_t flags, XER_NS MemoryManager *memManager, const NsNid *startId)
	: emptyElement_(false),
	  document_(0, memManager),
	  memMgr_(memManager),
	  doInit_(true),
	  popElement_(false),
	  entityCount_(0),
	  current_(NULL),
	  currentBuffer_(NULL),
	  cursor_(db->getNodeStorageDatabase(),
		  txn, CURSOR_READ, flags),
	  cursorFlags_(DB_CURSOR_GET_MASK(flags)),
	  reuseList_(NULL),
	  freeList_(NULL)
{
	DBXML_ASSERT(db);
	document_.initDoc(txn, db, ddb,
			  docId, flags);

	// set starting NID.  If NULL, start at beginning
	if (startId) {
		memset(&startId_, 0, sizeof(startId_));
		startId_.copyNid(memManager, startId);
	} else {
		startId_.setDocRootNid(NULL);
	}

	// do the first event separately
	getNode(current_);
	if (hasNext_) // handle no-content docs
		(void) doElement(true /*start*/);
}

NsEventReader::~NsEventReader()
{
	while(current_ != 0) {
		NsEventReaderNodeList *tmp = current_;
		current_ = tmp->parent;

		if(tmp->buffer)
			releaseNode(tmp->buffer);
		if(tmp->node)
			NsNode::freeNode(memMgr_, tmp->node);
		delete tmp;
	}

	while (freeList_) {
		NsEventReaderBuf *cur = freeList_;
		freeList_ = cur->freeNext;
		::free(cur);
	}
	startId_.freeNid(memMgr_);
}

//
// XmlEventReader methods
//

//
// generic methods
//

//
// names and values
//
// Depend on type:
//  o names of attributes and elements are what is expected
//  o value is generally what is expected.
//  o Special cases:
//      ProcessingInstruction -- local name is target, value is data
//    

const unsigned char *
NsEventReader::getNamespaceURI() const
{
	if (!localName_)
		throwIllegalOperation(type_, "getNamespaceURI");
	DBXML_ASSERT(node_ && node_ == current_->node);


	if (!node_->hasUri())
		return 0;

	document_.getStringForID(node_->uriIndex(), current_->curUri);
	return (const unsigned char *)current_->curUri.get_data();
}

const unsigned char *
NsEventReader::getLocalName() const
{
	if (!localName_)
		throwIllegalOperation(type_, "getLocalName");
	return localName_;
}

const unsigned char *
NsEventReader::getPrefix() const
{
	if (!localName_)
		throwIllegalOperation(type_, "getPrefix");
	DBXML_ASSERT(node_ && node_ == current_->node);
	int32_t prefix = node_->namePrefix();
	if (prefix == NS_NOPREFIX)
		return 0;
	document_.getStringForID(prefix, current_->curPrefix);
	return (const unsigned char *)current_->curPrefix.get_data();
}
	
const unsigned char *
NsEventReader::getValue(int &len) const
{
	if (type_ == ProcessingInstruction) {
		if (value_)
			len = ::strlen((const char *)value_);
		else
			len = 0;
		return value_;
	} else {
		if (!value_)
			throwIllegalOperation(type_, "getValue");
		len = valueLen_;
		return value_;
	}
}

// This method works for text strings and attributes values
bool
NsEventReader::needsEntityEscape(int index) const
{
	if (type_ == Characters) {
		return MAKEBOOL(textType_ & NS_ENTITY_CHK);
	} else if (type_ == StartElement) {
		ensureAttributes(index, "needsEntityEscape");
		return node_->attrNeedsEscape(index);
	} else if (type_ == CDATA)
		return false;
	throwIllegalOperation(type_, "needsEntityEscape");
	return true;
}

bool
NsEventReader::isEmptyElement() const {
	ensureType(StartElement, "isEmptyElement");
	return emptyElement_;
}

// EventReader extensions
const NsNode *
NsEventReader::getNsNode() const
{
	return node_;
}

const unsigned char *
NsEventReader::getVersion() const
{
	ensureType(StartDocument, "getVersion");
	return (const unsigned char*) document_.getXmlDecl();
}

const unsigned char *
NsEventReader::getEncoding() const
{
	ensureType(StartDocument, "getEncoding");
	return document_.getEncodingStr();
}

bool
NsEventReader::isStandalone() const
{
	// if there is a standalone string, it's easy,
	// but if not, there is no way to know.  The document
	// may or may not have an external DTD ref, so
	// default to 'no'
	// TBD: think about keeping standalone state during
	// initial parse, as a document-level flag.
	ensureType(StartDocument, "isStandalone");
	const xmlbyte_t *sa = document_.getStandaloneStr();
	if (sa && (::memcmp(sa, "yes", 3) == 0))
		return true;
	return false;
}

bool
NsEventReader::standaloneSet() const
{
	ensureType(StartDocument, "standaloneSet");
	if (document_.getStandaloneStr())
		return true;
	return false;
}

bool
NsEventReader::encodingSet() const
{
	ensureType(StartDocument, "encodingSet");
	if (document_.getEncodingStr())
		return true;
	return false;
}

//
// attribute access (includes namespace attributes)
// node_ will be 0 if no attributes are present
//

int
NsEventReader::getAttributeCount() const
{
	ensureType(StartElement, "getAttributeCount");
	return nattrs_;
}

bool
NsEventReader::isAttributeSpecified(int index) const
{
	ensureAttributes(index, "isAttributeSpecified");
	return node_->attrIsSpecified(index);
}

const unsigned char *
NsEventReader::getAttributeLocalName(int index) const
{
	ensureAttributes(index, "getAttributeLocalName");
	nsText_t &text = node_->getAttrName(index)->n_text;
	return (const unsigned char *)text.t_chars;
}

const unsigned char *
NsEventReader::getAttributeNamespaceURI(int index) const
{
	ensureAttributes(index, "getAttributeNamespaceURI");
	int32_t uri = node_->attrUri(index);
	if (uri == NS_NOURI)
		return 0;
	document_.getStringForID(uri, current_->attrUri);
	return (const unsigned char *)current_->attrUri.get_data();
}

const unsigned char *
NsEventReader::getAttributePrefix(int index) const
{
	ensureAttributes(index, "getAttributePrefix");
	int32_t prefix = node_->attrNamePrefix(index);
	if (prefix == NS_NOPREFIX)
		return 0;
	document_.getStringForID(prefix, current_->attrPrefix);
	return (const unsigned char *)current_->attrPrefix.get_data();
}

const unsigned char *
NsEventReader::getAttributeValue(int index) const
{
	ensureAttributes(index, "getAttributeValue");
	return (const unsigned char *) node_->attrValue(index);
}

XmlEventReader::XmlEventType
NsEventReader::next()
{
	if (!hasNext_)
		throw XmlException(
			XmlException::EVENT_ERROR,
			"XmlEventReader::next() called when hasNext() is false");
	if(doInit_) {
		// first event is done...
		doInit_ = false;
		if ((current_ == NULL)
		    || ((current_->parent == NULL) &&
			(popElement_ || emptyElement_))
			) {
			hasNext_ = false;
			cursor_.close();
		}
		current_->setStartElemDone();
		return type_;
	} else if (popElement_) {
		// this logic keeps the current element info
		// available through the EndElement event
		endElement();
		if (current_ == NULL) {
			// there is no next...
			throw XmlException(
				XmlException::EVENT_ERROR,
				"XmlEventReader::next() called when hasNext() is false");
		}
	}

	// Basic node loop:
	// 1.  leading text nodes (siblings)
	// 2.  start this element
	// 3.  if has child elements, get next node and process
	// 4.  do child text
	// 5.  end this element
	// 6.  if nextSibling, get next node, else pop to parent
	// 7.  release this node
	//
	while (current_ != NULL) {
		switch (current_->getState()) {
		case TEXT:
		{
			if (!doText())
				continue; // skip
			break;
		}
		case START_ELEMENT:
		{
			// if the current node has been started, need another
			if (current_->getStarted())
				getNode(current_);
			(void) doElement(/* start */true);
			current_->setStartElemDone();
			if (entityCount_ != 0)
				continue; // eat this event
			break;
		}
		case CHILD_ELEMENT:
		{
			current_->setChildElemDone();
			getNode(current_);
			continue;
		}
		case END_ELEMENT:
		{
			current_->setEndElemDone();
			bool validEvent = doElement(/* start */false);
			if (validEvent)
				return type_;
			// else fall through
		}
		case POP:
		default:
		{
			// pop to parent or next sibling.
			endElement();
			continue;
		}
		}
		return type_;
	}
	return type_;
}

// initialize state for a text event.
// Type may be: Characters, CDATA, Comment, Whitespace,
//   EntityReference, ProcessingInstruction, or DTD
// NOTES:
//  o Whitespace means ignorable whitespace
//  o CDATA will never be ignorable white space
// Returns true on "success" and false if the event
// needs to be skipped (e.g. start/end entity info
// that is not being reported)
bool
NsEventReader::doText()
{
	const nsTextEntry_t *entry =
		current_->node->getTextEntry(current_->getTextIndex());
	current_->incrText();

	// initialize text event state
	localName_ = 0;  // not valid for text
	value_ = (unsigned char*)entry->te_text.t_chars;
	valueLen_ = entry->te_text.t_len;
	textType_ = entry->te_type;

	switch (nsTextType(textType_)) {
	case NS_TEXT:
		if(textType_ & NS_IGNORABLE)
			type_ = Whitespace;
		else
			type_ = Characters;
		break;
	case NS_CDATA:
		DBXML_ASSERT(!(textType_ & NS_IGNORABLE));
		type_ = CDATA;
		break;
	case NS_COMMENT:
		type_ = Comment;
		break;
	case NS_PINST:
		// PI is encoding as name0data
		// return target in localName_, data in value_
		type_ = ProcessingInstruction;
		localName_ = value_;
		while (*value_++); // get past NULL
		break;
	case NS_SUBSET:
		type_ = DTD;
		break;
	case NS_ENTSTART:
		if (!(expandEntities_ || reportEntityInfo_))
			throw XmlException(
				XmlException::EVENT_ERROR,
				"Configuration error: must either expand or report entity information");
		if (!expandEntities_)
			++entityCount_;
		if (reportEntityInfo_) {
			type_ = StartEntityReference;
			return true;
		} else
			return false; // skip
		break;
	case NS_ENTEND:
		if (!expandEntities_)
			--entityCount_;
		if (reportEntityInfo_) {
			type_ = EndEntityReference;
			return true;
		} else
			return false; // skip
		break;
	default:
		DBXML_ASSERT(0); // bad
	}
	// if entityCount is 0, we're not eating events,
	// so return true
	return (entityCount_ == 0);
}

void
NsEventReader::getNode(NsEventReaderNodeList *parent)
{
	Dbt key;
	Dbt data;
	current_ = new NsEventReaderNodeList(parent, currentBuffer_);
	if(doInit_) {
		nextNode(&current_->buffer, key, data, &startId_);
		if(current_->buffer == 0) {
			hasNext_ = false; // no-content doc
			return;
		}
	} else {
		nextNode(&current_->buffer, key, data);
		DBXML_ASSERT(current_->buffer != 0);
		DBXML_ASSERT(data.get_data());
	}
	currentBuffer_ = current_->buffer;

	// Because the node buffer will stick around until we've
	// finished, we can set copyStrings to false, meaning that
	// the NsNode strings point directly into the node buffer.
	unsigned char *datap = (unsigned char *)data.get_data();

	const NsFormat &fmt = NsFormat::getFormat((int)(*datap));
	current_->node = fmt.unmarshalNodeData(memMgr_, datap,
		/*adoptBuffer*/false);

	current_->setState(doInit_);

	// Unmarshal the key to get the node ID
	DocID did;
	fmt.unmarshalNodeKey(did, current_->node->getNid(), memMgr_,
		(unsigned char *)key.get_data(), /*copyStrings*/false);

	DBXML_ASSERT(did == document_.getDocId());

	if(Log::isLogEnabled(Log::C_NODESTORE, Log::L_DEBUG)) {
		ostringstream oss;
		oss << "Fetched node: 0x";
		current_->node->getNid()->displayNid(oss);
		NsDocumentDatabase *ndb = document_.getDb();
		Log::log(ndb->getDbEnv(), Log::C_NODESTORE, Log::L_DEBUG,
			 ndb->getContainerName().c_str(), oss.str().c_str());
	}
}

// returns false either on end of document, or after
// end of empty element, which triggers the caller to skip
// the event
bool
NsEventReader::doElement(bool start)
{
	// initialize state
	value_ = 0;
	node_ = 0;
	// current_ is set by caller
	uint32_t flags = current_->node->getFlags();
	if (flags & NS_ISDOCUMENT) {
		if(start) {
			// TBD: not using sniffed encoding
			type_ = StartDocument;
		} else {
			type_ = EndDocument;
			popElement_ = true;
		}
	} else {
		emptyElement_ =
			!((flags & NS_HASCHILD) || (flags & NS_HASTEXTCHILD));
		// skip EndElement for empty elements
		if (emptyElement_ && !start)
			return false;

		// even for EndElement, reassign names
		node_ = current_->node;
		localName_ = (const unsigned char*)
			node_->getNameChars();
		if(start) {
			type_ = StartElement;
			nattrs_ = node_->numAttrs();
		} else {
			type_ = EndElement;
			popElement_ = true;
		}
	}
	if (entityCount_ != 0)
		return false; // skip this
	// set hasNext_
	if (!doInit_) {
		if ((current_ == NULL)
		    || ((current_->parent == NULL) &&
			(popElement_ || emptyElement_))
			) {
			hasNext_ = false;
			cursor_.close();
		}
	}
	return true;
}
//
// if startId is non-null, it's the first time, so
// do a GET vs NEXT cursor op.  *bufp will be NULL.
//
// No-content documents: it's possible to get here with a document
// ID that is not in the database, so silently handle such failures.
//

void
NsEventReader::nextNode(NsEventReaderBuf **bufp, Dbt &key, Dbt &data, NsNid *startId)
{
	NsEventReaderBuf *buf = *bufp;
	DBXML_ASSERT(buf || startId);
	uint32_t bufSize = NS_EVENT_DB_BUFSIZE;
	// are there any items left?
	if(buf) {
		if (!buf->iter.next(key, data)) {
			buf->usedUp = true;
			if (buf->nComplete == buf->nNodes) {
				buf->reuseNext = reuseList_;
				reuseList_ = buf;
			}
			buf = 0;
		}
	}
	
	while (!buf) {
		// "construct" the object plus data
		// pull off free list if available
		if (reuseList_ && (reuseList_->nSize >= bufSize)) {
			buf = reuseList_;
			reuseList_ = buf->reuseNext;
		} else {
			buf = (NsEventReaderBuf *)
				malloc(bufSize + sizeof(NsEventReaderBuf));
			if (!buf) {
				NsUtil::nsThrowException(
					XmlException::NO_MEMORY_ERROR,
					"Malloc failed",
					__FILE__, __LINE__);
				return;
			}
			buf->nSize = bufSize;
			buf->freeNext = freeList_;
			freeList_ = buf;
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
			NsFormat::marshalNodeKey(document_.getDocId(), startId, docKey_);
			ret = cursor_.get(
				&docKey_,
				&data1,
				cursorFlags_|DB_SET|DB_MULTIPLE_KEY);
		} else {
			ret = cursor_.get(
				&docKey_,
				&data1,
				cursorFlags_|DB_NEXT|DB_MULTIPLE_KEY);
		}
		if (ret != 0) {
			// if buf was just allocated, free it; otherwise,
			// put it back on the reuse list
			if (freeList_ == buf) {
				freeList_ = buf->freeNext;
				::free(buf);
			} else {
				buf->reuseNext = reuseList_;
				reuseList_ = buf;
			}
			if (ret == DB_BUFFER_SMALL) {
				// necessary size is in data1.get_size()
				while (bufSize < data1.get_size())
					bufSize <<= 1;
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
		(void) new (&(buf->iter)) DbMultipleKeyDataIterator(data1);
		if (!buf->iter.next(key, data)) {
			NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
						 "Failed to find node.",
						 __FILE__, __LINE__);
			return;
		}

		*bufp = buf;
	}
	buf->nNodes++;
}

// reset current_ to either the parent of the current node, or
// get the next sibling, remembering to set the appropriate parent
// return true if popping to parent, false if moving to sibling.
void
NsEventReader::endElement()
{
	popElement_ = false;
	NsEventReaderNodeList *tmp = current_;

	if (current_->hasSibling()) {
		getNode(tmp->parent);
	} else
		current_ = tmp->parent;
	releaseNode(tmp->buffer);
	NsNode::freeNode(memMgr_, tmp->node);
	delete tmp;
}
	
void
NsEventReader::releaseNode(NsEventReaderBuf *buf)
{
	buf->nComplete++;
	// it's not available until 1) all records have
	// been used up, and 2) all nodes it includes are
	// done processing
	if (buf->usedUp && (buf->nComplete == buf->nNodes)) {
		// done -- put it on free list
		buf->reuseNext = reuseList_;
		reuseList_ = buf;
	}
}

