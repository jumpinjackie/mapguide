//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlContains.cpp,v 1.23 2006/10/30 17:45:54 bostic Exp $
//

#include "../DbXmlInternal.hpp"
#include "DbXmlContains.hpp"
#include "DbXmlContext.hpp"
#include "DbXmlNav.hpp"
#include "DbXmlNodeTest.hpp"

#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/ast/XQContextItem.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

DbXmlContains::DbXmlContains(ASTNode *arg, XPath2MemoryManager* memMgr)
	: DbXmlASTNode(DBXML_CONTAINS, memMgr),
	  joinType_(Join::SELF),
	  nodeTest_(0),
	  arg_(arg)
{
	staticTyping(0);
}

DbXmlContains::DbXmlContains(Join::Type join, const DbXmlContains *cnt, XPath2MemoryManager* memMgr)
	: DbXmlASTNode(DBXML_CONTAINS, memMgr),
	  joinType_(join),
	  nodeTest_(cnt->nodeTest_),
	  arg_(cnt->arg_)
{
	setQueryPlanHolder(cnt);
	staticTyping(0);
}

DbXmlContains::DbXmlContains(Join::Type join, DbXmlNodeTest *nodeTest, const DbXmlContains *cnt,
	XPath2MemoryManager* memMgr)
	: DbXmlASTNode(DBXML_CONTAINS, memMgr),
	  joinType_(join),
	  nodeTest_(nodeTest),
	  arg_(cnt->arg_)
{
	setQueryPlanHolder(cnt);
	staticTyping(0);
}

ASTNode *DbXmlContains::staticTyping(StaticContext *context)
{
	DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);

	_src.clear();

	if(context != 0)
		arg_ = arg_->staticTyping(context);
	_src.add(arg_->getStaticResolutionContext());
	_src.contextItemUsed(true);
	_src.setProperties(Join::getJoinTypeProperties(joinType_));

	if((context == 0 || dbContext->runQueryPlans()) && qp_ != 0) {
		if(context != 0)
			qp_ = qp_->staticTyping(context);
		_src.add(qp_->getStaticResolutionContext());
	}

	if(nodeTest_ != 0) {
		bool isExact;
		nodeTest_->getStaticType(_src.getStaticType(), context, isExact, this);
	} else {
		_src.getStaticType().flags = StaticType::NODE_TYPE;
	}

	switch(joinType_) {
	case Join::SELF:
		if(context != 0)
			_src.getStaticType().typeIntersect(context->getContextItemType());
		break;
	case Join::ATTRIBUTE:
		_src.getStaticType().typeIntersect(StaticType::ATTRIBUTE_TYPE);
		break;
	case Join::NAMESPACE:
		_src.getStaticType().typeIntersect(StaticType::NAMESPACE_TYPE);
		break;
	case Join::ATTRIBUTE_OR_CHILD:
		_src.getStaticType().typeIntersect(StaticType::ELEMENT_TYPE | StaticType::TEXT_TYPE | StaticType::PI_TYPE |
			StaticType::COMMENT_TYPE | StaticType::ATTRIBUTE_TYPE);
		break;
	case Join::CHILD:
	case Join::DESCENDANT:
	case Join::FOLLOWING:
	case Join::FOLLOWING_SIBLING:
	case Join::PRECEDING:
	case Join::PRECEDING_SIBLING:
		_src.getStaticType().typeIntersect(StaticType::ELEMENT_TYPE | StaticType::TEXT_TYPE | StaticType::PI_TYPE |
			StaticType::COMMENT_TYPE);
		break;
	case Join::ANCESTOR:
	case Join::PARENT:
	case Join::PARENT_C:
		_src.getStaticType().typeIntersect(StaticType::DOCUMENT_TYPE | StaticType::ELEMENT_TYPE);
		break;
	case Join::PARENT_A:
		_src.getStaticType().typeIntersect(StaticType::ELEMENT_TYPE);
		break;
	case Join::DESCENDANT_OR_SELF:
	case Join::ANCESTOR_OR_SELF:
	case Join::NONE:
		break;
	}

	return this;
}

DbXmlResult DbXmlContains::createResult(const DbXmlResult &contextItems, unsigned int props, DynamicContext* context) const
{
	DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);

	DbXmlResult result = contextItems;

	if(result.isNull()) {
		result = new XQContextItem::ContextItemResult(this);
	}

	if(dbContext->runQueryPlans() && qp_ != 0) {
		// Only try to sort if we've got a query plan
		result = DbXmlNav::sortIfPossible(result, props, this, context);
	}

	result = new QueryPlanContainsResult(result, this,
		dbContext->runQueryPlans() && (props & StaticResolutionContext::DOCORDER) != 0);

	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DbXmlContains::QueryPlanContainsResult::QueryPlanContainsResult(const DbXmlResult &parent, const DbXmlContains *contains,
	bool useQP)
	: QueryPlanResultImpl(parent, useQP ? contains : 0, contains->getJoinType(), contains),
	  contains_(contains)
{
}

void DbXmlContains::QueryPlanContainsResult::setResult(const DbXmlNodeImpl *node, DynamicContext *context)
{
	if(result_.isNull()) {
		result_ = node->getAxisDbXmlResult((XQStep::Axis)contains_->getJoinType(),
			contains_->getNodeTest(), context, this);
	}
	if(!qpIsExact_) {
		result_ = new ContainsResult(result_, contains_);
	}
}

string DbXmlContains::QueryPlanContainsResult::asString(DynamicContext *context, int indent) const
{
	return "<DbXmlContains_QueryPlanContainsResult/>";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DbXmlContains::ContainsResult::ContainsResult(const Result &parent, const DbXmlContains *contains)
	: ResultImpl(contains),
	  parent_(parent),
	  contains_(contains)
{
}

Item::Ptr DbXmlContains::ContainsResult::next(DynamicContext *context)
{
	Item::Ptr item;
	while((item = parent_->next(context)) != NULLRCP) {
		const XMLCh *container = ((const Node*)item.get())->dmStringValue(context);

		Result arg_result = contains_->getArgument()->collapseTree(context);
		Item::Ptr arg_first = arg_result->next(context);

		const XMLCh* pattern = XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString;
		if(arg_first != NULLRCP)
			pattern = arg_first->asString(context);

		if(pattern == 0 || *pattern == 0)
			return item;
		if(container == 0 || *container == 0)
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
	parent_->skip();
}

string DbXmlContains::ContainsResult::asString(DynamicContext *context, int indent) const
{
	return "<DbXmlContains_ContainsResult/>";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DbXmlFunContains::DbXmlFunContains(FunctionContains *comp, XPath2MemoryManager *mm)
	: FunctionContains(comp->getArguments(), mm)
{
	_src.copy(comp->getStaticResolutionContext());
}
