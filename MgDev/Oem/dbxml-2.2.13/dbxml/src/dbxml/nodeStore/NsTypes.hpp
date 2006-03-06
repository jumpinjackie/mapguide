//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsTypes.hpp,v 1.29 2005/12/07 15:58:17 jsnelson Exp $
//

/*
 * NsTypes.hpp
 *
 * Core "C" structures used for node storage, along
 * with some inline accessors.
 */

#ifndef __DBXMLNSTYPES_HPP
#define __DBXMLNSTYPES_HPP

#include <dbxml/DbXmlFwd.hpp>

// Put all Xerces forward decls in one place (TBD: find better place)
namespace XERCESC {
	class MemoryManager;
};
#define XER_NS XERCESC::

/*
 * Atomic types for node storage.  Size matters.
 */
#include <db_cxx.h>
#define uint8_t u_int8_t
#define uint16_t u_int16_t
#define uint32_t u_int32_t

extern "C" {
	
/*
 * xmlch_t is typedef'd to XMLCh in NsUtil.hpp
 * This is to handle the fact that sometimes it's
 * unsigned short, and sometimes a wchar_t (windows)
 */
typedef unsigned char xmlbyte_t; /* UTF-8 character */
#define NID_BYTES_SIZE 8
typedef struct {
	union idStore_u {
		xmlbyte_t *idPtr;
		xmlbyte_t idBytes[NID_BYTES_SIZE];
	} idStore;
	uint32_t idLen;
} nid_t;

/*
 * NSDOC_FLAGS -- flags for the document itself
 */
enum NSDOC_FLAGS {
	NSDOC_STANDYES =  0x0001,
	NSDOC_STANDNO =   0x0002,
	NSDOC_HASENCODE = 0x0004,
	NSDOC_NAMESPACE = 0x0008, /**< do namespaces */
	NSDOC_HASNSNODE = 0x0010, /**< has namespace node */
	NSDOC_HASDECL   = 0x0020, /**< has xml decl */
	NSDOC_HASSNIFF  = 0x0040  /**< has sniffed encoding */
};
	
/*
 * Transient Node Format.
 *
 * The structure, nsNode_t, is used to hold element nodes, plus
 * related attributes and (child) text node information.  It is used
 * at the time of document creation (parsing) and for materialization of
 * parts of documents stored as nodes.  In addition, it is used for
 * transient DOM parsing.
 *
 * Character encoding issues:
 *  An instance of nsNode_t will either use UTF-8 or UTF-16 strings.
 *  At document creation/parse time, UTF-8 is used.  The only
 *  time UTF-16 is stored in nsNode_t structures is a purely
 *  transient DOM parse.
 *
 *  When node-storage documents are materialized, text is still
 *  UTF-8, and is transcoded to UTF-16 on demand, and the new
 *  strings are stored in the containing objects (not the nsNode_t).
 *
 * DOM note:
 *   A single nsNode_t structure is shared among all of the "DOM"
 *   nodes that require its data (one element, N attributes, N text nodes).
 *
 * The structure is split into a  mandatory header, followed
 * by a number of optional fields.  Which optional fields are present
 * is determined by the flags in the header.  When present, this is
 * the order of structures.  The order is based on an assumed
 * frequency of presence.  The penalty for being further down
 * the list is the need to navigate past the earlier fields:
 *   nsHeader_t -- mandatory
 *   nsChildList_t -- if (HASCHILD)
 *   nsAttrList_t -- if (HASATTR)
 *   nsTextList_t -- if (HASTEXT)
 */

/**
 * state flags -
 *
 * Most used bits should be the lowest in value, to aid the
 * compression of the flags int
 */
enum NS_FLAGS {
	NS_HASCHILD =   0x00000001, /**< has at least one child element */
	NS_HASATTR =    0x00000002, /**< has attributes */
	NS_HASTEXT =    0x00000004, /**< has text nodes */
	NS_NAMEPREFIX = 0x00000008, /**< name has a prefix */
	NS_HASURI =     0x00000010, /**< has a namespace URI */
	NS_HASNSINFO =  0x00000020, /**< has namespace decls in attrs */
	NS_NAMETYPE =   0x00000040, /**< element has a type */
	NS_UNIONTYPE =  0x00000080, /**< element has a type */
	NS_ISDOCUMENT = 0x00000100, /**< is the document node */
	NS_ISROOT =     0x00000200, /**< is the root node */

	// Flags that aren't put into the database

	NS_ALLOCATED =  0x20000000, /**< each part is allocated separatly (rather than in one large chunk) */
	NS_STANDALONE = 0x40000000, /**< is a DOM parse, or standalone node (not persistent at all) */
	NS_UTF16 =      0x80000000  /**< text is in UTF-16 vs UTF-8 */
};
#define NS_DBFLAGMASK (~(NS_ALLOCATED|NS_STANDALONE|NS_UTF16)) ///< flags that aren't put into the database

/* attribute-specific flags; keep under 7 bits */
enum NS_ATTR_FLAGS {
	NS_ATTR_PREFIX =      0x0001, /**< has a namespace prefix */
	NS_ATTR_TYPE   =      0x0002, /**< has a type */
	NS_ATTR_UNION_TYPE  = 0x0004, /**< has a union type as well */
	NS_ATTR_ENT    =      0x0008, /**< has entities for escaping */
	NS_ATTR_URI    =      0x0010, /**< has a namespace uri */
	NS_ATTR_NOT_SPECIFIED=0x0020  /**< NOT specified; default is specified */
};

/**
 * text type is 2 parts: a type (0-7), and flags (bit flags starting
 * at 0x8.  These values are persisted, and the format is intended
 * to keep the size to 7 bits.
 */
enum NS_TEXTTYPES {
	NS_TEXT =       0,
	NS_COMMENT =    1,  /**< text is comment */
	NS_CDATA =      2,  /**< text is CDATA */
	NS_PINST =      3,  /**< text is processing instruction */
	NS_SUBSET =     4,  /**< text is doc type decl */
	NS_ENTSTART =   5,  /**< text is an entity name -- start */
	NS_ENTEND =     6   /**< text is an entity name -- end */
};

enum NS_TEXTFLAGS {
	NS_ENTITY_CHK = 0x08,   /**< look for entities in serialize */
	NS_IGNORABLE  = 0x10    /**< ignorable whitespace */
};

#define NS_TEXTMASK 0x7	
#define NS_NOPREFIX (-1) // no namespace prefix
#define NS_NOURI (-1) // no uri
#define NS_NOTYPE (-1)   // no type information available
#define NS_NOTEXT (-1)	
	
/**
 * nsText_t -- holds text, such as names, attributes, text
 *
 * In memory, all strings are either UTF-16 (xmlch_t) or
 * UTF-8 (xmlbyte_t), and all are NULL terminated.
 *
 * Length (t_len) in this structure is a "character" count,
 * not including the trailing NULL (i.e. the
 * same as strlen() results).
 *
 * Characters that are encoded in more than one xmlch_t or
 * xmlbyte_t are counted as more than one (e.g. 3-byte utf-8
 * character counts as 3).  In the case of UTF-8, t_len
 * is equal to byte count.  For UTF-16, tlen is 1/2 of byte
 * count.
 *
 * Rules:
 *   o The string and t_len are in UTF-8 bytes except:
 *     -- when the parse is for DOM (NS_DOMPARSE is set)
 *     -- in this case, the NS_UTF16 flag is set in the containing
 *     nsNode_t structure's header.
 */
typedef struct {
	uint32_t  t_len;     /**< num characters, not including NULL */
        void  *t_chars;      /**< pointer to string, NULL-terminated */
} nsText_t;

/**
 * nsName_t -- name, which may be prefixed, and may be typed.
 *
 * An element or attribute name, which may have
 * a namespace prefix.  n_prefix is NS_NOPREFIX (-1)
 * if no prefix present.  n_type is NS_NOTYPE (-1) if
 * no type information is available.
 * XML Schema type information
 * (type name, associated type URI) is kept in the same
 * data structure as the namespace prefix mappings.
 *
 * The localname is stored in n_text.
 */
typedef struct {
	int32_t n_prefix;
	int32_t n_type;   /**< type id */
	int32_t n_utype;  /**< union type id if union */
	nsText_t n_text;
} nsName_t;
	
/**
 * nsHeader_t -- mandatory header
 *  Includes Node id, flags, parent id, and naming information
 */
typedef struct {
	nid_t       nh_id;
	uint32_t    nh_flags;
	nid_t       nh_parent;
	int32_t     nh_uriIndex; /**< uri index, 0 if default */
	nsName_t    nh_name;     /**< node name */
} nsHeader_t;

/**
 * nsChildEntry_t -- a single child entry
 */
typedef struct {
	uint32_t ce_textIndex;
	nid_t    ce_id;
} nsChildEntry_t;

/**
 * nsChildList_t -- present if the node has element children.
 */
typedef struct {
	uint32_t        cl_numChild; /**< number of children */
	uint32_t        cl_maxChild; /**< num entries that can be put in child text index list */
	nsChildEntry_t  cl_child[1]; /**< the text indexes and ids for the children */
} nsChildList_t;

/**
 * nsAttr_t -- stores one attribute/value pair.
 *
 * The nsAttr_t structure is used to store an attribute.
 * the a_name field is nsName_t, which stores *both*
 * the name and value in the form:
 *   attrName0attrValue0
 * The a_name.n_text.t_chars field references the name,
 * the a_value pointer points directly to the value.
 *
 * This scheme makes materialization faster, as well as simplifying memory
 * management.
 *
 * The a_len field of each nsAttr_t is the length
 * in characters of the entire name/value string,
 * minus the trailing NULL.
 * If the name has a namespace prefix, that information is
 * stored in the nsName_t n_prefix field.  If there is any
 * XML schema information available for this attribute, it is
 * kept in the nsName_t n_type field.
 */
typedef struct {
	nsName_t a_name;
	const void *a_value; ///< void to handle both xmlch_t and xmlbyte_t
	uint32_t a_flags; /**< attribute flags */
	int32_t a_uri;    /**< attribute uri */
} nsAttr_t;
	
// short cuts
#define a_len a_name.n_text.t_len
#define a_prefix a_name.n_prefix
#define a_type a_name.n_type
#define a_utype a_name.n_utype
#define a_name8 a_name.n_text.t_chars
	
/**
 * nsAttrList_t -- present if a node has attributes.
 */
typedef struct {
	/**
	 * a_len is total length, in (UTF-16|UTF-8) characters, of all
	 * name/value strings, including the NULLs.  Used to determine space
	 * needed for persistent storage.
	 */
	uint32_t  al_len;
	uint32_t  al_nattrs;   /**< number of attributes */
	uint32_t  al_max;   /**< num entries that can be put in attr list */
	nsAttr_t  al_attrs[1]; /**< the attributes */
} nsAttrList_t;

/**
 * nsTextEntry_t -- a single, typed text entry
 */
typedef struct {
	uint32_t  te_type;  /**< type of text */
	nsText_t  te_text;  /**< the text */
} nsTextEntry_t;
	
/**
 * nsTextList_t -- present if node has any text children
 *
 * When it is being created from a parse, the nsTextEntry_t
 * list may need to be reallocated, as new text nodes show up.
 * Start with room for 2.  Double on every realloc.
 */
#define NS_NTEXT_DEFAULT 2
	
typedef struct {
	/**
	 * total length, in characters, of all text strings,
	 * used to determine persistent storage size.  Includes NULLs.
	 */
	uint32_t  tl_len;
	uint32_t  tl_ntext;   /**< number of text nodes */
	uint32_t  tl_max;   /**< num entries that can be put in entry list */
	nsTextEntry_t tl_text[1];  /**< the nodes */
} nsTextList_t;
	
/**
 * nsNode_t -- the node itself
 *
 * Transient node.  Includes a header and pointers
 * to all optional fields. Only fields will be non-null.
 * This structure is used for both parsing and persistent
 * DOM materialization.
 *
 * During the parse, the optional structures are allocated
 * on demand.  When materialized from a DB record, the
 * presence and size of optional fields are known, and
 * can be allocated in a single, large chunk of memory,
 * carved up during initialization (unmarshaling).
 *
 * The structure includes a transient parent link that is
 * used only during initial parsing.
 */
typedef struct nsNode_s {
	nsHeader_t     nd_header;
	nsChildList_t *nd_child;
	nsAttrList_t  *nd_attrs;
	nsTextList_t  *nd_text;
	uint32_t       nd_level;
	/// Kept for memory management puposes
	unsigned char *nd_memory;
	/// Plumbing required for parsing
	struct nsNode_s *nd_parent;
} nsNode_t;

/*
 * Convenience accessors
 */
inline const bool nsIsAllocated(const nsNode_t *node)
{
	if (node->nd_header.nh_flags & NS_ALLOCATED)
		return true;
	return false;
}
	
inline const bool nsIsStandalone(const nsNode_t *node)
{
	if (node->nd_header.nh_flags & NS_STANDALONE)
		return true;
	return false;
}
	
inline const nid_t *nsGetNid(const nsNode_t *node)
{
	return (&(node->nd_header.nh_id));
}

inline const nid_t *nsGetParentNid(const nsNode_t *node)
{
	return ((node->nd_header.nh_flags & NS_STANDALONE) ? 
		0 : &(node->nd_header.nh_parent));
}

inline const bool nsIsDoc(const nsNode_t *node)
{
	if (node->nd_header.nh_flags & NS_ISDOCUMENT)
		return true;
	return false;
}

inline const bool nsIsRoot(const nsNode_t *node)
{
	if (node->nd_header.nh_flags & NS_ISROOT)
		return true;
	return false;
}

inline const bool nsHasChildElem(const nsNode_t *node)
{
	if (node->nd_header.nh_flags & NS_HASCHILD)
		return true;
	return false;
}

inline const uint32_t nsNumChildElem(const nsNode_t *node)
{
	if (nsHasChildElem(node))
		return (node->nd_child->cl_numChild);
	return 0;
}

inline const bool nsHasText(const nsNode_t *node)
{
	if (node->nd_header.nh_flags & NS_HASTEXT)
		return true;
	return false;
}

inline const int nsNumText(const nsNode_t *node)
{
	if (node->nd_header.nh_flags & NS_HASTEXT)
		return (node->nd_text->tl_ntext);
	return 0;
}

inline const uint32_t nsTextType(uint32_t type)
{
	return (type & NS_TEXTMASK);
}

inline const bool nsTextEntityChk(uint32_t type)
{
	if (type & NS_ENTITY_CHK)
		return true;
	return false;
}

inline const nsChildEntry_t *nsChild(const nsNode_t *node, uint32_t index)
{
	if(nsHasChildElem(node)) {
		return &node->nd_child->cl_child[index];
	}
	return 0;
}

inline const uint32_t nsChildTextIndex(const nsNode_t *node, uint32_t index)
{
	if (nsHasChildElem(node)) {
		if(index < node->nd_child->cl_numChild)
			return nsChild(node, index)->ce_textIndex;
		else return nsNumText(node);
	}
	return 0;
}

inline const nid_t *nsChildNid(const nsNode_t *node, uint32_t index)
{
	if (nsHasChildElem(node)) {
		if(index < node->nd_child->cl_numChild)
			return &nsChild(node, index)->ce_id;
	}
	return 0;
}

inline const nid_t *nsLastChildNid(const nsNode_t *node)
{
	if (nsHasChildElem(node)) {
		return &nsChild(node, node->nd_child->cl_numChild - 1)->ce_id;
	}
	return 0;
}

inline const uint32_t nsNumChild(const nsNode_t *node)
{
	uint32_t ret = nsNumChildElem(node);
	if (nsHasText(node))
		ret += nsNumText(node);
	return ret;
}

/// checks for both element and text children
inline const bool nsHasChildNode(const nsNode_t *node)
{
	if (node->nd_header.nh_flags & (NS_HASCHILD|NS_HASTEXT))
		return true;
	return false;
}

/// checks for attrs
inline bool nsHasAttrs(const nsNode_t *node)
{
	if (node->nd_header.nh_flags & NS_HASATTR)
		return true;
	return false;
}

/// checks for attrs
inline const uint32_t nsNumAttrs(const nsNode_t *node)
{
	if (nsHasAttrs(node))
		return (node->nd_attrs->al_nattrs);
	return 0;
}

inline const bool nsHasNamePrefix(const nsNode_t *node)
{
	if (node->nd_header.nh_flags & NS_NAMEPREFIX)
		return true;
	return false;
}

inline const int32_t nsNamePrefix(const nsNode_t *node)
{
	return (node->nd_header.nh_name.n_prefix);
}

inline void nsSetNamePrefix(nsNode_t *node, int32_t prefix)
{
	node->nd_header.nh_name.n_prefix = prefix;
}

inline const int32_t nsTypeIndex(const nsNode_t *node)
{
	return (node->nd_header.nh_name.n_type);
}

inline const bool nsHasUri(const nsNode_t *node)
{
	if (node->nd_header.nh_flags & NS_HASURI)
		return true;
	return false;
}

inline const uint32_t nsUriIndex(const nsNode_t *node)
{
	return (node->nd_header.nh_uriIndex);
}

inline void nsSetUriIndex(nsNode_t *node, uint32_t uri)
{
	node->nd_header.nh_uriIndex = uri;
}

inline const bool nsIsUTF16(const nsNode_t *node)
{
	if (node->nd_header.nh_flags & NS_UTF16)
		return true;
	return false;
}

inline const nsName_t *nsName(const nsNode_t *node)
{
	return  &(node->nd_header.nh_name);
}

inline nsText_t *nsText(const nsNode_t *node, uint32_t index)
{
	return  &(node->nd_text->tl_text[index].te_text);
}

inline uint32_t nsTextGetType(const nsNode_t *node, uint32_t index)
{
	return  node->nd_text->tl_text[index].te_type;
}

inline nsTextEntry_t * nsTextEntry(const nsNode_t *node, uint32_t index)
{
	return  &(node->nd_text->tl_text[index]);
}

inline nsAttr_t *nsAttr(const nsNode_t *node, uint32_t index)
{
	return  &(node->nd_attrs->al_attrs[index]);
}

inline nsName_t *nsAttrName(const nsNode_t *node, uint32_t index)
{
	return  &(nsAttr(node, index)->a_name);
}

inline uint32_t nsAttrFlags(const nsNode_t *node, uint32_t index)
{
	return  (node->nd_attrs->al_attrs[index].a_flags);
}

inline int32_t nsAttrNamePrefix(const nsNode_t *node, uint32_t index)
{
	return nsAttrName(node, index)->n_prefix;
}

inline int32_t nsAttrUri(const nsNode_t *node, uint32_t index)
{
	nsAttr_t *attr = nsAttr(node, index);
	if (attr->a_flags & NS_ATTR_URI)
		return attr->a_uri;
	return NS_NOURI;
}

inline bool nsAttrHasUri(const nsNode_t *node, uint32_t index)
{
	if (nsAttrFlags(node, index) | NS_ATTR_URI)
		return true;
	return false;
}

inline void nsSetAttrPrefix(nsAttr_t *attr, int32_t prefix)
{
	attr->a_name.n_prefix = prefix;
	if (prefix != NS_NOPREFIX)
		attr->a_flags |= NS_ATTR_PREFIX;
}

inline void nsSetAttrNamePrefix(nsNode_t *node, uint32_t index, int32_t prefix)
{
	nsSetAttrPrefix(nsAttr(node, index), prefix);
}

inline void nsSetAttrUri(nsAttr_t *attr, int32_t uri)
{
	attr->a_uri = uri;
	if (uri != NS_NOURI)
		attr->a_flags |= NS_ATTR_URI;
}

inline bool nsIsEntityText(uint32_t type)
{
	return  ((nsTextType(type) == NS_ENTSTART) ||
		 (nsTextType(type) == NS_ENTEND));
}

inline bool nsIsText(uint32_t type)
{
	return  (nsTextType(type) == NS_TEXT);
}

inline bool nsIsCDATA(uint32_t type)
{
	return  (nsTextType(type) == NS_CDATA);
}

inline bool nsIsComment(uint32_t type)
{
	return  (nsTextType(type) == NS_COMMENT);
}

inline bool nsIsPI(uint32_t type)
{
	return  (nsTextType(type) == NS_PINST);
}

inline bool nsIsSubset(uint32_t type)
{
	return  (nsTextType(type) == NS_SUBSET);
}

inline bool nsIsEntityStart(uint32_t type)
{
	return  (nsTextType(type) == NS_ENTSTART);
}

inline bool nsIsEntityEnd(uint32_t type)
{
	return  (nsTextType(type) == NS_ENTEND);
}

inline uint32_t nsGetLevel(const nsNode_t *node)
{
	return  (node->nd_level);
}

inline const bool nsHasNsinfo(const nsNode_t *node)
{
	if (node->nd_header.nh_flags & NS_HASNSINFO)
		return true;
	return false;
}

}; /* end of extern "C" */

#endif
