//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryPlanFunction.cpp,v 1.61.2.1 2007/01/19 14:56:01 jsnelson Exp $
//

#include <sstream>

#include "DbXmlInternal.hpp"
#include <dbxml/XmlResults.hpp>
#include <dbxml/XmlTransaction.hpp>
#include "QueryPlanFunction.hpp"
#include "DbXmlContext.hpp"
#include "../QueryContext.hpp"
#include "../Results.hpp"
#include "../UTF8.hpp"
#include "../Manager.hpp"
#include "../QueryPlan.hpp"
#include "../Transaction.hpp"
#include "../ReferenceMinder.hpp"
#include "../Document.hpp"
#include "../OperationContext.hpp"
#include "../Transaction.hpp"
#include "../QueryExecutionContext.hpp"
#include "DbXmlUri.hpp"
#include "DbXmlNodeImpl.hpp"
#include "DbXmlFactoryImpl.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/XMLString.hpp>
#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

QueryPlanFunction::QueryPlanFunction(ReferenceMinder &minder, Type type,
	ASTNode *arg,XPath2MemoryManager* memMgr)
	: DbXmlASTNode(QUERY_PLAN_FUNCTION, memMgr),
	  type_(type),
	  arg_(arg),
	  impliedSchema_(0),
	  minder_(minder)
{
	staticTyping(0);
}

void QueryPlanFunction::setImpliedSchema(ImpliedSchemaNode *impliedSchema) {
	impliedSchema_ = impliedSchema;
	impliedSchema_->setQueryPlanHolder(this);
}

ASTNode* QueryPlanFunction::staticResolution(StaticContext* context)
{
	if(arg_ != 0) {
		arg_ = arg_->staticResolution(context);
	}
	return this;
}

ASTNode *QueryPlanFunction::staticTyping(StaticContext *context)
{
	DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);

	_src.clear();

	unsigned int props = StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED |
		StaticResolutionContext::PEER | StaticResolutionContext::SUBTREE;

	if(type_ == DOCUMENT) {
		props |= StaticResolutionContext::ONENODE;
		_src.availableDocumentsUsed(true);
	} else {
		_src.availableCollectionsUsed(true);
	}

	if(arg_ != 0) {
		if(context != 0)
			arg_ = arg_->staticTyping(context);
		_src.add(arg_->getStaticResolutionContext());
	}

	if((context == 0 || dbContext->runQueryPlans()) && qp_ != 0) {
		if(context != 0)
			qp_ = qp_->staticTyping(context);
		_src.add(qp_->getStaticResolutionContext());
	}

	_src.setProperties(props);

	_src.getStaticType().flags = StaticType::NODE_TYPE;
	if(type_ == DOCUMENT || (qp_ && qpIsExecutable_)) {
		_src.getStaticType().flags = StaticType::DOCUMENT_TYPE;
	}

	return this;
}

DbXmlResult QueryPlanFunction::createResult(const DbXmlResult &contextItems, unsigned int props,
	DynamicContext *context) const
{
	DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);

	if(qp_) {
		if(qpIsExecutable_ && dbContext->runQueryPlans()) {
			return new QueryPlanResult(qp_, container_,
				documentUri_, this);
		} else {
			return new JITCompileResult(this, dbContext->runQueryPlans());
		}
	} else {
		return new DefaultBehaviourResult(this, context);
	}
}

static double costToFilter(long numDocuments)
{
	// The cost of filtering is one disk access to retrieve the document,
	// plus the cpu of parsing the document and applying the full XPath
	// implementation against the resultant DOM. For the moment let's just
	// assume that this is not in the same league as a disk access. - jpcs
	return (double)numDocuments;
}

