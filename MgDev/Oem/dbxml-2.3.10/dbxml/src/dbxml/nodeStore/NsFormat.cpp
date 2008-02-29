//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsFormat.cpp,v 1.15 2006/11/01 15:57:28 jsnelson Exp $
//
#include "NsNode.hpp"
#include "NsFormat.hpp"
#include "NsUtil.hpp"
#include "../DocID.hpp"
#include "../ScopedDbt.hpp"

#define FORMAT_DEBUG 1

#ifdef FORMAT_DEBUG
#include <ostream>
#include <sstream>
using namespace std;
#endif	

/*
 * NsFormat and subclasses
 *
 * The persistent node format is ultimately determined by the
 * marshaling functions.
 *
 * This class is partly static, and partly virtual, allowing
 * for multiple protocol versions/formats using the same basic
 * record layout.  Persistence models that don't fit a basic
 * node record style will probably not share this code.
 * 
 * NOTES: Namespace prefixes, uri indexes, and
 * type information are all integer indexes into
 * the maps that map the integer to a specific string.
 *
 * All string values are encoded
 * in UTF-8, and length fields are in bytes.
 * Integers are encoded in a variable-length format (see
 * comments in NsFormat.cpp on the format)
 *
 * All node storage documents are kept in a single DB btree
 * database.  The database allows duplicates, and keeps them
 * sorted.  Each individual document is a set of duplicate
 * keys, where the key is the integer document id.  The
 * duplicates are sorted by their values.  The first several
 * bytes of each node are used for sorting.  They are:
 *   protocol version byte, node id string (null-terminated).
 * There is a custom comparison function registered that knows
 * how to compare node ids.  The protocol version is ignored
 * for sorting purposes, as it's the same for each node.
 *
 *
 * Format Descriptions
 *
 * Protocol Version 1
 *
 * NOTE: implementation of unmarshal for version 1 is in
 * NsUpgrade.cpp.  Marshaling is no longer required.
 *
 * In protocol version 1, each document has 1-2 metadata
 * nodes (ids '1' and '2') that keep document metadata and
 * (optional) namespace/type information.
 * The first "regular" node of a document (id 'A') is the
 * "document" node, but looks like any regular node.
 *
 * This is the format of nodes in protocol version 1:
 *
 * <header information>
 *    Protocol version -- one byte (NS_PROTOCOL_VERSION)
 *    Node id -- variable length, null-terminated id
 *    Unmarshal Size -- integer size indicating how much space
 *         is needed to unmarshal this node
 *    Flags -- integer flags (NS_*, from NsNode.hpp)
 *    Level -- integer tree level of node (doc node is 0)
 *    Parent id -- node id of parent (except for document node)
 *    Node name (except for document node)
 *      optional integer prefix
 *      optional integer URI index
 *      optional integer type
 *      text name
 * <end header information>
 * <optional information>
 *    If has child nodes (elements),
 *      Integer number of child nodes
 *      For each child:
 *        If there are text children, an integer for each of the element
 *          children, representing the child's text index.
 *        A node id for the child
 *    If has attributes:
 *      Integer number of attributes
 *      For each attribute:
 *        optional integer prefix
 *        optional integer type
 *        name/value byte array (name0value0)
 *
 *    If has text nodes:
 *      Integer number of text nodes
 *      For each text node:
 *        integer type of text node (enum NS_TEXTTYPES)
 *        text bytes (PIs may have embedded nulls)
 *
 * This is the format of node id '1' -- document metadata:
 *  Protocol version byte (NS_PROTOCOL_VERSION)
 *  ID string -- 2 bytes: '1', 0
 *  Flags -- integer
 *  XML decl -- integer indicating xml 1.0 or 1.1
 *  Optional encoding string (if specified)
 *
 * The optional namespace/type info, id '2', is of the format:
 *  Protocol version byte (NS_PROTOCOL_VERSION)
 *  ID string -- 2 bytes: '2', 0
 *  Num Uris -- integer number of URIs
 *  For each URI, null-terminated uri string
 *  Num prefixes -- integer number of prefixes in map
 *  For each prefix:
 *    integer index into uri array
 *    null-terminated prefix string
 *
 */
