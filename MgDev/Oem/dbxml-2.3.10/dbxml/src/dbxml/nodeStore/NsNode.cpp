//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsNode.cpp,v 1.17.2.2 2007/01/25 19:50:18 jsnelson Exp $
//

#include "NsNode.hpp"
#include "NsDocument.hpp"
#include "NsConstants.hpp"
#include "NsFormat.hpp"
#include "UTF8.hpp"

using namespace DbXml;
XERCES_CPP_NAMESPACE_USE

//static
void
NsNode::copyText(MemoryManager *mmgr, nsText_t *to, const nsText_t *from,
		 bool isUTF8)
{
	if (to->t_chars != 0)
		mmgr->deallocate(to->t_chars);

	if (from->t_chars == 0) {
		to->t_chars = 0;
		to->t_len = 0;
	} else {
		uint32_t len = from->t_len + 1;
		if (isUTF8)
			len *= sizeof(xmlbyte_t);
		else
			len *= sizeof(xmlch_t);

		to->t_chars = mmgr->allocate(len);
		if (!to->t_chars)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
						 "copyNid failed to allocate memory",
						 __FILE__, __LINE__);
		memcpy(to->t_chars, from->t_chars, len);
		to->t_len = from->t_len;
	}
}

// static
void
NsNode::copyName(MemoryManager *mmgr, nsName_t *to, const nsName_t *from,
		 bool isUTF8)
{
	to->n_prefix = from->n_prefix;
#ifdef NS_USE_SCHEMATYPES	
	to->n_type = from->n_type;
	to->n_utype = from->n_utype;
#endif
	copyText(mmgr, &to->n_text, &from->n_text, isUTF8);
}

// static
nsNav_t *
NsNode::allocNav(XER_NS MemoryManager *mmgr)
{
	nsNav_t *nav = (nsNav_t *)mmgr->allocate(sizeof(nsNav_t));
	if (!nav)
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "allocNav: allocation failed",
					 __FILE__, __LINE__);
	memset(nav, 0, sizeof(nsNav_t));
	return nav;
}

// static
nsNav_t *
NsNode::copyNav(XER_NS MemoryManager *mmgr, nsNav_t *nav)
{
	nsNav_t *newnav = (nsNav_t *)mmgr->allocate(sizeof(nsNav_t));
	if (!nav)
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "allocNav: allocation failed",
					 __FILE__, __LINE__);
	memset(newnav, 0, sizeof(nsNav_t));
	if (!nav->nn_prev.isNull())
		newnav->nn_prev.copyNid(mmgr, &nav->nn_prev);
	if (!nav->nn_lastChild.isNull())
		newnav->nn_lastChild.copyNid(mmgr, &nav->nn_lastChild);
	return newnav;
}

// static
void
NsNode::freeNav(XER_NS MemoryManager *mmgr, nsNav_t *nav,
		bool freePointer)
{
	if (!nav->nn_prev.isNull())
		nav->nn_prev.freeNid(mmgr);
	if (!nav->nn_lastChild.isNull())
		nav->nn_lastChild.freeNid(mmgr);
	if (freePointer)
		mmgr->deallocate(nav);
}

//static
nsAttrList_t *
NsNode::allocAttrList(MemoryManager *mmgr, uint32_t attrCount)
{
	if (attrCount != 0) {
		size_t msize = sizeof(nsAttrList_t) +
			((attrCount - 1) * sizeof(nsAttr_t));
		nsAttrList_t *attrlist =
			(nsAttrList_t *)mmgr->allocate(msize);
		if (!attrlist)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
						 "nsAllocAttrList: allocation failed",
						 __FILE__, __LINE__);
		memset(attrlist, 0, msize);
		attrlist->al_max = attrCount;
		return attrlist;
	}
	return 0;
}

/*
 * reallocate list, copying array of nsAttr_t structs
 */
static nsAttrList_t *
_reallocAttrList(MemoryManager *mmgr, nsAttrList_t *list)
{
	if (list == 0)
		return NsNode::allocAttrList(mmgr, NS_NATTR_DEFAULT);

	/* realloc -- double size */
	uint32_t oldsize = sizeof(nsAttrList_t) +
		((list->al_max - 1) * sizeof(nsAttr_t));

	list->al_max <<= 1;
	nsAttrList_t *result = NsNode::allocAttrList(mmgr, list->al_max);

	memcpy(result, list, oldsize);
	mmgr->deallocate(list);
	return result;
}

// static
nsAttrList_t *
NsNode::copyAttrList(MemoryManager *mmgr, const nsAttrList_t *attrs,
		     bool isUTF8)
{
	uint32_t attrCount = attrs->al_nattrs;
	uint32_t amax = attrCount << 1; // leave some space for more attr nodes...
	size_t msize = sizeof(nsAttrList_t) +
		((amax - 1) * sizeof(nsAttr_t));
	nsAttrList_t *result =
		(nsAttrList_t *)mmgr->allocate(msize);
	if (!result)
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "copyAttrList: allocation failed",
					 __FILE__, __LINE__);

	memset(result, 0, msize);
	result->al_len = attrs->al_len;
	result->al_nattrs = attrCount;
	result->al_max = amax;

	if(isUTF8) {
		for(uint32_t i = 0; i < attrCount; ++i) {
			copyName(mmgr, &result->al_attrs[i].a_name,
				 &attrs->al_attrs[i].a_name, isUTF8);
			xmlbyte_t *value = (xmlbyte_t*)result->al_attrs[i].
				a_name.n_text.t_chars;
			while(*value++); // Get past the first null, to the value
			result->al_attrs[i].a_value = value;
			result->al_attrs[i].a_flags =
				attrs->al_attrs[i].a_flags;
			result->al_attrs[i].a_uri =
				attrs->al_attrs[i].a_uri;
		}
	} else {
		for(uint32_t i = 0; i < attrCount; ++i) {
			copyName(mmgr, &result->al_attrs[i].a_name,
				 &attrs->al_attrs[i].a_name, isUTF8);
			xmlch_t *value = (xmlch_t*)result->al_attrs[i].
				a_name.n_text.t_chars;
			while(*value++); // Get past the first null, to the value
			result->al_attrs[i].a_value = value;
			result->al_attrs[i].a_flags =
				attrs->al_attrs[i].a_flags;
			result->al_attrs[i].a_uri =
				attrs->al_attrs[i].a_uri;
		}
	}

	return result;
}

