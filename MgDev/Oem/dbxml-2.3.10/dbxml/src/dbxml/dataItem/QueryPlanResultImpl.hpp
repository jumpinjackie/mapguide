//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryPlanResultImpl.hpp,v 1.5 2006/11/17 18:18:29 jsnelson Exp $
//

#ifndef __QUERYPLANRESULTIMPL_HPP
#define	__QUERYPLANRESULTIMPL_HPP

#include "DbXmlNodeImpl.hpp"
#include "Join.hpp"

class DynamicContext;

namespace DbXml
{

class TransactedContainer;
class QueryPlanHolder;

class QueryPlanResultImpl : public DbXmlResultImpl
{
public:
        QueryPlanResultImpl(const DbXmlResult &parent, const QueryPlanHolder *holder, const LocationInfo *location);
        QueryPlanResultImpl(const DbXmlResult &parent, const QueryPlanHolder *holder, Join::Type joinType,
		const LocationInfo *location);

        Item::Ptr next(DynamicContext *context);
	Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);

        virtual void setResult(const DbXmlNodeImpl *node, DynamicContext *context);
        std::string asString(DynamicContext *context, int indent) const;

protected:
	void nextResult(Item::Ptr &item, DynamicContext *context);

        DbXmlResult parent_;
        const QueryPlanHolder *holder_;
        Join::Type joinType_;

        TransactedContainer *container_;
        IndexData::Ptr data_;
        IndexData::const_iterator it_;
	bool qpIsExact_;

        DbXmlResult result_;
};

class SelfJoinResult : public DbXmlResultImpl
{
public:
	SelfJoinResult(const DbXmlNodeImpl *node, IndexData::const_iterator &parentIt,
		IndexData::const_iterator end);
	Item::Ptr next(DynamicContext *context);
	Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);
	std::string asString(DynamicContext *context, int indent) const;
private:
	DbXmlNodeImpl::Ptr nodeObj_;
	DocID docID_;

	IndexData::const_iterator &parentIt_;
	IndexData::const_iterator it_;
	IndexData::const_iterator end_;
};

class NullJoinResult : public DbXmlResultImpl
{
public:
	NullJoinResult() : DbXmlResultImpl(0) {}
	Item::Ptr next(DynamicContext *context) { return 0; }
	Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context)
	{ return 0; }
	std::string asString(DynamicContext *context, int indent) const 
	{
		return "nulljoinresult";
	}
};

class AttributeJoinResult : public DbXmlResultImpl
{
public:
	AttributeJoinResult(const DbXmlNodeImpl *node, IndexData::const_iterator &parentIt,
		const IndexData::Ptr &data);
	Item::Ptr next(DynamicContext *context);
	Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);
	std::string asString(DynamicContext *context, int indent) const;
private:
	DbXmlNodeImpl::Ptr nodeObj_;
	DocID docID_;

	IndexData::const_iterator &parentIt_;
	IndexData::const_iterator it_;
	IndexData::Ptr data_;
};

class ChildJoinResult : public DbXmlResultImpl
{
public:
	ChildJoinResult(const DbXmlNodeImpl *node, IndexData::const_iterator &parentIt,
		const IndexData::Ptr &data);
	Item::Ptr next(DynamicContext *context);
	Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);
	std::string asString(DynamicContext *context, int indent) const;
private:
	DbXmlNodeImpl::Ptr nodeObj_;
	DocID docID_;

	IndexData::const_iterator &parentIt_;
	IndexData::const_iterator it_;
	IndexData::Ptr data_;
};

class DescendantJoinResult : public DbXmlResultImpl
{
public:
	DescendantJoinResult(const DbXmlNodeImpl *node, IndexData::const_iterator &parentIt,
		const IndexData::Ptr &data, bool orSelf);
	Item::Ptr next(DynamicContext *context);
	Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);
	std::string asString(DynamicContext *context, int indent) const;
private:
	bool orSelf_;

	DbXmlNodeImpl::Ptr nodeObj_;
	DocID docID_;

	IndexData::const_iterator &parentIt_;
	IndexData::const_iterator it_;
	IndexData::Ptr data_;
};

}

#endif
