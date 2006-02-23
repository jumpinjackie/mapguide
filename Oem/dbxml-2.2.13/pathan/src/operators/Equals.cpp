/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <assert.h>
#include <pathan/operators/Equals.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/AnyAtomicType.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/Collation.hpp>
#include <pathan/internal/collations/CodepointCollation.hpp>

/*static*/ const XMLCh Equals::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_q, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

Equals::Equals(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ComparisonOperator(name, args, memMgr)
{
}

/*static*/ bool Equals::equals(const Item::Ptr &arg1, const Item::Ptr &arg2, Collation* collation, DynamicContext* context)
{
  assert(arg1->isAtomicValue() && arg2->isAtomicValue());
  return equals((const AnyAtomicType::Ptr)arg1, (const AnyAtomicType::Ptr)arg2, collation, context);
}

/*static*/ bool Equals::equals(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, Collation* collation, DynamicContext* context) 
{
    // take care of the special case first
    if(atom1->getPrimitiveTypeIndex() == AnyAtomicType::STRING) {
        if(atom2->getPrimitiveTypeIndex() != AnyAtomicType::STRING) {
            DSLthrow(XPath2ErrorException,X("Equals::equals"), X("An attempt to compare a string type to a non string type has occurred"));
        }
        // if the function returns 0, then they are equal
        return (collation->compare(atom1->asString(context),atom2->asString(context))==0);
    } 

    return atom1->equals(atom2, context);
}

bool Equals::execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const
{
  Collation* collation=context->getDefaultCollation();
  if(collation==NULL)
    collation=context->getCollation(CodepointCollation::getCodepointCollationName());
  return equals(atom1, atom2, collation, context);
}

