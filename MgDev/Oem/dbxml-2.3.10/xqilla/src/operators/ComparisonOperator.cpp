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
 * $Id: ComparisonOperator.cpp,v 1.19 2006/11/02 15:07:56 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <sstream>

#include <xqilla/operators/ComparisonOperator.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xqilla/exceptions/XPath2TypeCastException.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/ast/ConvertFunctionArg.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

ComparisonOperator::ComparisonOperator(const XMLCh* opName, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQOperator(opName, args, memMgr)
{
}

ASTNode* ComparisonOperator::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    // Value comparisons are intended for comparing single values. The result of a value comparison is
    // defined by applying the following rules, in order:

    // 1. Atomization is applied to each operand. If the result, called an atomized operand, does not contain
    //    exactly one atomic value, a type error is raised.
    *i = new (mm) XQAtomize(*i, mm);
    (*i)->setLocationInfo(this);

    // 2. If the atomized operand is an empty sequence, the result of the value comparison is an empty sequence, 
    //    and the implementation need not evaluate the other operand or apply the operator. However, an 
    //    implementation may choose to evaluate the other operand in order to determine whether it raises an error.
    // 3. If the atomized operand is a sequence of length greater than one, a type error is raised [err:XPTY0004].
    SequenceType *seqType = new (mm) SequenceType(new (mm) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING),
                                                  SequenceType::QUESTION_MARK);
    seqType->setLocationInfo(*i);

    *i = new (mm) XQTreatAs(*i, seqType, mm);
    (*i)->setLocationInfo(this);

    // 4. Any atomized operand that has the dynamic type xdt:untypedAtomic is cast to the type xs:string.
    *i = new (mm) XQPromoteUntyped(*i, SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                   SchemaSymbols::fgDT_STRING, mm);
    (*i)->setLocationInfo(this);

    *i = (*i)->staticResolution(context);
  }

  return this;
}

ASTNode* ComparisonOperator::staticTyping(StaticContext *context)
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

Result ComparisonOperator::createResult(DynamicContext* context, int flags) const
{
  return new ComparisonResult(this);
}

AnyAtomicType::Ptr ComparisonOperator::getArgument(unsigned int index, DynamicContext *context) const
{
  assert(getNumArgs() > index);
  return (const AnyAtomicType::Ptr )_args[index]->collapseTree(context)->next(context);
}

ComparisonOperator::ComparisonResult::ComparisonResult(const ComparisonOperator *op)
  : SingleResult(op),
    _op(op)
{
}

Item::Ptr ComparisonOperator::ComparisonResult::getSingleResult(DynamicContext *context) const
{
  try {
    AnyAtomicType::Ptr left=_op->getArgument(0, context);
    if(left.isNull()) return 0;
    AnyAtomicType::Ptr right=_op->getArgument(1, context);
    if(right.isNull()) return 0;
    bool result = _op->execute(left, right, context);
    return (const Item::Ptr)context->getItemFactory()->createBoolean(result, context);
  }
  catch(XQException &e) {
      if(e.getXQueryLine() == 0)
        e.setXQueryPosition(this);
      throw;
  }
}

std::string ComparisonOperator::ComparisonResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<comparisonop/>" << std::endl;

  return oss.str();
}
