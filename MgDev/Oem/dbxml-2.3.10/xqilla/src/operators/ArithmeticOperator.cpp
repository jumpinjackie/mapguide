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
 * $Id: ArithmeticOperator.cpp,v 1.21 2006/11/02 15:07:56 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <sstream>

#include <xqilla/operators/ArithmeticOperator.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/XPath2TypeCastException.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/ast/ConvertFunctionArg.hpp>

#include <xercesc/framework/XMLBuffer.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

ArithmeticOperator::ArithmeticOperator(const XMLCh* opName, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQOperator(opName, args, memMgr)
{
}

ASTNode* ArithmeticOperator::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    // An arithmetic expression is evaluated by applying the following rules, in order,
    // until an error is raised or a value is computed:

    // 1. Atomization is applied to each operand.

    *i = new (mm) XQAtomize(*i, mm);
    (*i)->setLocationInfo(this);

    // 2. If either operand is an empty sequence, the result of the operation is an empty sequence.
    // 3. If either operand is now a sequence of length greater than one, then:
    //      * If XPath 1.0 compatibility mode is true, any items after the first item in the sequence are discarded.
    //        Otherwise, a type error is raised.

    if(!context->getXPath1CompatibilityMode()) {
      SequenceType *seqType = new (mm) SequenceType(new (mm) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING),
                                                    SequenceType::QUESTION_MARK);
      seqType->setLocationInfo(*i);

      *i = new (mm) XQTreatAs(*i, seqType, mm);
      (*i)->setLocationInfo(this);
    }

    // 4. If either operand is now of type xdt:untypedAtomic, it is cast to the default type for the given operator.
    //    If the cast fails, a type error is raised.

    *i = new (mm) XQPromoteUntyped(*i, SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                   SchemaSymbols::fgDT_DOUBLE, mm);
    (*i)->setLocationInfo(this);

    *i = (*i)->staticResolution(context);
  }

  return this;
}

ASTNode* ArithmeticOperator::staticTyping(StaticContext *context)
{
  _src.clear();

  bool emptyArgument = false;
  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    *i = (*i)->staticTyping(context);

    if((*i)->getStaticResolutionContext().getStaticType().flags == 0)
      emptyArgument = true;
    _src.add((*i)->getStaticResolutionContext());

    if((*i)->isDateOrTimeAndHasNoTimezone(context))
      _src.implicitTimezoneUsed(true);
  }

  _src.getStaticType().flags = 0;
  calculateStaticType();

  if(!emptyArgument && _src.getStaticType().flags == 0) {
    XMLBuffer errMsg;
    errMsg.set(X("The operator "));
    errMsg.append(_opName);
    errMsg.append(X(" has been called on invalid operand types [err:XPTY0004]"));
    XQThrow(XPath2ErrorException,X("ArithmeticOperator::staticResolution"), errMsg.getRawBuffer());
  }

  if(!_src.isUsed()) {
    return constantFold(context);
  }
  return this;
}

void ArithmeticOperator::calculateStaticTypeForNumerics(const StaticType &arg0, const StaticType &arg1)
{
  // Deal with numerics and numeric type promotion
  if(arg0.containsType(StaticType::DECIMAL_TYPE)) {
    if(arg1.containsType(StaticType::DECIMAL_TYPE)) {
      _src.getStaticType().flags |= StaticType::DECIMAL_TYPE;
    }
    if(arg1.containsType(StaticType::FLOAT_TYPE)) {
      _src.getStaticType().flags |= StaticType::FLOAT_TYPE;
    }
    // untypedAtomic will be promoted to xs:double
    if(arg1.containsType(StaticType::DOUBLE_TYPE|StaticType::UNTYPED_ATOMIC_TYPE)) {
      _src.getStaticType().flags |= StaticType::DOUBLE_TYPE;
    }
  }
  if(arg0.containsType(StaticType::FLOAT_TYPE)) {
    if(arg1.containsType(StaticType::DECIMAL_TYPE|StaticType::FLOAT_TYPE)) {
      _src.getStaticType().flags |= StaticType::FLOAT_TYPE;
    }
    // untypedAtomic will be promoted to xs:double
    if(arg1.containsType(StaticType::DOUBLE_TYPE|StaticType::UNTYPED_ATOMIC_TYPE)) {
      _src.getStaticType().flags |= StaticType::DOUBLE_TYPE;
    }
  }
  // untypedAtomic will be promoted to xs:double
  if(arg0.containsType(StaticType::DOUBLE_TYPE|StaticType::UNTYPED_ATOMIC_TYPE)) {
    if(arg1.containsType(StaticType::DECIMAL_TYPE|StaticType::FLOAT_TYPE|StaticType::DOUBLE_TYPE|StaticType::UNTYPED_ATOMIC_TYPE)) {
      _src.getStaticType().flags |= StaticType::DOUBLE_TYPE;
    }
  }
}

Result ArithmeticOperator::createResult(DynamicContext* context, int flags) const
{
  return new ArithmeticResult(this);
}

AnyAtomicType::Ptr ArithmeticOperator::getArgument(unsigned int index, DynamicContext *context) const
{
  if(getNumArgs() <= index) return 0;
  return (const AnyAtomicType::Ptr )_args[index]->collapseTree(context)->next(context);
}

ArithmeticOperator::ArithmeticResult::ArithmeticResult(const ArithmeticOperator *op)
  : SingleResult(op),
    _op(op)
{
}

Item::Ptr ArithmeticOperator::ArithmeticResult::getSingleResult(DynamicContext *context) const
{
  try {
    return _op->execute(_op->getArgument(0, context), _op->getArgument(1, context), context);
  }
  catch(XQException &e) {
      if(e.getXQueryLine() == 0)
        e.setXQueryPosition(this);
      throw;
  }
}

std::string ArithmeticOperator::ArithmeticResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<arithmeticop/>" << std::endl;

  return oss.str();
}
