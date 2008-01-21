//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlDocAvailable.cpp,v 1.16 2006/11/17 18:18:28 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include <dbxml/XmlTransaction.hpp>
#include "DbXmlDocAvailable.hpp"
#include "DbXmlContext.hpp"
#include "../QueryContext.hpp"
#include "../Manager.hpp"
#include "../QueryPlan.hpp"
#include "../Transaction.hpp"
#include "../ReferenceMinder.hpp"
#include "../OperationContext.hpp"
#include "../QueryExecutionContext.hpp"
#include "DbXmlUri.hpp"
#include "DbXmlNodeImpl.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>

#include <xercesc/util/XMLString.hpp>
#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

const XMLCh DbXmlDocAvailable::name[] = {
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_D,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_X,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_D,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_A,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_v,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e,
	XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

DbXmlDocAvailable::DbXmlDocAvailable(ReferenceMinder &minder, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
	: XQFunction(name, 1, 1, "string?", args, memMgr),
	  qp_(0),
	  invalid_(false),
	  container_(0),
	  minder_(minder)
{
	staticTyping(0);
}

ASTNode* DbXmlDocAvailable::staticResolution(StaticContext* context)
{
	for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
		*i = (*i)->staticResolution(context);
	}
	return this;
}

ASTNode* DbXmlDocAvailable::staticTyping(StaticContext* context)
{
	for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
		if(context != 0)
			*i = (*i)->staticTyping(context);
		_src.add((*i)->getStaticResolutionContext());
	}

	_src.getStaticType().flags = StaticType::BOOLEAN_TYPE;
	_src.availableDocumentsUsed(true);

	return this;
}

Sequence DbXmlDocAvailable::collapseTreeInternal(DynamicContext* context, int flags) const
{
	if(invalid_) {
		return Sequence(context->getItemFactory()->createBoolean(false, context), context->getMemoryManager());
	}

	DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);
	QueryContext &qc = dbContext->getQueryContext();
	OperationContext &oc = qc.getOperationContext();

	XmlContainer containerWrapper;
	TransactedContainer *container = container_;

	const XMLCh* currentUri = 0;
	QueryPlan *qp = qp_;
	if(qp == 0) {
		currentUri = getUriArg(context);
		DbXmlUri uri(context->getBaseURI(),
			currentUri, /*documentUri*/true);

		if(uri.isDbXmlScheme()) {
			if(uri.getDocumentName() != "") {
				containerWrapper = uri.openContainer(qc.getManager(), oc.txn());
				container = (TransactedContainer*)(Container*)containerWrapper;

				// This is a document lookup - wrap the query
				// plan in a document name index lookup
				qp = new (context->getMemoryManager())
					DocumentQP(uri.getDocumentName(),
						0, context->getMemoryManager());

				// Resolve the values in the query plan and
				// generate the optimised query plan
				bool fullyOptimised, exact;
				qp = qp->createPartiallyOptimisedQueryPlan(
					oc.txn(), *container, dbContext,
					/*nodeQP*/false, fullyOptimised, exact);
				DBXML_ASSERT(fullyOptimised);
			} else {
				// Invalid
				return Sequence(context->getItemFactory()->createBoolean(false, context), context->getMemoryManager());
			}
		}
	}

	if(qp) {
		// Look up the document in the database
		QueryExecutionContext &qec = *qc.getQueryExecutionContext();
		qec.setContainer(*container);
		qec.setCostToFilterFunction(0);
		qec.setDbXmlContext(dbContext);
		IDS::Ptr ids = qp->execute(oc, qec);

		return Sequence(context->getItemFactory()->createBoolean(!ids->empty(), context), context->getMemoryManager());
	}

	// Revert to the default behaviour
	if(currentUri == 0) currentUri = getUriArg(context);

	bool bSuccess=false;
	try {
		Sequence seq = context->resolveDocument(currentUri, this);
		if(!seq.isEmpty()) {
			// Force the lazy DbXmlNodeImpl to parse the document,
			// to check that it actually exists and is valid.
			const Item *item = seq.first();
			const DbXmlNodeImpl *impl = (const DbXmlNodeImpl*)item->getInterface(DbXmlNodeImpl::gDbXml);
			DBXML_ASSERT(impl);
			impl->getDOMNode();
			bSuccess = true;
		}
	}
	catch(...) {
		bSuccess = false;
	}
	return Sequence(context->getItemFactory()->createBoolean(bSuccess, context), context->getMemoryManager());
}

void DbXmlDocAvailable::generateQueryPlan(XmlManager &mgr, Transaction *txn, DynamicContext *context)
{
	if(_args[0]->isConstant()) {
		const XMLCh* currentUri = getUriArg(context);
		DbXmlUri uri(context->getBaseURI(),
			currentUri, /*documentUri*/true);

		if(uri.isDbXmlScheme()) {
			if(uri.getDocumentName() != "") {
				DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);

				XmlContainer containerWrapper(
					uri.openContainer(mgr, txn));
				container_ = (TransactedContainer*)
					(Container*)containerWrapper;
				minder_.addContainer(container_);

				// This is a document lookup - wrap the query
				// plan in a document name index lookup
				qp_ = new (getMemoryManager())
					DocumentQP(uri.getDocumentName(),
						0, getMemoryManager());

				// Resolve the values in the query plan and
				// generate the optimised query plan
				bool fullyOptimised, exact;
				qp_ = qp_->createPartiallyOptimisedQueryPlan(
					txn, *container_, dbContext,
					/*nodeQP*/false, fullyOptimised, exact);
				DBXML_ASSERT(fullyOptimised);
			} else {
				invalid_ = true;
			}
		}
	}
}

const XMLCh *DbXmlDocAvailable::getUriArg(DynamicContext *context) const
{
	const XMLCh *currentUri = 0;

	Item::Ptr arg = getParamNumber(1, context)->next(context);
	if(arg != NULLRCP) {
		// Get the uri argument
		currentUri = arg->asString(context);
	}

	try {
		context->getItemFactory()->
			createAnyURI(currentUri, context);
	} catch(XQException &e) {
		XQThrow(FunctionException,
			X("QueryPlanFunction::getUriArg"),
			X("Invalid URI format [err:FODC0005]"));
	}

	return currentUri;
}