/*
 * This is the format of nodes in protocol version 2:
 *
 * NOTE:
 *    o uri and type indexes reference into per-container lists
 *    o prefix map is kept with the document, and maps from an integer
 *    to a string, plus uri index.
 *    o Node ID now goes in the database key
 *
 * <header information> (same as version 1)
 *    Protocol version -- one byte (NS_PROTOCOL_VERSION)
 *    Flags -- integer flags (NS_*, from NsNode.hpp)
 *    If has attrs, integer number of attrs
 *    If has text, integer number of text nodes
 *    Last descendant nid
 *    Level -- integer tree level of node (doc node is 0)
 *    Parent id -- node id of parent (except for document node)
 *    Node name (except for document node)
 *      optional integer prefix
 *      optional integer URI index
 *      optional integer type
 *      text name
 * <end header information>
 * <optional information>
 *   If has sibling element(s)
 *     nextElem nid
 *   If has text:
 *     Integer skip size
 *     Integer num child text nodes (last)
 *     For each text node:
 *       type, bytes
 *       If type is NS_EXTERNAL, bytes is elsewhere (TBD)
 *     leading (sibling) text
 *     child text
 *   If has attrs:
 *     Integer skip size for attr list
 *     For each attribute:
 *        optional integer prefix
 *        optional integer type
 *        name/value byte array (name0value0)
 *   If has sibling element(s)
 *     prevElem nid
 *   If has child elements
 *     last child nid
 */
using namespace DbXml;

namespace DbXml {
	
#ifdef FORMAT_DEBUG
static int _dumpNodes = 0;
void dumpNode(NsNode *node);
#endif	

class NsFormat2: public NsFormat {
public:
	virtual int marshalNodeKey(const DocID &did, const NsNid *nid,
		unsigned char *buf, bool count) const;
	virtual void unmarshalNodeKey(DocID &did, NsNid *nid, XER_NS MemoryManager *mmgr,
		unsigned char *buf, bool copyStrings) const;

	virtual int marshalNodeData(const NsNode *node,
				unsigned char *buf, bool count) const;
	virtual  NsNode *unmarshalNodeData(XER_NS MemoryManager *mmgr,
				       unsigned char *buf, bool adoptBuffer) const;
};

// NOTE: format 1 is not implemented here.  See NsUpgrade.*	
NsFormat2 f2;	
NsFormat *NsFormat::formats[NS_NUM_PROTOCOLS] = {0, &f2};
};

XERCES_CPP_NAMESPACE_USE

/*
 * Marshaling/unmarshaling related functions
 */

// NS_UNMARSH_MIN* used on unmarshal side only

#define NS_UNMARSH_MIN_2 sizeof(NsNode) + \
	sizeof(nsTextList_t) +		  \
	sizeof(nsAttrList_t) +		  \
	sizeof(nsNav_t)

/*
 * internal functions/macros to count bytes for marshaling
 */

/* t_len is in UTF-8 bytes */
#define _nsTextSize(t) ((t).t_len + 1) /* include null */

/*
 * marshal UTF-8 text, including trailing null.
 * The text has already been transcoded to UTF-8.
 *
 * An advantage of using UTF-8 here is that there is no
 * chance of buffer overflow.
 *
 */
static inline int _nsMarshText(unsigned char *ptr, const nsText_t *text)
{
	memcpy(ptr, text->t_chars, text->t_len + 1);
	return (text->t_len + 1);
}

/*
 * marshal an attribute.  Format is:
 *  flags [prefixIndex] [uriIndex] [typeIndex] [unionTypeIndex] name0value0
 * where the various indexes (ints) are based on their related flags fields.
 * unionTypeIndex can only exist if typeIndex is set, also.
 */
static inline int _nsMarshAttr(unsigned char *ptr, const nsAttr_t *attr)
{
	// nsText_t field of the attribute name has entire
	// name/value string, contiguous.
	unsigned char *tptr = ptr;
	tptr += NsFormat::marshalInt(tptr, attr->a_flags);
	if (attr->a_flags & NS_ATTR_PREFIX)
		tptr += NsFormat::marshalInt(tptr, attr->a_name.n_prefix);
	if (attr->a_flags & NS_ATTR_URI)
		tptr += NsFormat::marshalInt(tptr, attr->a_uri);
#ifdef NS_USE_SCHEMATYPES	
	if (attr->a_flags & NS_ATTR_TYPE) {
		tptr += NsFormat::marshalInt(tptr, attr->a_name.n_type);
		if (attr->a_flags & NS_ATTR_UNION_TYPE)
			tptr += NsFormat::marshalInt(tptr, attr->a_name.n_utype);
	}
#endif
	tptr += _nsMarshText(tptr, &attr->a_name.n_text);

	return (tptr-ptr);
}