const XMLCh *QueryPlanFunction::getUriArg(DbXmlContext *context) const
{
	const XMLCh *currentUri = 0;

	Item::Ptr arg;
	if(arg_ != 0) {
		arg = arg_->collapseTree(context)->next(context);
	}

	if(arg.isNull()) {
		switch(type_) {
		case DOCUMENT: return 0;
		case COLLECTION: {
			// Use the default collection URI
			QueryContext &qc = context->getQueryContext();
			if(!qc.getDefaultCollection().empty()) {
				currentUri = context->getMemoryManager()
					->getPooledString(UTF8ToXMLCh(qc.getDefaultCollection()).str());
			} else {
				XQThrow(FunctionException,
					X("QueryPlanFunction::getUriArg"),
					X("The default collection has not been set [err:FODC0002]"));
			}
			break;
		}
		}
	}
	else {
		// Get the uri argument
		currentUri = arg->asString(context);
	}

	if(currentUri == 0) {
		XQThrow(FunctionException,
			X("QueryPlanFunction::getUriArg"),
			X("Error retrieving resource [err:FODC0002]"));
	}

	if(!XMLUri::isValidURI(true, currentUri)) {
		switch(type_) {
		case DOCUMENT:
			XQThrow(FunctionException,
				X("QueryPlanFunction::getUriArg"),
				X("Invalid URI format [err:FODC0005]"));
		case COLLECTION:
			XQThrow(FunctionException,
				X("QueryPlanFunction::getUriArg"),
				X("Invalid URI format [err:FODC0002]"));
		}
	}

	return currentUri;
}

void QueryPlanFunction::removeNonConstant(XmlManager &mgr, Transaction *txn,
	DbXmlContext *context)
{
	if(qp_ != 0) {

		qp_ = qp_->removeNonConstant();
		if(qp_ == 0) {
			qp_ = new (context->getMemoryManager())
				UniverseQP(context->getMemoryManager());
		}

		if(container_ == 0) {
			// We don't know what container the query references, so all
			// we can do is generate a raw optimised query plan.
			qp_ = qp_->createRawQueryPlan((Manager&)mgr, context);
		} else {
			// Generate a partially optimised query plan
			qp_ = qp_->createPartiallyOptimisedQueryPlan(txn, *container_,
				context, /*nodeQP*/false, /*fullyOptimised*/qpIsExecutable_,
				/*exact*/qpIsExact_);
		}
	}
}

void QueryPlanFunction::generateQueryPlan(XmlManager &mgr, Transaction *txn,
	DbXmlContext *context)
{
	if(qp_ == 0) return;

	// If we don't suceed in generating a query plan, we fallback
	// to using XQilla's uri resolving methods.

	if(arg_ == 0 || arg_->isConstant()) {
		const XMLCh* currentUri = getUriArg(context);
		if(currentUri != 0) {
			DbXmlUri uri(context->getBaseURI(),
				currentUri, type_ == DOCUMENT);

			if(uri.isDbXmlScheme()) {
				if(uri.getDocumentName() == "" &&
					type_ == DOCUMENT) {
					ostringstream oss;
					oss << "The URI '" <<
						uri.getResolvedUri() <<
						"' does not specifiy a document";
					throw XmlException(
						XmlException::DOCUMENT_NOT_FOUND,
						oss.str());
				}

				try {
					XmlContainer containerWrapper(
						uri.openContainer(mgr, txn));
					container_ = (TransactedContainer*)
						(Container*)containerWrapper;
					minder_.addContainer(container_);
				}
				catch(XmlException &e) {
					e.setLocationInfo(this);
					throw;
				}

				// If this is a document lookup, wrap the query
				// plan in a document name index lookup
				if(type_ == DOCUMENT) {
					qp_ = new (context->getMemoryManager())
						DocumentQP(uri.getDocumentName(),
							qp_, context->getMemoryManager());
				}

				// Generate a partially optimised query plan
				qp_ = qp_->createPartiallyOptimisedQueryPlan(txn, *container_,
					context, /*nodeQP*/false, /*fullyOptimised*/qpIsExecutable_,
					/*exact*/qpIsExact_);

				if(qpIsExecutable_) {
					try {
						XMLUri temp(context->getBaseURI(), context->getMemoryManager());
						XMLUri temp2(&temp, currentUri, context->getMemoryManager());
						documentUri_ = context->getMemoryManager()->getPooledString(temp2.getUriText());
					}
					catch(...) {
					}
				}
			}
		}
	} else {
		// We don't know what container the query references, so all
		// we can do is generate a raw optimised query plan.
		qp_ = qp_->createRawQueryPlan((Manager&)mgr, context);
	}
}

