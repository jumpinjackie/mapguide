//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: QueryPlanHolder.cpp,v 1.14 2005/12/19 16:47:37 jsnelson Exp $
//

#include "QueryPlanHolder.hpp"
#include "QueryPlanFunction.hpp"
#include "DbXmlContext.hpp"
#include "DbXmlFactoryImpl.hpp"
#include "../Manager.hpp"
#include "../QueryPlan.hpp"
#include "../Document.hpp"
#include "../QueryExecutionContext.hpp"

#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/TypeErrorException.hpp>

using namespace DbXml;
using namespace std;

/////////////////////////////////////
// QueryPlanHolder
/////////////////////////////////////

QueryPlanHolder::QueryPlanHolder()
	: qp_(0),
	  qpIsExecutable_(false),
	  runIfDocOnly_(false)
{
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

void QueryPlanHolder::generateQueryPlan(XmlManager &db, Transaction *txn, DynamicContext *context)
{
	DBXML_ASSERT(qp_->getType() == QueryPlan::PATHS);

	if(qp_ == 0) return;

	PathsQP *pqp = (PathsQP*)qp_;
	const PathsQP::Paths &paths = pqp->getPaths();
	if(paths.empty()) {
		qp_ = 0;
		return;
	}

	const ImpliedSchemaNode *root = 0;
	const TransactedContainer *container = 0;
	if(pqp->sameTree()) {
		root = paths.front()->getRoot();

		const QueryPlanFunction *qpf = root->getQueryPlanFunction();
		if(qpf == 0 || qpf->getQueryPlan() == 0) {
			qp_ = 0;
			return;
		}

		container = qpf->getContainer();
	}

	if(container == 0) {
		// We don't know what container the query references, so all
		// we can do is generate a partially optimised query plan.
		qp_ = qp_->createLocalRawQueryPlan(root, (Manager&)db,
			context, /*partiallyOptimize*/true);
	} else {
		qp_ = qp_->createLocalRawQueryPlan(root, *container,
			context, /*partiallyOptimize*/false);
		// Resolve the values in the query plan and
		// generate the optimised query plan
		qp_ = qp_->createExecutableQueryPlan(txn, *container, context,
			/*nodeQP*/true, /*lookupValues*/false, qpIsExecutable_);
	}

	if(qp_->getType() == QueryPlan::UNIVERSE) {
		// We don't want to execute a UniverseQP from a DataItemStep
		qp_ = 0;
	} else if(root != 0) {
		// We only do an index lookup for document ID filtering
		// if the main query plan is not a subset of ours.
		runIfDocOnly_ = !root->getQueryPlanFunction()->getQueryPlan()->isSubsetOf(qp_);
	}

	if(!runIfDocOnly_ && container != 0 && !container->getIndexNodes()) {
		// The query plan is definately not going to be run
		qp_ = 0;
	}
}

QueryPlan *QueryPlanHolder::generateJITQueryPlan(Transaction *txn, const Container &container,
	DynamicContext *context) const
{
	if(qpIsExecutable_) return qp_;
	if(qp_ == 0) return 0;

	// Resolve the values in the query plan and
	// generate the optimised query plan
	bool success = true;
	QueryPlan *qp = qp_->createExecutableQueryPlan(txn, container,
		context, /*nodeQP*/true, /*lookupValues*/true, success);

	if(!success) return 0;

	if(qp == 0 || qp->getType() == QueryPlan::UNIVERSE) {
		// We don't want to execute a UniverseQP from a DataItemStep
		return 0;
	}

	return qp;
}

/////////////////////////////////////
// QueryPlanResultImpl
/////////////////////////////////////

QueryPlanResultImpl::QueryPlanResultImpl(const Result &parent, const QueryPlanHolder *holder,
	DynamicContext *context)
	: ResultImpl(context),
	  parent_(parent),
	  holder_(holder),
	  joinType_(DataItemStep::SELF),
	  toDo_(parent.isNull()),
	  container_(0),
	  result_(0)
{
}

QueryPlanResultImpl::QueryPlanResultImpl(const Result &parent, const QueryPlanHolder *holder,
	DataItemStep::Axis joinType, DynamicContext *context)
	: ResultImpl(context),
	  parent_(parent),
	  holder_(holder),
	  joinType_(joinType),
	  toDo_(parent.isNull()),
	  container_(0),
	  result_(0)
{
}

Item::Ptr QueryPlanResultImpl::next(DynamicContext *context)
{
	Item::Ptr ret = result_.next(context);
	while(ret == NULLRCP) {
		result_ = 0;

		Item::Ptr item = 0;
		if(toDo_) {
			toDo_ = false;
			// This is the first step, so navigate from the context item
			item = context->getContextItem();
			if(item == NULLRCP) {
				DSLthrow(TypeErrorException,X("QueryPlanResultImpl::next"),
					X("An attempt was made to perform an axis step when the Context Item was not a node [err:XPTY0020]"));
			}
		}
		else {
			item = parent_.next(context);
			if(item == NULLRCP) {
				parent_ = 0;
				break;
			}
		}

		const DbXmlNodeImpl *node = (const DbXmlNodeImpl*)item->getInterface(DbXmlNodeImpl::gDbXml);
		DBXML_ASSERT(node != 0);

		Document &doc = (Document&)node->getXmlDocument();

		if(holder_->getQueryPlan() != 0 && doc.getContainer() != 0 &&
			(doc.getContainer()->getIndexNodes() || holder_->runQueryPlanIfDocOnly())) {

			if(doc.getContainer() != container_) {
				// Calculate the index data
				container_ = doc.getContainer();

				DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);
				QueryContext &qc = dbContext->getQueryContext();
				OperationContext &oc = qc.getOperationContext();

				QueryPlan *qp = holder_->generateJITQueryPlan(oc.txn(), *container_, context);
				if(qp != 0) {
					QueryExecutionContext &qec = qc.getQueryExecutionContext();
					qec.setContainer(*container_);
					qec.setCostToFilterFunction(0);

					data_ = qp->nodes_execute(oc, qec);
					it_ = data_->begin();
				} else {
					data_.reset();
				}
			}

			if(data_) {
				ID docID = doc.getID();

				// Calculate the correct position in the index data for this document
				IndexData::const_iterator end = data_->end();
				while(it_ != end && (*it_)->getDocID() < docID) {
					++it_;
				}
				if(it_ == end || (*it_)->getDocID() > docID) {
					continue;
				}

				// (*it_)->getDocID() == docID

				if((*it_)->isSpecified(IndexEntry::NODE_ID) &&
					(*it_)->isSpecified(IndexEntry::NODE_LEVEL)) {
					switch(joinType_) {
					case DataItemStep::SELF: {
						result_ = new SelfJoinResult(node, it_, end, context);
						break;
					}
					case DataItemStep::ATTRIBUTE: {
						result_ = new AttributeJoinResult(node, it_, end, context);
						break;
					}
					case DataItemStep::CHILD: {
						result_ = new ChildJoinResult(node, it_, end, context);
						break;
					}
					case DataItemStep::DESCENDANT: {
						result_ = new DescendantJoinResult(node, it_, end, /*orSelf*/false, context);
						break;
					}
					case DataItemStep::DESCENDANT_OR_SELF: {
						result_ = new DescendantJoinResult(node, it_, end, /*orSelf*/true, context);
						break;
					}
					default: break;
					}
				}
			}
		}

		setResult(node, context);
		ret = result_.next(context);
	}

	return ret;
}