/*
 * allocate "space" for xmlbyte_t from end of array.
 * endPP points to last *available* memory.
 */
static inline void *_allocChars(xmlbyte_t **endPP, int len) {
	*endPP -= (len);
	xmlbyte_t *ret = *endPP + 1;
	return (void*) ret;
}

#define _nsAllocChars(dest, endPP, len) (dest) = _allocChars((endPP), (len))

uint32_t
NsFormat::unmarshalId(MemoryManager *mmgr, const xmlbyte_t *ptr, NsNid *id)
{
	uint32_t len = NsUtil::nsStringLen(ptr) + 1;
	if(len == 1) {
		id->freeNid(mmgr);
	} else {
		id->copyNid(mmgr, ptr, len);
	}
	return len;
}

/*
 * Unmarshal ID.
 * Eventually, this will use space off of endPP, which is
 * why it's passed.
 */
uint32_t
NsFormat::unmarshalId(const xmlbyte_t *ptr, NsNid *id, xmlbyte_t ** endPP, bool copyStrings)
{
	uint32_t len = NsUtil::nsStringLen(ptr) + 1;
	id->setLen(len, /*alloced*/false);
	if (len > NsNid::nidStaticSize()) {
		xmlbyte_t *dest;
		if(copyStrings) {
			dest = (xmlbyte_t*)_allocChars(endPP, len);
			memcpy(dest, ptr, len);
		} else {
			dest = (xmlbyte_t*)ptr;
		}
		id->setPtr(dest);
	} else {
		// id's len must be set for this to work
		memcpy(id->getBytesForCopy(), ptr, len);
	}
	return len;
}

/*
 * Unmarshal text.  Text comes from a null-terminated, UTF-8 string,
 * pointed to by the ptr argument.  Return all characters moved, including
 * the NULL.
 */

//static
uint32_t
NsFormat::unmarshText(unsigned char *ptr, nsText_t *text,
		      xmlbyte_t **endPP, bool copyStrings)
{
	uint32_t len = NsUtil::nsStringLen(ptr) + 1;

	if(copyStrings) {
		/* allocate UTF-16 memory */
		_nsAllocChars(text->t_chars, endPP, len);
		memcpy(text->t_chars, ptr, len);
	}
	else {
		text->t_chars = (void*)ptr;
	}
	text->t_len = len - 1;
	return len;
}

/*
 * unmarshal attribute list
 * ptr points to packed attribute list of the form:
 *   name10value10name20value20...
 *
 */
//static
unsigned char *
NsFormat::unmarshAttrs(unsigned char *ptr, nsAttrList_t *attrs,
		       xmlbyte_t **endPP, bool copyStrings)
{
	for (uint32_t i = 0; i < attrs->al_nattrs; i++) {
		nsAttr_t *attr = &attrs->al_attrs[i];
		nsName_t *name = &attr->a_name;

		ptr += NsFormat::unmarshalInt(ptr,
					    (uint32_t*)&(attr->a_flags));
		if (attr->a_flags & NS_ATTR_PREFIX)
			ptr += NsFormat::unmarshalInt(
				ptr, (uint32_t*)&(name->n_prefix));
		else
			name->n_prefix = NS_NOPREFIX;
		if (attr->a_flags & NS_ATTR_URI)
			ptr += NsFormat::unmarshalInt(ptr,
					     (uint32_t*)&(attr->a_uri));
		else
			attr->a_uri = NS_NOURI;
#ifdef NS_USE_SCHEMATYPES		
		if (attr->a_flags & NS_ATTR_TYPE) {
			ptr += NsFormat::unmarshalInt(
				ptr, (uint32_t*)&(name->n_type));
			if (attr->a_flags & NS_ATTR_UNION_TYPE)
				ptr += NsFormat::unmarshalInt(
					ptr, (uint32_t*)&(name->n_utype));
		} else
			name->n_type = NS_NOTYPE;
#endif

		int name_len = NsUtil::nsStringLen(ptr) + 1; /* add null */
		int alen = name_len + NsUtil::nsStringLen(ptr + name_len) + 1;

		if(copyStrings) {
			_nsAllocChars(name->n_text.t_chars, endPP, alen);
			memcpy(name->n_text.t_chars, ptr, alen);
		}
		else {
			name->n_text.t_chars = (void*)ptr;
		}
		name->n_text.t_len = alen - 1;

		// set value
		attr->a_value = ((xmlbyte_t*)name->n_text.t_chars) + name_len;

		/* al_len includes all nulls */
		attrs->al_len += alen;
		ptr += alen;
	}
	return ptr;
}

