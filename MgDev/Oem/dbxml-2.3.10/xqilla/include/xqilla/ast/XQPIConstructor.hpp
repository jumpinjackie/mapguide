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
 * $Id: XQPIConstructor.hpp,v 1.1 2006/11/03 17:04:19 jpcs Exp $
 */

#if !defined(AFXQ_XQPICONSTRUCTOR_H__56E97972_3896_49D3_B055_36CC3E9E550A__INCLUDED_)
#define AFXQ_XQPICONSTRUCTOR_H__56E97972_3896_49D3_B055_36CC3E9E550A__INCLUDED_

#include <xqilla/ast/XQDOMConstructor.hpp>

class XQILLA_API XQPIConstructor : public XQDOMConstructor
{
public:
  XQPIConstructor(ASTNode* name, ASTNode* value, XPath2MemoryManager* mm);

  virtual Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  virtual const XMLCh* getNodeType() const;
  virtual const ASTNode *getName() const;
  virtual const ASTNode *getValue() const;

  virtual void setName(ASTNode *name);
  virtual void setValue(ASTNode *value);

private:
  ASTNode* m_name, *m_value;
};

#endif
