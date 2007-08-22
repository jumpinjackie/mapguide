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
 * $Id: XQVariable.cpp,v 1.16 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/ast/XQVariable.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/parser/QName.hpp>
#include <xqilla/context/VariableStore.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/context/VarHashEntry.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>
#include <xqilla/exceptions/DynamicErrorException.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xercesc/framework/XMLBuffer.hpp>

XQVariable::XQVariable(const XMLCh *qualifiedName, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr),
    _uri(0),
    _staticProperties(0)
{
  setType(ASTNode::VARIABLE);

  QualifiedName qname(qualifiedName, getMemoryManager());
  _prefix = qname.getPrefix();
  _name = qname.getName();
}

XQVariable::XQVariable(const XMLCh *prefix, const XMLCh *name, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr),
    _uri(0),
    _staticProperties(0)
{
  setType(ASTNode::VARIABLE);

  _prefix = getMemoryManager()->getPooledString(prefix);
  _name = getMemoryManager()->getPooledString(name);
}

XQVariable::~XQVariable()
{
}

Result XQVariable::createResult(DynamicContext* context, int flags) const
{
  std::pair<bool, Sequence> var = context->getVariableStore()->getVar(_uri, _name, context);

  if(!var.first) {
    const XMLCh* qname = XPath2NSUtils::qualifyName(_prefix, _name, context->getMemoryManager());
    const XMLCh* msg = XPath2Utils::concatStrings(X("Variable "), qname, X(" does not exist [err:XPDY0002]"), context->getMemoryManager());
    XQThrow(DynamicErrorException, X("XQVariable::collapseTreeInternal"), msg);
  }

  return var.second;
}

ASTNode* XQVariable::staticResolution(StaticContext *context)
{
  // An unprefixed variable reference is in no namespace.
  if(_prefix && *_prefix)
    _uri = context->getUriBoundToPrefix(_prefix, this);
  return this;
}

ASTNode *XQVariable::staticTyping(StaticContext *context)
{
  _src.clear();

  const StaticResolutionContext *var_src = context->getVariableTypeStore()->getVar(_uri, _name);
  if(var_src == NULL || (var_src->getProperties() & StaticResolutionContext::UNDEFINEDVAR)!=0) {
    const XMLCh* qname = XPath2NSUtils::qualifyName(_prefix, _name, context->getMemoryManager());
    const XMLCh* msg = XPath2Utils::concatStrings(X("Variable "), qname, X(" does not exist [err:XPST0008]"),
                                                  context->getMemoryManager());
    XQThrow(StaticErrorException, X("XQVariable::staticResolution"), msg);
  }
  if((var_src->getProperties() & StaticResolutionContext::FORWARDREF)!=0) {
    XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer errMsg(1023, context->getMemoryManager());
    errMsg.set(X("Cannot refer to global variable with name {"));
    errMsg.append(_uri);
    errMsg.append(X("}"));
    errMsg.append(_name);
    errMsg.append(X(" because it is declared later [err:XQST0054]"));
    XQThrow(StaticErrorException,X("XQVariable::staticResolution"), errMsg.getRawBuffer());
  }
  _src.setProperties(var_src->getProperties() & ~(StaticResolutionContext::SUBTREE|StaticResolutionContext::SAMEDOC));
  _src.getStaticType() = var_src->getStaticType();
  _src.variableUsed(_uri, _name);

  return this;
}

const XMLCh *XQVariable::getPrefix() const {
  return _prefix;
}

const XMLCh *XQVariable::getURI() const {
  return _uri;
}

const XMLCh *XQVariable::getName() const {
  return _name;
}
