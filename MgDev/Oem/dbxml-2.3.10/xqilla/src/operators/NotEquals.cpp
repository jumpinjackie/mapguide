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
 * $Id: NotEquals.cpp,v 1.9 2006/11/01 16:37:21 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/operators/NotEquals.hpp>
#include <xqilla/operators/Equals.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/context/impl/CodepointCollation.hpp>
#include <xqilla/context/DynamicContext.hpp>

/*static*/ const XMLCh NotEquals::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

NotEquals::NotEquals(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ComparisonOperator(name, args, memMgr)
{
}

/*static*/ bool NotEquals::not_equals(const AnyAtomicType::Ptr &arg1, const AnyAtomicType::Ptr &arg2, Collation* collation, DynamicContext* context, const LocationInfo *info)
{
  // define "ne" as "!eq"
  return !Equals::equals(arg1,arg2,collation,context, info);
}

bool NotEquals::execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const
{
  return not_equals(atom1, atom2, context->getDefaultCollation(this), context, this);
}












