/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: GeneralComp.cpp,v 1.18 2006/11/02 15:07:56 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <sstream>

#include <xqilla/operators/GeneralComp.hpp>
#include <xqilla/operators/Equals.hpp>
#include <xqilla/operators/NotEquals.hpp>
#include <xqilla/operators/LessThan.hpp>
#include <xqilla/operators/LessThanEqual.hpp>
#include <xqilla/operators/GreaterThan.hpp>
#include <xqilla/operators/GreaterThanEqual.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/context/impl/CodepointCollation.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/context/ContextHelpers.hpp>

/*static*/ const XMLCh GeneralComp::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

GeneralComp::GeneralComp(ComparisonOperation operation, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQOperator(name, args, memMgr),
    xpath1compat_(false)
{
  _operation=operation;
}

bool GeneralComp::compare(GeneralComp::ComparisonOperation operation, AnyAtomicType::Ptr first, AnyAtomicType::Ptr second,
                          Collation* collation, DynamicContext *context, bool xpath1compat, const LocationInfo *info)
{
  // The magnitude relationship between two atomic values is determined as follows:
  // 1) If either atomic value has the dynamic type xdt:untypedAtomic, that value is cast to a required type, 
  //    which is determined as follows:
  //    - If the dynamic type of the other atomic value is a numeric type, the required type is xs:double.
  //    - If the dynamic type of the other atomic value is xdt:untypedAtomic, the required type is xs:string.
  //    - Otherwise, the required type is the dynamic type of the other atomic value.
  //    If the cast to the required type fails, a dynamic error is raised.
  // 2) If XPath 1.0 compatibility mode is true, and at least one of the atomic values has a numeric type, 
  //    then both atomic values are cast to to the type xs:double.
  // 3) After any necessary casting, the atomic values are compared using one of the value comparison operators 
  //    eq, ne, lt, le, gt, or ge, depending on whether the general comparison operator was 
  //    =, !=, <, <=, >, or >=. The values have the required magnitude relationship if the result of this 
  //    value comparison is true.

  if(first->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC) {
    if (second->isNumericValue()) {
      first = first->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                            XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context);
    }
    else if(second->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC) {
      first = first->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                            XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
    }
    else {
      first = first->castAs(second->getTypeURI(),
                            second->getTypeName(), context);
    }
  }
  if(second->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC) {
    if(first->isNumericValue()) {
      second = second->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                              XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context);
    }
    else if(first->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC) {
      second = second->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                              XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
    }
    else {
      second = second->castAs(first->getTypeURI(),
                              first->getTypeName(), context);
    }
  }
  if(xpath1compat && (first->isNumericValue() || second->isNumericValue())) {
    first = first->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                          XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context);
    second = second->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                            XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context);
  }
  bool result = false;
  switch(operation) {
  case GeneralComp::EQUAL:              result = Equals::equals(first,second,collation,context,info); break;
  case GeneralComp::NOT_EQUAL:          result = NotEquals::not_equals(first,second,collation,context,info); break;
  case GeneralComp::LESS_THAN:          result = LessThan::less_than(first,second,collation,context,info); break;
  case GeneralComp::LESS_THAN_EQUAL:    result = LessThanEqual::less_than_equal(first,second,collation,context,info); break;
  case GeneralComp::GREATER_THAN:       result = GreaterThan::greater_than(first,second,collation,context,info); break;
  case GeneralComp::GREATER_THAN_EQUAL: result = GreaterThanEqual::greater_than_equal(first,second,collation,context,info); break;
  default:                 assert(false);
  }

  return result;
}

ASTNode* GeneralComp::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  xpath1compat_ = context->getXPath1CompatibilityMode();

  AutoNodeSetOrderingReset orderReset(context);

  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    *i = new (mm) XQAtomize(*i, mm);
    (*i)->setLocationInfo(this);
    *i = (*i)->staticResolution(context);
  }

  return this;
}

ASTNode* GeneralComp::staticTyping(StaticContext *context)
{
  _src.clear();

  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    *i = (*i)->staticTyping(context);
    _src.add((*i)->getStaticResolutionContext());

    if((*i)->isDateOrTimeAndHasNoTimezone(context))
      _src.implicitTimezoneUsed(true);
  }

  _src.getStaticType().flags = StaticType::BOOLEAN_TYPE;

  if(!_src.isUsed()) {
    return constantFold(context);
  }
  return this;
}

Result GeneralComp::createResult(DynamicContext* context, int flags) const
{
  return new GeneralCompResult(this);
}

GeneralComp::ComparisonOperation GeneralComp::getOperation() const {
  return _operation;
}

GeneralComp::GeneralCompResult::GeneralCompResult(const GeneralComp *op)
  : SingleResult(op),
    _op(op)
{
}

Item::Ptr GeneralComp::GeneralCompResult::getSingleResult(DynamicContext *context) const
{
  // Atomization is applied to each operand of a general comparison.
  Result arg1 = _op->getArgument(0)->collapseTree(context);
  Result arg2 = _op->getArgument(1)->collapseTree(context);

  Collation* collation=context->getDefaultCollation(this);

  // The result of the comparison is true if and only if there is a pair of atomic values, 
  // one belonging to the result of atomization of the first operand and the other belonging 
  // to the result of atomization of the second operand, that have the required magnitude relationship.
  // Otherwise the result of the general comparison is false.

  AnyAtomicType::Ptr item1 = (const AnyAtomicType::Ptr)arg1->next(context);
  if(item1 != NULLRCP) {
    // The first time we loop over arg2, we store it in a sequence
    AnyAtomicType::Ptr item2;
    Sequence arg2_sequence(context->getMemoryManager());
    while((item2 = (const AnyAtomicType::Ptr)arg2->next(context)) != NULLRCP) {
      if(compare(_op->getOperation(), item1, item2, collation, context, _op->getXPath1CompatibilityMode(), this))
        return (const Item::Ptr)context->getItemFactory()->createBoolean(true, context);
      arg2_sequence.addItem(item2);
    }

    // The second and subsequent times, we iterate over the sequence
    Sequence::iterator itSecond;
    while((item1 = (const AnyAtomicType::Ptr)arg1->next(context)) != NULLRCP) {
      for(itSecond = arg2_sequence.begin(); itSecond != arg2_sequence.end(); ++itSecond) {
        if(compare(_op->getOperation(), item1, (const AnyAtomicType::Ptr)*itSecond, collation, context,
                   _op->getXPath1CompatibilityMode(), this))
          return (const Item::Ptr)context->getItemFactory()->createBoolean(true, context);
      }
    }
  }
  return (const Item::Ptr)context->getItemFactory()->createBoolean(false, context);
}

std::string GeneralComp::GeneralCompResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<generalcomp/>" << std::endl;

  return oss.str();
}
