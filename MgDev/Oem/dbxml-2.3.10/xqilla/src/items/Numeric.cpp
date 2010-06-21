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
 * $Id: Numeric.cpp,v 1.15 2006/11/01 16:37:20 jpcs Exp $
 */

#include <assert.h>

#include "../config/xqilla_config.h"
#include <xqilla/items/Numeric.hpp>
#include <xqilla/items/impl/ATDecimalOrDerivedImpl.hpp>
#include <xqilla/items/impl/ATFloatOrDerivedImpl.hpp>
#include <xqilla/items/impl/ATDoubleOrDerivedImpl.hpp>
#include <xqilla/mapm/m_apm.h>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

const XMLCh Numeric::NaN_string[] = 
{ chLatin_N, chLatin_a, chLatin_N, chNull };

const XMLCh Numeric::NAN_string[] = 
{ chLatin_N, chLatin_A, chLatin_N, chNull };

const XMLCh Numeric::INF_string[] = 
{ chLatin_I, chLatin_N, chLatin_F, chNull };

const XMLCh Numeric::NegINF_string[] =
{ chDash, chLatin_I, chLatin_N, chLatin_F, chNull };

const XMLCh Numeric::NegZero_string[] =
{ chDash, chDigit_0, chNull };

const XMLCh Numeric::PosZero_string[] =
{ chDigit_0, chNull };

void Numeric::checkFloatLimits(Numeric::State &state, MAPM &value)
{
  if(state==NUM || state==NEG_NUM) {
    int exp=value.exponent();
    if(exp>38 || (exp==38 && value.abs()>MAPM("3.4028235e+38"))) {
      state=(state==NEG_NUM)?NEG_INF:INF;
      value = MM_Zero;
    }
    else if(exp<-38 || (exp==-38 && value.abs()<MAPM("1.1754944e-38")))
      value=MM_Zero;
  }
}

void Numeric::checkDoubleLimits(Numeric::State &state, MAPM &value)
{
  if(state==NUM || state==NEG_NUM) {
    int exp=value.exponent();
    if(exp>308 || (exp==308 && value.abs()>MAPM("1.7976931348623157e+308"))) {
      state=(state==NEG_NUM)?NEG_INF:INF;
      value = MM_Zero;
    }
    else if(exp<-308 || (exp==-308 && value.abs()<MAPM("2.2250738585072014e-308")))
      value=MM_Zero;
  }
}

inline int compare(Numeric::State state1, const MAPM &value1, Numeric::State state2, const MAPM &value2)
{
  // Arbitrarily ranks NaN as greater than all other states

  int cmp = state1 - state2;
  if(cmp != 0) return cmp;

  switch(state1) {
  case Numeric::NaN:
  case Numeric::INF:
  case Numeric::NEG_INF:
    return 0;
  case Numeric::NUM:
  case Numeric::NEG_NUM:
    return value1.compare(value2);
  }

  assert(false); // should never get here
  return 0;
}

inline int typePromoteCompare(const Numeric::Ptr &num1, const Numeric::Ptr &num2)
{
  Numeric::State state1 = num1->getState();
  MAPM value1 = num1->asMAPM();

  Numeric::State state2 = num2->getState();
  MAPM value2 = num2->asMAPM();

  switch(num1->getPrimitiveTypeIndex()) {
  case AnyAtomicType::DECIMAL:
    switch(num2->getPrimitiveTypeIndex()) {
    case AnyAtomicType::DECIMAL:
      break;
    case AnyAtomicType::FLOAT:
      Numeric::checkFloatLimits(state1, value1);
      Numeric::checkFloatLimits(state2, value2);
      break;
    case AnyAtomicType::DOUBLE:
      Numeric::checkDoubleLimits(state1, value1);
      Numeric::checkDoubleLimits(state2, value2);
      break;
    default: break;
    }
    break;
  case AnyAtomicType::FLOAT:
    switch(num2->getPrimitiveTypeIndex()) {
    case AnyAtomicType::DECIMAL:
    case AnyAtomicType::FLOAT:
      Numeric::checkFloatLimits(state1, value1);
      Numeric::checkFloatLimits(state2, value2);
      break;
    case AnyAtomicType::DOUBLE:
      Numeric::checkDoubleLimits(state1, value1);
      Numeric::checkDoubleLimits(state2, value2);
      break;
    default: break;
    }
    break;
  case AnyAtomicType::DOUBLE:
    Numeric::checkDoubleLimits(state1, value1);
    Numeric::checkDoubleLimits(state2, value2);
    break;
  default: break;
  }

  return compare(state1, value1, state2, value2);
}