/*
 * allocate a node.  Since attribute count is always known,
 * allocate space for it, as well.
 * Alignment node: since NsNode is all pointers, assume
 * that memory after it is sufficiently aligned to allow
 * placing nsAttrList_t there.
 */
// static
NsNode *
NsNode::allocNode(MemoryManager *mmgr, uint32_t attrCount,
		  uint32_t flags)
{
	/* node + [attrList + array of text for attrs] */
	size_t msize = sizeof(NsNode);
	NsNode *node = (NsNode *)mmgr->allocate(msize);
	if (!node)
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "allocNode: allocation failed",
					 __FILE__, __LINE__);

	memset(node, 0, msize);
	// NS_ALLOCATED must be set, or the node won't be deallocated properly
	node->setFlag(NS_ALLOCATED|flags);
	if (attrCount != 0) {
		node->setAttrList(allocAttrList(mmgr, attrCount));
		node->setFlag(NS_HASATTR);
	}
	node->setNamePrefix(NS_NOPREFIX);
	node->setFormat(&(NsFormat::getFormat(NS_PROTOCOL_VERSION)));
	return node;
}

// static
NsNode *
NsNode::copyNode(MemoryManager *mmgr, NsNode *node, bool dealloc)
{
	uint32_t flags = node->getFlags();
	bool isUTF8 = !node->isUTF16();

	NsNode *result = allocNode(mmgr, 0, flags);
	result->getNid()->copyNid(mmgr, &(node->nd_header.nh_id));
	result->getParentNid()->copyNid(mmgr, &(node->nd_header.nh_parent));
	result->getLastDescendantNidPtr()->
		copyNid(mmgr, node->getLastDescendantNidPtr());
	result->setUriIndex(node->uriIndex());
	copyName(mmgr, result->getName(), &(node->nd_header.nh_name),
		 isUTF8);
	result->setLevel(node->getLevel());

	if (flags & NS_HASATTR) {
		result->setAttrList(copyAttrList(mmgr, node->nd_attrs, isUTF8));
	}
	if (flags & NS_HASTEXT) {
		result->setTextList(copyTextList(mmgr, node->nd_text, isUTF8));
	}
	if (node->getNav()) {
		result->setNav(copyNav(mmgr, node->getNav()));
	}
	if (dealloc)
		freeNode(mmgr, node);
	return result;
}

/*
 * Free all memory associated with node.
 * Memory management issue: even for memory managers that can
 *  do all-in-one cleanup, this function should be used (assuming
 *  deallocate() does something meaningful).  The reason is to
 *  make it possible to parse/load huge documents, that would otherwise
 *  use all of memory.
 */
// static
void
NsNode::freeNode(MemoryManager *mmgr, NsNode *node)
{
	if(node->isAllocated()) {
		// Each part is allocated separatly, including all lists and strings
		uint32_t flags = node->getFlags();
		node->nd_header.nh_id.freeNid(mmgr);
		node->nd_header.nh_parent.freeNid(mmgr);
		node->nd_header.nh_lastDescendant.freeNid(mmgr);
		mmgr->deallocate(node->getNameChars());

		if (node->getNav()) {
			freeNav(mmgr, node->getNav());
		}
		if (flags & NS_HASATTR) {
			freeAttrList(mmgr, node->nd_attrs);
		}
		if (flags & NS_HASTEXT) {
			freeTextList(mmgr, node->nd_text);
		}
	} else {
		// Even if the node is one chunk, large node ids
		// could have resulted in extra allocations, so it's necessary
		// to free ids separately
		node->nd_header.nh_id.freeNid(mmgr);
		node->nd_header.nh_parent.freeNid(mmgr);
		node->nd_header.nh_lastDescendant.freeNid(mmgr);
		// freeNav calls freeNid on all navigational NsNids,
		// pass false to *not* free the pointer, too
		if (node->getNav())
			freeNav(mmgr, node->getNav(), false);

		
		// Free any extra memory attached to the NsNode
		if(node->nd_memory) {
			// We call free, because this memory comes from a Dbt
			// with DB_DBT_MALLOC set
			::free(node->nd_memory);
		}

	}
	// All NsNode memory allocated in one large chunk
	mmgr->deallocate(node);
}

//static
void
NsNode::freeAttrList(MemoryManager *mmgr, nsAttrList_t *attrs)
{
	int32_t nattrs = attrs->al_nattrs;
	for (int i = 0; i < nattrs; i++)
		mmgr->deallocate(attrs->al_attrs[i].a_name.n_text.t_chars);
	mmgr->deallocate(attrs);
}

//static
nsTextList_t *
NsNode::copyTextList(MemoryManager *mmgr, const nsTextList_t *text,
		     bool isUTF8)
{
	uint32_t ntext = text->tl_ntext;
	uint32_t nmax = ntext << 1; // leave some space for more text nodes...

	uint32_t size = sizeof(nsTextList_t) +
		((nmax - 1) * sizeof(nsTextEntry_t));
	nsTextList_t *result = (nsTextList_t*)mmgr->allocate(size);

	if (!result)
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "nsCopyTextList: allocation failed",
					 __FILE__, __LINE__);

	memset(result, 0, size);
	result->tl_len = text->tl_len;
	result->tl_ntext = ntext;
	result->tl_nchild = text->tl_nchild;
	result->tl_max = nmax;

	for(uint32_t i = 0; i < ntext; ++i) {
		result->tl_text[i].te_type = text->tl_text[i].te_type;
		copyText(mmgr, &result->tl_text[i].te_text,
			 &text->tl_text[i].te_text, isUTF8);
	}

	return result;
}

//static
void
NsNode::freeTextList(MemoryManager *mmgr, nsTextList_t *text)
{
	int32_t ntext = text->tl_ntext;
	for (int i = 0; i < ntext; i++)
		mmgr->deallocate(text->tl_text[i].te_text.t_chars);
	mmgr->deallocate(text);
}

//
// non-static methods
//

/*
 * If needed, reallocates the given node so that it's structures
 * are allocated separately. This is so that they can individually
 * be resized and changed as needed. Deallocates the original node.
 */
NsNode *
NsNode::makeTransient(MemoryManager *mmgr)
{
	if(isAllocated())
		return this;
	return  copyNode(mmgr, this, true);
}

void
NsNode::removeNids(MemoryManager *mmgr)
{
	getNid()->freeNid(mmgr);
	getParentNid()->freeNid(mmgr);
	memset(getNid(), 0, sizeof(NsNid));
	memset(getParentNid(), 0, sizeof(NsNid));
	if (nd_nav) {
		freeNav(mmgr, nd_nav);
		nd_nav = 0;
	}
	setFlag(NS_STANDALONE);
	setLevel(0);
}

