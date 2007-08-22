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
 * $Id: Mod.cpp,v 1.10 2006/11/01 16:37:21 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <xqilla/operators/Mod.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/items/Numeric.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/context/DynamicContext.hpp>

/*static*/ const XMLCh Mod::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

Mod::Mod(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ArithmeticOperator(name, args, memMgr)
{
}

void Mod::calculateStaticType()
{
  const StaticType &arg0 = _args[0]->getStaticResolutionContext().getStaticType();
  const StaticType &arg1 = _args[1]->getStaticResolutionContext().getStaticType();

  calculateStaticTypeForNumerics(arg0, arg1);
}

Item::Ptr Mod::execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const
{
  if(atom1 == NULLRCP || atom2 == NULLRCP) return 0;
  
  if(atom1->isNumericValue()) {
    if(atom2->isNumericValue()) {
      return (const Item::Ptr)((Numeric*)(const AnyAtomicType*)atom1)->mod((const Numeric::Ptr )atom2, context);
    }
    else {
      XQThrow(XPath2ErrorException,X("Mod::collapseTreeInternal"), X("An attempt to mod a non numeric type to a numeric type has occurred [err:XPTY0004]"));
		}
	}

  XQThrow(XPath2ErrorException,X("Mod::collapseTreeInternal"), X("The operator mod has been called on invalid operand types [err:XPTY0004]"));
}

