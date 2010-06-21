//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsIterator.hpp,v 1.5 2006/11/17 18:18:29 jsnelson Exp $
//

#ifndef __NSITERATOR_HPP
#define	__NSITERATOR_HPP

#include "NsNid.hpp"
#include "../IndexEntry.hpp"

namespace DbXml
{

/**
 * Down-to-the-metal result item that closely
 * follows the on-disk format.
 */
class NsItem
{
public:
	enum Type {
		ELEMENT,
		ATTRIBUTE
	};

	NsItem() {}

	bool isNull() const { return !ie_; }
	Type getType() const {
		if(ie_->isSpecified(IndexEntry::ATTRIBUTE_INDEX))
			return ATTRIBUTE;
		return ELEMENT;
	}
	// TBD some kind of container ref - jpcs
	DocID getDocID() const { return ie_->getDocID(); }
	const NsNid &getNodeID() const { return ie_->getNodeID(); }
	const NsNid &getLastDescendant() const { return ie_->getLastDescendant(); }
	u_int32_t getNodeLevel() const { return ie_->getNodeLevel(); }
	u_int32_t getAttrIndex() const { return ie_->getIndex(); }

	void clear() { ie_ = 0; }
	void setDbtFromThis(DbtOut &dbt) const { ie_->setDbtFromThis(dbt); }
	void setThisFromDbt(const Dbt &dbt)
	{
		ie_ = new IndexEntry();
		ie_->setThisFromDbt(dbt);
	}

	enum Relationship {
		REL_TOO_SMALL, ///< False as it's too small
		REL_TOO_BIG, ///< False as it's too big
		REL_TRUE ///< The relationship holds
	};

	Relationship isAncestorOf(const NsItem &d) const;

	const IndexEntry::Ptr &getIndexEntry() const { return ie_; }

private:
	IndexEntry::Ptr ie_;
};

/**
 * Lazy runtime algebra execution
 */
class NsIterator
{
public:
	virtual ~NsIterator() {}

	virtual bool next(NsItem &result) = 0;
	virtual bool seek(DocID did, const NsNid &nid, NsItem &result) = 0;
	virtual void reset() = 0;

protected:
	NsIterator() {}

private:
	NsIterator(const NsIterator &);
	NsIterator &operator=(const NsIterator&);
};

/**
 * 
 */
class NsStructuralJoin
{
public:
	NsStructuralJoin(NsIterator *ancestors, NsIterator *descendants);
	virtual ~NsStructuralJoin();

	virtual bool next(NsItem &result);
	virtual bool seek(DocID did, const NsNid &nid, NsItem &result);
	virtual void reset();

private:
	bool doJoin(NsItem &descendant, NsItem &result);

	NsIterator *ancestors_;
	NsIterator *descendants_;

	NsItem ancestor_;

	enum { INIT, RUNNING, DONE } state_;
};

/**
 *
 */
class NsAlgebra
{
public:
	enum Type {
		FOO
	};

	virtual ~NsAlgebra() {}

	virtual NsIterator *execute() const = 0;

	Type getType() const { return type_; }
	const NsAlgebra *getParent() const { return parent_; }

protected:
	NsAlgebra(Type type, NsAlgebra *parent)
		: type_(type), parent_(parent) {}

	Type type_;
	NsAlgebra *parent_;
};

}

#endif
