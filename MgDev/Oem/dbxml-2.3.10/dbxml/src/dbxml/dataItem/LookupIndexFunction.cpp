//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: LookupIndexFunction.cpp,v 1.16.2.1 2007/01/19 14:56:01 jsnelson Exp $
//

#include "../DbXmlInternal.hpp"

#include "LookupIndexFunction.hpp"
#include "DbXmlContext.hpp"
#include "DbXmlUri.hpp"
#include "DbXmlFactoryImpl.hpp"
#include "DbXmlNodeImpl.hpp"
#include "../QueryContext.hpp"
#include "../UTF8.hpp"
#include "../OperationContext.hpp"
#include "../TransactedContainer.hpp"
#include "../ReferenceMinder.hpp"
#include "../QueryExecutionContext.hpp"
#include "../QueryPlan.hpp"
#include "../TransactedContainer.hpp"
#include "../nodeStore/NsUtil.hpp"

#include <dbxml/XmlDocument.hpp>

#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

const XMLCh LookupIndexFunction::name[] = {
	chLatin_l,
	chLatin_o,
	chLatin_o,
	chLatin_k,
	chLatin_u,
	chLatin_p,
	chDash,
	chLatin_i,
	chLatin_n,
	chLatin_d,
	chLatin_e,
	chLatin_x,
	chNull
};

const unsigned int LookupIndexFunction::minArgs = 2;
const unsigned int LookupIndexFunction::maxArgs = 3;

// Args are: containerName, node QName, parent QName

LookupIndexFunction::LookupIndexFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
	: DbXmlFunction(name, minArgs, maxArgs, "string, string, string", args, memMgr),
	  childURIName_(0),
	  parentURIName_(0)
{
	_fURI = XMLChFunctionURI;
}

LookupIndexFunction::LookupIndexFunction(const XMLCh* fname, unsigned int argsFrom, unsigned int argsTo,
	const char* paramDecl, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
	: DbXmlFunction(fname, argsFrom, argsTo, paramDecl, args, memMgr),
	  childURIName_(0),
	  parentURIName_(0)
{
	_fURI = XMLChFunctionURI;
}

ASTNode* LookupIndexFunction::staticResolution(StaticContext* context)
{
	return resolveArguments(context);
}

ASTNode* LookupIndexFunction::staticTyping(StaticContext* context)
{
	XPath2MemoryManager *mm = context->getMemoryManager();

	_src.clear();

	_src.availableCollectionsUsed(true);
	_src.getStaticType().flags = StaticType::ELEMENT_TYPE;
	_src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED |
		StaticResolutionContext::SUBTREE);

	calculateSRCForArguments(context);

	AutoDelete<DbXmlContext> dContext(CAST_TO_DBXMLCONTEXT(context->createDynamicContext()));
	dContext->setMemoryManager(mm);

	container_ = getContainerArg(dContext, /*lookup*/false);
	childURIName_ = getURINameArg(2, dContext, /*lookup*/false);
	if(_args.size() == 3) {
		parentURIName_ = getURINameArg(3, dContext, /*lookup*/false);
	}

	if(childURIName_ != 0 && (_args.size() == 2 || parentURIName_ != 0)) {
		qp_ = createQueryPlan(dContext, /*lookup*/false);

		if(qp_ != 0 && container_ != 0) {
			QueryContext &qc = dContext->getQueryContext();
			OperationContext &oc = qc.getOperationContext();
			generateQueryPlan(container_, oc.txn(), dContext);

			LookupIndex *result = new (mm) LookupIndex(this, mm);
			result->setLocationInfo(this);
			return result;
		}
	}

	return this;
}

TransactedContainer *LookupIndexFunction::getContainerArg(DbXmlContext *context, bool lookup) const
{
	if(container_ != 0) return container_;

	if(!_args[0]->isConstant() && !lookup) return 0;

	QueryContext &qc = context->getQueryContext();

	Item::Ptr containerName = getParamNumber(1, context)->next(context);

	try {
		XmlContainer container = DbXmlUri::openContainer(XMLChToUTF8(containerName->asString(context)).str(),
			qc.getManager(), qc.getOperationContext().txn());

		TransactedContainer *tcont = (TransactedContainer*)((Container*)container);
		qc.getMinder()->addContainer(tcont);
		return tcont;
	}
	catch(XmlException &e) {
		e.setLocationInfo(this);
		throw;
	}

	// Never reached
	return 0;
}

const char *LookupIndexFunction::getURINameArg(unsigned int argNum, DbXmlContext *context, bool lookup) const
{
	XPath2MemoryManager *mm = context->getMemoryManager();

	if(!_args[argNum - 1]->isConstant() && !lookup) return 0;

	const XMLCh *uri, *name;
	if(lookup) {
		getQNameArg(argNum, uri, name, context);
	} else if(!getConstantQNameArg(argNum, uri, name, context)) {
		return 0;
	}

	Name cname(XMLChToUTF8(uri).str(), XMLChToUTF8(name).str());
	return (const char*)NsUtil::nsStringDup(mm, (const xmlbyte_t *)cname.getURIName().c_str(), 0);
}

