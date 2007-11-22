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
 * $Id: XQGlobalVariable.hpp,v 1.12 2006/11/01 16:37:11 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQGlobalVariable.hpp: interface for the XQGlobalVariable class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFXQ_XQGLOBALVARIABLE_H__A97FA5AD_5355_45CD_9092_ED94F5CD035B__INCLUDED_)
#define AFXQ_XQGLOBALVARIABLE_H__A97FA5AD_5355_45CD_9092_ED94F5CD035B__INCLUDED_

#include <xqilla/ast/ASTNodeImpl.hpp>

class SequenceType;

class XQILLA_API XQGlobalVariable : public LocationInfo
{
public:
  XQGlobalVariable(const XMLCh* varQName, SequenceType* seqType, ASTNode* value, XPath2MemoryManager *mm);

  void execute(DynamicContext* context) const;
  void staticResolution(StaticContext *context);
  void staticTyping(StaticContext *context);

  const XMLCh* getVariableName() const;
  bool isExternal() const;
  void setVariableExpr(ASTNode* value);

  const XMLCh *getVariableURI() const;
  const XMLCh *getVariableLocalName() const;
  const SequenceType *getSequenceType() const;
  const ASTNode *getVariableExpr() const;
  const StaticResolutionContext &getStaticResolutionContext() const { return _src; }

protected:
  const XMLCh* m_szQName;
  const XMLCh* m_szURI;
  const XMLCh* m_szLocalName;
  SequenceType* m_Type;
  ASTNode* m_Value;
  StaticResolutionContext _src;
};

#endif // !defined(AFXQ_XQGLOBALVARIABLE_H__A97FA5AD_5355_45CD_9092_ED94F5CD035B__INCLUDED_)
