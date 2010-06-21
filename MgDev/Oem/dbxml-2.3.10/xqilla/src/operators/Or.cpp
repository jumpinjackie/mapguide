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
 * $Id: Or.cpp,v 1.14 2006/11/01 16:37:21 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <sstream>

#include <xqilla/operators/Or.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/context/ContextHelpers.hpp>

/*static*/ const XMLCh Or::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

Or::Or(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQOperator(name, args, memMgr)
{
  // Nothing to do
}

Result Or::createResult(DynamicContext* context, int flags) const
{
  return new OrResult(this);
}

ASTNode* Or::staticResolution(StaticContext *context)
{
  AutoNodeSetOrderingReset orderReset(context);
  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    *i = (*i)->staticResolution(context);
  }
  return this;
}

ASTNode* Or::staticTyping(StaticContext *context)
{
  _src.clear();

  VectorOfASTNodes newArgs(XQillaAllocator<ASTNode*>(context->getMemoryManager()));

  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    *i = (*i)->staticTyping(context);
    const StaticResolutionContext &valueSrc = (*i)->getStaticResolutionContext();

    if(valueSrc.isUsed()) {
      _src.add(valueSrc);
      newArgs.push_back(*i);
    }
    else {
      AutoDelete<DynamicContext> dContext(context->createDynamicContext());
      dContext->setMemoryManager(context->getMemoryManager());
      if((*i)->collapseTree(dContext)->getEffectiveBooleanValue(dContext, *i)) {
        // It's constantly true, so this expression is true
        ASTNode* newBlock = new (getMemoryManager())
          XQSequence(dContext->getItemFactory()->createBoolean(true, dContext),
                           dContext, getMemoryManager());
	newBlock->setLocationInfo(this);
        return newBlock->staticResolution(context);
      }
    }
  }

  _src.getStaticType().flags = StaticType::BOOLEAN_TYPE;

  _args = newArgs;
  return this;
}

Or::OrResult::OrResult(const Or *op)
  : SingleResult(op),
    _op(op)
{
}

Item::Ptr Or::OrResult::getSingleResult(DynamicContext *context) const
{
  unsigned int numArgs=_op->getNumArgs();
  for(unsigned int i=0;i<numArgs;i++) {
    if(_op->getArgument(i)->collapseTree(context)->getEffectiveBooleanValue(context, _op->getArgument(i))) {
      return (const Item::Ptr)context->getItemFactory()->createBoolean(true, context);
    }
  }

	return (const Item::Ptr)context->getItemFactory()->createBoolean(false, context);
}

std::string Or::OrResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<or/>" << std::endl;

  return oss.str();
}