/*
 * unmarshal text list
 * ptr points to packed text list of the form:
 *   [type]text10[type]text20...
 * except for PI, which is [type]target0data0
 *
 */
//static
unsigned char *
NsFormat::unmarshTextList(unsigned char *ptr, nsTextList_t *text,
			  xmlbyte_t **endPP, bool copyStrings)
{
	for (uint32_t i = 0; i < text->tl_ntext; i++) {
		nsTextEntry_t *entry = &text->tl_text[i];
		entry->te_type = (uint32_t) *ptr++;
		int tlen = NsUtil::nsStringLen(ptr) + 1; /* add null */
		if (nsTextType(entry->te_type) == NS_PINST)
			tlen += NsUtil::nsStringLen(ptr + tlen) + 1;
		if(copyStrings) {
			_nsAllocChars(entry->te_text.t_chars, endPP, tlen);
			memcpy(entry->te_text.t_chars, ptr, tlen);
		}
		else {
			entry->te_text.t_chars = (void*)ptr;
		}
		entry->te_text.t_len = tlen - 1;
		/* tl_len includes all nulls */
		text->tl_len += tlen;
		ptr += tlen;
	}
	return ptr;
}

//
// Format-dependent functions
//

// static
void
NsFormat::marshalNodeKey(const DocID &did, const NsNid *nid, DbtOut &dbt)
{
	const NsFormat &fmt = NsFormat::getFormat(NS_PROTOCOL_VERSION);
	
	int size = fmt.marshalNodeKey(did, nid, 0, /*count*/true);
	dbt.set(0, size);

	fmt.marshalNodeKey(did, nid, (unsigned char*)dbt.get_data(),
		/*count*/false);
}

// static
void
NsFormat::marshalNextNodeKey(const DocID &did, const NsNid *nid, DbtOut &dbt)
{
	const NsFormat &fmt = NsFormat::getFormat(NS_PROTOCOL_VERSION);
	
	int size = fmt.marshalNodeKey(did, nid, 0, /*count*/true);
	dbt.set(0, size + 1);

	fmt.marshalNodeKey(did, nid, (unsigned char*)dbt.get_data(),
		/*count*/false);

	// We add an extra 0 at the end of the key - this turns it into an
	// invalid node ID, but that doesn't matter as we just want to use
	// this key with DB_SET_RANGE to get the next entry in the BTree.
	((unsigned char*)dbt.get_data())[size] = 0;
}

int NsFormat2::marshalNodeKey(const DocID &did, const NsNid *nid,
	unsigned char *ptr, bool count) const
{
	int size = 0;

	if(count) {
		size = did.marshalSize();
		if(nid != 0)
			size += countId(nid);
	} else {
		ptr += did.marshal(ptr);
		if(nid != 0)
			ptr += NsFormat::marshalId(ptr, nid);
	}

	return size;
}

void NsFormat2::unmarshalNodeKey(DocID &did, NsNid *nid,
	MemoryManager *mmgr, unsigned char *ptr, bool copyStrings) const
{
	ptr += did.unmarshal(ptr);

	if(copyStrings) {
		ptr += unmarshalId(mmgr, ptr, nid);
	} else {
		ptr += unmarshalId(ptr, nid, 0, copyStrings);
	}
}

