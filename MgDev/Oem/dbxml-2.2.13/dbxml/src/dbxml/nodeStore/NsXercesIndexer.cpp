//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsXercesIndexer.cpp,v 1.8 2005/04/20 18:31:33 bostic Exp $
//

#include "NsXercesIndexer.hpp"
#include "NsDocument.hpp"
#include "NsConstants.hpp"
#include "UTF8.hpp"
#define nsSetFlag(node, flag) (node)->nd_header.nh_flags |= (flag)

using namespace DbXml;

//
// NsEventAttrList8 implementation for indexer.
//
// This class must transcode on demand, which means it's
// complexity is in tracking allocations for transcoded strings,
// for cleanup.
//
NsEventAttrListIndexer::~NsEventAttrListIndexer()
{
	struct attrString *list = _toFree;
	while (list) {
		struct attrString *next = list->next;
		::free(list);
		list = next;
	}
}

inline struct attrString *
NsEventAttrListIndexer::_allocString(const xmlch_t *str, int &nchars) const
{
	nchars = NsUtil::nsStringLen(str) + 1;
	int len = nchars * 3;
	struct attrString *ret = (struct attrString *)
		::malloc(sizeof(struct attrString) + len);
	ret->next = _toFree;
	_toFree = ret;
	return ret;
}

const xmlbyte_t *
NsEventAttrListIndexer::localName(int index) const
{
	if (_alist) {
		const xmlch_t *lname = _alist->localName(index);
		if (lname) {
			int nchars;
			struct attrString *asName = _allocString(lname, nchars);
			xmlbyte_t *dest = asName->s;
			NsUtil::nsToUTF8(0, &dest, lname, nchars, nchars*3);
			return asName->s;
		}
	}
	return 0;
}

const xmlbyte_t *
NsEventAttrListIndexer::value(int index) const {
	if (_alist) {
		const xmlch_t *lvalue = _alist->value(index);
		if (lvalue) {
			int nchars;
			struct attrString *asVal = _allocString(lvalue,
								nchars);
			xmlbyte_t *dest = asVal->s;
			NsUtil::nsToUTF8(0, &dest, lvalue, nchars, nchars*3);
			return asVal->s;
		}
	}
	return 0;
}

const xmlbyte_t *
NsEventAttrListIndexer::uri(int index) const
{
	if (_alist) {
		const xmlch_t *luri = _alist->uri(index);
		if (luri) {
			int nchars;
			struct attrString *asUri = _allocString(luri,
								nchars);
			xmlbyte_t *dest = asUri->s;
			NsUtil::nsToUTF8(0, &dest, luri, nchars, nchars*3);
			return asUri->s;
		}
	}
	return 0;
}

NsXercesIndexer::NsXercesIndexer(NsEventHandler8 *handler) :
	_handler(handler)
{
}

NsXercesIndexer::~NsXercesIndexer()
{
}

void
NsXercesIndexer::startElement(const xmlch_t *localName,
			      const xmlch_t *prefix,
			      const xmlch_t *uri,
			      int32_t uriId,
			      NsEventAttrList *attrs,
			      const uint32_t attrCount,
			      bool isEmpty,
			      bool isRoot)
{
	if (_handler) {
		if (uri && !(*uri))
			uri = 0;
		// Indexer does not use: prefix, uriId, isEmpty, isRoot
		NsEventAttrListIndexer alist(attrs, localName, uri);
		_handler->startElement(
			(const xmlbyte_t *)XMLChToUTF8(localName).str(), 0,
			(uri ? (const xmlbyte_t*)XMLChToUTF8(uri).str() : 0),
			-1, &alist,
			attrCount, isEmpty, isRoot, 0);
	}
}

void
NsXercesIndexer::endElement(const xmlch_t *localName,
			       const xmlch_t *prefix,
			       const xmlch_t *uri,
			       bool isRoot)
{
	if (_handler) {
		// indexer doesn't use args, so ignore them
		_handler->endElement(0, 0, 0, isRoot, 0);
	}
}

void
NsXercesIndexer::characters(const xmlch_t *characters,
			       uint32_t len, bool isCDATA,
			       bool needsEscape)
{
	if (_handler) {
		XMLChToUTF8 chars(characters, len);
		_handler->characters((const xmlbyte_t *)chars.str(), chars.len(),
				     isCDATA, false);
	}
}

