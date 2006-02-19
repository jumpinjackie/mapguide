//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlCompare.cpp,v 1.10 2005/12/16 11:35:44 jsnelson Exp $
//

#include "../DbXmlInternal.hpp"
#include "DbXmlCompare.hpp"
#include "DbXmlContext.hpp"
#include "DbXmlNav.hpp"

#include <pathan/PathanFactory.hpp>
#include <pathan/operators/Equals.hpp>
#include <pathan/operators/NotEquals.hpp>
#include <pathan/operators/LessThan.hpp>
#include <pathan/operators/LessThanEqual.hpp>
#include <pathan/operators/GreaterThan.hpp>
#include <pathan/operators/GreaterThanEqual.hpp>
#include <pathan/dataItem/DataItemContextItem.hpp>
#include <pathan/internal/collations/CodepointCollation.hpp>
#include <pathan/exceptions/XPath2TypeCastException.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

DbXmlCompare::DbXmlCompare(GeneralComp::ComparisonOperation op, bool generalComp, DataItem *arg, XPath2MemoryManager* memMgr)
	: DataItemImpl(memMgr),
	  operation_(op),
	  generalComp_(generalComp),
	  arg_(arg)
{
	setType((DataItem::whichType)DbXmlContext::DBXML_COMPARE);
	_src.copy(arg->getStaticResolutionContext());
	_src.getStaticType().flags = StaticType::NODE_TYPE;
	_src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::PEER |
		StaticResolutionContext::SUBTREE | StaticResolutionContext::GROUPED |
		StaticResolutionContext::SAMEDOC | StaticResolutionContext::ONENODE);
}

DataItem *DbXmlCompare::staticResolution(StaticContext *context)
{
	DBXML_ASSERT(0);
	return this;
}

Result DbXmlCompare::createResult(DynamicContext* context, int flags) const
{
	DBXML_ASSERT(0);
	return 0;
}

Result DbXmlCompare::createResult(const Result &contextItems, unsigned int props, DynamicContext* context) const
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

	if(generalComp_) {
		result = new GeneralCompareResult(result, this, context);
	} else {
		result = new ValueCompareResult(result, this, context);
	}

	return result;
}

DbXmlCompare::ValueCompareResult::ValueCompareResult(const Result &parent, const DbXmlCompare *comp, DynamicContext *context)
	: ResultImpl(context),
	  parent_(parent),
	  compare_(comp)
{
}

Item::Ptr DbXmlCompare::ValueCompareResult::next(DynamicContext *context)
{
	Item::Ptr item;
	while((item = parent_.next(context)) != NULLRCP) {
		// Value comparisons are intended for comparing single values. The result of a value comparison is
		// defined by applying the following rules, in order:

		// 1. Atomization is applied to each operand. If the result, called an atomized operand, does not contain
		//    exactly one atomic value, a type error is raised.
		// TBD We assume that DB XML doesn't support typed XML trees here - jpcs
		const XMLCh *context_result = ((const Node*)item.get())->dmStringValue(context);
		Result arg_result = compare_->getArgument()->collapseTree(context, DataItem::UNORDERED);

		if(compare_->getArgument()->getStaticResolutionContext().getStaticType().flags & StaticType::NODE_TYPE) {
			arg_result = arg_result.atomize(context);
		}

		// 2. If the atomized operand is an empty sequence, the result of the value comparison is an empty sequence, 
		//    and the implementation need not evaluate the other operand or apply the operator. However, an 
		//    implementation may choose to evaluate the other operand in order to determine whether it raises an error.
		AnyAtomicType::Ptr arg_first = (const AnyAtomicType*)arg_result.next(context).get();
		if(arg_first == NULLRCP) continue;

		// 3. If the atomized operand is a sequence of length greater than one, a type error is raised [err:XPTY0004].
		Item::Ptr arg_second = arg_result.next(context);
		if(arg_second != NULLRCP) {
			DSLthrow(XPath2TypeCastException,X("DbXmlCompare::ValueCompareResult::next"),
				X("A parameter of the operator is not a single atomic value [err:XPTY0004]"));
		}

		// 4. Any atomized operand that has the dynamic type xdt:untypedAtomic is cast to the type xs:string.
		if(arg_first->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC) {
			arg_first = arg_first->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
				XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
		}

		AnyAtomicType::Ptr context_first = context->getPathanFactory()->createString(context_result, context);

		Collation* collation=context->getDefaultCollation();
		if(collation==NULL)
			collation=context->getCollation(CodepointCollation::getCodepointCollationName());

		bool result = false;
		switch(compare_->getOperation()) {
		case GeneralComp::EQUAL:              result = Equals::equals(context_first,arg_first,collation,context); break;
		case GeneralComp::NOT_EQUAL:          result = NotEquals::not_equals(context_first,arg_first,collation,context); break;
		case GeneralComp::LESS_THAN:          result = LessThan::less_than(context_first,arg_first,collation,context); break;
		case GeneralComp::LESS_THAN_EQUAL:    result = LessThanEqual::less_than_equal(context_first,arg_first,collation,context); break;
		case GeneralComp::GREATER_THAN:       result = GreaterThan::greater_than(context_first,arg_first,collation,context); break;
		case GeneralComp::GREATER_THAN_EQUAL: result = GreaterThanEqual::greater_than_equal(context_first,arg_first,collation,context); break;
		default: DBXML_ASSERT(0);
		}

		if(result) return item;
	}

	parent_ = 0;
	return 0;
}

