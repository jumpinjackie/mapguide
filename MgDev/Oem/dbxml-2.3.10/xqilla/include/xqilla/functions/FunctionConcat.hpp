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
 * $Id: FunctionConcat.hpp,v 1.8 2006/11/01 16:37:12 jpcs Exp $
 */

/*
  Concat operator function
*/

#ifndef _FUNCTIONCONCAT_HPP
#define _FUNCTIONCONCAT_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/** Concat operator function*/
class XQILLA_API FunctionConcat : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  ///Constructor.
  FunctionConcat(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
protected:
  /** concatenates two strings. Takes two strings, and gives a string */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONCONCAT_HPP
