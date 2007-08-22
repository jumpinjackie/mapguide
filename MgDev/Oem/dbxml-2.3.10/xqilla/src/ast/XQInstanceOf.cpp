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
 * $Id: XQInstanceOf.cpp,v 1.12 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <sstream>

#include <xqilla/ast/XQInstanceOf.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/items/AnyAtomicTypeConstructor.hpp>
#include <xqilla/context/ContextHelpers.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

XQInstanceOf::XQInstanceOf(ASTNode* expr, SequenceType* exprType, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr),
  _expr(expr),
  _exprType(exprType)
{
  setType(ASTNode::INSTANCE_OF);
}

Result XQInstanceOf::createResult(DynamicContext* context, int flags) const
{
  return new InstanceOfResult(this, flags);
}

ASTNode* XQInstanceOf::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  _exprType->staticResolution(context);

  _expr = new (mm) XQTreatAs(_expr, _exprType, mm);
  _expr->setLocationInfo(this);

  {
    AutoNodeSetOrderingReset orderReset(context);
    _expr = _expr->staticResolution(context);
  }

  return this;
}

ASTNode *XQInstanceOf::staticTyping(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  _src.clear();

  try {
    _expr = _expr->staticTyping(context);
  }
  catch(const XPath2TypeMatchException &ex) {
    // The expression was constant folded, and the type matching failed.
    AnyAtomicTypeConstructor *construct = 
      new (mm) AnyAtomicTypeConstructor(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                        SchemaSymbols::fgDT_BOOLEAN,
                                        SchemaSymbols::fgATTVAL_FALSE,
                                        AnyAtomicType::BOOLEAN);
    ASTNode *result = new (mm) XQSequence(construct, mm);
    result->setLocationInfo(this);
    return result->staticTyping(context);
  }

  _src.add(_expr->getStaticResolutionContext());
  _src.getStaticType().flags = StaticType::BOOLEAN_TYPE;

  if(_expr->isConstant()) {
    return constantFold(context);
  }
  return this;
}

const ASTNode *XQInstanceOf::getExpression() const {
  return _expr;
}

const SequenceType *XQInstanceOf::getSequenceType() const {
  return _exprType;
}

void XQInstanceOf::setExpression(ASTNode *item) {
  _expr = item;
}

XQInstanceOf::InstanceOfResult::InstanceOfResult(const XQInstanceOf *di, int flags)
  : SingleResult(di),
    _flags(flags),
    _di(di)
{
}

Item::Ptr XQInstanceOf::InstanceOfResult::getSingleResult(DynamicContext *context) const
{
  try {
    Result result = _di->getExpression()->collapseTree(context);
    while(result->next(context).notNull()) {}
  }
  catch(const XPath2TypeMatchException &ex) {
    return (const Item::Ptr)context->getItemFactory()->createBoolean(false, context);
  }

  return (const Item::Ptr)context->getItemFactory()->createBoolean(true, context);
}

std::string XQInstanceOf::InstanceOfResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<instanceof/>" << std::endl;

  return oss.str();
}
