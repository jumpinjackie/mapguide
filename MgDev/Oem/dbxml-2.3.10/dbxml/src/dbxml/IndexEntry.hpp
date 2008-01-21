//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: IndexEntry.hpp,v 1.30 2006/11/17 18:18:25 jsnelson Exp $
//

#ifndef __INDEXENTRY_HPP
#define	__INDEXENTRY_HPP

#include <iosfwd>
#include <set>

#include "nodeStore/NsTypes.hpp"
#include "nodeStore/NsNid.hpp"
#include "SharedPtr.hpp"
#include "IDS.hpp"

#include <xercesc/framework/MemoryManager.hpp>

class Dbt;
class DynamicContext;

namespace DbXml
{

class DbtOut;
class DbXmlNodeImpl;
class Document;
class NsNode;
	
class IndexEntry
{
public:
	typedef SharedPtr<IndexEntry> Ptr;

	/// Keep less than 1 byte
	enum Format {
		/// Has DOC_ID
		D_FORMAT = 0,
		NH_DOCUMENT_FORMAT = 0,
		/**
		 * Has DOC_ID, NODE_ID, LAST_CHILD_ID, and NODE_LEVEL.
		 * This is now DEPRECATED as it was meant to be like DSEL_FORMAT.
		 */
		DSEL_OLD_FORMAT = 1,
		/// Has DOC_ID, NODE_ID, LAST_DESCENDANT_ID, and NODE_LEVEL
		DSEL_FORMAT = 2,
		/// Used by node handle. Has DOC_ID, NODE_ID, and ATTRIBUTE_INDEX
		NH_ATTRIBUTE_FORMAT = 3,
		ATTR_OLD_FORMAT = 3,
		/**
		 * Has DOC_ID, NODE_ID, NODE_LEVEL and ATTRIBUTE_INDEX
		 * This is now DEPRECATED as it was meant to be like ATTRIBUTE_FORMAT.
		 */
		ATTR_OLD_FORMAT2 = 4,
		/// Has DOC_ID, NODE_ID, LAST_DESCENDANT_ID, NODE_LEVEL and ATTRIBUTE_INDEX
		ATTRIBUTE_FORMAT = 5,

		/// Used by node handle and for marshaling a key to look up. Has DOC_ID and NODE_ID
		NH_ELEMENT_FORMAT = 6,
		LOOKUP_FORMAT = 6,

		/// Used by node handle. Has DOC_ID, NODE_ID and TEXT_INDEX
		NH_TEXT_FORMAT = 7,
		/// Used by node handle. Has DOC_ID, NODE_ID and COMMENT_INDEX
		NH_COMMENT_FORMAT = 8,
		/// Used by node handle. Has DOC_ID, NODE_ID and PI_INDEX
		NH_PI_FORMAT = 9,

		KNOWN_FORMATS = 10
	};

	enum Info {
		NODE_ID = 0,
		LAST_CHILD_ID = 1,
		NODE_LEVEL = 2,
		PARENT_ID = 3,
		LAST_DESCENDANT_ID = 4,
		ATTRIBUTE_INDEX = 5,
		TEXT_INDEX = 6,
		COMMENT_INDEX = 7,
		PI_INDEX = 8,

		INFO_MAX = 9
	};

	enum Relationship {
		REL_TOO_SMALL, ///< False as it's too small
		REL_TOO_BIG, ///< False as it's too big
		REL_FALSE, ///< False, unknown reason
		REL_TRUE ///< The relationship holds
	};

	IndexEntry();
	~IndexEntry();

	void reset();

	Format getFormat() const { return format_; }
	const DocID &getDocID() const { return docid_; }
	const NsNid &getNodeID() const;
	u_int32_t getNodeLevel() const;
	const NsNid &getLastDescendant() const { return last_descendant_; }
	uint32_t getIndex() const { return index_; }

	bool hasNode() { return node_ != 0; }

