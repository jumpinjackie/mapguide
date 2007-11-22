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
 * $Id: FunctionSubsequence.hpp,v 1.11 2006/11/01 16:37:13 jpcs Exp $
 */

#ifndef _FUNCTIONSUBSEQUENCE_HPP
#define _FUNCTIONSUBSEQUENCE_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

class XQILLA_API FunctionSubsequence : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionSubsequence(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);
  Result createResult(DynamicContext* context, int flags=0) const;

private:
  class SubsequenceResult : public ResultImpl
  {
  public:
    SubsequenceResult(const FunctionSubsequence *func, int flags);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;
  private:
    int _flags;
    const FunctionSubsequence *_func;
    Numeric::Ptr _end;
    Numeric::Ptr _one;
    Numeric::Ptr _i;
    Result _source;
  };
};

#endif // _FUNCTIONSUBSEQUENCE_HPP





