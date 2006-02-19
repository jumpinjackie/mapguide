//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: QueryPlanFunction.cpp,v 1.37 2005/11/30 17:12:15 jsnelson Exp $
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

#include <pathan/DynamicContext.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/XMLString.hpp>
#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

const XMLCh QueryPlanFunction::name[] = {
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_Q,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_y,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_P,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_F,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n,
	XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

QueryPlanFunction::QueryPlanFunction(ReferenceMinder &minder, Type type,
				     const VectorOfDataItems &args,
				     XPath2MemoryManager* memMgr)
	: DataItemFunction(name, 0, 1, "string?", args, memMgr),
	  type_(type),
	  impliedSchema_(0),
	  qpIsExecutable_(false),
	  qp_(0),
	  container_(0),
	  minder_(minder)
{
	unsigned int props = StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED | StaticResolutionContext::PEER | StaticResolutionContext::SUBTREE;
	if(type_ == DOCUMENT) {
		props |= StaticResolutionContext::ONENODE;
	}
	_src.setProperties(props);
	_src.getStaticType().flags = StaticType::NODE_TYPE;

	if(type_ == DOCUMENT) {
		_src.availableDocumentsUsed(true);
	} else {
		_src.availableCollectionsUsed(true);
	}
}

void QueryPlanFunction::setImpliedSchema(ImpliedSchemaNode *impliedSchema) {
	impliedSchema_ = impliedSchema;
	impliedSchema_->setQueryPlanFunction(this);
}

DataItem* QueryPlanFunction::staticResolution(StaticContext* context)
{
	if(type_ == DOCUMENT) {
		_src.availableDocumentsUsed(true);
	} else {
		_src.availableCollectionsUsed(true);
	}

	return resolveDataItems(_args, context, false);
}

Result QueryPlanFunction::createResult(DynamicContext* context, int flags) const
{
	if(qp_) {
		if(qpIsExecutable_) {
			return new QueryPlanResult(qp_, container_, flags,
						   context);
		} else {
			return new JITCompileResult(this, flags, context);
		}
	} else {
		return new DefaultBehaviourResult(this, flags, context);
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

const XMLCh *QueryPlanFunction::getUriArg(DynamicContext *context) const
{
	const XMLCh *currentUri = 0;

	if(getNumArgs() == 0) {
		// Use the default collection URI
		DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);
		QueryContext &qc = dbContext->getQueryContext();
		if(!qc.getDefaultCollection().empty()) {
			currentUri = context->getMemoryManager()
				->getPooledString(UTF8ToXMLCh(qc.getDefaultCollection()).str());
		} else {
			DSLthrow(FunctionException,
				X("QueryPlanFunction::getUriArg"),
				X("The default collection has not been set [err:FODC0002]"));
		}
	} else {
		Item::Ptr arg = getParamNumber(1, context).next(context);
		if(arg != NULLRCP) {
			// Get the uri argument
			currentUri = arg->asString(context);
		}
	}

	if(currentUri == 0) {
		DSLthrow(FunctionException,
			X("QueryPlanFunction::getUriArg"),
			X("Error retrieving resource [err:FODC0002]"));
	}

	try {
		context->getPathanFactory()->
			createAnyURI(currentUri, context);
	} catch(DSLException &e) {
		DSLthrow(FunctionException,
			X("QueryPlanFunction::getUriArg"),
			X("Invalid URI format"));
	}

	return currentUri;
}

void QueryPlanFunction::generateQueryPlan(XmlManager &mgr, Transaction *txn,
					  QueryPlan *qp, DynamicContext *context)
{
	// If we don't suceed in generating a query plan, we fallback
	// to using Pathan's uri resolving methods.

	if(_args.empty() || _args[0]->isConstant()) {
		const XMLCh* currentUri = getUriArg(context);
		DbXmlUri uri(context->getBaseURI(),
			currentUri, type_ == DOCUMENT);

		if(uri.isValid()) {
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

			XmlContainer containerWrapper(
				uri.openContainer(mgr, txn));
			container_ = (TransactedContainer*)
				(Container*)containerWrapper;
			minder_.addContainer(container_);

			// Generate a local raw query plan
			qp_ = qp->createLocalRawQueryPlan(
				impliedSchema_, *container_, context,
				/*partiallyOptimize*/false);

			// If this is a document lookup, wrap the query
			// plan in a document name index lookup
			if(type_ == DOCUMENT) {
				qp_ = new (getMemoryManager())
					DocumentQP(uri.getDocumentName(),
						qp_,
						getMemoryManager());
			}

			// Resolve the values in the query plan and
			// generate the optimised query plan
			qp_ = qp_->createExecutableQueryPlan(
				txn, *container_, context,
				/*nodeQP*/false, /*lookupValues*/false,
				qpIsExecutable_);
		}
	} else {
		// We don't know what container the query references, so all
		// we can do is generate a partially optimised query plan.
		qp_ = qp->createLocalRawQueryPlan(
			impliedSchema_, (Manager&)mgr, context,
			/*partiallyOptimize*/true);
	}
}

QueryPlanFunction::DefaultBehaviourResult::DefaultBehaviourResult(
	const QueryPlanFunction *func, int flags, DynamicContext *context)
	: LazySequenceResult(context),
	  flags_(flags),
	  func_(func)
{
}

void QueryPlanFunction::DefaultBehaviourResult::getResult(
	Sequence &toFill, DynamicContext *context) const
{
	if(func_->type_ == QueryPlanFunction::COLLECTION) {
		toFill = context->resolveCollection(func_->getUriArg(context));
	} else {
		toFill = context->resolveDocument(func_->getUriArg(context));
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
	int flags, DynamicContext *context)
	: ResultImpl(context),
	  flags_(flags),
	  qp_(qp),
	  container_(container),
	  toDo_(true),
	  ids_(0)
{
}

Item::Ptr QueryPlanFunction::QueryPlanResult::next(DynamicContext *context)
{
	DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);

	QueryContext &qc = dbContext->getQueryContext();
	OperationContext &oc = qc.getOperationContext();

	ReferenceMinder *minder = qc.getMinder();
	DBXML_ASSERT(minder != 0);

	if(toDo_) {
		toDo_ = false;

		QueryExecutionContext &qec = qc.getQueryExecutionContext();
		qec.setContainer(*container_);
		qec.setCostToFilterFunction(costToFilter);

		ids_ = qp_->execute(oc, qec);
		it_ = ids_->begin();
	}

	Item::Ptr result;
	if(it_ != ids_->end()) {
		ostringstream id_oss;
		id_oss << it_->raw() << "/" << container_->getName();
		string identifier(id_oss.str());

		// Look up the document in the cache
		XmlDocument found;
		found = minder->findDocument(identifier);

		if(found.isNull()) {
			int err = ((Container*)container_)->getDocument(
				oc, *it_, found, qc.getFlags());
			// The document should be found
			DBXML_ASSERT(err == 0);

			// Store the document in the cache
			((Document&)found).setIdentifier(identifier);
			minder->addDocument(found);
		}

		result = ((DbXmlFactoryImpl*)context->getPathanFactory())->createNode(found, context);
		++it_;
	}

	return result;
}

std::string QueryPlanFunction::QueryPlanResult::asString(DynamicContext *context, int indent) const
{
	std::ostringstream oss;
	std::string in(getIndent(indent));

	oss << in << "<qpf_queryplan/>" << std::endl;

	return oss.str();
}

QueryPlanFunction::JITCompileResult::JITCompileResult(
	const QueryPlanFunction *func, int flags, DynamicContext *context)
	: ResultImpl(context),
	  flags_(flags),
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
			currentUri = func_->getUriArg(context);
			DbXmlUri uri(context->getBaseURI(),
				currentUri,
				func_->type_ == DOCUMENT);

			if(uri.isValid()) {
				if(uri.getDocumentName().empty() &&
					func_->type_ == DOCUMENT) {
					ostringstream oss;
					oss << "The URI '" <<
						uri.getResolvedUri() <<
						"' does not specifiy a document";
					throw XmlException(XmlException::DOCUMENT_NOT_FOUND, oss.str());
				}

				container_ =
					new XmlContainer(
						uri.openContainer(
							qc.getManager(),
							oc.txn()));
				t_cont = (TransactedContainer*)
					(Container*)*container_;

				// If this is a document lookup, wrap
				// the query plan in
				// a document name index lookup
				if(func_->type_ == DOCUMENT) {
					qp = new (context->getMemoryManager()) DocumentQP(uri.getDocumentName(), qp, context->getMemoryManager());
				}
			}
		}

		if(t_cont != 0) {
			// Resolve the values in the query plan and
			// generate the optimised query plan
			bool success = true;
			qp = qp->createExecutableQueryPlan(oc.txn(), *t_cont,
				context, /*nodeQP*/false, /*lookupValues*/true, success);

			if(success)
				results_ = new QueryPlanResult(qp, t_cont,
							       flags_, context);
		}

		if(results_.isNull()) {
			if(currentUri == 0) currentUri =
				func_->getUriArg(context);

			// If we haven't managed to generate a query plan, use
			// the default behaviour
			if(func_->type_ == QueryPlanFunction::COLLECTION) {
				results_ =
					context->resolveCollection(currentUri);
			} else {
				results_ = context->resolveDocument(currentUri);
			}
		}
	}

	Item::Ptr item = results_.next(context);

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
