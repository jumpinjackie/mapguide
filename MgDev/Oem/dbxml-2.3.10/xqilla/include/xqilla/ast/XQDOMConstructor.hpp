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
 * $Id: XQDOMConstructor.hpp,v 1.12 2006/11/03 17:04:19 jpcs Exp $
 */

#if !defined(AFXQ_XQDOMCONSTRUCTOR_H__56E97972_3896_49D3_B055_36CC3E9E550A__INCLUDED_)
#define AFXQ_XQDOMCONSTRUCTOR_H__56E97972_3896_49D3_B055_36CC3E9E550A__INCLUDED_

#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/items/Node.hpp>

#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/util/RefHashTableOf.hpp>

class XQILLA_API XQDOMConstructor : public ASTNodeImpl
{
public:
  XQDOMConstructor(XPath2MemoryManager* mm);

  virtual const XMLCh* getNodeType() const = 0;
  virtual const ASTNode *getName() const { return 0; }
  virtual const VectorOfASTNodes *getAttributes() const { return 0; }
  virtual const VectorOfASTNodes *getChildren() const { return 0; }
  virtual const ASTNode *getValue() const { return 0; }

  virtual void setName(ASTNode *name) {}
  virtual void setValue(ASTNode *value) {}

protected:
  static bool getStringValue(const VectorOfASTNodes* m_children, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer &value,
                             DynamicContext *context);
  static bool getStringValue(const ASTNode *child, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer &value,
                             DynamicContext *context);
  void unescapeEntities(XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer& buff) const;
};

#endif
