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
 * $Id: FunctionCodepointsToString.hpp,v 1.8 2006/11/01 16:37:12 jpcs Exp $
 */

/*
  codepoints-to-string function
*/

#ifndef _FUNCTIONCODEPOINTSTOSTRING_HPP
#define _FUNCTIONCODEPOINTSTOSTRING_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/** codepoints-to-string function */
class XQILLA_API FunctionCodepointsToString : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  ///Constructor.
  FunctionCodepointsToString(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
protected:
  /** takes a series of integers (codepoints) and turns them into a unicode string */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};
#endif // _FUNCTIONCODEPOINTSTOSTRING_HPP

