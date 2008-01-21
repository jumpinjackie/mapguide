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
 * $Id: XQElementConstructor.hpp,v 1.1 2006/11/03 17:04:19 jpcs Exp $
 */

#if !defined(AFXQ_XQELEMENTCONSTRUCTOR_H__56E97972_3896_49D3_B055_36CC3E9E550A__INCLUDED_)
#define AFXQ_XQELEMENTCONSTRUCTOR_H__56E97972_3896_49D3_B055_36CC3E9E550A__INCLUDED_

#include <xqilla/ast/XQDOMConstructor.hpp>

#include <xercesc/util/RefHashTableOf.hpp>

class XQILLA_API XQElementConstructor : public XQDOMConstructor
{
public:
  XQElementConstructor(ASTNode* name, VectorOfASTNodes* attrList, VectorOfASTNodes* children, XPath2MemoryManager *mm);

  virtual Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  virtual const XMLCh* getNodeType() const;
  virtual const ASTNode *getName() const;
  virtual const VectorOfASTNodes *getAttributes() const;
  virtual const VectorOfASTNodes *getChildren() const;

  virtual void setName(ASTNode *name);

protected:
  ASTNode* m_name;
  VectorOfASTNodes* m_attrList, *m_children;
  XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf< XMLCh >* m_namespaces;
};

#endif
