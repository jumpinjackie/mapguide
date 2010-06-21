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
 * $Id: ConvertFunctionArg.hpp,v 1.4 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _CONVERTFUNCTIONARG_HPP
#define _CONVERTFUNCTIONARG_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/ASTNodeImpl.hpp>

class XQILLA_API XQPromoteUntyped : public ASTNodeImpl
{
public:
  XQPromoteUntyped(ASTNode* expr, const XMLCh *uri, const XMLCh *name, XPath2MemoryManager* memMgr);

  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  const ASTNode *getExpression() const { return expr_; }
  void setExpression(ASTNode *expr) { expr_ = expr; }
  const XMLCh *getTypeURI() const { return uri_; }
  const XMLCh *getTypeName() const { return name_; }

protected:
  class PromoteUntypedResult : public ResultImpl
  {
  public:
    PromoteUntypedResult(const XQPromoteUntyped *di, const Result &parent, const XMLCh *uri, const XMLCh *name)
      : ResultImpl(di), parent_(parent), uri_(uri), name_(name) {}

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const { return "promoteuntypedresult"; }
  private:
    Result parent_;
    const XMLCh *uri_, *name_;
  };

  ASTNode* expr_;
  const XMLCh *uri_, *name_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class XQILLA_API XQPromoteNumeric : public ASTNodeImpl
{
public:
  XQPromoteNumeric(ASTNode* expr, const XMLCh *uri, const XMLCh *name, XPath2MemoryManager* memMgr);

  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  const ASTNode *getExpression() const { return expr_; }
  void setExpression(ASTNode *expr) { expr_ = expr; }
  const XMLCh *getTypeURI() const { return uri_; }
  const XMLCh *getTypeName() const { return name_; }

protected:
  class PromoteNumericResult : public ResultImpl
  {
  public:
    PromoteNumericResult(const XQPromoteNumeric *di, const Result &parent, const XMLCh *uri, const XMLCh *name)
      : ResultImpl(di), parent_(parent), uri_(uri), name_(name) {}

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const { return "promotenumericresult"; }
  private:
    Result parent_;
    const XMLCh *uri_, *name_;
  };

  ASTNode* expr_;
  const XMLCh *uri_, *name_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class XQILLA_API XQPromoteAnyURI : public ASTNodeImpl
{
public:
  XQPromoteAnyURI(ASTNode* expr, const XMLCh *uri, const XMLCh *name, XPath2MemoryManager* memMgr);

  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  const ASTNode *getExpression() const { return expr_; }
  void setExpression(ASTNode *expr) { expr_ = expr; }
  const XMLCh *getTypeURI() const { return uri_; }
  const XMLCh *getTypeName() const { return name_; }

protected:
  class PromoteAnyURIResult : public ResultImpl
  {
  public:
    PromoteAnyURIResult(const XQPromoteAnyURI *di, const Result &parent)
      : ResultImpl(di), parent_(parent) {}

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const { return "promoteanyuriresult"; }
  private:
    Result parent_;
  };

  ASTNode* expr_;
  const XMLCh *uri_, *name_;
};

#endif