void DbXmlCompare::ValueCompareResult::skip()
{
	parent_.skip();
}

string DbXmlCompare::ValueCompareResult::asString(DynamicContext *context, int indent) const
{
	return "<DbXmlCompare_ValueCompareResult/>";
}

DbXmlCompare::GeneralCompareResult::GeneralCompareResult(const Result &parent, const DbXmlCompare *comp, DynamicContext *context)
	: ResultImpl(context),
	  parent_(parent),
	  compare_(comp)
{
}

Item::Ptr DbXmlCompare::GeneralCompareResult::next(DynamicContext *context)
{
	Item::Ptr item;
	while((item = parent_.next(context)) != NULLRCP) {
		// Atomization is applied to each operand of a general comparison.
		// TBD We assume that DB XML doesn't support typed XML trees here - jpcs
		const XMLCh *context_result = ((const Node*)item.get())->dmStringValue(context);
		Result arg_result = compare_->getArgument()->collapseTree(context, DataItem::UNORDERED);

		if(compare_->getArgument()->getStaticResolutionContext().getStaticType().flags & StaticType::NODE_TYPE) {
			arg_result = arg_result.atomize(context);
		}

		Collation* collation=context->getDefaultCollation();
		if(collation==NULL)
			collation=context->getCollation(CodepointCollation::getCodepointCollationName());

		// The result of the comparison is true if and only if there is a pair of atomic values, 
		// one belonging to the result of atomization of the first operand and the other belonging 
		// to the result of atomization of the second operand, that have the required magnitude relationship.
		// Otherwise the result of the general comparison is false.

		AnyAtomicType::Ptr arg_item;
		while((arg_item  = (const AnyAtomicType::Ptr)arg_result.next(context)) != NULLRCP) {

			AnyAtomicType::Ptr toCompare = 0;
			if(arg_item->isNumericValue()) {
				toCompare = context->getPathanFactory()->createDouble(context_result, context);
			}
			else if(arg_item->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC) {
				toCompare = context->getPathanFactory()->createString(context_result, context);
				arg_item = context->getPathanFactory()->createString(arg_item->asString(context), context);
			}
			else {
				toCompare = context->getPathanFactory()->createDerivedFromAtomicType(arg_item->getTypeURI(),
					arg_item->getTypeName(), context_result, context);
			}


			bool result = false;
			switch(compare_->getOperation()) {
			case GeneralComp::EQUAL:              result = Equals::equals(toCompare,arg_item,collation,context); break;
			case GeneralComp::NOT_EQUAL:          result = NotEquals::not_equals(toCompare,arg_item,collation,context); break;
			case GeneralComp::LESS_THAN:          result = LessThan::less_than(toCompare,arg_item,collation,context); break;
			case GeneralComp::LESS_THAN_EQUAL:    result = LessThanEqual::less_than_equal(toCompare,arg_item,collation,context); break;
			case GeneralComp::GREATER_THAN:       result = GreaterThan::greater_than(toCompare,arg_item,collation,context); break;
			case GeneralComp::GREATER_THAN_EQUAL: result = GreaterThanEqual::greater_than_equal(toCompare,arg_item,collation,context);
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
	parent_.skip();
}

string DbXmlCompare::GeneralCompareResult::asString(DynamicContext *context, int indent) const
{
	return "<DbXmlCompare_GeneralCompareResult/>";
}
