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
 * $Id: FunctionInsertBefore.hpp,v 1.10 2006/11/01 16:37:13 jpcs Exp $
 */

/*
  xf:insert-before(item* $target, decimal $position, item* $inserts) => item*
*/

#ifndef _FUNCTIONINSERTBEFORE_HPP
#define _FUNCTIONINSERTBEFORE_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

class XQILLA_API FunctionInsertBefore : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionInsertBefore(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  ASTNode* staticResolution(StaticContext *context);
  ASTNode *staticTyping(StaticContext *context);
  Result createResult(DynamicContext* context, int flags=0) const;

private:
  class InsertBeforeResult : public ResultImpl
  {
  public:
    InsertBeforeResult(const FunctionInsertBefore *func);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;
  private:
    const FunctionInsertBefore *_func;
    Numeric::Ptr _position;
    Numeric::Ptr _one;
    Numeric::Ptr _i;
    bool _insertsDone;
    Result _target;
    Result _inserts;
  };
};

#endif // _FUNCTIONINSERTBEFORE_HPP