static void _throwLengthException(int expected, int actual)
{
	std::ostringstream oss;
	oss << "Invalid string length: " << expected;
	if (actual < expected)
		oss << ", actual length is " << actual;
	else
		oss << ", actual length is larger";
	oss << ".";
	throw XmlException(XmlException::EVENT_ERROR,
			   oss.str());
}
	
// memcpy that copies utf-8 chars, checking length,
// and throws an exception if they don't match.  This is
// because the length args may have come from the application,
// via XmlEventWriter.  The length should include the trailing null.
static void _memcpyCheckLen(void *dest, const void *src, int len)
{
	xmlbyte_t *tdest = (xmlbyte_t *)dest;
	const xmlbyte_t *tsrc = (const xmlbyte_t *)src;
	int tlen = 0;
	while ((*tdest++ = *tsrc++) != '\0') {
		++tlen;
		if (tlen == len)
			break;
	}
	if (tlen != (len - 1))
		_throwLengthException(len-1, tlen);
}

static bool _memcpyCheck8(xmlbyte_t *dest, const xmlbyte_t *src,
			  int len, enum checkType type, bool isMulti)
{
	bool hasEnt = false;
	bool *hasEntity = &hasEnt;
	xmlbyte_t c;
	int tlen = 0;
	do {
		c = *src++;
		CHECK_ENT(c);
		++tlen;
		*dest++ = c;
		if (!c && isMulti) {
			isMulti = false;
			continue;
		}
	} while (c);
	if (len != tlen)
		_throwLengthException(len-1, tlen-1);
	return hasEnt;
}

static bool _memcpyCheck16(xmlch_t *dest, const xmlch_t *src, int len,
			   enum checkType type)
{
	bool hasEnt = false;
	bool *hasEntity = &hasEnt;
	xmlch_t c;
	while (len--) {
		c = *src++;
		CHECK_ENT(c);
		*dest++ = c;
	}
	return hasEnt;
}

// Input is one of:
//   utf-16 -- always copied
//   utf-8 -- donated
//   utf-8 -- must be copied.
// In the last instance, the string is scanned during the copy
// to check for entities, and the result returned in hasEntity
//
// static
int
NsNode::createText(MemoryManager *mmgr, nsText_t *dest, const void *text,
	uint32_t length, bool isUTF8, bool donate, bool doubleString,
	bool *hasEntity, enum checkType type)
{
	uint32_t allocLen;
	if (length == 0 && text == 0) {
		// handle empty text
		static const char emptyText[4] = { 0, 0, 0, 0};
		if (!donate)
			text = (void*) emptyText;
		else {
			text = mmgr->allocate(4);
			if (!text)
				NsUtil::nsThrowException(
					XmlException::NO_MEMORY_ERROR,
					"nsCreateText: allocation failed",
					__FILE__, __LINE__);
			*((char*)text) = 0;
		}
	}
	length += 1; // trailing null
	if (!donate) {
		allocLen = length;
		if (!isUTF8)
			allocLen <<= 1;

		AutoDeallocate<xmlbyte_t> newchars(mmgr, allocLen);
		if (!newchars.get())
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
						 "nsCreateText: allocation failed",
						 __FILE__, __LINE__);
		if (hasEntity) {
			if (isUTF8)
				*hasEntity = _memcpyCheck8(
					newchars.get(),
					(const xmlbyte_t *)text,
					allocLen, type, false);
			else
				*hasEntity = _memcpyCheck16(
					(xmlch_t *)newchars.get(),
					(const xmlch_t *)text,
					(allocLen>>1), type);
		} else {
			if (isUTF8) {
				// No need to check the length if this is
				// a double string (str1\0str2\0), since this
				// won't have come from the user
				if(doubleString)
					memcpy(newchars, text, allocLen);
				else _memcpyCheckLen(newchars, text, allocLen);
			}
			else
				memcpy(newchars, text, allocLen);
		}
		dest->t_chars = newchars.adopt();
	} else
		dest->t_chars = (void *) text; // cast is safe on donate
	dest->t_len = length - 1; /* don't include trailing null */
	return length;
}

/*
 * Allocate default size for text list
 */
static nsTextList_t *
_nsCreateTextList(MemoryManager *mmgr)
{
	size_t msize = sizeof(nsTextList_t) +
		((NS_NTEXT_DEFAULT - 1) * sizeof(nsTextEntry_t));
	nsTextList_t *tlist = (nsTextList_t *) mmgr->allocate(msize);
	if (tlist != 0) {
		memset(tlist, 0, msize);
		tlist->tl_max = NS_NTEXT_DEFAULT;
	}
	return tlist;
}

/*
 * reallocate list, copying array of nsTextEntry_t structs
 */
static nsTextList_t *
_reallocTextList(MemoryManager *mmgr, nsTextList_t *list)
{
	if (list == 0)
		return _nsCreateTextList(mmgr);
	/* realloc -- double size */
	uint32_t oldsize = sizeof(nsTextList_t) +
		((list->tl_max - 1) * sizeof(nsTextEntry_t));
	list->tl_max <<= 1;
	size_t msize = sizeof(nsTextList_t) +
		((list->tl_max-1) * sizeof(nsTextEntry_t));

	// reallocate
	void *newlist = mmgr->allocate(msize);
	if (!newlist)
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "_reallocTextList: allocation failed",
					 __FILE__, __LINE__);
	memcpy(newlist, list, oldsize);
	mmgr->deallocate(list);
	return (nsTextList_t *) newlist;
}

/*
 * Add a text node to the list.  Length is
 * available in this case.  Return index used.
 * Create text list if necessary.
 */
// static
nsTextList_t *
NsNode::addText(MemoryManager *mmgr, nsTextList_t *list, const void *text,
		uint32_t length, uint32_t type, bool isUTF8, bool donate)
{
	AutoDeallocate<nsTextList_t> listGuard(0, mmgr);
	if ((list == 0) || (list->tl_ntext == list->tl_max)) {
		listGuard.set(_reallocTextList(mmgr, list));
		list = listGuard;
		DBXML_ASSERT(list);
	}
	/* add to list */
	int index = list->tl_ntext++;
	bool hasEntity = false;
	nsTextEntry_t *entry = &list->tl_text[index];
	entry->te_type = type;
	// set isCharacters -- it's ignored if the hasEntity arg is 0
	list->tl_len +=
		createText(mmgr, &entry->te_text,
			text, length, isUTF8, donate,
			nsTextType(type) == NS_PINST,
			(nsTextType(type) == NS_TEXT ? &hasEntity : 0),
			isCharacters);
	if (hasEntity)
		entry->te_type |= NS_ENTITY_CHK;
	return listGuard.get() == 0 ? list : listGuard.adopt();
}

