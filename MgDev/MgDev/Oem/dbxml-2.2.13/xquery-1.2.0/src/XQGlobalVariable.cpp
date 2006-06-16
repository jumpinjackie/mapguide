//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQGlobalVariable.cpp,v 1.17 2005/08/04 15:48:10 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQGlobalVariable.cpp: implementation of the XQGlobalVariable class.
//////////////////////////////////////////////////////////////////////

#include "xquery/XQEngine.hpp"
#include "xquery/XQContext.hpp"
#include "xquery/dataItem/XQGlobalVariable.hpp"
#include <pathan/Sequence.hpp>
#include <pathan/SequenceType.hpp>
#include <pathan/VariableStore.hpp>
#include <pathan/simpleVariables/VariableTypeStore.hpp>
#include <pathan/XPath2NSUtils.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/IllegalArgumentException.hpp>
#include <pathan/exceptions/XPath2TypeMatchException.hpp>
#include <xercesc/framework/XMLBuffer.hpp>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XQGlobalVariable::XQGlobalVariable(const XMLCh* varQName, SequenceType* seqType, DataItem* value, XPath2MemoryManager* expr)
  : DataItemImpl(expr),
    m_szURI(0),
    m_szLocalName(0),
    m_Type(seqType),
    m_Value(value)
{
  m_szQName=expr->getPooledString(varQName);
  setType((DataItem::whichType)XQContext::VARIABLE_DEFINITION);
}

Sequence XQGlobalVariable::collapseTreeInternal(DynamicContext* context, int flags) const
{
  VariableStore* varStore = context->getVariableStore();
  Sequence variableValue(context->getMemoryManager());
  if(m_Value==NULL) {
    // It's an external declaration, so check the user has set the value in the variable store
    std::pair<bool, Sequence> value=varStore->getGlobalVar(m_szURI, m_szLocalName, context);
    if(!value.first) {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer errMsg;
      errMsg.set(X("A value for the external variable '"));
      errMsg.append(m_szQName);
      errMsg.append(X("' has not been provided"));
      DSLthrow(IllegalArgumentException,X("XQGlobalVariable::collapseTreeInternal"),errMsg.getRawBuffer());
    }
    variableValue=value.second;
  }
  else {
    variableValue=m_Value->collapseTree(context).toSequence(context);
    varStore->setGlobalVar(m_szURI, m_szLocalName, variableValue, context);
  }
  if(m_Type!=NULL)
  {
    Result matchesRes=m_Type->matches(variableValue, context);
    try {
      while(matchesRes.next(context) != NULLRCP) {}
    }
    catch(const XPath2TypeMatchException &ex) {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer errMsg;
      errMsg.set(X("The value for the global variable '"));
      errMsg.append(m_szQName);
      errMsg.append(X("' does not match the declared type: "));
      errMsg.append(ex.getError());
      DSLthrow(XPath2TypeMatchException,X("XQGlobalVariable::collapseTreeInternal"),errMsg.getRawBuffer());
    }
  }

  return Sequence(context->getMemoryManager());
}

DataItem* XQGlobalVariable::staticResolution(StaticContext* context)
{
  m_szURI = context->getUriBoundToPrefix(XPath2NSUtils::getPrefix(m_szQName, context->getMemoryManager()));
  m_szLocalName = XPath2NSUtils::getLocalName(m_szQName);
  VariableTypeStore* varStore = context->getVariableTypeStore();
  if(m_Value!=NULL) {
    m_Value = m_Value->staticResolution(context);
    _src.add(m_Value->getStaticResolutionContext());
    varStore->declareGlobalVar(m_szURI, m_szLocalName, m_Value->getStaticResolutionContext());
  }
  else {
    m_Type->getItemType()->getStaticType(_src.getStaticType(), context);
    varStore->declareGlobalVar(m_szURI, m_szLocalName, _src);
  }
  return this;
}

const XMLCh* XQGlobalVariable::getVariableName() const
{
  return m_szQName;
}

bool XQGlobalVariable::isExternal() const
{
  return (m_Value==NULL);
}

void XQGlobalVariable::setVariableExpr(DataItem* value)
{
  m_Value=value;
}

const SequenceType *XQGlobalVariable::getSequenceType() const
{
  return m_Type;
}

const DataItem *XQGlobalVariable::getVariableExpr() const
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