//
// NsFormat2 implementation
//
// NB node ID now goes in the database key
// NB first child NID is implied, since it is the next
//    node in the database
// NB next sibling NID is implied by being the next node
//    after the last descendant
//
// NsFormat2 header: unmarshal
// protocol, flags, [nattrs], [ntext], last descendant nid,
//   level, parentNid, [name pfx],[name uri],[name type],
//   name text
//   if document node, no level, parent, name info
// Body:
// if has text:
//    skip size, num child text nodes,
//    leading (sibling) text nodes: <type, bytes>+
//    child text nodes: <type, bytes>+
// if has attr(s)
//    skip size, <[pfx],[type],name0value0>+
// if has previous sibling
//    prevElem nid
// if has child elems
//    last child nid
//
int
NsFormat2::marshalNodeData(const NsNode *nodep, unsigned char *buf, bool count) const
{
	// accessors to NsNode are non-const, but safe
	NsNode *node = (NsNode *)nodep;
	unsigned char *ptr = buf;
	int size = 0;
	// nattrs and ntext go in header to better enable
	// unmarshal to estimate memory requirements
	int32_t nattrs = 0;
	int32_t ntext = 0;
	if (node->hasAttributes())
		nattrs = node->numAttrs();
	if (node->hasText())
		ntext = node->getNumText();

	// set last_is_last_desc in flags
	// before counting flags.  These flags are *only* used during
	// marshal/unmarshal, and are not part of normal state
	if (node->hasChildElem() && count) {
		if (*node->getLastChildNid() == *node->getLastDescendantNid())
			node->setFlag(NS_LAST_IS_LAST_DESC);
	}
	
	uint32_t tflags = node->getFlags();
	DBXML_ASSERT((tflags & NS_UTF16) == 0);
	if (count) {
		size += 1; // Protocol version
		size += countInt(tflags & NS_DBFLAGMASK);
		if (nattrs)
			size += countInt(nattrs);
		if (ntext)
			size += countInt(ntext);
		if (node->hasChildElem())
			size += countId(node->getLastDescendantNid());
		if (!(tflags & NS_ISDOCUMENT)) {
			// document node has no parent, and no name, and level is 0
			size += countInt(node->getLevel());
			size += countId(node->getParentNid());
			size += _nsTextSize(*node->getNameText());
			if (tflags & NS_NAMEPREFIX)
				size += countInt(node->namePrefix());
			if (tflags & NS_HASURI)
				size += countInt(node->uriIndex());
#ifdef NS_USE_SCHEMATYPES			
			if (tflags & NS_NAMETYPE) {
				size += countInt(node->typeIndex());
				if (tflags & NS_UNIONTYPE)
					size += countInt(node->utypeIndex());
			}
#endif
		}
	} else {
		*ptr++ = NS_PROTOCOL_VERSION;
		// Remove flags that shouldn't go into the db
		ptr += marshalInt(ptr, tflags & NS_DBFLAGMASK);
		// number of attribute and text nodes, up front, to
		// aid in unmarshal
		if (nattrs)
			ptr += marshalInt(ptr, nattrs);
		if (ntext)
			ptr += marshalInt(ptr, ntext);
		if (node->hasChildElem())
			ptr += marshalId(ptr, node->getLastDescendantNid());
		if (!(tflags & NS_ISDOCUMENT)) {
			ptr += marshalInt(ptr, node->getLevel());
			ptr += marshalId(ptr, node->getParentNid());
			if (tflags & NS_NAMEPREFIX)
				ptr += marshalInt(ptr, node->namePrefix());
			if (tflags & NS_HASURI)
				ptr += marshalInt(ptr, node->uriIndex());
#ifdef NS_USE_SCHEMATYPES			
			if (tflags & NS_NAMETYPE) {
				ptr += marshalInt(ptr, node->typeIndex());
				if (tflags & NS_UNIONTYPE)
					ptr += marshalInt(
						ptr, node->utypeIndex());
			}
#endif
			ptr += _nsMarshText(ptr, node->getNameText());
		}
	}
	//
	// optional information, in order of:
	//  text (leading and child)
	//  attributes
	//  previous sibling, last child
	//

	// text: ntext is in header
	// <skipSize, [numChildText] (textType, text, 0)+>
	if (ntext) {
		nsTextList_t *list = node->getTextList();
		uint32_t msize = list->tl_len +
			list->tl_ntext; // type bytes
		if (count) {
			// ntext already counted
			size += msize;
			size += countInt(msize);
			if (node->hasTextChild()) {
				DBXML_ASSERT(list->tl_nchild);
				size += countInt(list->tl_nchild);
			}
		} else {
			ptr += marshalInt(ptr, msize);
			if (node->hasTextChild()) {
				DBXML_ASSERT(list->tl_nchild);
				ptr += marshalInt(ptr, list->tl_nchild);
			}
			for (int i = 0; i < ntext; i++) {
				*ptr++ = (unsigned char)list->tl_text[i].te_type;
				ptr += _nsMarshText(ptr,
					    &(list->tl_text[i].te_text));
			}
		}
	}

	// attributes: nattrs is in header
	// < skipSize, (nm0val0)+ >
	if (nattrs) {
		nsAttrList_t *attrs = node->getAttrList();
		if (count) {
			uint32_t msize = attrs->al_len;
			// num attrs already counted
			// need to count optional:
			//  prefixes, uris, and types
			for (int i = 0; i < nattrs; i++) {
				nsAttr_t *attr = &(attrs->al_attrs[i]);
				msize += 1; // a_flags are ALWAYS 1 byte
				if (attr->a_flags & NS_ATTR_PREFIX)
					msize += countInt(attr->a_name.n_prefix);
				if (attr->a_flags & NS_ATTR_URI)
					msize += countInt(attr->a_uri);
#ifdef NS_USE_SCHEMATYPES				
				if (attr->a_flags & NS_ATTR_TYPE) {
					msize += countInt(attr->a_name.n_type);
					if (attr->a_flags &
					    NS_ATTR_UNION_TYPE)
						msize += countInt(attr->a_name.n_utype);
				}
#endif
			}
			size += msize;
			size += countInt(msize);
			attrs->al_marshalLen = msize;
		} else {
			// TBD: either use skip size or nuke it...
			ptr += marshalInt(ptr, attrs->al_marshalLen);
			for (int i = 0; i < nattrs; i++) {
				ptr += _nsMarshAttr(ptr,
						    &(attrs->al_attrs[i]));
			}
		}
	}

	// previous sibling, last child
	if (count) {
		if (node->hasPrev())
			size += countId(node->getPrevNid());
		if (node->hasChildElem() && !node->lastIsLastDescendant()) {

			size += countId(node->getLastChildNid());

			DBXML_ASSERT(*node->getLastChildNid() !=
				*node->getLastDescendantNid());
		}
	} else {
		if (node->hasPrev())
			ptr += marshalId(ptr, node->getPrevNid());
		if (node->hasChildElem() && !node->lastIsLastDescendant()) {
			ptr += marshalId(ptr, node->getLastChildNid());
		}
	}
	
	return size;
}


