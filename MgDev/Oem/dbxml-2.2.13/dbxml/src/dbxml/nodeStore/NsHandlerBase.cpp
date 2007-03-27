//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsHandlerBase.cpp,v 1.18 2005/12/07 15:58:17 jsnelson Exp $
//

#include "NsHandlerBase.hpp"
#include "NsEvent.hpp"
#include "NsConstants.hpp"
#include <xercesc/framework/MemoryManager.hpp>

#define nsSetFlag(node, flag) (node)->nd_header.nh_flags |= (flag)

using namespace DbXml;

#define NSHANDLERBASE_INITIAL_ID_SIZE 4 // must be two or more

NsHandlerBase::NsHandlerBase(XER_NS MemoryManager *memManager, NsDocument *doc,
			     NsEventHandler8 *handler)
	: _memManager(memManager), _doc(doc), _handler(handler),
	  _current(0), _depth(0),
	  _nextText(0), _lastWasText(false), _idDigit(NSHANDLERBASE_INITIAL_ID_SIZE - 1)
{
	memset(_idBuf, 0, NSHANDLERBASE_IDBUF_SIZE);
	
	_idBuf[0] = NS_ROOT_ID;
	for(unsigned int i = 1; i < NSHANDLERBASE_INITIAL_ID_SIZE; ++i) {
		_idBuf[i] = NS_ID_FIRST;
	}
}

NsHandlerBase::~NsHandlerBase()
{
	if(!_doc->getDomFactory()) {
		nsNode_t *node;
		while(_current) {
			node = _current;
			_current = node->nd_parent;
			NsUtil::nsFreeNode(_memManager, node);
		}
	}
}

static inline void _nextId(unsigned char *id, uint32_t &digit)
{
	uint32_t i = digit;
	while(i >= 0 && id[i] == NS_ID_LAST) --i;

	if(i < 0) {
		id[++digit] = NS_ID_FIRST;
	}
	else {
		id[i] += 1;
		while(i < digit) id[++i] = NS_ID_FIRST;
	}
}

void
NsHandlerBase::nextId(nid_t *id)
{
	xmlbyte_t *dest;
	id->idLen = _idDigit + 2;
	if (id->idLen > NID_BYTES_SIZE) {
		dest = id->idStore.idPtr =
			(xmlbyte_t *) _memManager->allocate(id->idLen);
		if (!dest)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
				"nextId failed to allocate memory",
				__FILE__, __LINE__);
	} else {
		dest = id->idStore.idBytes;
	}
	memcpy(dest, _idBuf, id->idLen);
	_nextId(_idBuf, _idDigit); // Increment the id, for next time
}

/*
 * Generic parts of node creation --
 *  o allocate memory
 *  o initialize common parts of node, including
 *  linkage into node lists
 * if (current) set as parent
 * if (firstChild of parent not set)  set parent->firstchild
 * if (currentText) set as leading text, reset to none
 * if (previous)
 *  -- set as previous, set this as previous->next
 *  -- "done" with previous -- complete it
 *  -- reset previous to NULL
 */
