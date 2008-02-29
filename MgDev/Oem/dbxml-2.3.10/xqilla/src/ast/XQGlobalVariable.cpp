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
 * $Id: XQGlobalVariable.cpp,v 1.17 2006/11/01 16:37:18 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQGlobalVariable.cpp: implementation of the XQGlobalVariable class.
//////////////////////////////////////////////////////////////////////

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/XQGlobalVariable.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/context/VariableStore.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xqilla/ast/XQTreatAs.hpp>

XQGlobalVariable::XQGlobalVariable(const XMLCh* varQName, SequenceType* seqType, ASTNode* value, XPath2MemoryManager *mm)
  : m_szQName(mm->getPooledString(varQName)),
    m_szURI(0),
    m_szLocalName(0),
    m_Type(seqType),
    m_Value(value),
    _src(mm)
{
}

void XQGlobalVariable::execute(DynamicContext* context) const
{
  VariableStore* varStore = context->getVariableStore();
  try {
    if(m_Value == NULL) {
      // It's an external declaration, so check the user has set the value in the variable store
      std::pair<bool, Sequence> value = varStore->getGlobalVar(m_szURI, m_szLocalName, context);
      if(!value.first) {
        XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer errMsg;
        errMsg.set(X("A value for the external variable '"));
        errMsg.append(m_szQName);
        errMsg.append(X("' has not been provided [err:XPTY0002]"));
        XQThrow(IllegalArgumentException,X("XQGlobalVariable::collapseTreeInternal"),errMsg.getRawBuffer());
      }
      if(m_Type != NULL) {
        // Check the external value's type
        Result matchesRes = m_Type->matches(value.second, m_Type);
        while(matchesRes->next(context).notNull()) {}
      }
    }
    else {
      varStore->setGlobalVar(m_szURI, m_szLocalName, m_Value->collapseTree(context)->
                             toSequence(context), context);
    }
  }
  catch(const XPath2TypeMatchException &ex) {
    XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer errMsg;
    errMsg.set(X("The value for the global variable '"));
    errMsg.append(m_szQName);
    errMsg.append(X("' does not match the declared type: "));
    errMsg.append(ex.getError());
    XQThrow(XPath2TypeMatchException,X("XQGlobalVariable::collapseTreeInternal"),errMsg.getRawBuffer());
  }
}

void XQGlobalVariable::staticResolution(StaticContext* context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  if(m_Type) m_Type->staticResolution(context);

  // variables with no prefix are in no namespace
  const XMLCh* prefix=XPath2NSUtils::getPrefix(m_szQName, mm);
  if(prefix && *prefix)
    m_szURI = context->getUriBoundToPrefix(prefix, this);
  m_szLocalName = XPath2NSUtils::getLocalName(m_szQName);

  if(m_Value != NULL) {
    if(m_Type != NULL) {
      m_Value = new (mm) XQTreatAs(m_Value, m_Type, mm);
      m_Value->setLocationInfo(this);
    }
    m_Value = m_Value->staticResolution(context);
  }
}

void XQGlobalVariable::staticTyping(StaticContext* context)
{
  VariableTypeStore* varStore = context->getVariableTypeStore();

  if(m_Value != NULL) {
    m_Value = m_Value->staticTyping(context);
    _src.copy(m_Value->getStaticResolutionContext());
  }
  else {
    if(m_Type->getItemType() != NULL) {
      bool isPrimitive;
      m_Type->getItemType()->getStaticType(_src.getStaticType(), context, isPrimitive, m_Type);
    }
    else {
      _src.getStaticType().flags = 0;
    }
  }
  varStore->declareGlobalVar(m_szURI, m_szLocalName, _src);
}

const XMLCh* XQGlobalVariable::getVariableName() const
{
  return m_szQName;
}

bool XQGlobalVariable::isExternal() const
{
  return (m_Value==NULL);
}

void XQGlobalVariable::setVariableExpr(ASTNode* value)
{
  m_Value=value;
}

const SequenceType *XQGlobalVariable::getSequenceType() const
{
  return m_Type;
}

const ASTNode *XQGlobalVariable::getVariableExpr() const
{
  return m_Value;
}

const XMLCh *XQGlobalVariable::getVariableURI() const
{
  return m_szURI;
}

const XMLCh *XQGlobalVariable::getVariableLocalName() const
{
  return m_szLocalName;
}
