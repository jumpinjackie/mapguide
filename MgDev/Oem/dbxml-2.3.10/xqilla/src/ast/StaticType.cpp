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
 * $Id: StaticType.cpp,v 1.5 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"

#include <xqilla/ast/StaticType.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/context/StaticContext.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>

unsigned int StaticType::getFlagsFor(const XMLCh *uri, const XMLCh *name, const StaticContext *context,
                                     bool &isExact)
{
  if(XPath2Utils::equals(name, AnyAtomicType::fgDT_ANYATOMICTYPE) &&
     XPath2Utils::equals(uri, FunctionConstructor::XMLChXPath2DatatypesURI)) {
    isExact = true;
    return ANY_ATOMIC_TYPE;
  }
  else {
    return getFlagsFor(context->getItemFactory()->
                       getPrimitiveTypeIndex(uri, name, /*isPrimitive*/isExact));
  }
}

unsigned int StaticType::getFlagsFor(AnyAtomicType::AtomicObjectType primitiveType)
{
  switch(primitiveType) {
  case AnyAtomicType::ANY_SIMPLE_TYPE: return StaticType::ANY_SIMPLE_TYPE;
  case AnyAtomicType::ANY_URI: return StaticType::ANY_URI_TYPE;
  case AnyAtomicType::BASE_64_BINARY: return StaticType::BASE_64_BINARY_TYPE;
  case AnyAtomicType::BOOLEAN: return StaticType::BOOLEAN_TYPE;
  case AnyAtomicType::DATE: return StaticType::DATE_TYPE;
  case AnyAtomicType::DATE_TIME: return StaticType::DATE_TIME_TYPE;
  case AnyAtomicType::DAY_TIME_DURATION: return StaticType::DAY_TIME_DURATION_TYPE;
  case AnyAtomicType::DECIMAL: return StaticType::DECIMAL_TYPE;
  case AnyAtomicType::DOUBLE: return StaticType::DOUBLE_TYPE;
  case AnyAtomicType::DURATION: return StaticType::DURATION_TYPE;
  case AnyAtomicType::FLOAT: return StaticType::FLOAT_TYPE;
  case AnyAtomicType::G_DAY: return StaticType::G_DAY_TYPE;
  case AnyAtomicType::G_MONTH: return StaticType::G_MONTH_TYPE;
  case AnyAtomicType::G_MONTH_DAY: return StaticType::G_MONTH_DAY_TYPE;
  case AnyAtomicType::G_YEAR: return StaticType::G_YEAR_TYPE;
  case AnyAtomicType::G_YEAR_MONTH: return StaticType::G_YEAR_MONTH_TYPE;
  case AnyAtomicType::HEX_BINARY: return StaticType::HEX_BINARY_TYPE;
  case AnyAtomicType::NOTATION: return StaticType::NOTATION_TYPE;
  case AnyAtomicType::QNAME: return StaticType::QNAME_TYPE;
  case AnyAtomicType::STRING: return StaticType::STRING_TYPE;
  case AnyAtomicType::TIME: return StaticType::TIME_TYPE;
  case AnyAtomicType::UNTYPED_ATOMIC: return StaticType::UNTYPED_ATOMIC_TYPE;
  case AnyAtomicType::YEAR_MONTH_DURATION: return StaticType::YEAR_MONTH_DURATION_TYPE;
  default: break;
  }
  return 0;
}

void StaticType::typeUnion(const StaticType &st)
{
  flags |= st.flags;
}

void StaticType::typeIntersect(const StaticType &st)
{
  flags &= st.flags;
}

bool StaticType::containsType(unsigned int type) const
{
  return (flags & type) != 0;
}

bool StaticType::isType(unsigned int type) const
{
  return (flags & type) != 0 && (flags & ~type) == 0;
}
