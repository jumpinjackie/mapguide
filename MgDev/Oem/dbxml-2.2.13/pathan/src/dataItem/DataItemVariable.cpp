/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/dataItem/DataItemVariable.hpp>
#include <pathan/XPath2NSUtils.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/parser/QName.hpp>
#include <pathan/VariableStore.hpp>
#include <pathan/simpleVariables/VariableTypeStore.hpp>
#include <pathan/simpleVariables/VarHashEntry.hpp>
#include <pathan/exceptions/StaticErrorException.hpp>
#include <pathan/exceptions/DynamicErrorException.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>

DataItemVariable::DataItemVariable(const XMLCh *qualifiedName, XPath2MemoryManager* memMgr)
  : DataItemImpl(memMgr),
    _uri(0),
    _staticProperties(0)
{
  setType(DataItem::VARIABLE);

  QualifiedName qname(qualifiedName, getMemoryManager());
  _prefix = qname.getPrefix();
  _name = qname.getName();
}

DataItemVariable::DataItemVariable(const XMLCh *prefix, const XMLCh *name, XPath2MemoryManager* memMgr)
  : DataItemImpl(memMgr),
    _uri(0),
    _staticProperties(0)
{
  setType(DataItem::VARIABLE);

  _prefix = getMemoryManager()->getPooledString(prefix);
  _name = getMemoryManager()->getPooledString(name);
}

DataItemVariable::~DataItemVariable()
{
}

Result DataItemVariable::createResult(DynamicContext* context, int flags) const
{
  std::pair<bool, Sequence> var = context->getVariableStore()->getVar(_uri, _name, context);

  if(!var.first) {
    const XMLCh* qname = XPath2NSUtils::qualifyName(_prefix, _name, context->getMemoryManager());
    const XMLCh* msg = XPath2Utils::concatStrings(X("Variable "), qname, X(" does not exist [err:XPDY0002]"), context->getMemoryManager());
    DSLthrow(DynamicErrorException, X("DataItemVariable::collapseTreeInternal"), msg);
  }

  return var.second;
}

DataItem* DataItemVariable::staticResolution(StaticContext *context)
{
  _uri = context->getUriBoundToPrefix(_prefix);
  const StaticResolutionContext *var_src = context->getVariableTypeStore()->getVar(_uri, _name);
  if(var_src == NULL) {
    const XMLCh* qname = XPath2NSUtils::qualifyName(_prefix, _name, context->getMemoryManager());
    const XMLCh* msg = XPath2Utils::concatStrings(X("Variable "), qname, X(" does not exist [err:XPST0001]"), context->getMemoryManager());
    DSLthrow(StaticErrorException, X("DataItemVariable::staticResolution"), msg);
  }
  _src.setProperties(var_src->getProperties() & ~(StaticResolutionContext::SUBTREE|StaticResolutionContext::SAMEDOC));
  _src.getStaticType() = var_src->getStaticType();
  _src.variableUsed(_uri, _name);

  return resolvePredicates(context);
}

const XMLCh *DataItemVariable::getPrefix() const {
  return _prefix;
}

const XMLCh *DataItemVariable::getURI() const {
  return _uri;
}

const XMLCh *DataItemVariable::getName() const {
  return _name;
}
