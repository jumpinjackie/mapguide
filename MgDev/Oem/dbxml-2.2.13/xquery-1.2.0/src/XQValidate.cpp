//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQValidate.cpp,v 1.24 2005/11/30 17:08:55 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQValidate.cpp: implementation of the XQValidate class.
//////////////////////////////////////////////////////////////////////

#include "xquery/XQEngine.hpp"
#include "xquery/dataItem/XQValidate.hpp"
#include "xquery/context/XQDynamicContextImpl.hpp"
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/Node.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/SequenceType.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/validators/schema/SchemaGrammar.hpp>
#include <xercesc/validators/common/ContentSpecNode.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/dataItem/DataItemSequence.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>

////////////////////////////////////////////////////////////////////////////////////////
//
//

XQValidate::XQValidate(DataItem* valExpr, DocumentCache::ValidationMode valMode, XPath2MemoryManager* expr)
  : DataItemImpl(expr)
{
  _expr=valExpr;
  _validationMode=valMode;
  setType((DataItem::whichType)XQContext::VALIDATE);
}

Sequence XQValidate::collapseTreeInternal(DynamicContext* context, int flags) const
{
  Result toBeValidated = _expr->collapseTree(context, DataItem::UNORDERED|DataItem::RETURN_TWO);
  const Item::Ptr first = toBeValidated.next(context);
  const Item::Ptr second = toBeValidated.next(context);

  if(first == NULLRCP || second != NULLRCP || !first->isNode())
    DSLthrow(FunctionException,X("XQValidate::collapseTreeInternal"), X("The expression to be validated must evaluate to exactly one document or element node [err:XQ0030]"));

  Node::Ptr node = (Node::Ptr)first;
  if(node->dmNodeKind() != Node::element_string &&
     node->dmNodeKind() != Node::document_string)
    DSLthrow(FunctionException,X("XQValidate::collapseTreeInternal"), X("The expression to be validated must evaluate to exactly one document or element node [err:XQ0030]"));

  // perform validation on this item
  Node::Ptr validatedElt = context->validate(node, _validationMode);
  if(validatedElt.isNull())
    DSLthrow(FunctionException,X("XQValidate::collapseTreeInternal"), X("Unexpected error while validating"));

  return Sequence(validatedElt, context->getMemoryManager());
}

DataItem* XQValidate::staticResolution(StaticContext* ctx)
{
  _expr = _expr->staticResolution(ctx);
  _src.add(_expr->getStaticResolutionContext());

  _src.getStaticType().flags = StaticType::NODE_TYPE;
  _src.forceNoFolding(true);
  return resolvePredicates(ctx); // Never constant fold
}

const DataItem *XQValidate::getExpression() const
{
  return _expr;
}

DocumentCache::ValidationMode XQValidate::getValidationMode() const
{
  return _validationMode;
}

void XQValidate::setExpression(DataItem *expr)
{
  _expr = expr;
}
