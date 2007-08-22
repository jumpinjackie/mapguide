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
 * $Id: ResultImpl.cpp,v 1.11 2006/11/01 16:37:22 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <sstream>

#include <xqilla/runtime/ResultImpl.hpp>
#include <xqilla/runtime/SequenceResult.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Numeric.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>

static inline bool getEffectiveBooleanValueInternal(const Item::Ptr &first, const Item::Ptr &second, DynamicContext* context, const LocationInfo *info)
{
  // If its operand is a singleton value ...
  if(second == NULLRCP && first->isAtomicValue()) {
    const AnyAtomicType::Ptr atom=first;
    // ... of type xs:boolean or derived from xs:boolean, fn:boolean returns the value of its operand unchanged.
    if(atom->getPrimitiveTypeIndex() == AnyAtomicType::BOOLEAN)
        return ((const ATBooleanOrDerived::Ptr)atom)->isTrue();

    // ... of type xs:string, xdt:untypedAtomic, or a type derived from one of these, fn:boolean returns false 
    // if the operand value has zero length; otherwise it returns true.
    if((atom->getPrimitiveTypeIndex() == AnyAtomicType::STRING || 
        atom->getPrimitiveTypeIndex() == AnyAtomicType::ANY_URI || 
        atom->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC))
      return !XPath2Utils::equals(atom->asString(context), XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString);

    // ... of any numeric type or derived from a numeric type, fn:boolean returns false if the operand value is 
    // NaN or is numerically equal to zero; otherwise it returns true.
    if(atom->isNumericValue()) {
      const Numeric::Ptr number=atom;
      return (!number->isZero() && !number->isNaN());
    }
  }

  // In all other cases, fn:boolean raises a type error. 
  XQThrow3(XPath2TypeMatchException,X("ResultImpl::getEffectiveBooleanValue"), X("Effective Boolean Value cannot be extracted from this type [err:FORG0006]"), info);
  return true;
}

bool ResultImpl::getEffectiveBooleanValue(DynamicContext* context, const LocationInfo *info)
{
  const Item::Ptr first = next(context);
  if(first == NULLRCP) return false;
  if(first->isNode()) return true;
  return getEffectiveBooleanValueInternal(first, next(context), context, info);
}

bool ResultImpl::getEffectiveBooleanValue(const Item::Ptr &first, const Item::Ptr &second, DynamicContext* context, const LocationInfo *info)
{
  // From $ 15.1.4 of the F&O specs:
  // The effective boolean value of an operand is defined as follows:
  //
  // If its operand is an empty sequence, fn:boolean returns false.
  if(first == NULLRCP) {
    return false;
  }

  // If its operand is a sequence whose first item is a node, fn:boolean returns true.
  if(first->isNode())
    return true;

  return getEffectiveBooleanValueInternal(first, second, context, info);
}

Sequence ResultImpl::toSequence(DynamicContext *context)
{
  Sequence result(context->getMemoryManager());

  Item::Ptr item = 0;
  while((item = next(context)) != NULLRCP) {
    result.addItem(item);
  }

  return result;
}

std::string ResultImpl::getIndent(int indent)
{
  std::ostringstream oss;

  for(int i = 0; i < indent; ++i) {
    oss << "  ";
  }

  return oss.str();
}
