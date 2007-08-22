//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlCompare.cpp,v 1.29 2006/10/30 17:45:54 bostic Exp $
//

#include "../DbXmlInternal.hpp"
#include "DbXmlCompare.hpp"
#include "DbXmlContext.hpp"
#include "DbXmlNav.hpp"
#include "DbXmlNodeTest.hpp"

#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/ast/XQContextItem.hpp>
#include <xqilla/context/impl/CodepointCollation.hpp>
#include <xqilla/exceptions/XPath2TypeCastException.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

DbXmlCompare::DbXmlCompare(GeneralComp::ComparisonOperation op, bool generalComp, ASTNode *arg, XPath2MemoryManager* memMgr)
	: DbXmlASTNode(DBXML_COMPARE, memMgr),
	  joinType_(Join::SELF),
	  nodeTest_(0),
	  operation_(op),
	  generalComp_(generalComp),
	  arg_(arg),
	  castURI_(0),
	  castName_(0)
{
	staticTyping(0);
}

DbXmlCompare::DbXmlCompare(Join::Type join, const DbXmlCompare *cmp, XPath2MemoryManager* memMgr)
	: DbXmlASTNode(DBXML_COMPARE, memMgr),
	  joinType_(join),
	  nodeTest_(cmp->nodeTest_),
	  operation_(cmp->operation_),
	  generalComp_(cmp->generalComp_),
	  arg_(cmp->arg_),
	  castURI_(cmp->castURI_),
	  castName_(cmp->castName_)
{
	setQueryPlanHolder(cmp);
	staticTyping(0);
}

DbXmlCompare::DbXmlCompare(Join::Type join, DbXmlNodeTest *nodeTest, const DbXmlCompare *cmp,
	XPath2MemoryManager* memMgr)
	: DbXmlASTNode(DBXML_COMPARE, memMgr),
	  joinType_(join),
	  nodeTest_(nodeTest),
	  operation_(cmp->operation_),
	  generalComp_(cmp->generalComp_),
	  arg_(cmp->arg_),
	  castURI_(cmp->castURI_),
	  castName_(cmp->castName_)
{
	setQueryPlanHolder(cmp);
	staticTyping(0);
}

ASTNode *DbXmlCompare::staticTyping(StaticContext *context)
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

