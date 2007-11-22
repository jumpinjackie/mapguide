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
 * $Id: FunctionRemove.hpp,v 1.10 2006/11/01 16:37:13 jpcs Exp $
 */

#ifndef _FUNCTIONREMOVE_HPP
#define _FUNCTIONREMOVE_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

class XQILLA_API FunctionRemove : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionRemove(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  ASTNode* staticResolution(StaticContext *context);
  ASTNode *staticTyping(StaticContext *context);
  Result createResult(DynamicContext* context, int flags=0) const;

private:
  class RemoveResult : public ResultImpl
  {
  public:
    RemoveResult(const FunctionRemove *func);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;
  private:
    const FunctionRemove *_func;
    Numeric::Ptr _position;
    Numeric::Ptr _one;
    Numeric::Ptr _i;
    bool _removeDone;
    Result _target;
  };
};

#endif // _FUNCTIONREMOVE_HPP
