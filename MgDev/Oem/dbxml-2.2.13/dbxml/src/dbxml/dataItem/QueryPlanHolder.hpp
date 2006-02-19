//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: QueryPlanHolder.hpp,v 1.6 2005/11/28 13:13:22 jsnelson Exp $
//

#ifndef __QUERYPLANHOLDER_HPP
#define	__QUERYPLANHOLDER_HPP

#include "DbXmlNodeImpl.hpp"

class DynamicContext;

namespace DbXml
{

class QueryPlan;
class PathsQP;
class XmlManager;
class Transaction;
class Container;

class QueryPlanHolder
{
public:
	QueryPlanHolder();

	const QueryPlan *getQueryPlan() const { return qp_; }
	void setQueryPlan(QueryPlan *qp) { qp_ = qp; }
	void addToQueryPlan(PathsQP *qp);

	bool isQueryPlanExecutable() const { return qpIsExecutable_; }
	void setQueryPlanExecutable(bool v) { qpIsExecutable_ = v; }

	bool runQueryPlanIfDocOnly() const { return runIfDocOnly_; }
	void setRunQueryPlanIfDocOnly(bool v) { runIfDocOnly_ = v; }

	void generateQueryPlan(XmlManager &db, Transaction *txn, DynamicContext *context);
	QueryPlan *generateJITQueryPlan(Transaction *txn, const Container &container,
		DynamicContext *context) const;

protected:
	QueryPlan *qp_;
	bool qpIsExecutable_;
	bool runIfDocOnly_;
};

class QueryPlanResultImpl : public ResultImpl
{
public:
	QueryPlanResultImpl(const Result &parent, const QueryPlanHolder *holder,
		DynamicContext *context);
	Item::Ptr next(DynamicContext *context);
	void skip();
	virtual void setResult(const DbXmlNodeImpl *node, DynamicContext *context);
	std::string asString(DynamicContext *context, int indent) const;
protected:
	QueryPlanResultImpl(const Result &parent, const QueryPlanHolder *holder,
		DataItemStep::Axis joinType, DynamicContext *context);

	Result parent_;
	const QueryPlanHolder *holder_;
	DataItemStep::Axis joinType_;
	bool toDo_;

	TransactedContainer *container_;
	IndexData::SharedPtr data_;
	IndexData::const_iterator it_;

	Result result_;
};

class SelfJoinResult : public ResultImpl
{
public:
	SelfJoinResult(const DbXmlNodeImpl *node, IndexData::const_iterator &parentIt,
		IndexData::const_iterator end, DynamicContext *context);
	Item::Ptr next(DynamicContext *context);
	std::string asString(DynamicContext *context, int indent) const;
private:
	DbXmlNodeImpl::Ptr nodeObj_;
	u_int32_t docID_;

	IndexData::const_iterator &parentIt_;
	IndexData::const_iterator it_;
	IndexData::const_iterator end_;
};

class AttributeJoinResult : public ResultImpl
{
public:
	AttributeJoinResult(const DbXmlNodeImpl *node, IndexData::const_iterator &parentIt,
		IndexData::const_iterator end, DynamicContext *context);
	Item::Ptr next(DynamicContext *context);
	std::string asString(DynamicContext *context, int indent) const;
private:
	DbXmlNodeImpl::Ptr nodeObj_;
	u_int32_t docID_;

	IndexData::const_iterator &parentIt_;
	IndexData::const_iterator it_;
	IndexData::const_iterator end_;
};

class ChildJoinResult : public ResultImpl
{
public:
	ChildJoinResult(const DbXmlNodeImpl *node, IndexData::const_iterator &parentIt,
		IndexData::const_iterator end, DynamicContext *context);
	Item::Ptr next(DynamicContext *context);
	std::string asString(DynamicContext *context, int indent) const;
private:
	DbXmlNodeImpl::Ptr nodeObj_;
	u_int32_t docID_;

	IndexData::const_iterator &parentIt_;
	IndexData::const_iterator it_;
	IndexData::const_iterator end_;
};

class DescendantJoinResult : public ResultImpl
{
public:
	DescendantJoinResult(const DbXmlNodeImpl *node, IndexData::const_iterator &parentIt,
		IndexData::const_iterator end, bool orSelf, DynamicContext *context);
	Item::Ptr next(DynamicContext *context);
	std::string asString(DynamicContext *context, int indent) const;
private:
	bool orSelf_;

	DbXmlNodeImpl::Ptr nodeObj_;
	u_int32_t docID_;

	IndexData::const_iterator &parentIt_;
	IndexData::const_iterator it_;
	IndexData::const_iterator end_;
};

}

#endif
