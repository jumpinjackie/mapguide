/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>

#include <pathan/dataItem/DataItemFor.hpp>

#include <pathan/SequenceType.hpp>
#include <pathan/VariableStore.hpp>
#include <pathan/simpleVariables/VariableTypeStore.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/context/DynamicContextImpl.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/dataItem/DataItemLiteral.hpp>
#include <pathan/dataItem/DataItemSequence.hpp>
#include <pathan/simpleVariables/VarHashEntry.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/internal/parser/QName.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/XPath2NSUtils.hpp>
#include <pathan/XPath2Utils.hpp>

DataItemFor::VB::VB(const XMLCh* variable, DataItem* value, XPath2MemoryManager* memMgr)
  : _variable(variable),
    _uri(0),
    _name(0),
    _allValues(value),
    _valuesExprDependsOnPreviousBindings(false),
    _needsNewScope(false),
    _src(memMgr)
{
}

DataItemFor::VB::VB(const VB &o, XPath2MemoryManager* memMgr)
  :	_variable(o._variable),
    _uri(o._uri),
    _name(o._name),
    _allValues(o._allValues),
    _valuesExprDependsOnPreviousBindings(o._valuesExprDependsOnPreviousBindings),
    _needsNewScope(o._needsNewScope),
    _src(o._src, memMgr)
{
}

DataItemFor::DataItemFor(VectorOfBindings* bindings, DataItem* returnExpr, XPath2MemoryManager* memMgr)
  : DataItemImpl(memMgr),
    _bindings(PathanAllocator<VB*>(memMgr)),
    _returnExpr(returnExpr)
{
  setType(DataItem::FOR);

  for(VectorOfBindings::iterator i = bindings->begin(); i != bindings->end(); ++i) {
    _bindings.push_back(new (memMgr) VB((*i)->getVariable(), (*i)->getValue(), memMgr));
  }
}

DataItemFor::VBExecution::VBExecution(const DataItemFor::VB *vb, DynamicContext *context)
  : _vb(vb),
    _values(0),
    _valuesBuffer(0),
    _refVar(0)
{
}

bool DataItemFor::VBExecution::initialise(DynamicContext *context)
{
  if(_vb->_valuesExprDependsOnPreviousBindings) {
    _values = _vb->_allValues->collapseTree(context);
  }
  else {
    if(_valuesBuffer.isNull()) {
      _valuesBuffer = _vb->_allValues->collapseTree(context);
    }
    _values = _valuesBuffer.createResult(context);
  }

  const Item::Ptr item = _values.next(context);

  if(item == NULLRCP) {
    return false;
  }

  VariableStore* varStore = context->getVariableStore();
  if(_vb->_needsNewScope) {
    varStore->addLogicalBlockScope();
  }
  if(_vb->_variable) {
    varStore->declareVar(_vb->_uri, _vb->_name, Sequence(item, context->getMemoryManager()), context);
    _refVar = varStore->getReferenceVar(_vb->_uri, _vb->_name, context);
  }

  return true;
}

bool DataItemFor::VBExecution::next(DynamicContext *context)
{
  const Item::Ptr item = _values.next(context);

  if(item == NULLRCP) {
    if(_vb->_needsNewScope) {
      context->getVariableStore()->removeScope();
    }
    return false;
  }

  if(_vb->_variable) {
    _refVar->setValue(Sequence(item, context->getMemoryManager()));
  }

  return true;
}

bool DataItemFor::nextState(ExecutionBindings &ebs, DynamicContext *context, bool initialisationState)
{
  ExecutionBindings::reverse_iterator rend = ebs.rend();
  ExecutionBindings::reverse_iterator rbegin = ebs.rbegin();
  ExecutionBindings::reverse_iterator it;

  if(initialisationState) {
    it = rend;
  }
  else {
    it = rbegin;
  }

  while(true) {
    if(initialisationState) {
      if(it == rbegin) {
        return true;
      }
      else {
        --it;
        if(!it->initialise(context)) {
          initialisationState = false;
          ++it;
        }
      }
    }
    else {
      if(it == rend) {
        return false;
      }
      else {
        if(it->next(context)) {
          initialisationState = true;
        }
        else {
          ++it;
        }
      }
    }
  }
}

Result DataItemFor::createResult(DynamicContext* context, int flags) const
{
  return createResultImpl(_bindings.begin(), _bindings.end(), context, flags);
}

Result DataItemFor::createResultImpl(Bindings::const_iterator it, Bindings::const_iterator end, DynamicContext* context, int flags) const
{
  return new ForResult(it, end, _returnExpr, flags, context);
}

DataItem* DataItemFor::staticResolution(StaticContext *context)
{
  staticResolutionImpl(context);

  if(_src.isUsed()) {
    return resolvePredicates(context);
  }
  else {
    _returnExpr->addPredicates(getPredicates());
    return _returnExpr->staticResolution(context);
  }
}

