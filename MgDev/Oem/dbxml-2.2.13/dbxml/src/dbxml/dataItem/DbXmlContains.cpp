//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlContains.cpp,v 1.7 2005/12/16 11:35:44 jsnelson Exp $
//

#include "../DbXmlInternal.hpp"
#include "DbXmlContains.hpp"
#include "DbXmlContext.hpp"
#include "DbXmlNav.hpp"

#include <pathan/SequenceType.hpp>
#include <pathan/PathanFactory.hpp>
#include <pathan/dataItem/DataItemContextItem.hpp>
#include <pathan/exceptions/XPath2TypeMatchException.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

DbXmlContains::DbXmlContains(DataItem *arg, XPath2MemoryManager* memMgr)
	: DataItemImpl(memMgr),
	  arg_(arg)
{
	setType((DataItem::whichType)DbXmlContext::DBXML_CONTAINS);
	_src.copy(arg->getStaticResolutionContext());
	_src.getStaticType().flags = StaticType::NODE_TYPE;
	_src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::PEER |
		StaticResolutionContext::SUBTREE | StaticResolutionContext::GROUPED |
		StaticResolutionContext::SAMEDOC | StaticResolutionContext::ONENODE);
}

DataItem* DbXmlContains::staticResolution(StaticContext *context)
{
	DBXML_ASSERT(0);
	return this;
}

Result DbXmlContains::createResult(DynamicContext* context, int flags) const
{
	DBXML_ASSERT(0);
	return 0;
}

Result DbXmlContains::createResult(const Result &contextItems, unsigned int props, DynamicContext* context) const
{
	Result result = contextItems;

	if(qp_ != 0) {
		// Only try to sort if we've got a query plan
		result = DbXmlNav::sortIfPossible(contextItems, props, context);
	}
	if(qp_ != 0 && (props & StaticResolutionContext::DOCORDER)) {
		result = new QueryPlanResultImpl(result, this, context);
	} else if(result.isNull()) {
		result = new DataItemContextItem::ContextItemResult(context);
	}

	return new ContainsResult(result, this, context);
}

DbXmlContains::ContainsResult::ContainsResult(const Result &parent, const DbXmlContains *contains, DynamicContext *context)
	: ResultImpl(context),
	  parent_(parent),
	  contains_(contains)
{
}

Item::Ptr DbXmlContains::ContainsResult::next(DynamicContext *context)
{
	static const SequenceType stringType(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
		SchemaSymbols::fgDT_STRING, SequenceType::QUESTION_MARK);

	Item::Ptr item;
	while((item = parent_.next(context)) != NULLRCP) {
		const XMLCh *container = ((const Node*)item.get())->dmStringValue(context);

		Result arg_result = contains_->getArgument()->collapseTree(context)
			.convertFunctionArg(&stringType, contains_->getArgument()->getStaticResolutionContext().getStaticType(), context);
		Item::Ptr arg_first = arg_result.next(context);

		const XMLCh* pattern = XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString;
		if(arg_first != NULLRCP)
			pattern = arg_first->asString(context);

		if(XMLString::stringLen(pattern)==0)
			return item;
		if(XMLString::stringLen(container)==0)
			continue;

		if(XMLString::patternMatch(container, pattern) > -1)
			return item;
	}

	if(item == NULLRCP) {
		parent_ = 0;
	}

	return 0;
}

void DbXmlContains::ContainsResult::skip()
{
	parent_.skip();
}

string DbXmlContains::ContainsResult::asString(DynamicContext *context, int indent) const
{
	return "<DbXmlContains_ContainsResult/>";
}
