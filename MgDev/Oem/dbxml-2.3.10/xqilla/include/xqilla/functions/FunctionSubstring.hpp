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
 * $Id: FunctionSubstring.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

/*

  Substring function

*/

#ifndef _FUNCTIONSUBSTRING_HPP
#define _FUNCTIONSUBSTRING_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/** Substring function. */
class XQILLA_API FunctionSubstring : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionSubstring(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  /** Returns the substring to the right of parm2 until optional parm3 from string parm1 **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONSUBSTRING_HPP





