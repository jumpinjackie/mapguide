//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQQuantified.cpp,v 1.17 2005/11/30 17:08:55 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQQuantified.cpp: implementation of the XQQuantified class.
//////////////////////////////////////////////////////////////////////

#include <sstream>

#include "xquery/XQEngine.hpp"
#include "xquery/dataItem/XQQuantified.hpp"
#include "xquery/context/XQDynamicContextImpl.hpp"
#include <pathan/VariableStore.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/dataItem/DataItemSequence.hpp>
#include <pathan/PathanFactory.hpp>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XQQuantified::XQQuantified(XQQuantified::QuantifierType qType, VectorOfVariableBinding* bindings, DataItem* returnExpr, XPath2MemoryManager* expr)
  : XQFLWOR(bindings, NULL, NULL, returnExpr, expr)
{
	_qType=qType;
    setType((DataItem::whichType)XQContext::FLWOR_QUANTIFIED);
}

DataItem* XQQuantified::staticResolution(StaticContext* context) {
  XQFLWOR::staticResolutionImpl(context);

  _src.getStaticType().flags = StaticType::OTHER_TYPE;

  if(_src.isUsed()) {
    if(_return->isConstant()) {
      AutoRelease<DynamicContext> dContext(context->createDynamicContext());
      dContext->setMemoryManager(context->getMemoryManager());
      bool value = _return->collapseTree(dContext).getEffectiveBooleanValue(dContext);
      DataItem *newBlock = new (getMemoryManager())
	      DataItemSequence(dContext->getPathanFactory()->createBoolean(value, dContext),
			       dContext, getMemoryManager());
      newBlock->addPredicates(getPredicates());
      return newBlock->staticResolution(context);
    }
    else {
      return resolvePredicates(context);
    }
  }
  else {
    return constantFold(context);
  }
}

Result XQQuantified::createResultImpl(VectorOfVariableBinding::const_iterator it, VectorOfVariableBinding::const_iterator end, DynamicContext* context, int flags) const
{
  return new QuantifiedResult(it, end, this, context);
}

XQQuantified::QuantifierType XQQuantified::getQuantifierType() const
{
  return _qType;
}

XQQuantified::QuantifiedResult::QuantifiedResult(VectorOfVariableBinding::const_iterator it, VectorOfVariableBinding::const_iterator end,
                                                 const XQQuantified *quantified, DynamicContext *context)
  : SingleResult(context),
    _quantified(quantified)
{
  for(; it != end; ++it) {
    _ebs.push_back(ProductFactor(*it, context));
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
      bool result = _quantified->getReturnExpr()->collapseTree(context, DataItem::UNORDERED|DataItem::RETURN_TWO).getEffectiveBooleanValue(context);
      if(defaultResult != result) {
        defaultResult = result;
        break;
      }
    } while(_quantified->nextState(ebs, context, false));
  }
  varStore->removeScope();

  return (const Item::Ptr)context->getPathanFactory()->createBoolean(defaultResult, context);
}

std::string XQQuantified::QuantifiedResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<quantified_flwor/>" << std::endl;

  return oss.str();
}
