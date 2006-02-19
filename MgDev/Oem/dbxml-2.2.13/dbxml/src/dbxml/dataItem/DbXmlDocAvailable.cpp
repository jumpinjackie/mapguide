//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlDocAvailable.cpp,v 1.3 2005/09/08 09:45:42 jsnelson Exp $
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

#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/FunctionException.hpp>

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

DbXmlDocAvailable::DbXmlDocAvailable(ReferenceMinder &minder, const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
	: DataItemFunction(name, 1, 1, "string?", args, memMgr),
	  qp_(0),
	  invalid_(false),
	  container_(0),
	  minder_(minder)
{
	_src.availableDocumentsUsed(true);
}

DataItem* DbXmlDocAvailable::staticResolution(StaticContext* context)
{
	_src.availableDocumentsUsed(true);
	return resolveDataItems(_args, context, false);
}

Sequence DbXmlDocAvailable::collapseTreeInternal(DynamicContext* context, int flags) const
{
	if(invalid_) {
		return Sequence(context->getPathanFactory()->createBoolean(false, context), context->getMemoryManager());
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

		if(uri.isValid()) {
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
				bool success = true;
				qp = qp->createExecutableQueryPlan(
					oc.txn(), *container, context,
					/*nodeQP*/false, /*lookupValues*/true,
					success);
				DBXML_ASSERT(success); // Should always succeed
			} else {
				// Invalid
				return Sequence(context->getPathanFactory()->createBoolean(false, context), context->getMemoryManager());
			}
		}
	}

	if(qp) {
		// Look up the document in the database
		QueryExecutionContext &qec = qc.getQueryExecutionContext();
		qec.setContainer(*container);
		qec.setCostToFilterFunction(0);
		IDS::SharedPtr ids = qp->execute(oc, qec);

		return Sequence(context->getPathanFactory()->createBoolean(!ids->empty(), context), context->getMemoryManager());
	}

	// Revert to the default behaviour
	if(currentUri == 0) currentUri = getUriArg(context);

	bool bSuccess=false;
	try {
		Sequence seq = context->resolveDocument(currentUri);
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
	return Sequence(context->getPathanFactory()->createBoolean(bSuccess, context), context->getMemoryManager());
}

void DbXmlDocAvailable::generateQueryPlan(XmlManager &mgr, Transaction *txn, DynamicContext *context)
{
	if(_args[0]->isConstant()) {
		const XMLCh* currentUri = getUriArg(context);
		DbXmlUri uri(context->getBaseURI(),
			currentUri, /*documentUri*/true);

		if(uri.isValid()) {
			if(uri.getDocumentName() != "") {
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
				bool success = true;
				qp_ = qp_->createExecutableQueryPlan(
					txn, *container_, context,
					/*nodeQP*/false, /*lookupValues*/false,
					success);
				DBXML_ASSERT(success); // Should always succeed
			} else {
				invalid_ = true;
			}
		}
	}
}

const XMLCh *DbXmlDocAvailable::getUriArg(DynamicContext *context) const
{
	const XMLCh *currentUri = 0;

	Item::Ptr arg = getParamNumber(1, context).next(context);
	if(arg != NULLRCP) {
		// Get the uri argument
		currentUri = arg->asString(context);
	}

	try {
		context->getPathanFactory()->
			createAnyURI(currentUri, context);
	} catch(DSLException &e) {
		DSLthrow(FunctionException,
			X("QueryPlanFunction::getUriArg"),
			X("Invalid URI format [err:FODC0005]"));
	}

	return currentUri;
}

