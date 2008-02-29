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
 * $Id: XQTypeswitch.cpp,v 1.15 2006/11/01 16:37:18 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQTypeswitch.cpp: implementation of the XQTypeswitch class.
//////////////////////////////////////////////////////////////////////

#include <sstream>

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/XQTypeswitch.hpp>
#include <xqilla/context/impl/XQDynamicContextImpl.hpp>
#include <xqilla/context/VariableStore.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/runtime/ResultBuffer.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>

XQTypeswitch::XQTypeswitch(ASTNode* eval, VectorOfClause* clauses, Clause* defReturn, XPath2MemoryManager* expr)
  : ASTNodeImpl(expr)
{
  _expr=eval;
  _default=defReturn;
  _clauses=clauses;
  setType(ASTNode::TYPESWITCH);
}

Result XQTypeswitch::createResult(DynamicContext *context, int flags) const
{
  return new TypeswitchResult(this, flags);
}

ASTNode* XQTypeswitch::staticResolution(StaticContext *context)
{
  // Statically resolve the test expression
  _expr = _expr->staticResolution(context);

  // Call static resolution on the clauses
  for(VectorOfClause::iterator it = _clauses->begin(); it != _clauses->end(); ++it) {
    (*it)->_type->staticResolution(context);
    (*it)->staticResolution(_expr->getStaticResolutionContext(), context, _src);
  }

  _default->staticResolution(_expr->getStaticResolutionContext(), context, _src);

  return this;
}

ASTNode* XQTypeswitch::staticTyping(StaticContext *context)
{
  _src.clear();

  // Statically resolve the test expression
  _expr = _expr->staticTyping(context);
  const StaticResolutionContext &exprSrc = _expr->getStaticResolutionContext();

  _src.getStaticType().flags = 0;

  if(exprSrc.isUsed()) {
    _src.add(exprSrc);

    // Do basic static type checking on the clauses,
    // to eliminate ones which will never be matches,
    // and find ones which will always be matched.
    VectorOfClause newClauses(XQillaAllocator<Clause*>(context->getMemoryManager()));
    VectorOfClause::iterator it = _clauses->begin();
    for(; it != _clauses->end(); ++it) {
      const SequenceType::ItemType *itemType = (*it)->_type->getItemType();
      if(itemType != NULL) {
        const StaticType &sType = _expr->getStaticResolutionContext().getStaticType();

        bool isExact;
        StaticType clauseType;
        itemType->getStaticType(clauseType, context, isExact, *it);

        if(isExact && (sType.flags & clauseType.flags) != 0 &&
           (sType.flags & ~clauseType.flags) == 0 &&
           (*it)->_type->getOccurrenceIndicator() == SequenceType::STAR) {
          // It always matches, so set this clause as the
          // default clause and remove all the others
          newClauses.clear();
          _default = *it;
          _default->_type = 0;
          break;
        }
        else if((sType.flags & clauseType.flags) == 0 &&
                ((*it)->_type->getOccurrenceIndicator() == SequenceType::EXACTLY_ONE ||
                 (*it)->_type->getOccurrenceIndicator() == SequenceType::PLUS)) {
          // It never matches, so don't add it to the new clauses
        }
        else {
          newClauses.push_back(*it);
        }
      }
    }
    *_clauses = newClauses;

    // Call static resolution on the new clauses
    _src.setProperties((unsigned int)-1);
    for(it = _clauses->begin(); it != _clauses->end(); ++it) {
      (*it)->staticTyping(_expr->getStaticResolutionContext(), context, _src);
    }

    _default->staticTyping(_expr->getStaticResolutionContext(), context, _src);

    return this;
  }
  else {
    // If it's constant, we can narrow it down to the correct clause
    AutoDelete<DynamicContext> dContext(context->createDynamicContext());
    dContext->setMemoryManager(context->getMemoryManager());
    Sequence value = _expr->collapseTree(dContext)->toSequence(dContext);

    Clause *match = 0;
    for(VectorOfClause::iterator it=_clauses->begin();it!=_clauses->end();++it) {
      try {
        (*it)->_type->matches(value, (*it)->_type)->toSequence(dContext);
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
    _default->staticTyping(_expr->getStaticResolutionContext(), context, _src);

    // Constant fold if possible
    if(!_src.isUsed()) {
      return constantFold(context);
    }
    return this;
  }
}

void XQTypeswitch::Clause::staticResolution(const StaticResolutionContext &var_src, StaticContext* context, StaticResolutionContext &src)
{
  _expr = _expr->staticResolution(context);
}

void XQTypeswitch::Clause::staticTyping(const StaticResolutionContext &var_src, StaticContext* context,
                                                            StaticResolutionContext &src)
{
  VariableTypeStore* varStore=context->getVariableTypeStore();

  if(_variable != 0) {
    varStore->addLogicalBlockScope();
    _uri = context->getUriBoundToPrefix(XPath2NSUtils::getPrefix(_variable, context->getMemoryManager()), this);
    _name = XPath2NSUtils::getLocalName(_variable);
    varStore->declareVar(_uri, _name, var_src);
  }

  StaticResolutionContext newSrc(context->getMemoryManager());
  _expr = _expr->staticTyping(context);
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
  src.setProperties(src.getProperties() & _expr->getStaticResolutionContext().getProperties());
  src.add(newSrc);
}

const ASTNode *XQTypeswitch::getExpression() const
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

void XQTypeswitch::setExpression(ASTNode *expr)
{
  _expr = expr;
}

XQTypeswitch::TypeswitchResult::TypeswitchResult(const XQTypeswitch *di, int flags)
  : ResultImpl(di),
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
        (*it)->_type->matches(value.createResult(), (*it)->_type)->toSequence(context);
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
      varStore->declareVar(clause->_uri, clause->_name, value.createResult()->toSequence(context), context);
      _scopeRemoved = false;
    }

    _result = clause->_expr->collapseTree(context, _flags);
  }
  else if(_scope != 0) {
    varStore->setScopeState(_scope);
  }

  const Item::Ptr item = _result->next(context);

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