QueryPlanFunction::DefaultBehaviourResult::DefaultBehaviourResult(
	const QueryPlanFunction *func, DynamicContext *context)
	: LazySequenceResult(func, context),
	  func_(func)
{
}

void QueryPlanFunction::DefaultBehaviourResult::getResult(
	Sequence &toFill, DynamicContext *context) const
{
	DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);

	const XMLCh *currentUri = func_->getUriArg(dbContext);
	if(func_->type_ == QueryPlanFunction::COLLECTION) {
		toFill = context->resolveCollection(currentUri, this);
	} else {
		// fn:doc() returns () if the argument is ()
		if(currentUri != 0)
			toFill = context->resolveDocument(currentUri, this);
	}
}

std::string QueryPlanFunction::DefaultBehaviourResult::asString(
	DynamicContext *context, int indent) const
{
	std::ostringstream oss;
	std::string in(getIndent(indent));

	oss << in << "<qpf_default/>" << std::endl;

	return oss.str();
}

QueryPlanFunction::QueryPlanResult::QueryPlanResult(
	const QueryPlan *qp, const TransactedContainer *container,
	const XMLCh *documentUri, const LocationInfo *location)
	: DbXmlResultImpl(location),
	  qp_(qp),
	  container_(container),
	  documentUri_(documentUri),
	  toDo_(true),
	  ids_(0)
{
}

void QueryPlanFunction::QueryPlanResult::init(DynamicContext *context)
{
	toDo_ = false;

	DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);

	QueryContext &qc = dbContext->getQueryContext();
	OperationContext &oc = qc.getOperationContext();

	QueryExecutionContext &qec = *qc.getQueryExecutionContext();
	qec.setContainer(*container_);
	qec.setCostToFilterFunction(costToFilter);
	qec.setDbXmlContext(dbContext);

	ids_ = qp_->execute(oc, qec);
	it_ = ids_->begin();
}

Item::Ptr QueryPlanFunction::QueryPlanResult::next(DynamicContext *context)
{
	context->testInterrupt();

	if(toDo_) init(context);

	if(!ids_ || it_ == ids_->end()) return 0;

	IndexEntry::Ptr ie = new IndexEntry();
	ie->setFormat(IndexEntry::D_FORMAT);
	ie->setDocID(*it_);
	++it_;

	Item::Ptr result = ((DbXmlFactoryImpl*)context->getItemFactory())->
		createNode(ie, container_, documentUri_, context);

	// TBD remove this special case - jpcs
	if(container_->getContainerType() == XmlContainer::WholedocContainer) {
		// We force the Document object to be materialised, because
		// otherwise the scope of the Document object is too small,
		// causing the DLS parsed tree to be deleted just before it
		// is needed again.
		//
		// We need a better way than this - maybe keep up to N old
		// pre-parsed documents around. Maybe it won't be such a
		// problem to keep them all around when they are in a
		// temporary DB BTree.
		((DbXmlNodeImpl*)result.get())->getXmlDocument();
	}

	return result;
}

Item::Ptr QueryPlanFunction::QueryPlanResult::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	context->testInterrupt();

	if(toDo_) init(context);

	if(!ids_) return 0;

	// Iterate to the correct position
	IDS::iterator end = ids_->end();
	while(it_ != end && (*it_ < did ||
		      (*it_ == did && NsNid::getRootNid()->compareNids(&nid) < 0))) {
		++it_;
	}

	return next(context);
}

