//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: MetaDataFunction.cpp,v 1.17 2005/11/30 17:12:15 jsnelson Exp $
//

#include "../DbXmlInternal.hpp"
#include <sstream>

#include "MetaDataFunction.hpp"
#include "DbXmlContext.hpp"
#include "DbXmlNodeImpl.hpp"
#include "../QueryContext.hpp"

#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/exceptions/IllegalArgumentException.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/dom/DOMNode.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;

const XMLCh MetaDataFunction::name[] = {
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
	XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

/* http://www.sleepycat.com/2002/dbxml */
const XMLCh MetaDataFunction::XMLChFunctionURI[] = {
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p,
	XERCES_CPP_NAMESPACE_QUALIFIER chColon,
	XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,
	XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,
	XERCES_CPP_NAMESPACE_QUALIFIER chPeriod,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_y,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
	XERCES_CPP_NAMESPACE_QUALIFIER chPeriod,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m,
	XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,
	XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2,
	XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0,
	XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0,
	XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2,
	XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l,
	XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

MetaDataFunction::MetaDataFunction(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
	: DataItemFunction(name, 1, 2, "string?, node()", args, memMgr)
{
	_fURI = XMLChFunctionURI;
}

DataItem* MetaDataFunction::staticResolution(StaticContext* context)
{
	if(_args.size() == 1) {
		_src.contextItemUsed(true);
	}
	_src.getStaticType().flags = StaticType::NUMERIC_TYPE | StaticType::OTHER_TYPE;
	return resolveDataItems(_args, context, _args.size() != 1);
}

Result MetaDataFunction::createResult(DynamicContext* context, int flags) const
{
	return new MetaDataResult(this, flags, context);
}

bool MetaDataFunction::getConstantQNameArg(const XMLCh *&uri, const XMLCh *&name, DynamicContext *context) const
{
	if(!_args[0]->isConstant()) {
		return false;
	}

	// Get the string name argument
	Item::Ptr name_item = getParamNumber(1, context).next(context);

	// Convert the string to a qname, thus checking it's syntax
	// and resolving it's prefix in one go
	AnyAtomicType::Ptr qname;
	try {
		qname = context->getPathanFactory()->
			createDerivedFromAtomicType(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
				SchemaSymbols::fgDT_QNAME, name_item->asString(context), context);
	}
	catch(XPath2ErrorException &e) {
		return false;
	}
	catch(::IllegalArgumentException &e) {
		return false;
	}

	const ATQNameOrDerived *qname_impl = (const ATQNameOrDerived*)qname.get();

	uri = qname_impl->getURI();
	name = qname_impl->getName();

	return true;
}

MetaDataFunction::MetaDataResult::MetaDataResult(const MetaDataFunction *func, int flags, DynamicContext *context)
	: SingleResult(context),
	  flags_(flags),
	  func_(func)
{
}

Item::Ptr MetaDataFunction::MetaDataResult::getSingleResult(DynamicContext *context) const
{
	// Get the string name argument
	Item::Ptr name = func_->getParamNumber(1, context).next(context);

	// Get the node argument
	Item::Ptr node = 0;
	if(func_->getNumArgs() == 1) {
		// node argument is the context item
		node = context->getContextItem();
		if(node == NULLRCP || !node->isNode()) {
			DSLthrow(FunctionException,X("MetaDataFunction::MetaDataResult::getSingleResult"),
				 X("The context item is not a node in function dbxml:metadata [err:FODC0001]"));
		}
	} else {
		// node argument is given
		node = func_->getParamNumber(2, context).next(context);
	}

	// Convert the string to a qname, thus checking it's syntax
	// and resolving it's prefix in one go
	AnyAtomicType::Ptr qname;
	try {
		// check for bad name.  0 args are OK, they are not
		// used.  This throw ends up in the catch, several lines down
		if (name.isNull())
			throw XPath2ErrorException(0, 0, 0, 0);

		qname = context->getPathanFactory()->
			createDerivedFromAtomicType(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
				SchemaSymbols::fgDT_QNAME, name->asString(context), context);
	}
	catch(XPath2ErrorException &e) {
		DSLthrow(FunctionException,X("MetaDataFunction::MetaDataResult::getSingleResult"),
			 X("Invalid QName as first argument to function dbxml:metadata"));
	}
	catch(::IllegalArgumentException &e) {
		DSLthrow(FunctionException,X("MetaDataFunction::MetaDataResult::getSingleResult"),
			 X("No namespace binding for prefix in first argument to function dbxml:metadata"));
	}

	// Lookup the metadata
	const DbXmlNodeImpl *nodeImpl = (const DbXmlNodeImpl*)node->getInterface(DbXmlNodeImpl::gDbXml);
	DBXML_ASSERT(nodeImpl != 0);

	const ATQNameOrDerived *qname_impl = (const ATQNameOrDerived*)qname.get();
	return nodeImpl->getMetaData(qname_impl->getURI(), qname_impl->getName(), context);
}

std::string MetaDataFunction::MetaDataResult::asString(DynamicContext *context, int indent) const
{
	std::ostringstream oss;
	std::string in(getIndent(indent));

	oss << in << "<metadata/>" << std::endl;

	return oss.str();
}
