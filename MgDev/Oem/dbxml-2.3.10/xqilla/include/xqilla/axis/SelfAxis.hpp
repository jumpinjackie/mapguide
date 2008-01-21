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
 * $Id: SelfAxis.hpp,v 1.9 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _SELFAXIS_HPP
#define _SELFAXIS_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/runtime/ResultImpl.hpp>
#include <xqilla/items/Node.hpp>

class XQILLA_API SelfAxis : public ResultImpl
{
public:
  SelfAxis(const LocationInfo *info, Node::Ptr contextNode);
  Item::Ptr next(DynamicContext *context);
  std::string asString(DynamicContext *context, int indent) const;
private:
  bool toDo_;
  Node::Ptr contextNode_;
};

#endif