// this code is shared by insertText and insertPI
static nsTextEntry_t *
_insertTextEntry(MemoryManager *mmgr, NsNode *node, uint32_t index)
{
	DBXML_ASSERT((int32_t)index != -1);
	nsTextList_t *textList = node->getTextList();
	if ((textList == 0) || (textList->tl_ntext == textList->tl_max)) {
		textList = _reallocTextList(mmgr, textList);
		DBXML_ASSERT(textList);
		node->setFlag(NS_HASTEXT);
		node->setTextList(textList);
	}

	nsTextEntry_t *entry =  &(textList->tl_text[index]);

	if(textList->tl_ntext > index) {
		// Copy texts up to leave a gap at index
		memmove(entry + 1, entry, (textList->tl_ntext - index) * sizeof(nsTextEntry_t));
	}
	return entry;
}

static void
_insertText(MemoryManager *mmgr, NsNode *node, uint32_t index,
	    const void *text, uint32_t length, uint32_t type,
	    bool isUTF8, bool donate, bool isChild)
{
	nsTextEntry_t *entry =  _insertTextEntry(mmgr, node, index);
	nsTextList_t *textList = node->getTextList();	
	// Create the new text entry
	bool hasEntity = false;
	entry->te_type = type;
	// set isCharacters -- ignored if hasEntity is null
	textList->tl_len +=
		NsNode::createText(mmgr, &entry->te_text,
			text, length, isUTF8, donate,
			nsTextType(type) == NS_PINST,
			(nsTextType(type) == NS_TEXT ?
				&hasEntity : 0), isCharacters);
	if (hasEntity)
		entry->te_type |= NS_ENTITY_CHK;

	// Increase the number of text
	textList->tl_ntext ++;
	if (isChild) {
		textList->tl_nchild++;
		node->setFlag(NS_HASTEXTCHILD);
	}
}

void
NsNode::insertText(MemoryManager *mmgr, uint32_t index,
		   const xmlch_t *text, uint32_t type, bool isChild)
{
	bool utf8 = !isUTF16();
	// TBD: NOTE: This method does not coalesce adjacent text
	// nodes.  While this is not required by any spec,
	// it should do it.  Another option would be to
	// materialize adjacent text nodes as one, if possible.
	//
	if(utf8) {
		enum checkType ttype = (type == NS_TEXT) ? isCharacters : ignore;
		NsDonator chars(mmgr, text, NsUtil::nsStringLen(text),
				ttype);
		if (chars.getHasEntity())
			type |= NS_ENTITY_CHK;
		_insertText(mmgr, this, index, chars.getStr(), chars.getLen(),
			    type, true, true, isChild);
	} else {
		_insertText(mmgr, this, index, text, NsUtil::nsStringLen(text),
			    type, false, false, isChild);
	}
}

// Need to handle both leading text and child text cases
// TBD: NOTE: does not coalesce text if possible, following
// removal (e.g. text - comment - text, then remove comment)
// This is a 'would be nice if...'
void
NsNode::removeText(MemoryManager *mmgr, int32_t index)
{
	DBXML_ASSERT(index != -1);
	nsTextList_t *textList = nd_text;

	if (textList == 0 || textList->tl_ntext <= (uint32_t)index)
		return;

	if(textList->tl_ntext == 1) {
		// Simple -- no text nodes left; remove the list
		freeTextList(mmgr, textList);
		nd_text = 0;
		nd_header.nh_flags &= ~(NS_HASTEXT|NS_HASTEXTCHILD);
	} else {
		// if this a child text node, reduce the number
		if (index >= getNumLeadingText()) {
			textList->tl_nchild--;
			if (textList->tl_nchild == 0)
				nd_header.nh_flags &= ~(NS_HASTEXTCHILD);
		}
		
		nsTextEntry_t *text =  &(textList->tl_text[index]);

		// Reduce total length of text, and number of text
		textList->tl_len -= text->te_text.t_len + 1;
		textList->tl_ntext -= 1;

		// Free the text's values
		if(text->te_text.t_chars != 0)
			mmgr->deallocate(text->te_text.t_chars);

		// Copy higher entries down to fill space
		memmove(text, text + 1, (textList->tl_ntext - index) * sizeof(nsTextEntry_t));

		// Materialized nodes must be renumbered by caller
	}
}

// static
int
NsNode::coalesceText(MemoryManager *mmgr, nsTextList_t *list, 
		     const void *text, uint32_t len, bool isUTF8, bool checkEnt)
{
	// get last entry for coalesce
	nsTextEntry_t *entry = &list->tl_text[list->tl_ntext-1];
	uint32_t allocLen, copyLen, newLen, oldLen = entry->te_text.t_len;
	newLen = oldLen + len + 1;

	// do copy operations in bytes
	if (isUTF8) {
		allocLen = newLen;
		copyLen = len + 1; // add trailing null
	} else {
		allocLen = newLen << 1;
		oldLen <<= 1;
		copyLen = (len + 1) << 1;
	}

	xmlbyte_t *newText = (xmlbyte_t *)mmgr->allocate(allocLen);

	if (!newText)
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "coalesceText: allocation failed",
					 __FILE__, __LINE__);
	// copy original string, but don't copy null
	memcpy(newText, entry->te_text.t_chars, oldLen);
	// only do entity check copy if 1) not already set, and
	// 2) checkEnt is true
	// this function is only called for character text
	if (checkEnt && !(entry->te_type & NS_ENTITY_CHK)) {
		DBXML_ASSERT(isUTF8);
		checkEnt = _memcpyCheck8(
			(xmlbyte_t*)(newText + oldLen),
			(xmlbyte_t*) text, copyLen,
			isCharacters, false);
		if (checkEnt)
			entry->te_type |= NS_ENTITY_CHK;
	} else {
		// add trailing null for this copy
		if (isUTF8)
			_memcpyCheckLen(newText + oldLen, text, copyLen);
		else
			memcpy(newText + oldLen, text, copyLen);
	}
	mmgr->deallocate(entry->te_text.t_chars);
	entry->te_text.t_chars = newText;
	entry->te_text.t_len = newLen - 1; // subtract null
	// NOTE: tl_len is ONLY used for marshaling, which means
	// it's a *byte* character count, and is irrelevant for the
	// non-UTF8 case, which is never marshaled.
	list->tl_len += len; // trailing null already accounted for
	return allocLen;
}

