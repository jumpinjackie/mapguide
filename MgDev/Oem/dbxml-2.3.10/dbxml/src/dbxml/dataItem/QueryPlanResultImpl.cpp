//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryPlanResultImpl.cpp,v 1.11 2006/11/17 18:18:29 jsnelson Exp $
//

#include "QueryPlanResultImpl.hpp"
#include "QueryPlanHolder.hpp"
#include "DbXmlContext.hpp"
#include "../Manager.hpp"
#include "../QueryPlan.hpp"
#include "../Document.hpp"
#include "../QueryExecutionContext.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/runtime/SequenceResult.hpp>
#include <xqilla/exceptions/TypeErrorException.hpp>

using namespace DbXml;
using namespace std;

/////////////////////////////////////
// QueryPlanResultImpl
/////////////////////////////////////

QueryPlanResultImpl::QueryPlanResultImpl(const DbXmlResult &parent, const QueryPlanHolder *holder,
	const LocationInfo *location)
	: DbXmlResultImpl(location),
	  parent_(parent),
	  holder_(holder),
	  joinType_(Join::SELF),
	  container_(0),
	  result_(0)
{
}

QueryPlanResultImpl::QueryPlanResultImpl(const DbXmlResult &parent, const QueryPlanHolder *holder,
	Join::Type joinType, const LocationInfo *location)
	: DbXmlResultImpl(location),
	  parent_(parent),
	  holder_(holder),
	  joinType_(joinType),
	  container_(0),
	  qpIsExact_(false),
	  result_(0)
{
}

void QueryPlanResultImpl::nextResult(Item::Ptr &item, DynamicContext *context)
{
	result_ = 0;

	while(item.notNull()) {
		context->testInterrupt();

		const DbXmlNodeImpl *node = (const DbXmlNodeImpl*)item->getInterface(DbXmlNodeImpl::gDbXml);
		DBXML_ASSERT(node != 0);

		TransactedContainer *container = node->getContainer();

		if(holder_ != 0 && holder_->getQueryPlan() != 0 && container != 0 &&
			(container->getIndexNodes() || holder_->runQueryPlanIfDocOnly())) {

			if(container != container_) {
				// Calculate the index data
				container_ = container;

				DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);
				QueryContext &qc = dbContext->getQueryContext();
				OperationContext &oc = qc.getOperationContext();

				QueryExecutionContext &qec = *qc.getQueryExecutionContext();
				qec.setContainer(*container_);
				qec.setDbXmlContext(dbContext);
				qec.setCostToFilterFunction(0);

				holder_->executeQueryPlan(data_, oc, qec, qpIsExact_);
				if(data_) it_ = data_->begin();
			}

			if(data_) {
				DocID docID = node->getDocID();

				// Calculate the correct position in the
				// index data for this document
				IndexData::const_iterator end = data_->end();
				while(it_ != end && (*it_)->getDocID() < docID) {
					++it_;
				}
				if(it_ == end) {
					item = parent_->next(context);
					continue;
				}
				if((*it_)->getDocID() > docID) {
					item = parent_->seek((*it_)->getDocID(), *NsNid::getRootNid(), context);
					continue;
				}

				// (*it_)->getDocID() == docID

				if((*it_)->isSpecified(IndexEntry::NODE_ID) &&
					(*it_)->isSpecified(IndexEntry::NODE_LEVEL)) {
					switch(joinType_) {
					case Join::SELF: {
						result_ = new SelfJoinResult(node, it_, end);
						break;
					}
					case Join::ATTRIBUTE: {
						if(node->dmNodeKind() == Node::element_string)
							result_ = new AttributeJoinResult(node, it_, data_);
						else result_ = new NullJoinResult();
						break;
					}
					case Join::CHILD: {
						if(node->dmNodeKind() == Node::element_string ||
							node->dmNodeKind() == Node::document_string)
							result_ = new ChildJoinResult(node, it_, data_);
						else result_ = new NullJoinResult();
						break;
					}
					case Join::DESCENDANT: {
						if(node->dmNodeKind() == Node::element_string ||
							node->dmNodeKind() == Node::document_string)
							result_ = new DescendantJoinResult(node, it_, data_,
								/*orSelf*/false);
						else result_ = new NullJoinResult();
						break;
					}
					case Join::DESCENDANT_OR_SELF: {
						if(node->dmNodeKind() == Node::element_string ||
							node->dmNodeKind() == Node::document_string)
							result_ = new DescendantJoinResult(node, it_, data_,
								/*orSelf*/true);
						// no else
						break;
					}
					default: break;
					}
				} else {
					qpIsExact_ = false;
				}
			} else {
				qpIsExact_ = false;
			}
		} else {
			qpIsExact_ = false;
		}

		setResult(node, context);
		return;
	}

	parent_ = 0;
}

Item::Ptr QueryPlanResultImpl::next(DynamicContext *context)
{
	Item::Ptr ret;
	Item::Ptr item;
	while((ret = result_->next(context)).isNull() && !parent_.isNull()) {
		context->testInterrupt();
		item = parent_->next(context);
		nextResult(item, context);
	}
	return ret;
}