int Numeric::compare(const Numeric::Ptr &other, const DynamicContext *context) const
{
  return ::compare(getState(), asMAPM(), other->getState(), other->asMAPM());
}

bool Numeric::equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const
{
  if(!target->isNumericValue()) {
    XQThrow2(::IllegalArgumentException,X("Numeric::equals"),
	    X("Equality operator for given types not supported [err:XPTY0004]"));
  } 

  const Numeric *otherImpl = (const Numeric*)target.get();

  if(getState() == NaN || otherImpl->getState() == NaN) return false;
  return typePromoteCompare(this, otherImpl) == 0;
}

/** Returns true if this is less than other, false otherwise */
bool Numeric::lessThan(const Numeric::Ptr &other, const DynamicContext* context) const
{
  if(getState() == NaN || other->getState() == NaN) return false;
  return typePromoteCompare(this, other) < 0;
}

/** Returns true if this is greater than other, false otherwise */
bool Numeric::greaterThan(const Numeric::Ptr &other, const DynamicContext* context) const
{
  if(getState() == NaN || other->getState() == NaN) return false;
  return typePromoteCompare(this, other) > 0;
}

AnyAtomicType::Ptr Numeric::castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetURI,
                                           const XMLCh* targetType, const DynamicContext* context) const
{
  switch(targetIndex) {
  case DECIMAL:
    switch(getState()) {
    case NaN:
    case INF:
    case NEG_INF:
      XQThrow2(::IllegalArgumentException, X("Numeric::castAsInternal"),
              X("Special values like NaN, INF or -INF cannot be cast to decimal [err:FOCA0002]"));
    case NUM:
    case NEG_NUM:
      if(context->isTypeOrDerivedFromType(targetURI, targetType,
                                          SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                          SchemaSymbols::fgDT_INTEGER)) {
        if(isNegative()) {
          return context->getItemFactory()->createDecimalOrDerived(targetURI, targetType, asMAPM().ceil(), context);
        }
        else {
          return context->getItemFactory()->createDecimalOrDerived(targetURI, targetType, asMAPM().floor(), context);
        }
      }    
      else {
        return context->getItemFactory()->createDecimalOrDerived(targetURI, targetType, asMAPM(), context);
      }
    }
  case FLOAT:
    switch(getState()) {
    case NaN:
      return context->getItemFactory()->createFloatOrDerived(targetURI, targetType, NaN_string, context);
    case INF:
      return context->getItemFactory()->createFloatOrDerived(targetURI, targetType, INF_string, context);
    case NEG_INF:
      return context->getItemFactory()->createFloatOrDerived(targetURI, targetType, NegINF_string, context);
    case NEG_NUM:
      if(isZero()) {
        return context->getItemFactory()->createFloatOrDerived(targetURI, targetType, NegZero_string, context);
      }
      // Fall through
    case NUM:
      return context->getItemFactory()->createFloatOrDerived(targetURI, targetType, asMAPM(), context);
    }
  case DOUBLE:
    switch(getState()) {
    case NaN:
      return context->getItemFactory()->createDoubleOrDerived(targetURI, targetType, NaN_string, context);
    case INF:
      return context->getItemFactory()->createDoubleOrDerived(targetURI, targetType, INF_string, context);
    case NEG_INF:
      return context->getItemFactory()->createDoubleOrDerived(targetURI, targetType, NegINF_string, context);
    case NEG_NUM:
      if(isZero()) {
        return context->getItemFactory()->createDoubleOrDerived(targetURI, targetType, NegZero_string, context);
      }
      // Fall through
    case NUM:
      return context->getItemFactory()->createDoubleOrDerived(targetURI, targetType, asMAPM(), context);
    }
  case BOOLEAN:
    if(isZero() || isNaN()) {
      return context->getItemFactory()->createBooleanOrDerived(targetURI, targetType, false, context);
    } else {
      return context->getItemFactory()->createBooleanOrDerived(targetURI, targetType, true, context);
    }
  default:
    return AnyAtomicType::castAsInternal(targetIndex, targetURI, targetType, context);
  }
}

