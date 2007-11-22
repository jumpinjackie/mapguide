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
 * $Id: XQIf.hpp,v 1.8 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _XQIF_HPP
#define _XQIF_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ASTNodeImpl.hpp>

/** If function. */
class XQILLA_API XQIf : public ASTNodeImpl
{
public:
  XQIf(ASTNode* test, ASTNode* whenTrue, ASTNode* whenFalse, XPath2MemoryManager* memMgr);
  
  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  const ASTNode *getTest() const;
  const ASTNode *getWhenTrue() const;
  const ASTNode *getWhenFalse() const;

  void setTest(ASTNode *item);
  void setWhenTrue(ASTNode *item);
  void setWhenFalse(ASTNode *item);

protected:
  class IfResult : public ResultImpl
  {
  public:
    IfResult(const XQIf *di, int flags);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;

  private:
    int _flags;
    const XQIf *_di;
    Result _results;
  };

  ASTNode* _test,* _whenTrue,* _whenFalse;
};

#endif // _XQIF_HPP