void QueryPlanResultImpl::skip()
{
	result_ = 0;
	parent_.skip();
}

void QueryPlanResultImpl::setResult(const DbXmlNodeImpl *node, DynamicContext *context)
{
	if(result_.isNull()) {
		result_ = Sequence(node, context->getMemoryManager());
	}
}

string QueryPlanResultImpl::asString(DynamicContext *context, int indent) const
{
	return "<QueryPlanResultImpl/>";
}

/////////////////////////////////////
// SelfJoinResult
/////////////////////////////////////

SelfJoinResult::SelfJoinResult(const DbXmlNodeImpl *node, IndexData::const_iterator &parentIt,
	IndexData::const_iterator end, DynamicContext *context)
	: ResultImpl(context),
	  nodeObj_(node),
	  docID_(((Document&)nodeObj_->getXmlDocument()).getID().raw()),
	  parentIt_(parentIt),
	  it_(parentIt),
	  end_(end)
{
	DBXML_ASSERT(node->dmNodeKind() == Node::element_string || node->dmNodeKind() == Node::document_string ||
		node->dmNodeKind() == Node::attribute_string);
}

Item::Ptr SelfJoinResult::next(DynamicContext *context)
{
	while(it_ != end_ && (*it_)->getDocID() == docID_) {
		switch((*it_)->isSelf(nodeObj_.get())) {
		case IndexEntry::REL_TOO_SMALL: {
			parentIt_ = it_;
			// Fall through
		}
		case IndexEntry::REL_FALSE: {
			++it_;
			break;
		}
		case IndexEntry::REL_TRUE: {
			parentIt_ = it_;
			++it_;
			return nodeObj_;
		}
		case IndexEntry::REL_TOO_BIG: {
			return 0;
		}
		}
	}
	return 0;
}

string SelfJoinResult::asString(DynamicContext *context, int indent) const
{
	return "<SelfJoinResult/>";
}