Item::Ptr QueryPlanResultImpl::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	Item::Ptr ret = result_->seek(did, nid, context);

	if(!ret.isNull()) {
		const DbXmlNodeImpl *node = (const DbXmlNodeImpl*)ret->getInterface(DbXmlNodeImpl::gDbXml);
		// This is only safe because the result_ iterator only
		// ever returns nodes from the same document
		if(node->getDocID() >= did) {
			return ret;
		}
	}

	if(!parent_.isNull()) {
		Item::Ptr item = parent_->seek(did, *NsNid::getRootNid(), context);
		nextResult(item, context);
		while((ret = result_->next(context)).isNull() && !parent_.isNull()) {
			context->testInterrupt();
			item = parent_->next(context);
			nextResult(item, context);
		}
	}
	return ret;
}


void QueryPlanResultImpl::setResult(const DbXmlNodeImpl *node, DynamicContext *context)
{
	if(result_.isNull()) {
		result_ = new SequenceResult(this, Sequence(node, context->getMemoryManager()));
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
	IndexData::const_iterator end)
	: DbXmlResultImpl(0),
	  nodeObj_(node),
	  docID_(nodeObj_->getDocID()),
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
			parentIt_ = it_;
			return 0;
		}
		}
	}
	return 0;
}

Item::Ptr SelfJoinResult::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(docID_ < did) {
		return 0;
	}
	return next(context);
}

string SelfJoinResult::asString(DynamicContext *context, int indent) const
{
	return "<SelfJoinResult/>";
}


/////////////////////////////////////
// AttributeJoinResult
/////////////////////////////////////

AttributeJoinResult::AttributeJoinResult(const DbXmlNodeImpl *node, IndexData::const_iterator &parentIt,
	const IndexData::Ptr &data)
	: DbXmlResultImpl(0),
	  nodeObj_(node),
	  docID_(nodeObj_->getDocID()),
	  parentIt_(parentIt),
	  it_(parentIt),
	  data_(data)
{
	DBXML_ASSERT(node->dmNodeKind() == Node::element_string || node->dmNodeKind() == Node::document_string);
}

Item::Ptr AttributeJoinResult::next(DynamicContext *context)
{
	while(it_ != data_->end() && (*it_)->getDocID() == docID_) {
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
			return nodeObj_->getAttribute((*(it_++))->getIndex(), context);
		}
		case IndexEntry::REL_TOO_BIG: {
			parentIt_ = it_;
			return 0;
		}
		}
	}
	return 0;
}

Item::Ptr AttributeJoinResult::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(docID_ < did) {
		return 0;
	}
	return next(context);
}

string AttributeJoinResult::asString(DynamicContext *context, int indent) const
{
	return "<AttributeJoinResult/>";
}

/////////////////////////////////////
// ChildJoinResult
/////////////////////////////////////

ChildJoinResult::ChildJoinResult(const DbXmlNodeImpl *node, IndexData::const_iterator &parentIt,
	const IndexData::Ptr &data)
	: DbXmlResultImpl(0),
	  nodeObj_(node),
	  docID_(nodeObj_->getDocID()),
	  parentIt_(parentIt),
	  it_(parentIt),
	  data_(data)
{
	DBXML_ASSERT(node->dmNodeKind() == Node::element_string || node->dmNodeKind() == Node::document_string);
}

Item::Ptr ChildJoinResult::next(DynamicContext *context)
{
	while(it_ != data_->end() && (*it_)->getDocID() == docID_) {
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

Item::Ptr ChildJoinResult::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(docID_ < did) {
		return 0;
	}
	return next(context);
}

string ChildJoinResult::asString(DynamicContext *context, int indent) const
{
	return "<ChildJoinResult/>";
}

/////////////////////////////////////
// DescendantJoinResult
/////////////////////////////////////

DescendantJoinResult::DescendantJoinResult(const DbXmlNodeImpl *node, IndexData::const_iterator &parentIt,
	const IndexData::Ptr &data, bool orSelf)
	: DbXmlResultImpl(0),
	  orSelf_(orSelf),
	  nodeObj_(node),
	  docID_(nodeObj_->getDocID()),
	  parentIt_(parentIt),
	  it_(parentIt),
	  data_(data)
{
	DBXML_ASSERT(node->dmNodeKind() == Node::element_string || node->dmNodeKind() == Node::document_string);
}

Item::Ptr DescendantJoinResult::next(DynamicContext *context)
{
	while(it_ != data_->end() && (*it_)->getDocID() == docID_) {
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
			parentIt_ = it_;
			if(orSelf_ && nodeObj_->getNID()->compareNids(
				   &(*it_)->getNodeID()) == 0) {
				it_++;
				return nodeObj_;
			}
			orSelf_ = false; // We aren't going to find ourselves, after the first node
			return nodeObj_->getDescendantElement(*(it_++), context);
		}
		case IndexEntry::REL_TOO_BIG: {
			parentIt_ = it_;
			return 0;
		}
		}
	}
	return 0;
}

Item::Ptr DescendantJoinResult::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(docID_ < did) {
		return 0;
	}
	return next(context);
}

string DescendantJoinResult::asString(DynamicContext *context, int indent) const
{
	return "<DescendantJoinResult/>";
}