QueryPlanFunction::JITCompileResult::JITCompileResult(
	const QueryPlanFunction *func, bool useQPs)
	: ResultImpl(func),
	  useQPs_(useQPs),
	  func_(func),
	  container_(0),
	  toDo_(true),
	  results_(0)
{
}

QueryPlanFunction::JITCompileResult::~JITCompileResult()
{
	if(container_ != 0) delete container_;
}

Item::Ptr QueryPlanFunction::JITCompileResult::next(DynamicContext *context)
{
	if(toDo_) {
		toDo_ = false;

		DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);
		QueryContext &qc = dbContext->getQueryContext();
		OperationContext &oc = qc.getOperationContext();

		const XMLCh *currentUri = 0;
		TransactedContainer *t_cont = func_->container_;
		QueryPlan *qp = func_->qp_;

		if(t_cont == 0) {
			currentUri = func_->getUriArg(dbContext);
			if(currentUri != 0) {
				DbXmlUri uri(context->getBaseURI(),
					currentUri,
					func_->type_ == DOCUMENT);

				if(uri.isDbXmlScheme()) {
					if(uri.getDocumentName().empty() &&
						func_->type_ == DOCUMENT) {
						ostringstream oss;
						oss << "The URI '" <<
							uri.getResolvedUri() <<
							"' does not specifiy a document";
						throw XmlException(XmlException::DOCUMENT_NOT_FOUND, oss.str());
					}

					try {
						container_ = new XmlContainer(
							uri.openContainer(qc.getManager(), oc.txn()));
						t_cont = (TransactedContainer*)(Container*)*container_;
					}
					catch(XmlException &e) {
						e.setLocationInfo(this);
						throw;
					}

					// If this is a document lookup, wrap
					// the query plan in
					// a document name index lookup
					if(func_->type_ == DOCUMENT) {
						qp = new (context->getMemoryManager()) DocumentQP(uri.getDocumentName(),
							qp, context->getMemoryManager());
					}
				}
			}
		}

		if(t_cont != 0) {
			if(!useQPs_) {
				// Don't run the query plan
				QueryPlan *tmpqp = QueryPlan::createUniverseQueryPlan(oc.txn(), *t_cont, dbContext);
				if(qp->getType() == QueryPlan::DOCUMENT) {
					qp = new (context->getMemoryManager())
						DocumentQP(((DocumentQP*)qp)->getValue().getValue(),
							tmpqp, context->getMemoryManager());
				} else {
					qp = tmpqp;
				}
			}

			// Resolve the values in the query plan and
			// generate the optimised query plan
			bool exact;
			qp = qp->createExecutableQueryPlan(oc.txn(), *t_cont, dbContext,
				/*nodeQP*/false, exact);


			if(qp != 0) {
				const XMLCh *documentUri = 0;
				try {
					XMLUri temp(context->getBaseURI(), context->getMemoryManager());
					XMLUri temp2(&temp, currentUri, context->getMemoryManager());
					documentUri = context->getMemoryManager()->getPooledString(temp2.getUriText());
				}
				catch(...) {
				}

				results_ = new QueryPlanResult(qp, t_cont,
					documentUri, this);
			}
		}

		if(results_.isNull()) {
			if(currentUri == 0) currentUri =
				func_->getUriArg(dbContext);

			// If we haven't managed to generate a query plan, use
			// the default behaviour
			if(func_->type_ == QueryPlanFunction::COLLECTION) {
				results_ =
					context->resolveCollection(currentUri, this);
			} else {
				// fn:doc() returns () if the argument is ()
				if(currentUri != 0)
					results_ = context->resolveDocument(currentUri, this);
			}
		}
	}

	Item::Ptr item = results_->next(context);

	if(item == NULLRCP) {
		results_ = 0;
	}

	return item;
}

std::string QueryPlanFunction::JITCompileResult::asString(
	DynamicContext *context, int indent) const
{
	std::ostringstream oss;
	std::string in(getIndent(indent));

	oss << in << "<qpf_jitcompile/>" << std::endl;

	return oss.str();
}