const XMLCh *Numeric::asDecimalString(int significantDigits, const StaticContext* context) const
{
  return asDecimalString(asMAPM(), significantDigits, context);
}

const XMLCh *Numeric::asDecimalString(const MAPM &number, int significantDigits, const StaticContext* context)
{
  char obuf[1024];
  if(number.is_integer())
    number.toIntegerString(obuf);
  else {
    number.toFixPtString(obuf, significantDigits);

    // Note in the canonical representation the decimal point is required
    // and there must be at least one digit to the right and one digit to 
    // the left of the decimal point (which may be 0)
    if(strchr(obuf,'.')!=0) {
      // remove trailing 0's
      char* lastChar=obuf+strlen(obuf)-1;
      while(*lastChar=='0') {
        *lastChar--=0;
      }
      // remove decimal point, if there are no digits after it
      if(*lastChar=='.')
        *lastChar=0;
    }
  }
  return context->getMemoryManager()->getPooledString(obuf);
}

const XMLCh *Numeric::asDoubleString(int significantDigits, const StaticContext* context) const
{
  State state1 = getState();
  MAPM value1 = asMAPM();

  switch(getPrimitiveTypeIndex()) {
  case FLOAT:
    checkFloatLimits(state1, value1);
    break;
  case DOUBLE:
    checkDoubleLimits(state1, value1);
    break;
  default: break;
  }

  return asDoubleString(state1, value1, significantDigits, context);
}

const XMLCh *Numeric::asDoubleString(State state1, const MAPM &value1, int significantDigits, const StaticContext* context)
{
  switch(state1) {
  case NaN:     return NaN_string;
  case INF:     return INF_string;
  case NEG_INF: return NegINF_string;
  case NEG_NUM:
    if(value1.sign() == 0) return NegZero_string;
    break;
  case NUM:
    if(value1.sign() == 0) return PosZero_string;
    break;
  }

  MAPM absVal = value1.abs();
  MAPM lower("0.000001"), upper("1000000");
  if(absVal < upper && absVal >= lower) {
    return asDecimalString(value1, significantDigits, context);
  }
  else {
    char obuf[1024];
    if(significantDigits==7)
    {
      value1.toString(obuf, 25);
      double num=atof(obuf);
      sprintf(obuf, "%.*G", min(8,value1.significant_digits()), num);
      MAPM val=obuf;
      int precision = val.significant_digits() - 1;
      if(precision <= 0) precision = 1;
      val.toString(obuf, precision);
      return context->getMemoryManager()->getPooledString(obuf);
    }
    else if(significantDigits==16)
    {
      value1.toString(obuf, 25);
      double num=atof(obuf);
      sprintf(obuf, "%.*G", min(17,value1.significant_digits()), num);
      MAPM val=obuf;
      int precision = val.significant_digits() - 1;
      if(precision <= 0) precision = 1;
      val.toString(obuf, precision);
      return context->getMemoryManager()->getPooledString(obuf);
    }
    int precision = value1.significant_digits() - 1;
    if(precision > significantDigits) precision = significantDigits;
    if(precision <= 0) precision = 1;
    value1.toString(obuf, precision);
    return context->getMemoryManager()->getPooledString(obuf);
  }

  return 0;
}
