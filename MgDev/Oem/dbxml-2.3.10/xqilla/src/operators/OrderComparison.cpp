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
 * $Id: OrderComparison.cpp,v 1.14 2006/11/01 16:37:21 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <sstream>

#include <xqilla/operators/OrderComparison.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/schema/SequenceType.hpp>

/*static*/ const XMLCh OrderComparison::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chDash, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

OrderComparison::OrderComparison(const VectorOfASTNodes &args, bool testBefore, XPath2MemoryManager* memMgr)
  : XQOperator(name, args, memMgr)
{
	_testBefore = testBefore;
}

bool OrderComparison::getTestBefore() const
{
    return _testBefore;
}

ASTNode* OrderComparison::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    SequenceType *seqType = new (mm) SequenceType(new (mm) SequenceType::ItemType(SequenceType::ItemType::TEST_NODE),
                                                  SequenceType::QUESTION_MARK);
    seqType->setLocationInfo(this);

    *i = new (mm) XQTreatAs(*i, seqType, mm);
    (*i)->setLocationInfo(this);

    *i = (*i)->staticResolution(context);
  }

  return this;
}

ASTNode* OrderComparison::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.getStaticType().flags = StaticType::BOOLEAN_TYPE;

  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    *i = (*i)->staticTyping(context);
    _src.add((*i)->getStaticResolutionContext());
  }

  return this;
}

Result OrderComparison::createResult(DynamicContext* context, int flags) const
{
  return new OrderComparisonResult(this);
}

OrderComparison::OrderComparisonResult::OrderComparisonResult(const OrderComparison *op)
  : SingleResult(op),
    _op(op)
{
}

Item::Ptr OrderComparison::OrderComparisonResult::getSingleResult(DynamicContext *context) const
{
  Item::Ptr arg1 = _op->getArgument(0)->collapseTree(context)->next(context);
  if(arg1.isNull()) return 0;
  Item::Ptr arg2 = _op->getArgument(1)->collapseTree(context)->next(context);
  if(arg2.isNull()) return 0;
  
	if(_op->getTestBefore())
    return context->getItemFactory()->createBoolean(((Node*)arg1.get())->lessThan((Node*)arg2.get(), context), context);

  return context->getItemFactory()->createBoolean(((Node*)arg2.get())->lessThan((Node*)arg1.get(), context), context);
}

std::string OrderComparison::OrderComparisonResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<ordercomparison/>" << std::endl;

  return oss.str();
}