/*
 * Add text of the form, name0value0 to a text node.
 * Return total number of characters used, including all
 * nulls.  Value can be null, in which case, use a null string
 */
static const xmlbyte_t nullUTF8[1] = { 0 };
static const xmlch_t nullUTF16[1] = { 0 };

static int
_addMultiText(MemoryManager *mmgr, const void *name, const void *value,
	      nsText_t *text, bool isUTF8, bool *hasEntity)
{
	int nlen, vlen, ncopyLen, vcopyLen, allocLen;
	// operate in byte lengths
	if (isUTF8) {
		if (value == 0)
			value = (const void*) nullUTF8;
		nlen = NsUtil::nsStringLen((const xmlbyte_t*)name);
		vlen = NsUtil::nsStringLen((const xmlbyte_t *)value);
		allocLen = nlen + vlen + 2;
		ncopyLen = nlen + 1;
		vcopyLen = vlen + 1;
	} else {
		if (value == 0)
			value = (const void*) nullUTF16;
		nlen = NsUtil::nsStringLen((const xmlch_t*)name);
		vlen = NsUtil::nsStringLen((const xmlch_t *)value);
		allocLen = (nlen + vlen + 2) << 1;
		ncopyLen = (nlen + 1) << 1;
		vcopyLen = (vlen + 1) << 1;
	}

	text->t_chars =	mmgr->allocate(allocLen);
	if (!text->t_chars)
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "_addMultiText: allocation failed",
					 __FILE__, __LINE__);
	text->t_len = nlen + vlen + 1; // don't count final NULL

	// copy to format, name0value0, only do entity check on value
	// hasEntity is ONLY non-zero for attributes...
	memcpy(text->t_chars, name, ncopyLen);
	if (hasEntity) {
		if (isUTF8)
			*hasEntity = _memcpyCheck8(
				((xmlbyte_t*)text->t_chars) +
				ncopyLen, (const xmlbyte_t*)value,
				vcopyLen, isAttrVal, true);
		else
			*hasEntity = _memcpyCheck16(
				((xmlch_t*)text->t_chars) +
				(ncopyLen>>1), (const xmlch_t*)value,
				(vcopyLen>>1), isAttrVal);
	} else {
		// Don't use memcpyCheckLen() in this path -- all
		// lengths come via strlen, not directly from user.
		memcpy(((xmlbyte_t*)text->t_chars) + ncopyLen,
		       value, vcopyLen);
	}
	return (nlen + vlen + 2);
}

nsTextList_t *
NsNode::startElem(XER_NS MemoryManager *mmgr, NsNode *child,
		  nsTextList_t *textList)
{
	DBXML_ASSERT(!child->getTextList());
	child->setTextList(textList);
	nd_header.nh_flags |= NS_HASCHILD;
	if (textList)
		child->nd_header.nh_flags |= NS_HASTEXT;
	if (!noNav()) {
		// handle first child and child's parent
		// last child is handled in endElem
		if (!nd_nav)
			nd_nav = allocNav(mmgr);
		child->getParentNid()->copyNid(mmgr, getNid());
	}
	return 0; // text list has been consumed
}

nsTextList_t *
NsNode::endElem(XER_NS MemoryManager *mmgr,
		NsNode *parent, NsNode *previous,
		nsTextList_t *textList)
{
	if (textList) {
		nd_header.nh_flags |= (NS_HASTEXTCHILD|NS_HASTEXT);
		nsTextList_t *oldlist = getTextList();
		if (oldlist) {
			// append new list to old, and make
			// new entries child entries
			DBXML_ASSERT(oldlist->tl_nchild == 0);
			for (unsigned int i = 0; i < textList->tl_ntext; i++) {
				nsTextEntry_t *entry = &textList->tl_text[i];
				oldlist = addText(mmgr, oldlist,
						  entry->te_text.t_chars,
						  entry->te_text.t_len,
						  entry->te_type,
						  !isUTF16(),
						  false);
				oldlist->tl_nchild++;
			}
			setTextList(oldlist); // in case of reallocation
		} else {
			// all nodes are child nodes
			textList->tl_nchild = textList->tl_ntext;
			setTextList(textList); // eat the list
			textList = 0;
		}
	}
	DBXML_ASSERT(!hasTextChild() || getTextList()->tl_nchild != 0);
	// if not donated, free it
	if (textList)
		freeTextList(mmgr, textList);

	//
	// Done with text, handle navigational structure, if not
	// otherwise indicated (e.g. by transient dom, which
	// maintains its own)
	//
	if (!noNav()) {
		if (previous) {
			DBXML_ASSERT(nd_nav);
			nd_nav->nn_lastChild.copyNid(mmgr,
						    previous->getNid());
		}
	}
	//
	// Set parent's last descendant.  It's either
	// this node's last descendant, or this node itself
	// Parent is null for document element
	//
	if (parent) {
		if (getLastDescendantNid()) {
			parent->getLastDescendantNidPtr()->copyNid(
				mmgr,
				getLastDescendantNid());
		} else {
			parent->getLastDescendantNidPtr()->copyNid(
				mmgr, getNid());
		}
	}
	return 0;
}

void
NsNode::clearNext(XER_NS MemoryManager *mmgr)
{
	nd_header.nh_flags &= ~NS_HASNEXT;
}

void
NsNode::clearPrev(XER_NS MemoryManager *mmgr)
{
	nd_header.nh_flags &= ~NS_HASPREV;
	if (!noNav()) {
		DBXML_ASSERT(nd_nav);
		nd_nav->nn_prev.freeNid(mmgr);
		memset(&nd_nav->nn_prev, 0, sizeof(NsNid));
	}
}

void
NsNode::setNextPrev(XER_NS MemoryManager *mmgr, NsNode *previous)
{
	// Need to set next/prev flags even if not copying
	// nids, so NsDom will function correctly -- materializing
	// text on demand.
	previous->setFlag(NS_HASNEXT);
	setFlag(NS_HASPREV);
	if (!noNav()) {
		if (!nd_nav)
			nd_nav = allocNav(mmgr);
		nd_nav->nn_prev.copyNid(mmgr, previous->getNid());
	}
}

