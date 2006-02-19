/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <assert.h>
#include <pathan/operators/IntegerDivide.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/DynamicContext.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

/*static*/ const XMLCh IntegerDivide::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_v, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

IntegerDivide::IntegerDivide(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ArithmeticOperator(name, args, memMgr)
{
  // Nothing to do
}

Item::Ptr IntegerDivide::execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const
{
  if(atom1 == NULLRCP || atom2 == NULLRCP) return 0;

  if(atom1->isNumericValue() && atom2->isNumericValue()) {
    // it divides the first argument by the second, and returns the integer obtained by truncating the fractional part 
    // of the result.
    Numeric::Ptr div=((Numeric*)(const AnyAtomicType*)atom1)->divide((const Numeric::Ptr )atom2, context);
    if(div->isPositive()) div = div->floor(context);
    else if(div->isNegative()) div = div->ceiling(context);

    return div->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                       XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context);
  }
  DSLthrow(XPath2ErrorException,X("IntegerDivide::execute"), X("An attempt to integer divide a non numeric type to a numeric type has occurred [err:XPTY0004]"));
}