QueryPlan *LookupIndexFunction::createQueryPlan(DbXmlContext *context, bool lookup) const
{
	XPath2MemoryManager *mm = context->getMemoryManager();

	const char *child = childURIName_;
	if(child == 0) {
		child = getURINameArg(2, context, lookup);
		if(child == 0) return 0;
	}

	const char *parent = 0;
	if(_args.size() == 3) {
		parent = parentURIName_;
		if(parent == 0) {
			parent = getURINameArg(3, context, lookup);
			if(parent == 0) return 0;
		}
	}

	return new (mm) PresenceQP(ImpliedSchemaNode::CHILD, parent, child, mm);
}

Result LookupIndexFunction::createResult(DynamicContext* context, int flags) const
{
	return new LookupIndexFunctionResult(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const XMLCh LookupAttributeIndexFunction::name[] = {
	chLatin_l,
	chLatin_o,
	chLatin_o,
	chLatin_k,
	chLatin_u,
	chLatin_p,
	chDash,
	chLatin_a,
	chLatin_t,
	chLatin_t,
	chLatin_r,
	chLatin_i,
	chLatin_b,
	chLatin_u,
	chLatin_t,
	chLatin_e,
	chDash,
	chLatin_i,
	chLatin_n,
	chLatin_d,
	chLatin_e,
	chLatin_x,
	chNull
};

const unsigned int LookupAttributeIndexFunction::minArgs = 2;
const unsigned int LookupAttributeIndexFunction::maxArgs = 3;

// Args are: containerName, node QName, parent QName

LookupAttributeIndexFunction::LookupAttributeIndexFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
	: LookupIndexFunction(name, minArgs, maxArgs, "string, string, string", args, memMgr)
{
}

ASTNode* LookupAttributeIndexFunction::staticTyping(StaticContext* context)
{
	XPath2MemoryManager *mm = context->getMemoryManager();

	_src.clear();

	_src.availableCollectionsUsed(true);
	_src.getStaticType().flags = StaticType::ATTRIBUTE_TYPE;
	_src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED |
		StaticResolutionContext::SUBTREE);

	calculateSRCForArguments(context);

	AutoDelete<DbXmlContext> dContext(CAST_TO_DBXMLCONTEXT(context->createDynamicContext()));
	dContext->setMemoryManager(mm);

	container_ = getContainerArg(dContext, /*lookup*/false);
	childURIName_ = getURINameArg(2, dContext, /*lookup*/false);
	if(_args.size() == 3) {
		parentURIName_ = getURINameArg(3, dContext, /*lookup*/false);
	}

	if(childURIName_ != 0 && (_args.size() == 2 || parentURIName_ != 0)) {
		qp_ = createQueryPlan(dContext, /*lookup*/false);

		if(qp_ != 0 && container_ != 0) {
			QueryContext &qc = dContext->getQueryContext();
			OperationContext &oc = qc.getOperationContext();
			generateQueryPlan(container_, oc.txn(), dContext);

			LookupIndex *result = new (mm) LookupIndex(this, mm);
			result->setLocationInfo(this);
			return result;
		}
	}

	return this;
}

QueryPlan *LookupAttributeIndexFunction::createQueryPlan(DbXmlContext *context, bool lookup) const
{
	XPath2MemoryManager *mm = context->getMemoryManager();

	const char *child = childURIName_;
	if(child == 0) {
		child = getURINameArg(2, context, lookup);
		if(child == 0) return 0;
	}

	const char *parent = 0;
	if(_args.size() == 3) {
		parent = parentURIName_;
		if(parent == 0) {
			parent = getURINameArg(3, context, lookup);
			if(parent == 0) return 0;
		}
	}

	return new (mm) PresenceQP(ImpliedSchemaNode::ATTRIBUTE, parent, child, mm);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const XMLCh LookupMetaDataIndexFunction::name[] = {
	chLatin_l,
	chLatin_o,
	chLatin_o,
	chLatin_k,
	chLatin_u,
	chLatin_p,
	chDash,
	chLatin_m,
	chLatin_e,
	chLatin_t,
	chLatin_a,
	chLatin_d,
	chLatin_a,
	chLatin_t,
	chLatin_a,
	chDash,
	chLatin_i,
	chLatin_n,
	chLatin_d,
	chLatin_e,
	chLatin_x,
	chNull
};

const unsigned int LookupMetaDataIndexFunction::minArgs = 2;
const unsigned int LookupMetaDataIndexFunction::maxArgs = 2;

// Args are: containerName, name QName

LookupMetaDataIndexFunction::LookupMetaDataIndexFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
	: LookupIndexFunction(name, minArgs, maxArgs, "string, string", args, memMgr)
{
}

ASTNode* LookupMetaDataIndexFunction::staticTyping(StaticContext* context)
{
	XPath2MemoryManager *mm = context->getMemoryManager();

	_src.clear();

	_src.availableCollectionsUsed(true);
	_src.getStaticType().flags = StaticType::DOCUMENT_TYPE;
	_src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED |
		StaticResolutionContext::SUBTREE);

	calculateSRCForArguments(context);

	AutoDelete<DbXmlContext> dContext(CAST_TO_DBXMLCONTEXT(context->createDynamicContext()));
	dContext->setMemoryManager(mm);

	container_ = getContainerArg(dContext, /*lookup*/false);
	childURIName_ = getURINameArg(2, dContext, /*lookup*/false);

	if(childURIName_ != 0) {
		qp_ = createQueryPlan(dContext, /*lookup*/false);

		if(qp_ != 0 && container_ != 0) {
			QueryContext &qc = dContext->getQueryContext();
			OperationContext &oc = qc.getOperationContext();
			generateQueryPlan(container_, oc.txn(), dContext);

			LookupIndex *result = new (mm) LookupIndex(this, mm);
			result->setLocationInfo(this);
			return result;
		}
	}

	return this;
}

QueryPlan *LookupMetaDataIndexFunction::createQueryPlan(DbXmlContext *context, bool lookup) const
{
	XPath2MemoryManager *mm = context->getMemoryManager();

	const char *child = childURIName_;
	if(child == 0) {
		child = getURINameArg(2, context, lookup);
		if(child == 0) return 0;
	}

	return new (mm) PresenceQP(ImpliedSchemaNode::METADATA, 0, child, mm);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Item::Ptr LookupIndexFunction::LookupIndexFunctionResult::next(DynamicContext *context)
{
	if(result_.isNull()) {
		if(func_->getQueryPlan() == 0) {
			DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);
			QueryContext &qc = dbContext->getQueryContext();
			OperationContext &oc = qc.getOperationContext();

			TransactedContainer *container = func_->getContainerArg(dbContext, /*lookup*/true);

			setQueryPlan(func_->createQueryPlan(dbContext, /*lookup*/true));
			generateQueryPlan(container, oc.txn(), dbContext);
			result_ = new LookupIndexResult(this, func_);
		} else {
			result_ = new LookupIndexResult(func_, func_);
		}
	}

	return result_->next(context);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LookupIndex::LookupIndex(const QueryPlanHolder *qph, XPath2MemoryManager *mm)
	: DbXmlASTNode(LOOKUP_INDEX, mm)
{
	setQueryPlanHolder(qph);
	staticTyping(0);
}

ASTNode *LookupIndex::staticTyping(StaticContext *context)
{
	_src.clear();

	_src.availableCollectionsUsed(true);

	if(qp_ != 0) {
		if(context != 0)
			qp_ = qp_->staticTyping(context);
		_src.copy(qp_->getStaticResolutionContext());
		_src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED |
			StaticResolutionContext::SUBTREE);
		if(_src.getStaticType().isType(StaticType::DOCUMENT_TYPE)) {
			_src.setProperties(_src.getProperties() | StaticResolutionContext::PEER);
		}
	}

	return this;
}

DbXmlResult LookupIndex::createResult(const DbXmlResult &contextItems, unsigned int props,
	DynamicContext *context) const
{
	return new LookupIndexResult(this, this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void LookupIndexResult::init(DynamicContext *context)
{
	toDo_ = false;

	DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);
	QueryContext &qc = dbContext->getQueryContext();
	OperationContext &oc = qc.getOperationContext();

	QueryExecutionContext &qec = *qc.getQueryExecutionContext();
	qec.setContainer(*holder_->getContainer());
	qec.setDbXmlContext(dbContext);
	qec.setCostToFilterFunction(0);

	bool exact;
	holder_->executeQueryPlan(data_, oc, qec, exact);

	if(data_) it_ = data_->begin();
}

Item::Ptr LookupIndexResult::next(DynamicContext *context)
{
	context->testInterrupt();

	if(toDo_) init(context);

	if(!data_ || it_ == data_->end()) return 0;

	Item::Ptr result = ((DbXmlFactoryImpl*)context->getItemFactory())->
		createNode(*it_, holder_->getContainer(), holder_->getDocumentURI(), context);

	++it_;

	// TBD remove this special case - jpcs
	if(holder_->getContainer()->getContainerType() == XmlContainer::WholedocContainer) {
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

Item::Ptr LookupIndexResult::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	context->testInterrupt();

	if(toDo_) init(context);

	if(!data_) return 0;

	// Iterate to the correct position
	IndexData::const_iterator end = data_->end();
	while(it_ != end && ((*it_)->getDocID() < did ||
		      ((*it_)->getDocID() == did && (*it_)->getNodeID().compareNids(&nid) < 0))) {
		++it_;
	}

	return next(context);
}

