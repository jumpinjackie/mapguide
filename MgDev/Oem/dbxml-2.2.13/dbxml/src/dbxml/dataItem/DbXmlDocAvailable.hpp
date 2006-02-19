//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlDocAvailable.hpp,v 1.1 2005/08/16 11:56:31 jsnelson Exp $
//

#ifndef __DBXMLDOCAVAILABLE_HPP
#define	__DBXMLDOCAVAILABLE_HPP

#include <pathan/dataItem/DataItemFunction.hpp>

namespace DbXml
{

class TransactedContainer;
class QueryPlan;
class ReferenceMinder;
class XmlManager;
class Transaction;

class DbXmlDocAvailable : public DataItemFunction
{
public:
	static const XMLCh name[];

	DbXmlDocAvailable(ReferenceMinder &minder, const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

	virtual DataItem* staticResolution(StaticContext* context);
	virtual Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

	void generateQueryPlan(XmlManager &mgr, Transaction *txn, DynamicContext *context);

	const TransactedContainer *getContainer() const { return container_; }

	const QueryPlan *getQueryPlan() const {
		return qp_;
	}

private:
	const XMLCh *getUriArg(DynamicContext *context) const;

	QueryPlan *qp_;
	bool invalid_;

	TransactedContainer *container_;
	ReferenceMinder &minder_;
};

}

#endif