DbXmlResult DbXmlCompare::createResult(const DbXmlResult &contextItems, unsigned int props, DynamicContext* context) const
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

	if(generalComp_) {
		result = new QueryPlanGeneralCompareResult(result, this,
			dbContext->runQueryPlans() && (props & StaticResolutionContext::DOCORDER) != 0);
	} else {
		result = new QueryPlanValueCompareResult(result, this,
			dbContext->runQueryPlans() && (props & StaticResolutionContext::DOCORDER) != 0);
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DbXmlCompare::QueryPlanValueCompareResult::QueryPlanValueCompareResult(const DbXmlResult &parent, const DbXmlCompare *comp,
	bool useQP)
	: QueryPlanResultImpl(parent, useQP ? comp : 0, comp->getJoinType(), comp),
	  compare_(comp)
{
}

void DbXmlCompare::QueryPlanValueCompareResult::setResult(const DbXmlNodeImpl *node, DynamicContext *context)
{
	if(result_.isNull()) {
		result_ = node->getAxisDbXmlResult((XQStep::Axis)compare_->getJoinType(),
			compare_->getNodeTest(), context, this);
	}
	if(!qpIsExact_) {
		result_ = new ValueCompareResult(result_, compare_);
	}
}

string DbXmlCompare::QueryPlanValueCompareResult::asString(DynamicContext *context, int indent) const
{
	return "<DbXmlCompare_QueryPlanValueCompareResult/>";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DbXmlCompare::QueryPlanGeneralCompareResult::QueryPlanGeneralCompareResult(const DbXmlResult &parent,
	const DbXmlCompare *comp, bool useQP)
	: QueryPlanResultImpl(parent, useQP ? comp : 0, comp->getJoinType(), comp),
	  compare_(comp)
{
}

void DbXmlCompare::QueryPlanGeneralCompareResult::setResult(const DbXmlNodeImpl *node, DynamicContext *context)
{
	if(result_.isNull()) {
		result_ = node->getAxisDbXmlResult((XQStep::Axis)compare_->getJoinType(),
			compare_->getNodeTest(), context, this);
	}
	if(!qpIsExact_) {
		result_ = new GeneralCompareResult(result_, compare_);
	}
}

string DbXmlCompare::QueryPlanGeneralCompareResult::asString(DynamicContext *context, int indent) const
{
	return "<DbXmlCompare_QueryPlanGeneralCompareResult/>";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DbXmlCompare::ValueCompareResult::ValueCompareResult(const Result &parent, const DbXmlCompare *comp)
	: ResultImpl(comp),
	  parent_(parent),
	  compare_(comp)
{
}

Item::Ptr DbXmlCompare::ValueCompareResult::next(DynamicContext *context)
{
	Item::Ptr item;
	while((item = parent_->next(context)) != NULLRCP) {
		// Value comparisons are intended for comparing single values. The result of a value comparison is
		// defined by applying the following rules, in order:

		// 1. Atomization is applied to each operand. If the result, called an atomized operand, does not contain
		//    exactly one atomic value, a type error is raised.
		// TBD We assume that DB XML doesn't support typed XML trees here - jpcs
		const XMLCh *context_result = ((const Node*)item.get())->dmStringValue(context);
		Result arg_result = compare_->getArgument()->collapseTree(context);

		// 2. If the atomized operand is an empty sequence, the result of the value comparison is an empty sequence, 
		//    and the implementation need not evaluate the other operand or apply the operator. However, an 
		//    implementation may choose to evaluate the other operand in order to determine whether it raises an error.
		AnyAtomicType::Ptr arg_first = (const AnyAtomicType*)arg_result->next(context).get();
		if(arg_first == NULLRCP) continue;

		// 3. If the atomized operand is a sequence of length greater than one, a type error is raised [err:XPTY0004].
		Item::Ptr arg_second = arg_result->next(context);
		if(arg_second != NULLRCP) {
			XQThrow(XPath2TypeCastException,X("DbXmlCompare::ValueCompareResult::next"),
				X("A parameter of the operator is not a single atomic value [err:XPTY0004]"));
		}

		// 4. Any atomized operand that has the dynamic type xdt:untypedAtomic is cast to the type xs:string.
		if(arg_first->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC) {
			arg_first = arg_first->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
				XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
		}

		AnyAtomicType::Ptr context_first;
		try {
			if(compare_->getCastURI() != 0) {
				context_first = context->getItemFactory()->
					createDerivedFromAtomicType(compare_->getCastURI(),
						compare_->getCastName(), context_result, context);
			} else {
				context_first = context->getItemFactory()->createString(context_result, context);
			}
		}
		catch(XQException &e) {
			XQThrow(XPath2TypeCastException, X("DbXmlCompare::ValueCompareResult::next"),
				X("Invalid lexical value [err:FORG0001]"));
		}

		Collation* collation=context->getDefaultCollation(this);
		if(collation==NULL)
			collation=context->getCollation(CodepointCollation::getCodepointCollationName(), this);

		bool result = false;
		switch(compare_->getOperation()) {
		case GeneralComp::EQUAL:              result = Equals::equals(context_first,arg_first,collation,context,this); break;
		case GeneralComp::NOT_EQUAL:          result = NotEquals::not_equals(context_first,arg_first,collation,context,this); break;
		case GeneralComp::LESS_THAN:          result = LessThan::less_than(context_first,arg_first,collation,context,this); break;
		case GeneralComp::LESS_THAN_EQUAL:    result = LessThanEqual::less_than_equal(context_first,arg_first,collation,context,this); break;
		case GeneralComp::GREATER_THAN:       result = GreaterThan::greater_than(context_first,arg_first,collation,context,this); break;
		case GeneralComp::GREATER_THAN_EQUAL: result = GreaterThanEqual::greater_than_equal(context_first,arg_first,collation,context,this); break;
		default: DBXML_ASSERT(0);
		}

		if(result) return item;
	}

	parent_ = 0;
	return 0;
}

void DbXmlCompare::ValueCompareResult::skip()
{
	parent_->skip();
}

string DbXmlCompare::ValueCompareResult::asString(DynamicContext *context, int indent) const
{
	return "<DbXmlCompare_ValueCompareResult/>";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DbXmlCompare::GeneralCompareResult::GeneralCompareResult(const Result &parent, const DbXmlCompare *comp)
	: ResultImpl(comp),
	  parent_(parent),
	  compare_(comp)
{
}

Item::Ptr DbXmlCompare::GeneralCompareResult::next(DynamicContext *context)
{
	Item::Ptr item;
	while((item = parent_->next(context)) != NULLRCP) {
		// Atomization is applied to each operand of a general comparison.
		// TBD We assume that DB XML doesn't support typed XML trees here - jpcs
		const XMLCh *context_result = ((const Node*)item.get())->dmStringValue(context);
		Result arg_result = compare_->getArgument()->collapseTree(context);

		Collation* collation=context->getDefaultCollation(this);
		if(collation==NULL)
			collation=context->getCollation(CodepointCollation::getCodepointCollationName(), this);

		// The result of the comparison is true if and only if there is a pair of atomic values, 
		// one belonging to the result of atomization of the first operand and the other belonging 
		// to the result of atomization of the second operand, that have the required magnitude relationship.
		// Otherwise the result of the general comparison is false.

		AnyAtomicType::Ptr arg_item;
		while((arg_item  = (const AnyAtomicType::Ptr)arg_result->next(context)) != NULLRCP) {

			AnyAtomicType::Ptr toCompare = 0;
			try {
				if(compare_->getCastURI() != 0) {
					toCompare = context->getItemFactory()->
						createDerivedFromAtomicType(compare_->getCastURI(),
							compare_->getCastName(), context_result, context);
				} else {
					if(arg_item->isNumericValue()) {
						toCompare = context->getItemFactory()->
							createDouble(context_result, context);
					}
					else if(arg_item->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC) {
						toCompare = context->getItemFactory()->
							createString(context_result, context);
						arg_item = context->getItemFactory()->
							createString(arg_item->asString(context), context);
					}
					else {
						toCompare = context->getItemFactory()->
							createDerivedFromAtomicType(arg_item->getTypeURI(),
								arg_item->getTypeName(), context_result, context);
					}
				}
			}
			catch(XQException &e) {
				XQThrow(XPath2TypeCastException, X("DbXmlCompare::GeneralCompareResult::next"),
					X("Invalid lexical value [err:FORG0001]"));
			}

			bool result = false;
			switch(compare_->getOperation()) {
			case GeneralComp::EQUAL:
				result = Equals::equals(toCompare,arg_item,collation,context,this); break;
			case GeneralComp::NOT_EQUAL:
				result = NotEquals::not_equals(toCompare,arg_item,collation,context,this); break;
			case GeneralComp::LESS_THAN:
				result = LessThan::less_than(toCompare,arg_item,collation,context,this); break;
			case GeneralComp::LESS_THAN_EQUAL:
				result = LessThanEqual::less_than_equal(toCompare,arg_item,collation,context,this); break;
			case GeneralComp::GREATER_THAN:
				result = GreaterThan::greater_than(toCompare,arg_item,collation,context,this); break;
			case GeneralComp::GREATER_THAN_EQUAL:
				result = GreaterThanEqual::greater_than_equal(toCompare,arg_item,collation,context,this);
				break;
			default: DBXML_ASSERT(0);
			}
			if(result) return item;
		}
	}

	parent_ = 0;
	return 0;
}

void DbXmlCompare::GeneralCompareResult::skip()
{
	parent_->skip();
}

string DbXmlCompare::GeneralCompareResult::asString(DynamicContext *context, int indent) const
{
	return "<DbXmlCompare_GeneralCompareResult/>";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DbXmlGeneralComp::DbXmlGeneralComp(GeneralComp *comp, XPath2MemoryManager *mm)
	: GeneralComp(comp->getOperation(), comp->getArguments(), mm)
{
	_src.copy(comp->getStaticResolutionContext());
}

DbXmlEquals::DbXmlEquals(Equals *comp, XPath2MemoryManager *mm)
	: Equals(comp->getArguments(), mm)
{
	_src.copy(comp->getStaticResolutionContext());
}

DbXmlNotEquals::DbXmlNotEquals(NotEquals *comp, XPath2MemoryManager *mm)
	: NotEquals(comp->getArguments(), mm)
{
	_src.copy(comp->getStaticResolutionContext());
}

DbXmlLessThan::DbXmlLessThan(LessThan *comp, XPath2MemoryManager *mm)
	: LessThan(comp->getArguments(), mm)
{
	_src.copy(comp->getStaticResolutionContext());
}

DbXmlLessThanEqual::DbXmlLessThanEqual(LessThanEqual *comp, XPath2MemoryManager *mm)
	: LessThanEqual(comp->getArguments(), mm)
{
	_src.copy(comp->getStaticResolutionContext());
}

DbXmlGreaterThan::DbXmlGreaterThan(GreaterThan *comp, XPath2MemoryManager *mm)
	: GreaterThan(comp->getArguments(), mm)
{
	_src.copy(comp->getStaticResolutionContext());
}

DbXmlGreaterThanEqual::DbXmlGreaterThanEqual(GreaterThanEqual *comp, XPath2MemoryManager *mm)
	: GreaterThanEqual(comp->getArguments(), mm)
{
	_src.copy(comp->getStaticResolutionContext());
}
