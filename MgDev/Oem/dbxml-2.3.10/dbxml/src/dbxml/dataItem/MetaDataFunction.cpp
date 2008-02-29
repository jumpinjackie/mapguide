//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: MetaDataFunction.cpp,v 1.26 2006/10/30 17:45:55 bostic Exp $
//

#include "../DbXmlInternal.hpp"
#include <sstream>

#include "MetaDataFunction.hpp"
#include "DbXmlContext.hpp"
#include "DbXmlNodeImpl.hpp"
#include "../QueryContext.hpp"

#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/dom/DOMNode.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;

const XMLCh MetaDataFunction::name[] = {
	chLatin_m,
	chLatin_e,
	chLatin_t,
	chLatin_a,
	chLatin_d,
	chLatin_a,
	chLatin_t,
	chLatin_a,
	chNull
};

const unsigned int MetaDataFunction::minArgs = 1;
const unsigned int MetaDataFunction::maxArgs = 2;

MetaDataFunction::MetaDataFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
	: DbXmlFunction(name, minArgs, maxArgs, "string, node()", args, memMgr)
{
	_fURI = XMLChFunctionURI;
}

ASTNode* MetaDataFunction::staticResolution(StaticContext* context)
{
	return resolveArguments(context);
}

ASTNode* MetaDataFunction::staticTyping(StaticContext* context)
{
	if(_args.size() == 1) {
		_src.contextItemUsed(true);
	}
	_src.getStaticType().flags = StaticType::ANY_ATOMIC_TYPE;
	return calculateSRCForArguments(context);
}

Result MetaDataFunction::createResult(DynamicContext* context, int flags) const
{
	return new MetaDataResult(this);
}

MetaDataFunction::MetaDataResult::MetaDataResult(const MetaDataFunction *func)
	: SingleResult(func),
	  func_(func)
{
}

Item::Ptr MetaDataFunction::MetaDataResult::getSingleResult(DynamicContext *context) const
{
	// Get the node argument
	Item::Ptr node = 0;
	if(func_->getNumArgs() == 1) {
		// node argument is the context item
		node = context->getContextItem();
		if(node == NULLRCP || !node->isNode()) {
			XQThrow(FunctionException,X("MetaDataFunction::MetaDataResult::getSingleResult"),
				 X("The context item is not a node in function dbxml:metadata [err:FODC0001]"));
		}
	} else {
		// node argument is given
		node = func_->getParamNumber(2, context)->next(context);
	}

	// Resolve the string argument as a QName
	const XMLCh *uri, *name;
	func_->getQNameArg(1, uri, name, context);

	// Lookup the metadata
	const DbXmlNodeImpl *nodeImpl = (const DbXmlNodeImpl*)node->getInterface(DbXmlNodeImpl::gDbXml);
	DBXML_ASSERT(nodeImpl != 0);

	return nodeImpl->getMetaData(uri, name, context);
}

std::string MetaDataFunction::MetaDataResult::asString(DynamicContext *context, int indent) const
{
	std::ostringstream oss;
	std::string in(getIndent(indent));

	oss << in << "<metadata/>" << std::endl;

	return oss.str();
}