void DataItemFor::staticResolutionImpl(StaticContext* context)
{
  Bindings newBindings(PathanAllocator<VB*>(context->getMemoryManager()));

  VariableTypeStore* varStore = context->getVariableTypeStore();

  // Add all the binding variables to the new scope
  Bindings::iterator end = _bindings.end();
  for(Bindings::iterator it0 = _bindings.begin(); it0 != end; ++it0) {
    // Work out the uri and localname of the variable binding
    (*it0)->_uri = context->getUriBoundToPrefix(XPath2NSUtils::getPrefix((*it0)->_variable, context->getMemoryManager()));
    (*it0)->_name = XPath2NSUtils::getLocalName((*it0)->_variable);

    // call static resolution on the value
    (*it0)->_allValues = (*it0)->_allValues->staticResolution(context);
    (*it0)->_src.getStaticType() = (*it0)->_allValues->getStaticResolutionContext().getStaticType();
    (*it0)->_src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED | StaticResolutionContext::PEER | StaticResolutionContext::SUBTREE | StaticResolutionContext::ONENODE);

    varStore->addLogicalBlockScope();
    varStore->declareVar((*it0)->_uri, (*it0)->_name, (*it0)->_src);
  }

  // Call staticResolution on the return expression
  _returnExpr = _returnExpr->staticResolution(context);
  _src.getStaticType() = _returnExpr->getStaticResolutionContext().getStaticType();
  _src.add(_returnExpr->getStaticResolutionContext());

  Bindings::reverse_iterator rend = _bindings.rend();
  for(Bindings::reverse_iterator it = _bindings.rbegin(); it != rend; ++it) {
    VB *newVB = new (context->getMemoryManager()) VB(**it, context->getMemoryManager());

    // Remove our variable binding and the scope we added
    varStore->removeScope();

    // Remove our binding variable from the StaticResolutionContext data (removing it if it's not used)
    if(!_src.removeVariable(newVB->_uri, newVB->_name)) {
      newVB->_variable = 0;
    }

    const StaticResolutionContext &valueSrc = newVB->_allValues->getStaticResolutionContext();
    _src.add(valueSrc);

    Bindings::reverse_iterator it2;
    // Check if previous variable bindings have been used by this value,
    for(it2 = it + 1; it2 != rend; ++it2) {
      if((*it2)->_variable && valueSrc.isVariableUsed((*it2)->_uri, (*it2)->_name)) {
        newVB->_valuesExprDependsOnPreviousBindings = true;
        break;
      }
    }

    // Check to see if this binding has the same name as any before it
    for(it2 = it + 1; it2 != rend; ++it2) {
      if(newVB->_variable && (*it2)->_variable &&
         XPath2Utils::equals(newVB->_name, (*it2)->_name) && XPath2Utils::equals(newVB->_uri, (*it2)->_uri)) {
        newVB->_needsNewScope = true;
        break;
      }
    }

    // Add the new VB at the front of the new Bindings
    newBindings.insert(newBindings.begin(), newVB);

    if(!_src.isUsed()) {
      AutoRelease<DynamicContext> dContext(context->createDynamicContext());
      dContext->setMemoryManager(context->getMemoryManager());
      Result result = createResultImpl(newBindings.begin(), newBindings.end(), dContext);
      _returnExpr = new (getMemoryManager()) DataItemSequence(result, dContext, getMemoryManager());
      newBindings.clear();
    }
  }

  // Overwrite our bindings with the new ones
  _bindings = newBindings;
}

const DataItemFor::Bindings &DataItemFor::getBindings() const {
  return _bindings;
}

const DataItem *DataItemFor::getReturnExpr() const {
  return _returnExpr;
}

 void DataItemFor::setReturnExpr(DataItem *item)
{
  _returnExpr = item;
}

DataItemFor::ForResult::ForResult(Bindings::const_iterator it, Bindings::const_iterator end, const DataItem *returnExpr, int flags, DynamicContext *context)
  : ResultImpl(context),
    _toInit(true),
    _flags(flags),
    _scope(0),
    _returnExpr(returnExpr),
    _returnResult(0)
{
  for(; it != end; ++it) {
    _ebs.push_back(VBExecution(*it, context));
  }
}

Item::Ptr DataItemFor::ForResult::next(DynamicContext *context)
{
  VariableStore* varStore = context->getVariableStore();

  VariableStore::MyScope *oldScope = varStore->getScopeState();

  if(_toInit) {
    _toInit = false;
    varStore->addLogicalBlockScope();
    if(DataItemFor::nextState(_ebs, context, true)) {
      _returnResult = _returnExpr->collapseTree(context, _flags);
    }
    else {
      varStore->removeScope();
      _scope = 0;
      return 0;
    }
  }
  else {
    if(_scope == 0) {
      return 0;
    }
    varStore->setScopeState(_scope);
  }

  Item::Ptr result = _returnResult.next(context);

  while(result == NULLRCP) {
    if(DataItemFor::nextState(_ebs, context, false)) {
      _returnResult = _returnExpr->collapseTree(context, _flags);
      result = _returnResult.next(context);
    }
    else {
      varStore->removeScope();
      _scope = 0;
      _returnResult = 0;
      return 0;
    }
  }

  _scope = varStore->getScopeState();
  varStore->setScopeState(oldScope);

  return result;
}

std::string DataItemFor::ForResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<for/>" << std::endl;

  return oss.str();
}
