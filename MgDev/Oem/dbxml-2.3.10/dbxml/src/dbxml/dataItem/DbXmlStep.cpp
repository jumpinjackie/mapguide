//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlStep.cpp,v 1.7 2006/10/30 17:45:55 bostic Exp $
//

#include "../DbXmlInternal.hpp"
#include "DbXmlStep.hpp"
#include "DbXmlContext.hpp"
#include "DbXmlNodeTest.hpp"
#include "DbXmlNav.hpp"
#include "QueryPlanResultImpl.hpp"

#include <xqilla/ast/XQContextItem.hpp>

using namespace DbXml;
using namespace std;

DbXmlStep::DbXmlStep(const XQStep *step, XPath2MemoryManager* memMgr)
	: DbXmlASTNode(DBXML_STEP, memMgr),
	  joinType_((Join::Type)step->getAxis()),
	  nodeTest_(new (memMgr) DbXmlNodeTest(step->getNodeTest(), memMgr))
{
	staticTyping(0);
}

DbXmlStep::DbXmlStep(Join::Type join, XPath2MemoryManager* memMgr)
	: DbXmlASTNode(DBXML_STEP, memMgr),
	  joinType_(join),
	  nodeTest_(0)
{
	staticTyping(0);
}

DbXmlStep::DbXmlStep(Join::Type join, const DbXmlStep *step, XPath2MemoryManager* memMgr)
	: DbXmlASTNode(DBXML_STEP, memMgr),
	  joinType_(join),
	  nodeTest_(const_cast<DbXmlNodeTest*>(step->getNodeTest()))
{
	setQueryPlanHolder(step);
	staticTyping(0);
}

DbXmlStep::DbXmlStep(Join::Type join, DbXmlNodeTest *nodeTest, XPath2MemoryManager* memMgr)
	: DbXmlASTNode(DBXML_STEP, memMgr),
	  joinType_(join),
	  nodeTest_(nodeTest)
{
	staticResolution(0);
}

ASTNode* DbXmlStep::staticResolution(StaticContext *context)
{
	if(context != 0 && nodeTest_ != 0)
		nodeTest_->staticResolution(context, this);
	return this;
}

ASTNode* DbXmlStep::staticTyping(StaticContext *context)
{
	DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);

	_src.clear();

	_src.setProperties(Join::getJoinTypeProperties(joinType_));
	_src.contextItemUsed(true);

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

class QueryPlanStepResult : public QueryPlanResultImpl
{
public:
	QueryPlanStepResult(const DbXmlResult &parent, const DbXmlStep *step, bool useQP)
		: QueryPlanResultImpl(parent, useQP ? step : 0, step->getJoinType(), step),
		  step_(step)
	{
	}

	void setResult(const DbXmlNodeImpl *node, DynamicContext *context)
	{
		if(result_.isNull()) {
			result_ = node->getAxisDbXmlResult((XQStep::Axis)step_->getJoinType(),
				step_->getNodeTest(), context, this);
		}
	}

	string asString(DynamicContext *context, int indent) const
	{
		return "QueryPlanStepResult";
	}
private:
	const DbXmlStep *step_;
};

DbXmlResult DbXmlStep::createResult(const DbXmlResult &contextItems, unsigned int props, DynamicContext* context) const
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

	result = new QueryPlanStepResult(result, this,
		dbContext->runQueryPlans() && (props & StaticResolutionContext::DOCORDER) != 0);

	return result;
}
