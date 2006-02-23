/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/operators/NotEquals.hpp>
#include <pathan/operators/Equals.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/internal/collations/CodepointCollation.hpp>
#include <pathan/DynamicContext.hpp>

/*static*/ const XMLCh NotEquals::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

NotEquals::NotEquals(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ComparisonOperator(name, args, memMgr)
{
}

/*static*/ bool NotEquals::not_equals(const AnyAtomicType::Ptr &arg1, const AnyAtomicType::Ptr &arg2, Collation* collation, DynamicContext* context)
{
  // define "ne" as "!eq"
  return !Equals::equals(arg1,arg2,collation,context);
}

bool NotEquals::execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const
{
  Collation* collation=context->getDefaultCollation();
  if(collation==NULL)
    collation=context->getCollation(CodepointCollation::getCodepointCollationName());
  return not_equals(atom1, atom2, collation, context);
}