/////////////////////////////////////
// AttributeJoinResult
/////////////////////////////////////

AttributeJoinResult::AttributeJoinResult(const DbXmlNodeImpl *node, IndexData::const_iterator &parentIt,
	IndexData::const_iterator end, DynamicContext *context)
	: ResultImpl(context),
	  nodeObj_(node),
	  docID_(((Document&)nodeObj_->getXmlDocument()).getID().raw()),
	  parentIt_(parentIt),
	  it_(parentIt),
	  end_(end)
{
	DBXML_ASSERT(node->dmNodeKind() == Node::element_string || node->dmNodeKind() == Node::document_string ||
		node->dmNodeKind() == Node::attribute_string);
}

Item::Ptr AttributeJoinResult::next(DynamicContext *context)
{
	while(it_ != end_ && (*it_)->getDocID() == docID_) {
		switch((*it_)->isSelf(nodeObj_.get())) {
		case IndexEntry::REL_TOO_SMALL: {
			parentIt_ = it_;
			// Fall through
		}
		case IndexEntry::REL_FALSE: {
			++it_;
			break;
		}
		case IndexEntry::REL_TRUE: {
			parentIt_ = it_;
			return nodeObj_->getAttribute((*(it_++))->getAttrIndex(), context);
		}
		case IndexEntry::REL_TOO_BIG: {
			return 0;
		}
		}
	}
	return 0;
}

string AttributeJoinResult::asString(DynamicContext *context, int indent) const
{
	return "<AttributeJoinResult/>";
}

/////////////////////////////////////
// ChildJoinResult
/////////////////////////////////////

ChildJoinResult::ChildJoinResult(const DbXmlNodeImpl *node, IndexData::const_iterator &parentIt,
	IndexData::const_iterator end, DynamicContext *context)
	: ResultImpl(context),
	  nodeObj_(node),
	  docID_(((Document&)nodeObj_->getXmlDocument()).getID().raw()),
	  parentIt_(parentIt),
	  it_(parentIt),
	  end_(end)
{
	DBXML_ASSERT(node->dmNodeKind() == Node::element_string || node->dmNodeKind() == Node::document_string);
}

Item::Ptr ChildJoinResult::next(DynamicContext *context)
{
	while(it_ != end_ && (*it_)->getDocID() == docID_) {
		switch((*it_)->isChildOf(nodeObj_.get())) {
		case IndexEntry::REL_TOO_SMALL: {
			parentIt_ = it_;
			// Fall through
		}
		case IndexEntry::REL_FALSE: {
			++it_;
			break;
		}
		case IndexEntry::REL_TRUE: {
			return nodeObj_->getDescendantElement(*(it_++), context);
		}
		case IndexEntry::REL_TOO_BIG: {
			return 0;
		}
		}
	}
	return 0;
}

string ChildJoinResult::asString(DynamicContext *context, int indent) const
{
	return "<ChildJoinResult/>";
}

/////////////////////////////////////
// DescendantJoinResult
/////////////////////////////////////

DescendantJoinResult::DescendantJoinResult(const DbXmlNodeImpl *node, IndexData::const_iterator &parentIt,
	IndexData::const_iterator end, bool orSelf, DynamicContext *context)
	: ResultImpl(context),
	  orSelf_(orSelf),
	  nodeObj_(node),
	  docID_(((Document&)nodeObj_->getXmlDocument()).getID().raw()),
	  parentIt_(parentIt),
	  it_(parentIt),
	  end_(end)
{
	DBXML_ASSERT(node->dmNodeKind() == Node::element_string || node->dmNodeKind() == Node::document_string);
}

Item::Ptr DescendantJoinResult::next(DynamicContext *context)
{
	while(it_ != end_ && (*it_)->getDocID() == docID_) {
		switch((*it_)->isDescendantOf(nodeObj_.get(), orSelf_, context)) {
		case IndexEntry::REL_TOO_SMALL: {
			parentIt_ = it_;
			// Fall through
		}
		case IndexEntry::REL_FALSE: {
			++it_;
			break;
		}
		case IndexEntry::REL_TRUE: {
			if(orSelf_ && NsUtil::nsCompareIds(nodeObj_->getNID(),
				   &(*it_)->getNodeID()) == 0) {
				return nodeObj_;
			}
			orSelf_ = false; // We aren't going to find ourselves, after the first node
			return nodeObj_->getDescendantElement(*(it_++), context);
		}
		case IndexEntry::REL_TOO_BIG: {
			return 0;
		}
		}
	}
	return 0;
}

string DescendantJoinResult::asString(DynamicContext *context, int indent) const
{
	return "<DescendantJoinResult/>";
}
