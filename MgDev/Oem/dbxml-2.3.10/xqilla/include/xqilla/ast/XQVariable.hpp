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
 * $Id: XQVariable.hpp,v 1.7 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _XQVARIABLE_HPP
#define _XQVARIABLE_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/runtime/Sequence.hpp>

/** Deals with all the simple variable stuff.*/
class XQILLA_API XQVariable : public ASTNodeImpl
{
public:
  XQVariable(const XMLCh *qualifiedName, XPath2MemoryManager* memMgr);
  XQVariable(const XMLCh *prefix, const XMLCh *name, XPath2MemoryManager* memMgr);
  ~XQVariable();

  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  const XMLCh *getPrefix() const;
  const XMLCh *getURI() const;
  const XMLCh *getName() const;

private:
  const XMLCh* _prefix;
  const XMLCh* _uri;
  const XMLCh* _name;
  unsigned int _staticProperties;
};

#endif


