//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryPlanHolder.hpp,v 1.22.2.2 2007/01/19 14:56:01 jsnelson Exp $
//

#ifndef __QUERYPLANHOLDER_HPP
#define	__QUERYPLANHOLDER_HPP

#include "../QueryPlan.hpp"

class XPath2MemoryManager;

namespace DbXml
{

class XmlManager;
class Transaction;
class TransactedContainer;
class DbXmlContext;
class QueryExecutionContext;
class OperationContext;
class QueryContext;

class QueryPlanHolder
{
public:
	QueryPlanHolder();
	virtual ~QueryPlanHolder() {}

	void setQueryPlanHolder(const QueryPlanHolder *o);
	void clearQueryPlanHolder();
	void intersectQueryPlan(const QueryPlanHolder *o, XPath2MemoryManager *mm);
	void intersectDocQueryPlan(const QueryPlanHolder *o, XPath2MemoryManager *mm);
	void addToQueryPlan(PathsQP *qp);

	const QueryPlan *getQueryPlan() const { return qp_; }
	void setQueryPlan(QueryPlan *qp) { qp_ = qp; }

	const TransactedContainer *getContainer() const { return container_; }
	const XMLCh *getDocumentURI() const { return documentUri_; }

	bool isQueryPlanExecutable() const { return qpIsExecutable_; }
	bool isQueryPlanExact() const { return qpIsExact_; }
	bool runQueryPlanIfDocOnly() const { return runIfDocOnly_; }

	virtual void generateQueryPlan(XmlManager &db, Transaction *txn, DbXmlContext *context);
	virtual void generateQueryPlan(TransactedContainer *container, Transaction *txn, DbXmlContext *context);

	bool isSuitableForLookupIndex() const;
	bool isSuitableForQPHCombine() const;

	bool costQueryPlan(QueryPlan::Cost &cost, DbXmlContext *context) const;
	void executeQueryPlan(IndexData::Ptr &data, OperationContext &oc,	QueryExecutionContext &qec,
		bool &exact) const;

protected:
	QueryPlan *qp_;
	TransactedContainer *container_;
	const XMLCh *documentUri_;
	bool qpIsExecutable_;
	bool qpIsExact_;
	bool runIfDocOnly_;
};

}

#endif