// use child == 0 to clear
void
NsNode::setLastChild(XER_NS MemoryManager *mmgr, NsNode *child)
{
	if (!noNav()) {
		if (!nd_nav)
			nd_nav = allocNav(mmgr);
		if (child) {
			DBXML_ASSERT(hasChildElem());
			nd_nav->nn_lastChild.copyNid(mmgr, child->getNid());
		} else {
			nd_nav->nn_lastChild.freeNid(mmgr);
			memset(&nd_nav->nn_lastChild, 0, sizeof(NsNid));
		}
	}
	if (!child) // no more children
		nd_header.nh_flags &= ~NS_HASCHILD;
}

void
NsNode::setLastDescendantNid(XER_NS MemoryManager *mmgr, const NsNid *nid)
{
	if (hasChildElem())
		getLastDescendantNidPtr()->copyNid(mmgr, nid);
	else {
		// no kids, no descendants
		getLastDescendantNidPtr()->freeNid(mmgr);
		memset(getLastDescendantNidPtr(), 0, sizeof(NsNid));
	}
}

/*
 * Set attribute in the list.
 * If donate is true:
 *   o isUTF8 must be true
 *   o the string, starting at name, must be of the form, "name0value0"
 *   o value must point to the correct string
 *   o len must be set to indicate the length of the string, including both nulls
 */
static nsAttr_t *
_setAttr(MemoryManager *mmgr, NsNode *node, uint32_t index,
	 const void *name, const void *value, bool isUTF8,
	 bool donate, uint32_t len, bool specified)
{
	nsAttrList_t *attrList = node->getAttrList();
	nsAttr_t *attr =  &(attrList->al_attrs[index]);

	if(attr->a_name.n_text.t_chars != 0)
		mmgr->deallocate(attr->a_name.n_text.t_chars);

	// add both name/value to a_name.n_text
	attr->a_name.n_prefix = NS_NOPREFIX;
#ifdef NS_USE_SCHEMATYPES	
	attr->a_name.n_type = NS_NOTYPE;
#endif
	attr->a_uri = NS_NOURI;
	attr->a_flags = 0;
	if (donate) {
		DBXML_ASSERT(isUTF8 && len);
		attr->a_name.n_text.t_chars = (void *) name;
		attr->a_name.n_text.t_len = len - 1; // len includes trailing null
		attr->a_value = (void *)value;
	} else {
		// do real work
		bool hasEntity = false;
		len = _addMultiText(mmgr, name, value,
				    &(attr->a_name.n_text),
				    isUTF8, &hasEntity);
		if (isUTF8) {
			xmlbyte_t *tptr8 =
				(xmlbyte_t *) attr->a_name.n_text.t_chars;
			while (*tptr8++); // get past name
			attr->a_value = tptr8;
		} else {
			xmlch_t *tptr =
				(xmlch_t *) attr->a_name.n_text.t_chars;
			while (*tptr++); // get past name
			attr->a_value = tptr;
		}
		if (hasEntity)
			attr->a_flags |= NS_ATTR_ENT;
	}
	if (!specified)
		attr->a_flags |= NS_ATTR_NOT_SPECIFIED;
	attrList->al_len += len;
	return attr;
}

nsAttr_t *
NsNode::addAttr(NsDocument *doc, const xmlbyte_t *prefix,
		const xmlbyte_t *uri, const xmlbyte_t *localname,
		const xmlbyte_t *value, bool specified)
{
	nsAttrList_t *attrList = nd_attrs;
	MemoryManager *mmgr = doc->getMemoryManager();

	if ((attrList == 0) || (attrList->al_nattrs == attrList->al_max)) {
		attrList = _reallocAttrList(mmgr, attrList);
		DBXML_ASSERT(attrList);
		nd_header.nh_flags |= NS_HASATTR;
		nd_attrs = attrList;
	}

	uint32_t index = attrList->al_nattrs++;
	nsAttr_t *attr =  &(attrList->al_attrs[index]);
	memset(attr, 0, sizeof(attr));
	
	attr = _setAttr(mmgr, this, index,
			localname, value, true, false,
			0, specified);
	int32_t uriIndex = NS_NOURI;
	int32_t prefixIndex = NS_NOPREFIX;
	if (prefix == 0 &&
	    NsUtil::nsStringEqual(localname,
				  (const xmlbyte_t *)_xmlnsPrefix8)) {
		/* default ns -- xmlns="uri" */
		setFlag(NS_HASNSINFO);
	}
	if (NsUtil::nsStringEqual(prefix,
				  (const xmlbyte_t *)_xmlnsPrefix8)) {
		/* prefix mapping -- xmlns:xxx */
		setFlag(NS_HASNSINFO);
	}
	if (uri) {
		uriIndex = doc->addIDForString((const char *)uri,
					       ::strlen((const char *)uri));
		// xmlns="" has uri, but no prefix
		if (prefix) {
			prefixIndex = doc->addIDForString(
				(const char *)prefix,
				::strlen((const char *)prefix));
		}
	}
	setAttrPrefix(attr, prefixIndex);
	setAttrUri(attr, uriIndex);
#ifdef NS_USE_SCHEMATYPES		
	// TBD.  If the attribute has type info,
	// add it to namespace prefix,
	// and put prefix in a_name.n_type.
#endif
	return attr;
}

nsAttr_t *
NsNode::setAttr(NsDocument *doc, uint32_t index,
		const xmlch_t *prefix, const xmlch_t *uri,
		const xmlch_t *localname, const xmlch_t *value,
		bool specified)
{
	MemoryManager *mmgr = doc->getMemoryManager();

	bool utf8 = !isUTF16();

	int32_t uriIndex = NS_NOURI;
	int32_t prefixIndex = NS_NOPREFIX;
	nsAttr_t *attr = 0;
	if(utf8) {
		NsDonator tattr(mmgr, localname, value, isAttrVal);
		attr = _setAttr(mmgr, this, index,
				tattr.getStr(),
				tattr.getStr2(),
				true, true,
				tattr.getLen(), specified);
		if (tattr.getHasEntity())
			attr->a_flags |= NS_ATTR_ENT;
	} else {
		attr = _setAttr(mmgr, this, index,
				localname, value,
				false, false, 0, specified);
	}

	if (prefix == 0 &&
	    NsUtil::nsStringEqual(localname,
				  _xmlnsPrefix16)) {
		/* default ns -- xmlns="uri" */
		setFlag(NS_HASNSINFO);
	}
	if (NsUtil::nsStringEqual(prefix,
				  _xmlnsPrefix16)) {
		/* prefix mapping -- xmlns:xxx */
		setFlag(NS_HASNSINFO);
	}
	if (uri) {
		XMLChToUTF8Null xuri(uri);
		uriIndex = doc->addIDForString(xuri.str(), xuri.len());
		// xmlns="" has uri, but no prefix
		if (prefix) {
			XMLChToUTF8Null xprefix(prefix);
			prefixIndex = doc->addIDForString(xprefix.str(),
							  xprefix.len());
		}
	}
	setAttrPrefix(attr, prefixIndex);
	setAttrUri(attr, uriIndex);
#ifdef NS_USE_SCHEMATYPES		
	// TBD.  If the attribute has type info,
	// add it to namespace prefix,
	// and put prefix in a_name.n_type.
#endif
	return attr;
}

