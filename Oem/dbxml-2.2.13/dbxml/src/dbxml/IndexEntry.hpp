//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: IndexEntry.hpp,v 1.14 2005/12/19 16:47:37 jsnelson Exp $
//

#ifndef __INDEXENTRY_HPP
#define	__INDEXENTRY_HPP

#include <iosfwd>
#include <set>

#include "nodeStore/NsTypes.hpp"
#include "shared_ptr.hpp"
#include "IDS.hpp"

#include <xercesc/framework/MemoryManager.hpp>

class Dbt;
class DynamicContext;

namespace DbXml
{

class DbtOut;
class DbXmlNodeImpl;

class IndexEntry
{
public:
	typedef shared_ptr<IndexEntry> SharedPtr;

	/// Keep less than 1 byte
	enum Format {
		/// Has DOC_ID
		D_FORMAT = 0,
		/**
		 * Has DOC_ID, NODE_ID, LAST_CHILD_ID, and NODE_LEVEL.
		 * This is now DEPRECATED as it was meant to be like DSEL_FORMAT.
		 */
		DSEL_OLD_FORMAT = 1,
		/// Has DOC_ID, NODE_ID, LAST_DESCENDANT_ID, and NODE_LEVEL
		DSEL_FORMAT = 2,
		/**
		 * Has DOC_ID, NODE_ID, and ATTRIBUTE_INDEX
		 * This is now DEPRECATED as it was meant to be like ATTRIBUTE_FORMAT.
		 */
		ATTR_OLD_FORMAT = 3,
		/// Has DOC_ID, NODE_ID, NODE_LEVEL and ATTRIBUTE_INDEX
		ATTRIBUTE_FORMAT = 4,

		KNOWN_FORMATS = 5
	};

	enum Info {
		NODE_ID = 0,
		LAST_CHILD_ID = 1,
		NODE_LEVEL = 2,
		PARENT_ID = 3,
		LAST_DESCENDANT_ID = 4,
		ATTRIBUTE_INDEX = 5,

		INFO_MAX = 6
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
	ID getDocID() const { return docid_; }
	const nid_t &getNodeID() const;
	u_int32_t getNodeLevel() const;
	const nid_t &getLastDescendant() const { return last_descendant_; }
	uint32_t getAttrIndex() const { return attr_index_; }

	bool hasNode() { return node_ != 0; }

	void setFormat(Format format) { format_ = format; }
	void setDocID(ID docid) { docid_ = docid; }
	void setNode(const nsNode_t *node);
	void setLastDescendant(const nid_t *last_descendant);
	void setAttrIndex(uint32_t attr_index) { attr_index_ = attr_index; }

	Relationship isSelf(const DbXmlNodeImpl *node) const;
	Relationship isChildOf(const DbXmlNodeImpl *parent) const;
	Relationship isDescendantOf(const DbXmlNodeImpl *parent, bool orSelf, const DynamicContext *context) const;

	void setDbtFromThis(DbtOut &dbt) const;
	void setThisFromDbt(const Dbt &dbt);

	/// Returns the marshal size, if count is true
	int marshal(xmlbyte_t *buffer, bool count) const;

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

	void unmarshal(const Dbt &dbt);
private:
	Format format_;
	ID docid_;

	nsNode_t *node_;

	nid_t node_id_;
	u_int32_t node_level_;
	nid_t last_descendant_;
	uint32_t attr_index_;

	/// A truth table for index entry formats
	static const bool indexFormats_[KNOWN_FORMATS][INFO_MAX];
};

std::ostream& operator<<(std::ostream& s, const IndexEntry &ie);

struct IndexEntrySort
{
	bool operator()(const IndexEntry::SharedPtr &a, const IndexEntry::SharedPtr &b)
	{
		return *a < *b;
	}
};

class IndexData : public std::set<IndexEntry::SharedPtr, IndexEntrySort>
{
public:
	typedef shared_ptr<IndexData> SharedPtr;

	IndexData() {}

	void set_union(const SharedPtr &o);
	void set_intersection(const SharedPtr &o);

private:
	IndexData(const IndexData &);
	IndexData &operator=(const IndexData &);
};

class IndexReadCache
{
public:
	IndexReadCache();
	void reset();

	const IDS::SharedPtr &getIDS(const Container &container, OperationContext &context,
		SecondaryDatabase::Operation op1, const Key &k1,
		SecondaryDatabase::Operation op2 = SecondaryDatabase::NONE, const Key &k2 = Key());

	const IndexData::SharedPtr &getIndexData(const Container &container, OperationContext &context,
		SecondaryDatabase::Operation op1, const Key &k1,
		SecondaryDatabase::Operation op2 = SecondaryDatabase::NONE, const Key &k2 = Key());

	bool containsIndexData(const Container &container, SecondaryDatabase::Operation op1, const Key &k1,
			 SecondaryDatabase::Operation op2 = SecondaryDatabase::NONE, const Key &k2 = Key()) const;

private:
	struct IndexKey {
		IndexKey(const Container *c, SecondaryDatabase::Operation op1, const Key &k1, SecondaryDatabase::Operation op2, const Key &k2)
			: container(c), operation1(op1), operation2(op2), key1(k1), key2(k2) {}
		bool operator<(const IndexKey &o) const;

		const Container *container;
		SecondaryDatabase::Operation operation1, operation2;
		Key key1, key2;
	};

	typedef std::map<IndexKey, IDS::SharedPtr> IDSMap;
	typedef std::map<IndexKey, IndexData::SharedPtr> IndexMap;

	const IDS::SharedPtr &lookupIDS(OperationContext &context, const IndexKey &indexKey);
	const IndexData::SharedPtr &lookupIndexData(OperationContext &context, const IndexKey &indexKey);

	IDSMap idsMap_;
	IndexMap indexMap_;
};

}

#endif