NsNode *
NsFormat2::unmarshalNodeData(MemoryManager *mmgr, unsigned char *buf,
	bool adoptBuffer) const
{
	uint32_t allocSize = NS_UNMARSH_MIN_2;
	uint32_t flags, dummy;
	uint32_t nattrs = 0;
	uint32_t ntext = 0;
	NsNode *node;
	char *curP;
	unsigned char *ptr = buf;
	xmlbyte_t *endP;

	if (*ptr++ != NS_PROTOCOL_VERSION)
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "protocal mismatch in node",
					 __FILE__, __LINE__);
	//
	// unmarshal size is determined by number of attributes and text nodes,
	// plus fixed overhead.
	// Header is: protocol, flags, [nattrs], [ntext], ...
	// 

	ptr += unmarshalInt(ptr, &flags);
	if (flags & NS_HASATTR) {
		ptr += unmarshalInt(ptr, &nattrs);
		allocSize += nattrs * sizeof(nsAttr_t);
	}
	if (flags & NS_HASTEXT) {
		ptr += unmarshalInt(ptr, &ntext);
		allocSize += ntext * sizeof(nsTextEntry_t);
	}

	// Allocate memory for the node
	DBXML_ASSERT(mmgr);
	curP = (char *)mmgr->allocate(allocSize);
	if (!curP)
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "unmarshalNode failed to allocate memory",
					 __FILE__, __LINE__);
	memset(curP, 0, allocSize);

	node = (NsNode*)curP;
	node->setFormat(this);
	*node->getFlagsPtr() = flags;

	if(adoptBuffer) node->setMemory(buf);

	endP = (xmlbyte_t *) (curP + allocSize - 1);
	curP = (char *) (node + 1);

	// unmarshal the rest of the header
	if (node->hasChildElem())
		ptr += unmarshalId(ptr, node->getLastDescendantNidPtr(),
			&endP, /*copyStrings*/false);

	if(!(node->checkFlag(NS_ISDOCUMENT))) {
		ptr += NsFormat::unmarshalInt(ptr, node->getLevelPtr());
		nsName_t *nm = node->getName();
		ptr += NsFormat::unmarshalId(ptr, node->getParentNid(),
					   0, /*copyStrings*/false);

		if (node->checkFlag(NS_NAMEPREFIX))
			ptr += NsFormat::unmarshalInt(ptr,
		            (uint32_t*)&(nm->n_prefix));
		else
			nm->n_prefix = NS_NOPREFIX;
		if (node->checkFlag(NS_HASURI))
			ptr += NsFormat::unmarshalInt(
				ptr, (uint32_t*)node->uriIndexPtr());
		else
			*(node->uriIndexPtr()) = 0;
#ifdef NS_USE_SCHEMATYPES		
		if (node->checkFlag(NS_NAMETYPE)) {
			ptr += NsFormat::unmarshalInt(ptr,
		            (uint32_t*)&(nm->n_type));
			if (node->checkFlag(NS_UNIONTYPE))
				ptr += NsFormat::unmarshalInt(
					ptr, (uint32_t*)&(nm->n_utype));
		} else
			nm->n_type = NS_NOTYPE;
#endif
		ptr += unmarshText(ptr, &(nm->n_text),
				   0, /*copyStrings*/false);
	} else
		node->setLevel(0); // document node is level 0



	//
	// TBD: unmarshal on demand -- either that, or drop skip size
	// from text and attrs
	//
	// Body: text, attrs, prev sib, last child

	// text: ntext is in header
	// <skipSize, [numChildText] (textType, text, 0)+>
	if (node->hasText()) {
		DBXML_ASSERT(ntext);
		ptr += unmarshalInt(ptr, &dummy); // not using skip size for now
		nsTextList_t *text = (nsTextList_t *)curP;
		node->setTextList(text);
		text->tl_ntext = ntext; // from header
		if (node->hasTextChild())
			ptr += unmarshalInt(ptr, &(text->tl_nchild));
		text->tl_max = ntext;
		curP = (char *)(text + 1);
		curP += ((ntext - 1) * sizeof(nsTextEntry_t));
		// TBD: handle external text
		ptr = unmarshTextList(ptr, text, &endP, /*copyStrings*/false);
	}

	// attributes: nattrs is in header
	// < skipSize, (nm0val0)+ >
	if (node->hasAttributes()) {
		DBXML_ASSERT(nattrs);
		ptr += unmarshalInt(ptr, &dummy); // not using skip size for now
		nsAttrList_t *attrs = (nsAttrList_t *)curP;
		node->setAttrList(attrs);
		attrs->al_nattrs = nattrs;
		attrs->al_max = nattrs;
		curP = (char *)(attrs + 1);
		/* add nattrs-1 worth of nsText_t */
		curP += ((nattrs - 1) * sizeof(nsAttr_t));
		/* unmarshal the attributes */
		ptr = unmarshAttrs(ptr, attrs, &endP, /*copyStrings*/false);
	}

	if (node->needsNav()) {
		nsNav_t *nav = (nsNav_t *)curP;
		node->setNav(nav);
		curP += sizeof(nsNav_t);
	}

	// previous sibling
	if (node->hasPrev())
		ptr += unmarshalId(ptr, node->getPrevNid(),
				   &endP, /*copyStrings*/false);

	// last child
	if (node->hasChildElem()) {
		if(node->lastIsLastDescendant()) {
			node->getLastChildNid()->copyNid(mmgr,
				node->getLastDescendantNid());
		} else {
			ptr += unmarshalId(ptr, node->getLastChildNid(),
				&endP, /*copyStrings*/false);
		}
	}

	/* debug */
	if (curP >= (char *)endP) {
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "unmarshalNode overlap",
					 __FILE__, __LINE__);
	}
	// clear LAST_IS_LAST_DESC flags
	node->clearFlag(NS_LAST_IS_LAST_DESC);
