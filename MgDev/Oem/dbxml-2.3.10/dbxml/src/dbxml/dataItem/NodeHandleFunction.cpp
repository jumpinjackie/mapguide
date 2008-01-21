//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NodeHandleFunction.cpp,v 1.3.2.1 2007/01/19 14:56:01 jsnelson Exp $
//

#include "../DbXmlInternal.hpp"
#include <sstream>

#include "NodeHandleFunction.hpp"
#include "DbXmlContext.hpp"
#include "DbXmlNodeImpl.hpp"
#include "DbXmlUri.hpp"
#include "DbXmlFactoryImpl.hpp"
#include "../QueryContext.hpp"
#include "../UTF8.hpp"
#include "../OperationContext.hpp"
#include "../ReferenceMinder.hpp"
#include "../TransactedContainer.hpp"

#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/dom/DOMNode.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

const XMLCh NodeToHandleFunction::name[] = {
	chLatin_n,
	chLatin_o,
	chLatin_d,
	chLatin_e,
	chDash,
	chLatin_t,
	chLatin_o,
	chDash,
	chLatin_h,
	chLatin_a,
	chLatin_n,
	chLatin_d,
	chLatin_l,
	chLatin_e,
	chNull
};

const unsigned int NodeToHandleFunction::minArgs = 0;
const unsigned int NodeToHandleFunction::maxArgs = 1;

NodeToHandleFunction::NodeToHandleFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
	: DbXmlFunction(name, minArgs, maxArgs, "node()", args, memMgr)
{
	_fURI = XMLChFunctionURI;
}

ASTNode* NodeToHandleFunction::staticResolution(StaticContext* context)
{
	return resolveArguments(context);
}

ASTNode* NodeToHandleFunction::staticTyping(StaticContext* context)
{
	if(_args.size() == 0) {
		_src.contextItemUsed(true);
	}
	_src.getStaticType().flags = StaticType::STRING_TYPE;
	return calculateSRCForArguments(context);
}

Result NodeToHandleFunction::createResult(DynamicContext* context, int flags) const
{
	return new NodeToHandleResult(this);
}

NodeToHandleFunction::NodeToHandleResult::NodeToHandleResult(const NodeToHandleFunction *func)
	: SingleResult(func),
	  func_(func)
{
}

Item::Ptr NodeToHandleFunction::NodeToHandleResult::getSingleResult(DynamicContext *context) const
{
	// Get the node argument
	Item::Ptr node = 0;
	if(func_->getNumArgs() == 0) {
		// node argument is the context item
		node = context->getContextItem();
		if(node == NULLRCP || !node->isNode()) {
			XQThrow(FunctionException,X("NodeToHandleFunction::NodeToHandleResult::getSingleResult"),
				 X("The context item is not a node in function dbxml:node-handle [err:FODC0001]"));
		}
	} else {
		// node argument is given
		node = func_->getParamNumber(1, context)->next(context);
	}

	const DbXmlNodeImpl *nodeImpl = (const DbXmlNodeImpl*)node->getInterface(DbXmlNodeImpl::gDbXml);
	DBXML_ASSERT(nodeImpl != 0);

	string handle = nodeImpl->getNodeHandle();

	return context->getItemFactory()->createString(UTF8ToXMLCh(handle).str(), context);
}

std::string NodeToHandleFunction::NodeToHandleResult::asString(DynamicContext *context, int indent) const
{
	std::ostringstream oss;
	std::string in(getIndent(indent));

	oss << in << "<node-to-handle/>" << std::endl;

	return oss.str();
}

const XMLCh HandleToNodeFunction::name[] = {
	chLatin_h,
	chLatin_a,
	chLatin_n,
	chLatin_d,
	chLatin_l,
	chLatin_e,
	chDash,
	chLatin_t,
	chLatin_o,
	chDash,
	chLatin_n,
	chLatin_o,
	chLatin_d,
	chLatin_e,
	chNull
};

const unsigned int HandleToNodeFunction::minArgs = 2;
const unsigned int HandleToNodeFunction::maxArgs = 2;

HandleToNodeFunction::HandleToNodeFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
	: DbXmlFunction(name, minArgs, maxArgs, "string, string", args, memMgr),
	  container_(0)
{
	_fURI = XMLChFunctionURI;
}

ASTNode* HandleToNodeFunction::staticResolution(StaticContext* context)
{
	return resolveArguments(context);
}

ASTNode* HandleToNodeFunction::staticTyping(StaticContext* context)
{
	_src.availableCollectionsUsed(true);
	_src.getStaticType().flags = StaticType::NODE_TYPE;
	_src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED |
		StaticResolutionContext::SUBTREE | StaticResolutionContext::ONENODE);

	calculateSRCForArguments(context);

	AutoDelete<DbXmlContext> dContext(CAST_TO_DBXMLCONTEXT(context->createDynamicContext()));
	dContext->setMemoryManager(context->getMemoryManager());

	container_ = getContainerArg(dContext, /*lookup*/false);

	return this;
}

TransactedContainer *HandleToNodeFunction::getContainerArg(DbXmlContext *context, bool lookup) const
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

Result HandleToNodeFunction::createResult(DynamicContext* context, int flags) const
{
	return new HandleToNodeResult(this);
}

HandleToNodeFunction::HandleToNodeResult::HandleToNodeResult(const HandleToNodeFunction *func)
	: SingleResult(func),
	  func_(func)
{
}

Item::Ptr HandleToNodeFunction::HandleToNodeResult::getSingleResult(DynamicContext *context) const
{
	try {
		DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);

		TransactedContainer *container = func_->getContainerArg(dbContext, /*lookup*/true);

		Item::Ptr handleItem = func_->getParamNumber(2, context)->next(context);
		XMLChToUTF8 handle(handleItem->asString(context));

		IndexEntry::Ptr ie(new IndexEntry);
		ie->setFromNodeHandle(handle.str());

		return ((DbXmlFactoryImpl*)context->getItemFactory())->
			createNode(ie, container, 0, context);
	}
	catch(XmlException &ex) {
		ex.setLocationInfo(this);
		throw;
	}
}

std::string HandleToNodeFunction::HandleToNodeResult::asString(DynamicContext *context, int indent) const
{
	std::ostringstream oss;
	std::string in(getIndent(indent));

	oss << in << "<handle-to-node/>" << std::endl;

	return oss.str();
}

