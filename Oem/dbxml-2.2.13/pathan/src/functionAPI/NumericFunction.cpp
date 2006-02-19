/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functionAPI/NumericFunction.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/exceptions/DSLException.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/AnyAtomicType.hpp>
#include <pathan/ATFloatOrDerived.hpp>
#include <pathan/ATDoubleOrDerived.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>

NumericFunction::NumericFunction(const XMLCh* name, unsigned int argsFrom, unsigned int argsTo, const char* paramDecl, const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name,argsFrom, argsTo, paramDecl, args, memMgr) 
{ 
}

Result NumericFunction::getParamNumber(unsigned int number, DynamicContext* context, int flags) const 
{
	return getParamNumber(number,number,context,flags);
}

Result NumericFunction::getParamNumber(unsigned int paramNumber, unsigned int signatureNumber, DynamicContext* context, int flags) const
{
  Sequence arg = DataItemFunction::getParamNumber(paramNumber, signatureNumber, context, flags).toSequence(context);

  if (arg.isEmpty()) {
    return arg;
  } else  if (arg.first()->isAtomicValue() && ((const AnyAtomicType::Ptr)arg.first())->isNumericValue()) {
    return arg;
  } else {
    DSLthrow(FunctionException,X("NumericFunction::getParamNumber"), X("Non-numeric argument in numeric function."));
  }
}

bool NumericFunction::isNaN(const Numeric::Ptr &number) const
{
  const XMLCh* numType=number->getPrimitiveTypeName();
  if(XPath2Utils::equals(numType,XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT) &&
      ((const ATFloatOrDerived::Ptr)number)->isNaN())
     return true;
  if(XPath2Utils::equals(numType,XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE) &&
      ((const ATDoubleOrDerived::Ptr)number)->isNaN())
     return true;
  return false;
}

bool NumericFunction::isINF(const Numeric::Ptr &number) const
{
  const XMLCh* numType=number->getPrimitiveTypeName();
  if(XPath2Utils::equals(numType,XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT) &&
      ((const ATFloatOrDerived::Ptr)number)->isInfinite())
     return true;
  if(XPath2Utils::equals(numType,XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE) &&
      ((const ATDoubleOrDerived::Ptr)number)->isInfinite())
     return true;
  return false;
}
