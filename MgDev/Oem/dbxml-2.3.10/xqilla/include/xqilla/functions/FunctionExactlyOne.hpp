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
 * $Id: FunctionExactlyOne.hpp,v 1.9 2006/11/01 16:37:13 jpcs Exp $
 */

/*
 * xf:exactly-one(item* $srcval) => item
 */

#ifndef _FUNCTIONEXACTLYONE_HPP
#define _FUNCTIONEXACTLYONE_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

class XQILLA_API FunctionExactlyOne : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionExactlyOne(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  Result createResult(DynamicContext* context, int flags=0) const;

private:
  class ExactlyOneResult : public ResultImpl
  {
  public:
    ExactlyOneResult(const FunctionExactlyOne *func, int flags);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;
  private:
    int _flags;
    const FunctionExactlyOne *_func;
    Result _arg;
    unsigned int _argNo;
  };
};

#endif // _FUNCTIONEXACTLYONE_HPP