#ifdef FORMAT_DEBUG
	if (_dumpNodes)
		dumpNode(node);
#endif	
	return node;
}

/**
 * 64-bit integer marshaling
 */
int NsFormat::marshalInt64(xmlbyte_t *buf, uint64_t i)
{
	if (i <= NSINT_MAX1) {
		// no swapping for one byte value
		buf[0] = (unsigned char) i;
		return 1;
	} else {
		uint64_t tmp = i;
		if (!Globals::isBigendian_)
			M_64_SWAP(tmp);
		unsigned char *p = (unsigned char *) & tmp;
		if (i <= NSINT_MAX2) {
			buf[0] = (p[6] | NSINT_2BYTE_VAL);
			buf[1] = p[7];
			return 2;
		} else if (i <= NSINT_MAX3) {
			buf[0] = (p[5] | NSINT_3BYTE_VAL);
			buf[1] = p[6];
			buf[2] = p[7];
			return 3;
		} else  if (i <= NSINT_MAX4) {
			buf[0] = (p[4] | NSINT_4BYTE_VAL);
			buf[1] = p[5];
			buf[2] = p[6];
			buf[3] = p[7];
			return 4;
		} else if (i <= NSINT_MAX5) {
			buf[0] = (p[3] | NSINT_5BYTE_VAL);
			buf[1] = p[4];
			buf[2] = p[5];
			buf[3] = p[6];
			buf[4] = p[7];
			return 5;
		} else if (i <= NSINT_MAX6) {
			buf[0] = NSINT_6BYTE_VAL;
			buf[1] = p[3];
			buf[2] = p[4];
			buf[3] = p[5];
			buf[4] = p[6];
			buf[5] = p[7];
			return 6;
		} else {
			/* 64-bit */
			buf[0] = NSINT_9BYTE_VAL;
			buf[1] = p[0];
			buf[2] = p[1];
			buf[3] = p[2];
			buf[4] = p[3];
			buf[5] = p[4];
			buf[6] = p[5];
			buf[7] = p[6];
			buf[8] = p[7];
			return 9;
		}
	}
}

