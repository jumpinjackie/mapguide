//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsHandlerBase.cpp,v 1.26 2006/11/17 18:18:29 jsnelson Exp $
//

#include "NsHandlerBase.hpp"
#include "NsEvent.hpp"
#include "NsConstants.hpp"
#include <xercesc/framework/MemoryManager.hpp>

using namespace DbXml;


NsHandlerBase::NsHandlerBase(XER_NS MemoryManager *memManager, NsDocument *doc,
			     EventWriter *ewriter)
	: _memManager(memManager), _doc(doc), _ewriter(ewriter),
	  _current(0), _previous(0), _depth(0),
	  _lastWasText(false), _textList(0)
{
}

NsHandlerBase::~NsHandlerBase()
{
	if(!_doc->getDomFactory()) {
		NsNode *node;
		while(_current) {
			node = _current;
			_current = node->getParent();
			NsNode::freeNode(_memManager, node);
		}
		if(_previous) {
			NsNode::freeNode(_memManager, _previous);
		}
		if (_textList)
			NsNode::freeTextList(_memManager, _textList);
	}
}

void
NsHandlerBase::nextId(NsNid *id)
{
	_nidGen.nextId(_memManager, id);
}

/*
 * Generic parts of node creation --
 */
void
NsHandlerBase::startElem(NsNode *node,
			 const void *baseName,
			 const char *uri, // may be null
			 const char *prefix,
			 bool isUTF8,
			 bool isDonated)
{
	int uriIndex = 0;
	int prefixIndex = NS_NOPREFIX;
	//
	// Structural initialization
	//
	++_depth;
	node->setParent(_current);
	node->setLevel(_depth);
	if (_previous) {
		node->setNextPrev(_memManager, _previous);
		_doc->completeNode(_previous);
		_previous = 0;
	}
	
	//
	// element-specific initialization
	//
	//
	// Update namespace info
	//
	if (uri) {
		uriIndex = _doc->addIDForString(uri, ::strlen(uri));
		node->setFlag(NS_HASURI);
		// cannot be prefix without uri
		if (prefix) {
			prefixIndex = _doc->addIDForString(prefix,
							   ::strlen(prefix));
			node->setFlag(NS_NAMEPREFIX);
		}
	}
	
	// name
	node->setUriIndex(uriIndex);
	node->setNamePrefix(prefixIndex);
	uint32_t nlen = (isUTF8 ? NsUtil::nsStringLen((xmlbyte_t*)baseName) :
			 NsUtil::nsStringLen((xmlch_t *)baseName));
	NsNode::createText(_memManager, node->getNameText(),
			   baseName, nlen, isUTF8, isDonated);

	//
	// conditional initialization
	//
	if (_current) {
		DBXML_ASSERT(node->getParent() == _current);

		// tell node we're about to start it up.  It's responsible
		// for copying parent/child NIDs if necessary
		_textList = _current->startElem(_memManager, node, _textList);
	}
	_lastWasText = false;

	// reset current, parse-global state
	_current = node;

	// addition of attributes and calling of event handler
	// are done in the calling instance
}

void
NsHandlerBase::endDoc()
{
	endElem();
	if (_previous) {
		_doc->completeNode(_previous);
		_previous = 0;
	}
	_doc->completeDocument();
}

void
NsHandlerBase::endElem()
{
	NsNode *node = _current;
	_current = node->getParent();
	
	// tell node that we're about to be done with it, passing
	// parent along
	_textList = node->endElem(_memManager, _current, _previous, _textList);
	if (_previous)
		_doc->completeNode(_previous);
	_previous = node;

	// pop depth counter, but not if was doc node
	// which means _current is null
	if (_current)
		--_depth;

	// must reset current text after element ends
	_lastWasText = false;
}

//
// Add text to the current text list, allocating one if necessary
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
		/* can coalesce */
		NsNode::coalesceText(_memManager, _textList,
				     text, len,
				     isUTF8, isDonated);
		if (isDonated) {
			_memManager->deallocate(text);
		}
	} else {
		// textType may include NS_ENTITY_CHK, if donated.
		_textList = _current->addText(_memManager, _textList, text, len,
					      textType, isUTF8, isDonated);
		_lastWasText = (nsTextType(textType) == NS_TEXT);
	}
	if (textType & NS_ENTITY_CHK)
		_textList->tl_text[_textList->tl_ntext - 1].te_type |=
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

	_textList = _current->addPI(_memManager, _textList, target, data,
				    isUTF8, isDonated, len);

	_lastWasText = false;
}
