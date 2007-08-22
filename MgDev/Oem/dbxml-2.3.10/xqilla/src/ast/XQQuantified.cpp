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
 * $Id: XQQuantified.cpp,v 1.15 2006/11/01 16:37:18 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQQuantified.cpp: implementation of the XQQuantified class.
//////////////////////////////////////////////////////////////////////

#include <sstream>

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/XQQuantified.hpp>
#include <xqilla/context/impl/XQDynamicContextImpl.hpp>
#include <xqilla/context/VariableStore.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/context/ItemFactory.hpp>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XQQuantified::XQQuantified(XQQuantified::QuantifierType qType, VectorOfVariableBinding* bindings, ASTNode* returnExpr, XPath2MemoryManager* expr)
  : XQFLWOR(bindings, NULL, NULL, returnExpr, expr)
{
	_qType=qType;
    setType(ASTNode::FLWOR_QUANTIFIED);
}

ASTNode *XQQuantified::staticTyping(StaticContext *context)
{
  _src.clear();

  XQFLWOR::staticTypingImpl(context);

  _src.getStaticType().flags = StaticType::BOOLEAN_TYPE;

  if(!_bindings->empty()) {
    if(_return->isConstant()) {
      AutoDelete<DynamicContext> dContext(context->createDynamicContext());
      dContext->setMemoryManager(context->getMemoryManager());
      bool value = _return->collapseTree(dContext)->getEffectiveBooleanValue(dContext, _return);
      ASTNode *newBlock = new (getMemoryManager())
	      XQSequence(dContext->getItemFactory()->createBoolean(value, dContext),
			       dContext, getMemoryManager());
      newBlock->setLocationInfo(this);
      return newBlock->staticResolution(context);
    }
    return this;
  }
  else {
    return constantFold(context);
  }
}

Result XQQuantified::createResultImpl(VectorOfVariableBinding::const_iterator it, VectorOfVariableBinding::const_iterator end, DynamicContext* context, int flags) const
{
  return new QuantifiedResult(it, end, this);
}

XQQuantified::QuantifierType XQQuantified::getQuantifierType() const
{
  return _qType;
}

XQQuantified::QuantifiedResult::QuantifiedResult(VectorOfVariableBinding::const_iterator it, VectorOfVariableBinding::const_iterator end,
                                                 const XQQuantified *quantified)
  : SingleResult(quantified),
    _quantified(quantified)
{
  for(; it != end; ++it) {
    _ebs.push_back(ProductFactor(*it));
  }
}

Item::Ptr XQQuantified::QuantifiedResult::getSingleResult(DynamicContext *context) const
{
  VariableStore* varStore = context->getVariableStore();
  bool defaultResult = (_quantified->getQuantifierType() == some) ? false : true;

  ExecutionBindings &ebs = const_cast<ExecutionBindings&>(_ebs); // _ebs is mutable

  // Initialise and run the execution bindings
  varStore->addLogicalBlockScope();
  if(_quantified->nextState(ebs, context, true)) {
    do {
      bool result = _quantified->getReturnExpr()->collapseTree(context)->getEffectiveBooleanValue(context, _quantified->getReturnExpr());
      if(defaultResult != result) {
        defaultResult = result;
        break;
      }
    } while(_quantified->nextState(ebs, context, false));
  }
  varStore->removeScope();

  return (const Item::Ptr)context->getItemFactory()->createBoolean(defaultResult, context);
}

std::string XQQuantified::QuantifiedResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<quantified_flwor/>" << std::endl;

  return oss.str();
}