int NsFormat::unmarshalInt64(const xmlbyte_t *buf, uint64_t *i)
{
	int len;
	uint64_t tmp = 0;
	unsigned char *p = (unsigned char *) &tmp;
	unsigned char c = buf[0];
	if ((c & NSINT_1BIT_MASK) == 0) {
		// no swapping for one byte value
		*i = buf[0];
		return 1;
	} else if ((c & NSINT_2BIT_MASK) == NSINT_1BIT_MASK) {
		p[6] = (c & ~NSINT_2BIT_MASK);
		p[7] = buf[1];
		len = 2;
		goto out;
	} else if ((c & NSINT_3BIT_MASK) == NSINT_2BIT_MASK) {
		p[5] = (c & ~NSINT_3BIT_MASK);
		p[6] = buf[1];
		p[7] = buf[2];
		len = 3;
		goto out;
	} else if ((c & NSINT_5BIT_MASK) == NSINT_4BYTE_VAL) {
		p[4] = (c & ~NSINT_5BIT_MASK);
		p[5] = buf[1];
		p[6] = buf[2];
		p[7] = buf[3];
		len = 4;
		goto out;
	} else if ((c & NSINT_5BIT_MASK) == NSINT_5BYTE_VAL) {
		p[3] = (c & ~NSINT_5BIT_MASK);
		p[4] = buf[1];
		p[5] = buf[2];
		p[6] = buf[3];
		p[7] = buf[4];
		len = 5;
		goto out;
	} else if ((c & NSINT_8BIT_MASK) == NSINT_6BYTE_VAL) {
		p[3] = buf[1];
		p[4] = buf[2];
		p[5] = buf[3];
		p[6] = buf[4];
		p[7] = buf[5];
		len = 6;
		goto out;
	} else if ((c & NSINT_8BIT_MASK) == NSINT_9BYTE_VAL) {
		p[0] = buf[1];
		p[1] = buf[2];
		p[2] = buf[3];
		p[3] = buf[4];
		p[4] = buf[5];
		p[5] = buf[6];
		p[6] = buf[7];
		p[7] = buf[8];
		len = 9;
		goto out;
	}
 out:
	*i = tmp;
	if (!Globals::isBigendian_)
		M_64_SWAP(*i);
	return len;
}

#ifdef FORMAT_DEBUG
namespace DbXml {
void dumpNode(NsNode *node)
{
	ostringstream out;
	out << "Node: " << (const char *)node->getNameChars() << ",";
	node->getNid()->displayNid(out);
	if (!node->isDoc()) {
		out << "\n\tparent: ";
		node->getParentNid()->displayNid(out);
	} else
		out << "\n\tDocument";
	out << "\n\tlast child: ";
	if (node->getLastChildNid())
		node->getLastChildNid()->displayNid(out);
	else
		out << "null";

	out << "\n\tprev/lastDesc: ";
	if (node->getPrevNid())
		node->getPrevNid()->displayNid(out);
	else
		out << "null";
	out << "/";
	if (node->getLastDescendantNid())
		node->getLastDescendantNid()->displayNid(out);
	else
		out << "null";
	out << "\n\t";

	std::cout << out.str() << endl;
}
}
#endif

