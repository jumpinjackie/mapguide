//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQOrderingChange.cpp,v 1.7 2005/08/04 15:48:10 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQOrderingChange.cpp: implementation of the XQOrderingChange class.
//////////////////////////////////////////////////////////////////////

#include <sstream>

#include "xquery/XQEngine.hpp"
#include "xquery/XQContext.hpp"
#include "xquery/dataItem/XQOrderingChange.hpp"

#include <pathan/DynamicContext.hpp>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XQOrderingChange::XQOrderingChange(StaticContext::NodeSetOrdering ordering, DataItem* dataItem, XPath2MemoryManager* expr)
  : DataItemImpl(expr),
    m_nOrdering(ordering),
    m_pExpr(dataItem)
{
  setType((DataItem::whichType)XQContext::ORDERING_CHANGE);
}

Result XQOrderingChange::createResult(DynamicContext *context, int flags/*=0*/) const
{
    return new OrderingChangeResult(this, flags, context);
}

DataItem* XQOrderingChange::staticResolution(StaticContext* context)
{
    StaticContext::NodeSetOrdering oldOrder=context->getNodeSetOrdering();
    context->setNodeSetOrdering(m_nOrdering);

    m_pExpr=m_pExpr->staticResolution(context);
    _src.getStaticType() = m_pExpr->getStaticResolutionContext().getStaticType();
    _src.add(m_pExpr->getStaticResolutionContext());

    context->setNodeSetOrdering(oldOrder);

    if(_src.isUsed()) {
      return resolvePredicates(context);
    }
    else {
      return constantFold(context);
    }
}

StaticContext::NodeSetOrdering XQOrderingChange::getOrderingValue() const
{
    return m_nOrdering;
}

DataItem* XQOrderingChange::getExpr() const
{
    return m_pExpr;
}

void XQOrderingChange::setExpr(DataItem *expr)
{
	m_pExpr = expr;
}

XQOrderingChange::OrderingChangeResult::OrderingChangeResult(const XQOrderingChange* orderChange, int flags, DynamicContext *context)
: ResultImpl(context),
  _oc(orderChange),
  _flags(flags),
  _innerExpr(0)
{
}

Item::Ptr XQOrderingChange::OrderingChangeResult::next(DynamicContext *context)
{
    StaticContext::NodeSetOrdering oldOrder=context->getNodeSetOrdering();
    context->setNodeSetOrdering(_oc->getOrderingValue());
    if(_innerExpr.isNull())
        _innerExpr=_oc->getExpr()->collapseTree(context,_flags);
    Item::Ptr result=_innerExpr.next(context);
    context->setNodeSetOrdering(oldOrder);
    return result;
}

std::string XQOrderingChange::OrderingChangeResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<ordering_change/>" << std::endl;

  return oss.str();
}

