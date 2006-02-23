/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _NUMERICFUNCTION_HPP
#define _NUMERICFUNCTION_HPP

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

class Numeric;

/** Virtual interface class for functions */
class PATHAN_EXPORT NumericFunction : public ConstantFoldingFunction
{
public:

  /// constructor, checks for the correct number of arguments.
  NumericFunction(const XMLCh* name, unsigned int argsFrom, unsigned int argsTo, const char* paramDecl, const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

protected:

	/** casts the expression given as a parameter into the appropriate type and returns the guaranteed correctly typed objects in a sequence  */
	Result getParamNumber(unsigned int number, DynamicContext* context, int flags=0) const;

	/** casts the expression given as a parameter into the specified type and returns the guaranteed correctly typed objects in a sequence  */
	Result getParamNumber(unsigned int paramNumber, unsigned int signatureNumber, DynamicContext* context, int flags=0) const;

  /** Checks if the given numer is a NaN */
  bool isNaN(const Numeric::Ptr &number) const;

  /** Checks if the given numer is an infinite number */
  bool isINF(const Numeric::Ptr &number) const;
};
#endif // _NUMERICFUNCTION_HPP