void
NsHandlerBase::startElem(nsNode_t *node,
	const void *baseName,
	uint32_t uriId, // -1 if no map used
	const void *uri, // may be null
	const void *const prefix,
	bool isUTF8,
	bool isDonated,
	bool copyIDs)
{
	int uriIndex = 0;
	int prefixIndex = NS_NOPREFIX;
	//
	// allocate/init basic node structure
	//
	++_depth;
	node->nd_parent = _current;
	node->nd_level = _depth;
	//
	// element-specific initialization
	//
	//
	// Update namespace info
	//
	if (uri && _doc->getDoNamespaces()) {
		uriIndex = _doc->addUri(uri, uriId, isUTF8, isDonated,
				(isDonated ?
				 NsUtil::nsStringLen((const xmlbyte_t *)uri) :
				 0));
		nsSetFlag(node, NS_HASURI);
		// cannot be prefix without uri
		if (prefix) {
			prefixIndex = _doc->addPrefix(prefix, uriIndex,
				isUTF8, isDonated, (isDonated ?
				NsUtil::nsStringLen((const xmlbyte_t *)prefix) : 0));
			nsSetFlag(node, NS_NAMEPREFIX);
		}
	}
	
	// name
	node->nd_header.nh_uriIndex = uriIndex;
	node->nd_header.nh_name.n_prefix = prefixIndex;
	uint32_t nlen = (isUTF8 ? NsUtil::nsStringLen((xmlbyte_t*)baseName) :
			 NsUtil::nsStringLen((xmlch_t *)baseName));
	NsUtil::nsCreateText(_memManager, &node->nd_header.nh_name.n_text,
			     baseName, nlen, isUTF8, isDonated);

	//
	// conditional initialization
	//
	if (_current) {
		DBXML_ASSERT(node->nd_parent == _current);

		// "add" to child list of current
		int index = NsUtil::nsAddChild(_memManager, _current, _nextText);

		if(copyIDs) {
			_doc->copyId(&_current->nd_child->cl_child[index].ce_id,
				&node->nd_header.nh_id);

			// set our parent id
			_doc->copyId(&node->nd_header.nh_parent,
				&_current->nd_header.nh_id);
		}
	}
	_lastWasText = false;
	_nextText = 0;

	// reset current, parse-global state
	_current = node;

	// addition of attributes and calling of event handler
	// are done in the calling instance
}

void
NsHandlerBase::endDoc()
{
	endElem();
	_doc->completeDocument();
}

void
NsHandlerBase::endElem()
{
	nsNode_t *node = _current;
	_current = node->nd_parent;
	_doc->completeNode(node);

	// pop depth counter, but not if was doc node
	// which means _current is null
	if (_current)
		--_depth;

	// must reset current text after element ends
	_lastWasText = false;
	_nextText = 0;
	if(_current) {
		nsChildList_t *child = _current->nd_child;
		if(child)
			_nextText = child->cl_child[child->cl_numChild - 1].ce_textIndex;
	}
}

//
// Add text to the current node.
//  text -- pointer to text
//  len -- length in UTF-16 characters OR bytes (utf-8), not including null.
//  textType -- type of text, plus, optionally NS_ENTITY_CHK flag (means
//    checks must use logical operators).
//  isUTF8 -- true if text is utf-8
//  isDonated -- true if text string is donated memory (will be true
//    only if it's utf-8).
//
void
NsHandlerBase::addText(void *text, uint32_t len, uint32_t textType,
		       bool isUTF8, bool isDonated)
{
	DBXML_ASSERT(_current);
	if (_lastWasText && (nsTextType(textType) == NS_TEXT)) {
		DBXML_ASSERT(_nextText > 0);
		/* can coalesce */
		NsUtil::nsCoalesceText(_memManager, _current->nd_text,
				       _nextText - 1, text, len,
		 		       isUTF8, isDonated);
		if (isDonated) {
			_memManager->deallocate(text);
		}
	} else {
		// textType may include NS_ENTITY_CHK, if donated.
		int index = NsUtil::nsAddText(_memManager, _current, text, len,
					      textType, isUTF8, isDonated);
		_lastWasText = (nsTextType(textType) == NS_TEXT);
		DBXML_ASSERT(_nextText == index);
		++_nextText;
	}
	if (textType & NS_ENTITY_CHK)
		_current->nd_text->tl_text[_nextText - 1].te_type |=
			NS_ENTITY_CHK;
}

//
// Add processing instruction to the current node.
//  target -- pointer to target text
//  data -- pointer to data text
//  isUTF8 -- true if text is utf-8
//  isDonated -- true if text string is donated memory (will be true
//    only if it's utf-8).
//  len -- length in UTF-16 characters OR bytes (utf-8) of both strings, including null.
//
void
NsHandlerBase::addPI(void *target, void *data,
		     bool isUTF8, bool isDonated, uint32_t len)
{
	DBXML_ASSERT(_current);

	int index = NsUtil::nsAddPI(_memManager, _current, target, data,
				    isUTF8, isDonated, len);

	_lastWasText = false;
	DBXML_ASSERT(_nextText == index);
	++_nextText;
}
