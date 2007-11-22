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
 * $Id: Range.hpp,v 1.9 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _RANGE_HPP
#define _RANGE_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/XQOperator.hpp>

class Numeric;

/** Range operator function*/
class XQILLA_API Range : public XQOperator
{
public:
  static const XMLCh name[];

  ///Constructor
  Range(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);
  Result createResult(DynamicContext* context, int flags=0) const;

private:
  class RangeResult : public ResultImpl
  {
  public:
    RangeResult(const Range *op, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;

  private:
    const Range *_op;
    Numeric::Ptr _last;
    Numeric::Ptr _step;
    Numeric::Ptr _end;
  };
};

#endif // _RANGE_HPP