nsAttr_t *
NsNode::addAttr(NsDocument *doc, const xmlch_t *prefix,
		const xmlch_t *uri, const xmlch_t *localname,
		const xmlch_t *value, bool specified)
{
	nsAttrList_t *attrList = nd_attrs;

	if ((attrList == 0) || (attrList->al_nattrs == attrList->al_max)) {
		attrList = _reallocAttrList(doc->getMemoryManager(), attrList);
		DBXML_ASSERT(attrList);
		nd_header.nh_flags |= NS_HASATTR;
		nd_attrs = attrList;
	}

	uint32_t index = attrList->al_nattrs++;
	nsAttr_t *attr =  &(attrList->al_attrs[index]);
	memset(attr, 0, sizeof(*attr));

	return setAttr(doc, index, prefix, uri, localname, value, specified);
}

void
NsNode::removeAttr(MemoryManager *mmgr, uint32_t index)
{
	nsAttrList_t *attrList = nd_attrs;

	if (attrList == 0 || attrList->al_nattrs <= index) return;

	if(attrList->al_nattrs == 1) {
		// There are no attributes left, so remove the nsAttrList_t
		freeAttrList(mmgr, attrList);
		nd_attrs = 0;
		nd_header.nh_flags &= ~NS_HASATTR;
	} else {
		nsAttr_t *attr =  &(attrList->al_attrs[index]);

		// Reduce total length of attrs, and number of attrs
		attrList->al_len -= attr->a_name.n_text.t_len + 1;
		attrList->al_nattrs -= 1;

		// Free the attr's values
		if(attr->a_name.n_text.t_chars != 0) {
			mmgr->deallocate(attr->a_name.n_text.t_chars);
			attr->a_name.n_text.t_chars = 0;
		}
		// al_nattrs has already been decremented, so don't subtract 1
		if (index < attrList->al_nattrs) {
			// Copy higher attrs down to fill space of removed attr
			// Use memmove, as src/dest may overlap
			memmove(attr, attr + 1,
				(attrList->al_nattrs - index) * sizeof(nsAttr_t));
		}
	}
}

int
NsNode::createPI(MemoryManager *mmgr, nsText_t *dest,
		 const void *target, const void *data, bool isUTF8,
		 bool donate, uint32_t len)
{
	if (donate) {
		DBXML_ASSERT(isUTF8 && len);
		dest->t_chars = (void *) target;
		dest->t_len = len - 1; // subtract trailing null
		return len;
	} else {
		return _addMultiText(mmgr, target, data,
				     dest, isUTF8, 0);
	}
}

//
// if donate is true:
//  o isUTF8 must be true
//  o string, starting at target, is of the form, target0data0
//  o len is the byte length of the entire string, including both nulls
//  o data is ignored, and can be null
//
//static
nsTextList_t *
NsNode::addPI(MemoryManager *mmgr, nsTextList_t *list, const void *target,
	      const void *data, bool isUTF8, bool donate, uint32_t len)
{
	/* all this is duplicated from addText() */
	if ((list == 0) || (list->tl_ntext == list->tl_max)) {
		list = _reallocTextList(mmgr, list);
		DBXML_ASSERT(list);
	}
        /* add to list */
	int index = list->tl_ntext++;
	nsTextEntry_t *entry = &list->tl_text[index];
	entry->te_type = NS_PINST;

	len = createPI(mmgr, &entry->te_text, target,
		       data, isUTF8, donate, len);
	DBXML_ASSERT(len > 0);

	list->tl_len += len;
	return list;
}

static void
_nsInsertPI(MemoryManager *mmgr, NsNode *node, uint32_t index, const void *target,
	    const void *data, bool isUTF8, bool donate, uint32_t len, bool isChild)
{
	nsTextEntry_t *entry =  _insertTextEntry(mmgr, node, index);
	nsTextList_t *textList = node->getTextList();
	// Create the new text entry
	entry->te_type = NS_PINST;
	textList->tl_len += NsNode::createPI(mmgr, &entry->te_text, target,
					     data, isUTF8, donate, len);

	// Increase the number of text
	textList->tl_ntext += 1;
	if (isChild) {
		textList->tl_nchild ++;
		node->setFlag(NS_HASTEXTCHILD);
	}
}

void
NsNode::insertPI(MemoryManager *mmgr, uint32_t index,
		 const xmlch_t *target, const xmlch_t *data, bool isChild)
{
	bool utf8 = !isUTF16();

	if(utf8) {
		NsDonator chars(mmgr, target, data);
		_nsInsertPI(mmgr, this, index, chars.getStr(), chars.getStr2(),
			    true, true, chars.getLen(), isChild);
	} else {
		_nsInsertPI(mmgr, this, index, target, data,
			    false, false, 0, isChild);
	}
}

// utilities and debug routines
#if defined(NS_DEBUG)

#define NS_CH_CAST xmlbyte_t
static const char crText[2] = {'\\','r'};
static const char lfText[2] = {'\\','n'};
static const char tabText[2] = {'\\','t'};
static const char spText[2] = {'s','p'};
static const unsigned char ch_CR = '\015';
static const unsigned char ch_LF = '\012';
static const unsigned char ch_Tab = '\027';
static const unsigned char ch_Space = ' ';
static const char *

_asWhiteSpace(const char *chars, char *buf)
{
	const char* ptr = chars;
	char *toptr = buf;
	bool isAllWS = true;
	while (*ptr) {
		if (*ptr == ch_CR) {
			memcpy(toptr, crText, 2);
			toptr += 2;
		} else if (*ptr == ch_LF) {
			memcpy(toptr, lfText, 2);
			toptr += 2;
		} else if (*ptr == ch_Tab) {
			memcpy(toptr, tabText, 2);
			toptr += 2;
		} else if (*ptr == ch_Space) {
			memcpy(toptr, spText, 2);
			toptr += 2;
		} else {
			*toptr++ = *ptr;
			isAllWS = false;
		}
		ptr++;
	}
	*toptr = 0;
	return (isAllWS ? buf : chars);
}

