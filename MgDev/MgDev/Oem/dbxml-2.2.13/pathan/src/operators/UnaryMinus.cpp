/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <assert.h>
#include <pathan/operators/UnaryMinus.hpp>
#include <pathan/operators/Multiply.hpp>
#include <pathan/dataItem/DataItemLiteral.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>

/*static*/ const XMLCh UnaryMinus::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

UnaryMinus::UnaryMinus(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ArithmeticOperator(name, args, memMgr)
{
	assert(_args.size() == 1);
}

Item::Ptr UnaryMinus::execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const
{
  assert(atom2 == NULLRCP);

  if(atom1 == NULLRCP) return 0;

  // only works on Numeric types
  if(atom1->isNumericValue()) {
    return (const Item::Ptr)((Numeric*)(const AnyAtomicType*)atom1)->invert(context);
  } else {
    DSLthrow(XPath2ErrorException,X("UnaryMinus::collapseTreeInternal"), X("An attempt to apply unary minus a non numeric type has occurred [err:XPTY0004]"));
  }
}
