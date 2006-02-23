/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>

#include <pathan/operators/ArithmeticOperator.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/XPath2TypeCastException.hpp>
#include <pathan/Item.hpp>
#include <pathan/AnyAtomicType.hpp>

ArithmeticOperator::ArithmeticOperator(const XMLCh* opName, const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemOperator(opName, args, memMgr)
{
}

DataItem* ArithmeticOperator::staticResolution(StaticContext *context)
{
  _src.getStaticType().flags = 0;

  bool allConstant = true;
  for(VectorOfDataItems::iterator i = _args.begin(); i != _args.end(); ++i) {
    *i = (*i)->staticResolution(context);

    if((*i)->getStaticResolutionContext().getStaticType().flags & StaticType::NODE_TYPE) {
      _src.getStaticType().flags |= StaticType::NUMERIC_TYPE | StaticType::OTHER_TYPE;
    }
    if((*i)->getStaticResolutionContext().getStaticType().flags & StaticType::NUMERIC_TYPE) {
      _src.getStaticType().flags |= StaticType::NUMERIC_TYPE;
    }
    if((*i)->getStaticResolutionContext().getStaticType().flags & StaticType::OTHER_TYPE) {
      _src.getStaticType().flags |= StaticType::OTHER_TYPE;
    }

    _src.add((*i)->getStaticResolutionContext());
    if(!(*i)->isConstantAndHasTimezone(context)) {
      allConstant = false;
      if((*i)->isConstant()) {
        _src.implicitTimezoneUsed(true);
      }
    }
  }

  if(allConstant) {
    return constantFold(context);
  }
  else {
    return resolvePredicates(context);
  }
}

Result ArithmeticOperator::createResult(DynamicContext* context, int flags) const
{
  return new ArithmeticResult(this, context);
}

AnyAtomicType::Ptr ArithmeticOperator::getArgument(unsigned int index, DynamicContext *context) const
{
  if(getNumArgs() <= index) {
    return 0;
  }

  // An arithmetic expression is evaluated by applying the following rules, in order,
  // until an error is raised or a value is computed:

  // 1. Atomization is applied to each operand.
  // 2. If either operand is an empty sequence, the result of the operation is an empty sequence.
  // (to be enforced by the caller)
  // 3. If either operand is now a sequence of length greater than one, then:
  //      * If XPath 1.0 compatibility mode is true, any items after the first item in the sequence are discarded.
  //        Otherwise, a type error is raised.
  Result arg_result(_args[index]->collapseTree(context));

  if(_args[index]->getStaticResolutionContext().getStaticType().flags & StaticType::NODE_TYPE) {
	  arg_result = arg_result.atomize(context);
  }

  Item::Ptr first = arg_result.next(context);

  if(!context->getXPath1CompatibilityMode()) {
    Item::Ptr second = arg_result.next(context);

    if(first != NULLRCP && second != NULLRCP) {
      DSLthrow(XPath2TypeCastException,X("ArithmeticOperator::getArgument"),
               X("A parameter of the operator is not an empty sequence or a single atomic value [err:XPTY0004]"));
    }
  }
    
  // 4. If either operand is now of type xdt:untypedAtomic, it is cast to the default type for the given operator.
  //    If the cast fails, a type error is raised.
  //
  if(first != NULLRCP) {
    if(first->isAtomicValue()) {
      if(((const AnyAtomicType*)(const Item*)first)->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC) {
        first = (const Item::Ptr)((const AnyAtomicType*)(const Item*)first)->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                                                                    XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, 
                                                                                    context);
      }
    }
    else {
      DSLthrow(XPath2TypeCastException,X("ArithmeticOperator::getArgument"), X("A parameter of the operator is not an atomic value"));
    }
  }

  return (const AnyAtomicType::Ptr )first;
}

ArithmeticOperator::ArithmeticResult::ArithmeticResult(const ArithmeticOperator *op, DynamicContext *context)
  : SingleResult(context),
    _op(op)
{
}

Item::Ptr ArithmeticOperator::ArithmeticResult::getSingleResult(DynamicContext *context) const
{
  return _op->execute(_op->getArgument(0, context), _op->getArgument(1, context), context);
}

std::string ArithmeticOperator::ArithmeticResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<arithmeticop/>" << std::endl;

  return oss.str();
}
