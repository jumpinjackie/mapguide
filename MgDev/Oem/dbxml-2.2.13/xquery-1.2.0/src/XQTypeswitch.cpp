//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQTypeswitch.cpp,v 1.18 2005/08/04 15:48:10 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQTypeswitch.cpp: implementation of the XQTypeswitch class.
//////////////////////////////////////////////////////////////////////

#include <sstream>

#include "xquery/XQEngine.hpp"
#include "xquery/dataItem/XQTypeswitch.hpp"
#include "xquery/context/XQDynamicContextImpl.hpp"
#include <pathan/VariableStore.hpp>
#include <pathan/simpleVariables/VariableTypeStore.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/dataItem/ResultBuffer.hpp>
#include <pathan/XPath2NSUtils.hpp>
#include <pathan/SequenceType.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/XPath2TypeMatchException.hpp>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XQTypeswitch::XQTypeswitch(DataItem* eval, VectorOfClause* clauses, Clause* defReturn, XPath2MemoryManager* expr)
  : DataItemImpl(expr)
{
  _expr=eval;
  _default=defReturn;
  _clauses=clauses;
  setType((DataItem::whichType)XQContext::TYPESWITCH);
}

Result XQTypeswitch::createResult(DynamicContext *context, int flags) const
{
  return new TypeswitchResult(this, flags, context);
}

DataItem* XQTypeswitch::staticResolution(StaticContext *context)
{
  // Statically resolve the test expression
  _expr = _expr->staticResolution(context);
  const StaticResolutionContext &exprSrc = _expr->getStaticResolutionContext();

  _src.getStaticType().flags = 0;

  if(exprSrc.isUsed()) {
    // If it's not constant, it could be any of the clauses.
    // (Until we static type resolution)
    _src.add(exprSrc);

    for(VectorOfClause::iterator it=_clauses->begin();it!=_clauses->end();++it) {
      (*it)->staticResolution(_expr->getStaticResolutionContext(), context, _src);
    }

    _default->staticResolution(_expr->getStaticResolutionContext(), context, _src);

    return resolvePredicates(context);
  }
  else {
    // If it's constant, we can narrow it down to the correct clause
    AutoRelease<DynamicContext> dContext(context->createDynamicContext());
    dContext->setMemoryManager(context->getMemoryManager());
    Sequence value = _expr->collapseTree(dContext);

    Clause *match = 0;
    for(VectorOfClause::iterator it=_clauses->begin();it!=_clauses->end();++it) {
      try {
        (*it)->_type->matches(value,dContext).toSequence(dContext);
        match = *it;
        break;
      }
      catch(const XPath2TypeMatchException &ex) {
        // Well, it doesn't match that one then...
      }
    }

    // Replace the default with the matched clause and
    // remove the remaining clauses, as they don't match
    if(match) {
      _default = match;
      _default->_type = 0;
    }
    _clauses->clear();

    // Statically resolve the default clause
    _default->staticResolution(_expr->getStaticResolutionContext(), context, _src);

    // Constant fold if possible
    if(_src.isUsed()) {
      return resolvePredicates(context);
    }
    else {
      return constantFold(context);
    }
  }
}

void XQTypeswitch::Clause::staticResolution(const StaticResolutionContext &var_src, StaticContext* context, StaticResolutionContext &src)
{
  VariableTypeStore* varStore=context->getVariableTypeStore();

  if(_variable != 0) {
    varStore->addLogicalBlockScope();
    _uri = context->getUriBoundToPrefix(XPath2NSUtils::getPrefix(_variable, context->getMemoryManager()));
    _name = XPath2NSUtils::getLocalName(_variable);
    varStore->declareVar(_uri, _name, var_src);
  }

  StaticResolutionContext newSrc(context->getMemoryManager());
  _expr = _expr->staticResolution(context);
  newSrc.add(_expr->getStaticResolutionContext());

  if(_variable != 0) {
    // Remove the local variable from the StaticResolutionContext
    if(!newSrc.removeVariable(_uri, _name)) {
      // If the variable isn't used, don't bother setting it when we execute
      _variable = 0;
    }
    varStore->removeScope();
  }

  src.getStaticType().typeUnion(_expr->getStaticResolutionContext().getStaticType());
  src.add(newSrc);
}

const DataItem *XQTypeswitch::getExpression() const
{
  return _expr;
}

const XQTypeswitch::Clause *XQTypeswitch::getDefaultClause() const
{
  return _default;
}

const XQTypeswitch::VectorOfClause *XQTypeswitch::getClauses() const
{
  return _clauses;
}

void XQTypeswitch::setExpression(DataItem *expr)
{
  _expr = expr;
}

XQTypeswitch::TypeswitchResult::TypeswitchResult(const XQTypeswitch *di, int flags, DynamicContext *context)
  : ResultImpl(context),
    _flags(flags),
    _di(di),
    _scope(0),
    _result(0),
    _scopeRemoved(true)
{
}

Item::Ptr XQTypeswitch::TypeswitchResult::next(DynamicContext *context)
{
  VariableStore* varStore = context->getVariableStore();
  Scope<Sequence> *oldScope = varStore->getScopeState();

  if(_result.isNull()) {
    // retrieve the value of the operand expression
    ResultBuffer value(_di->getExpression()->collapseTree(context));

    const Clause *clause = 0;

    // find the effective case
    for(VectorOfClause::const_iterator it = _di->getClauses()->begin();
        it != _di->getClauses()->end(); ++it) {
      try {
        value.createResult(context).matches((*it)->_type, context).toSequence(context);
        clause = *it;
        break;
      }
      catch(const XPath2TypeMatchException &ex) {
        // Well, it doesn't match that one then...
      }
    }

    // if no case is satisfied, use the default one
    if(clause == 0) {
      clause = _di->getDefaultClause();
    }

    // Execute the clause
    if(clause->_variable != 0) {
      varStore->addLogicalBlockScope();
      varStore->declareVar(clause->_uri, clause->_name, value.createResult(context).toSequence(context), context);
      _scopeRemoved = false;
    }

    _result = clause->_expr->collapseTree(context, _flags);
  }
  else if(_scope != 0) {
    varStore->setScopeState(_scope);
  }

  const Item::Ptr item = _result.next(context);

  if(!_scopeRemoved) {
    if(item == NULLRCP) {
      varStore->removeScope();
      _scope = 0;
    }
    else {
      _scope = varStore->getScopeState();
      varStore->setScopeState(oldScope);
    }
  }

  return item;
}

std::string XQTypeswitch::TypeswitchResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<typeswitch/>" << std::endl;

  return oss.str();
}
