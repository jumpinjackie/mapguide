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
 * $Id: FunctionRoundHalfToEven.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

/*
  xf:round(numeric? $srcval) => numeric?
*/

#ifndef _FUNCTIONROUNDHALFTOEVEN_HPP
#define _FUNCTIONROUNDHALFTOEVEN_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/NumericFunction.hpp>

class XQILLA_API FunctionRoundHalfToEven : public NumericFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionRoundHalfToEven(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONROUNDHALFTOEVEN_HPP

