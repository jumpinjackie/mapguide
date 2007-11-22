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
 * $Id: XQContextItem.hpp,v 1.10 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _XQCONTEXTITEM_HPP
#define _XQCONTEXTITEM_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/runtime/SingleResult.hpp>

/** Returns the context item. Raises [err:XP0002] if the context item is null. */
class XQILLA_API XQContextItem : public ASTNodeImpl
{
public:
  XQContextItem(XPath2MemoryManager* memMgr);
  ~XQContextItem();

  /**
   * Called during static analysis of expression to determine is statically
   * correct.  If not an appropriate error is thrown.
   */
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  virtual Result createResult(DynamicContext* context, int flags=0) const;

  class XQILLA_API ContextItemResult : public SingleResult
  {
  public:
    ContextItemResult(const LocationInfo *location);
    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;    
  };
};

#endif

