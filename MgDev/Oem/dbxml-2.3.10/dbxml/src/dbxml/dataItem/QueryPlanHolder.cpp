//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryPlanHolder.cpp,v 1.34.2.2 2007/01/19 14:56:01 jsnelson Exp $
//

#include "QueryPlanHolder.hpp"
#include "QueryPlanFunction.hpp"
#include "DbXmlContext.hpp"
#include "DbXmlFactoryImpl.hpp"
#include "../Manager.hpp"
#include "../QueryPlan.hpp"
#include "../Document.hpp"
#include "../QueryExecutionContext.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/TypeErrorException.hpp>

using namespace DbXml;
using namespace std;

QueryPlanHolder::QueryPlanHolder()
	: qp_(0),
	  container_(0),
	  documentUri_(0),
	  qpIsExecutable_(false),
	  qpIsExact_(false),
	  runIfDocOnly_(false)
{
}

void QueryPlanHolder::setQueryPlanHolder(const QueryPlanHolder *o)
{
	qp_ = o->qp_;
	container_ = o->container_;
	documentUri_ = o->documentUri_;
	qpIsExecutable_ = o->qpIsExecutable_;
	qpIsExact_ = o->qpIsExact_;
	runIfDocOnly_ = o->runIfDocOnly_;
}

void QueryPlanHolder::clearQueryPlanHolder()
{
	qp_ = 0;
	container_ = 0;
	documentUri_ = 0;
	qpIsExecutable_ = false;
	qpIsExact_ = false;
	runIfDocOnly_ = false;
}

void QueryPlanHolder::intersectQueryPlan(const QueryPlanHolder *o, XPath2MemoryManager *mm)
{
	DBXML_ASSERT(container_ == 0 || o->container_ == 0 || container_ == o->container_);

	qp_ = new (mm) IntersectQP(qp_, o->qp_, mm);
	qp_ = qp_->removeSubsets(*container_);

	qpIsExact_ = qpIsExact_ && o->qpIsExact_;
	qpIsExecutable_ = qpIsExecutable_ && o->qpIsExecutable_;
	runIfDocOnly_ = runIfDocOnly_ && o->runIfDocOnly_;
}

void QueryPlanHolder::intersectDocQueryPlan(const QueryPlanHolder *o, XPath2MemoryManager *mm)
{
	DBXML_ASSERT(container_ == 0 || o->container_ == 0 || container_ == o->container_);

	QueryPlan *oqp = o->getQueryPlan()->copy(mm);
	oqp->removeNonConstant();
	// TBD resolveIndexes - jpcs

	qp_ = new (mm) IntersectDocsQP(oqp, qp_, mm);
	qp_ = qp_->removeSubsets(*container_);

	qpIsExecutable_ = qpIsExecutable_ && o->qpIsExecutable_;
	qpIsExact_ = qpIsExact_ && o->qpIsExact_;
	runIfDocOnly_ = runIfDocOnly_ && o->runIfDocOnly_;
}

void QueryPlanHolder::addToQueryPlan(PathsQP *qp)
{
        if(qp_ != 0) {
                DBXML_ASSERT(qp_->getType() == QueryPlan::PATHS);
                ((PathsQP*)qp_)->addPaths(qp->getPaths());
        }
        else {
                qp_ = qp;
        }
}