	void setFormat(Format format) { format_ = format; }
	void setDocID(const DocID &docid) { docid_ = docid; }
	void setNodeID(const NsNid *nid);
	void setNode(const NsNode *node);
	void setLastDescendant(const NsNid *last_descendant);
	void setIndex(uint32_t index) { index_ = index; }

	Relationship isSelf(const DbXmlNodeImpl *node) const;
	Relationship isChildOf(const DbXmlNodeImpl *parent) const;
	Relationship isDescendantOf(const DbXmlNodeImpl *parent, bool orSelf, const DynamicContext *context) const;

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *fetchNode(const Document *document) const;

	std::string getNodeHandle() const;
	void setFromNodeHandle(const std::string &str);

	void setDbtFromThis(DbtOut &dbt) const;
	void setThisFromDbt(const Dbt &dbt);

	/// Returns the marshal size, if count is true
	int marshal(xmlbyte_t *buffer, bool count) const;
	int unmarshal(const xmlbyte_t *ptr);

	bool isSpecified(Info info) const {
		return indexFormats_[format_][info];
	}
	static bool isSpecified(Format format, Info info) {
		return indexFormats_[format][info];
	}

	/// for sorting
	bool operator<(const IndexEntry &o) const;
private:
	IndexEntry(const IndexEntry &);
	IndexEntry &operator=(const IndexEntry &);

protected:
	Format format_;
	DocID docid_;

	NsNode *node_;

	NsNid node_id_;
	u_int32_t node_level_;
	NsNid last_descendant_;
	uint32_t index_;

	/// A truth table for index entry formats
	static const bool indexFormats_[KNOWN_FORMATS][INFO_MAX];
};

std::ostream& operator<<(std::ostream& s, const IndexEntry &ie);

struct IndexEntrySort
{
	bool operator()(const IndexEntry::Ptr &a, const IndexEntry::Ptr &b)
	{
		return *a < *b;
	}
};

class IndexData : public std::set<IndexEntry::Ptr, IndexEntrySort>
{
public:
	typedef SharedPtr<IndexData> Ptr;

	void set_union(const Ptr &o);
	void set_intersection(const Ptr &o);
	void set_intersection(const IDS::Ptr &o);

private:
	IndexData &operator=(const IndexData &);
};

class IndexReadCache
{
public:
	IndexReadCache();
	void reset();

	const IDS::Ptr &getIDS(const Container &container, OperationContext &context,
		SecondaryDatabase::Operation op1, const Key &k1,
		SecondaryDatabase::Operation op2 = SecondaryDatabase::NONE, const Key &k2 = Key(0));

	const IndexData::Ptr &getIndexData(const Container &container, OperationContext &context,
		SecondaryDatabase::Operation op1, const Key &k1,
		SecondaryDatabase::Operation op2 = SecondaryDatabase::NONE, const Key &k2 = Key(0));

	bool containsIndexData(const Container &container, SecondaryDatabase::Operation op1, const Key &k1,
			 SecondaryDatabase::Operation op2 = SecondaryDatabase::NONE, const Key &k2 = Key(0)) const;

private:
	struct IndexKey {
		IndexKey(const Container *c, SecondaryDatabase::Operation op1, const Key &k1, SecondaryDatabase::Operation op2, const Key &k2)
			: container(c), operation1(op1), operation2(op2), key1(k1), key2(k2) {}
		bool operator<(const IndexKey &o) const;

		const Container *container;
		SecondaryDatabase::Operation operation1, operation2;
		Key key1, key2;
	};

	typedef std::map<IndexKey, IDS::Ptr> IDSMap;
	typedef std::map<IndexKey, IndexData::Ptr> IndexMap;

	const IDS::Ptr &lookupIDS(OperationContext &context, const IndexKey &indexKey);
	const IndexData::Ptr &lookupIndexData(OperationContext &context, const IndexKey &indexKey);

	IDSMap idsMap_;
	IndexMap indexMap_;
};

}

#endif