static const char *
_printFlags(uint32_t flags, char *buf)
{
	*buf = 0;
	if (flags  & NS_ALLOCATED)
		strcat(buf, "ALLOCATED|");
	if (flags & NS_HASCHILD)
		strcat(buf, "CHILD|");
	if (flags & NS_HASTEXTCHILD)
		strcat(buf, "TEXTCHILD|");
	if (flags & NS_HASNEXT)
		strcat(buf, "NEXT|");
	if (flags & NS_HASPREV)
		strcat(buf, "PREV|");
	if (flags & NS_HASATTR)
		strcat(buf, "ATTR|");
	if (flags & NS_HASTEXT)
		strcat(buf, "TEXT|");
	if (flags & NS_HASURI)
		strcat(buf, "HASURI|");
	if (flags & NS_LAST_IS_LAST_DESC)
		strcat(buf, "LAST_IS_LAST_DESC|");
	if (flags & NS_ISDOCUMENT)
		strcat(buf, "ISDOCUMENT|");
	if (flags & NS_HASNSINFO)
		strcat(buf, "NSINFO|");
	if (flags & NS_STANDALONE)
		strcat(buf, "STANDALONE|");
	if (flags & NS_UTF16)
		strcat(buf, "UTF16|");
	int l = strlen(buf);
	if (buf[l-1] == '|')
		buf[l-1] = '\0';
	return buf;
}

static void
_printNav(nsNav_t *nav)
{
	std::ostringstream oss;
	oss << "    Nav:\n";
	if (!nav->nn_prev.isNull()) {
		oss << "\tPrev: ";
		nav->nn_prev.displayNid(oss);
		oss << "\n";
	}
	if (!nav->nn_lastChild.isNull()) {
		oss << "\tlastChild: ";
		nav->nn_lastChild.displayNid(oss);
		oss << "\n";
	}
	printf("%s", oss.str().c_str());
}

//
// assume text is UTF-8
//
static void
_printText(nsTextEntry_t *entry, int index)
{
	const char *type = "text";
	const char *text = (char *)entry->te_text.t_chars;
	char *buf = (char *)malloc((entry->te_text.t_len + 1) << 1);
	if (!buf)
		return;

	printf("\t%d, len %d ", index, entry->te_text.t_len);
	if (nsTextType(entry->te_type) == NS_COMMENT)
		type = "comment";
	else if (nsTextType(entry->te_type) == NS_CDATA)
		type = "cdata";
	else  if (nsTextType(entry->te_type) == NS_PINST) {
		/* PI is stored as target0data0 */
		const char *datap = text + NsUtil::nsStringLen((const unsigned char *)text) + 1;
		type = "pinst";
		printf("(%s): ", type);
		printf("%s=", _asWhiteSpace(text, buf));
		printf("%s\n", _asWhiteSpace(datap, buf));
		free(buf);
		return;
	}
	printf("(%s): %s\n", type, _asWhiteSpace((const char *)text, buf));
	free(buf);
}

void
NsNode::printNode(NsDocument *doc)
{
	char fbuf[512];
	const char *name = "#DOCUMENT";
	int32_t prefix = NS_NOPREFIX;
	int32_t typeIndex = NS_NOTYPE;
	printf("Node: ");
	if (!(nd_header.nh_flags & NS_ISDOCUMENT)) {
		name = (const char *)
			nd_header.nh_name.n_text.t_chars;
		prefix = nd_header.nh_name.n_prefix;
#ifdef NS_USE_SCHEMATYPES			
		typeIndex = nd_header.nh_name.n_type;
#endif
	}

	if (prefix != NS_NOPREFIX)
		if(doc == 0)
			printf(" %d:", prefix);
		else
			printf(" %s:", doc->getStringForID(prefix));
	printf("%s, level %d, %s, NS prefix %d",
	       name,
	       nd_level,
	       _printFlags(nd_header.nh_flags, fbuf),
	       prefix);
	if (typeIndex != NS_NOTYPE)
		if(doc == 0)
			printf(", type: %d", typeIndex);
		else
			printf(", type: %s", doc->getStringForID(typeIndex));
	printf("\n");
	std::ostringstream oss;
	oss << "    ";
	getNid()->displayNid(oss);
	printf("%s\n", oss.str().c_str());
	if (nd_attrs) {
		nsAttrList_t *attrs = nd_attrs;
		printf("    Attrs (num, total len, max):  %d, %d, %d\n",
		       attrs->al_nattrs, attrs->al_len, attrs->al_max);
		for (uint32_t i = 0; i < attrs->al_nattrs; i++) {
			nsText_t *text = &attrs->al_attrs[i].a_name.n_text;
			printf("\t");
			if (attrs->al_attrs[i].a_name.n_prefix != NS_NOPREFIX)
				if(doc == 0)
					printf("%d:", attrs->al_attrs[i].
						a_name.n_prefix);
				else
					printf("%s:", doc->getStringForID(
						       attrs->al_attrs[i].
						       a_name.n_prefix));
			const xmlbyte_t *value = (const xmlbyte_t *)attrs->
				al_attrs[i].a_value;
			printf("%s = %s", (const xmlbyte_t *)
			       text->t_chars, value);
#ifdef NS_USE_SCHEMATYPES				
			if (attrs->al_attrs[i].a_name.n_type != NS_NOTYPE) {
				if(doc == 0)
					printf(", attrtype: %d", attrs->al_attrs[i].
						a_name.n_type);
				else
					printf(", attrtype: %s",
						doc->getStringForID(
							attrs->al_attrs[i].
							a_name.n_type));
			}
#endif
			printf("\n");
		}
	}
	if (nd_text) {
		nsTextEntry_t *entries = nd_text->tl_text;
		printf("    Text nodes (num, total len, max):  %d, %d, %d\n",
		       nd_text->tl_ntext,
		       nd_text->tl_len, nd_text->tl_max);
		for (uint32_t i = 0; i < nd_text->tl_ntext; i++) {
			_printText(&entries[i], i);
		}
	}
	if (nd_nav)
		_printNav(nd_nav);
}

#endif // NS_DEBUG