void QueryPlanHolder::generateQueryPlan(XmlManager &mgr, Transaction *txn, DbXmlContext *context)
{
	if(qp_ == 0) return;

	QPHSet qphset;
	qp_->findQueryPlanHolders(qphset);

	for(QPHSet::iterator it = qphset.begin(); it != qphset.end(); ++it) {
		if(*it == 0) {
			container_ = 0;
			documentUri_ = 0;
			break;
		}
		else if(container_ == 0) {
			container_ = const_cast<TransactedContainer*>((*it)->getContainer());
			documentUri_ = (*it)->getDocumentURI();
		}
		else if(container_ != (*it)->getContainer()) {
			// Two different containers, so we can't work
			// out the indexes to use until runtime.
			container_ = 0;
			documentUri_ = 0;
			break;
		}
	}

	if(container_ == 0) {
		// We don't know what container the query references, so all
		// we can do is generate a partially optimised query plan.
		qp_ = qp_->createRawQueryPlan((Manager&)mgr, context);
	}
	else {
		qp_ = qp_->createPartiallyOptimisedQueryPlan(
			txn, *container_, context, /*nodeQP*/true,
			/*fullyOptimised*/qpIsExecutable_,
			/*exact*/qpIsExact_);
	}

	if(qp_->getType() == QueryPlan::UNIVERSE) {
		// We don't want to execute a UniverseQP with node results
		qp_ = 0;
	}
	else {
		runIfDocOnly_ = false;
		for(QPHSet::iterator it = qphset.begin(); it != qphset.end(); ++it) {
			if((*it)->getQueryPlan() != 0 && !(*it)->getQueryPlan()->isSubsetOf(qp_)) {
				runIfDocOnly_ = true;
				break;
			}
		}
	}

	if(!runIfDocOnly_ && container_ != 0 && !container_->getIndexNodes()) {
		// The query plan is definately not going to be run
		qp_ = 0;
	}
}

void QueryPlanHolder::generateQueryPlan(TransactedContainer *container, Transaction *txn, DbXmlContext *context)
{
	container_ = container;

	// Resolve the values in the query plan and
	// generate the partially optimised query plan
	qp_ = qp_->createPartiallyOptimisedQueryPlan(
		txn, *container_, context, /*nodeQP*/true,
		/*fullyOptimised*/qpIsExecutable_,
		/*exact*/qpIsExact_);

	if(qp_->getType() == QueryPlan::UNIVERSE) {
		// We don't want to execute a UniverseQP with node results
		qp_ = 0;
	}
}

bool QueryPlanHolder::isSuitableForLookupIndex() const
{
	return qp_ != 0 && qpIsExecutable_ && container_ != 0 && container_->getIndexNodes();
}

bool QueryPlanHolder::isSuitableForQPHCombine() const
{
	return qp_ != 0 && qpIsExecutable_ && container_ != 0;
}

bool QueryPlanHolder::costQueryPlan(QueryPlan::Cost &cost, DbXmlContext *context) const
{
	if(qp_ == 0 || !qpIsExecutable_) return false;

	QueryPlan *qp = qp_;
	if(qp->getType() == QueryPlan::DOCUMENT) {
		qp = ((DocumentQP*)qp)->getArgument();
		if(qp == 0) return false;
	}

	QueryContext &qc = context->getQueryContext();
	OperationContext &oc = qc.getOperationContext();

	QueryExecutionContext &qec = *qc.getQueryExecutionContext();
	qec.setContainer(*container_);
	qec.setDbXmlContext(context);
	qec.setCostToFilterFunction(0);

	cost = qp->cost(oc, qec);
	return true;
}

void QueryPlanHolder::executeQueryPlan(IndexData::Ptr &data, OperationContext &oc, QueryExecutionContext &qec,
	bool &exact) const
{
	if(qp_ == 0) {
		data.reset();
		return;
	}

	QueryPlan *qp = qp_;

	if(!qpIsExecutable_) {
		// Resolve the values in the query plan and
		// generate the optimised query plan
		qp = qp->createExecutableQueryPlan(oc.txn(), qec.getContainer(),
			qec.getDbXmlContext(), /*nodeQP*/true, exact);

		if(qp != 0 && qp->getType() == QueryPlan::UNIVERSE) {
			// We don't want to execute a UniverseQP with node results
			qp = 0;
		}
	} else {
		exact = qpIsExact_;
	}

	if(qp != 0) {
		data = qp->nodes_execute(oc, qec);
	} else {
		data.reset();
	}
}
