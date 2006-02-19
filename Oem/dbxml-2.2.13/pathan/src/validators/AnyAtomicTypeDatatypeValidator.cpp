/*
 * Copyright (c) 2003, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/internal/AnyAtomicTypeDatatypeValidator.hpp>
#include <xercesc/validators/datatype/DatatypeValidator.hpp>
#include <xercesc/util/RefHashTableOf.hpp>
#include <xercesc/util/RefArrayVectorOf.hpp>
#include <xercesc/util/XercesDefs.hpp>


AnyAtomicTypeDatatypeValidator::AnyAtomicTypeDatatypeValidator(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const manager) :  XERCES_CPP_NAMESPACE_QUALIFIER AnySimpleTypeDatatypeValidator(manager) {
}

AnyAtomicTypeDatatypeValidator::~AnyAtomicTypeDatatypeValidator () {
}

XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator* AnyAtomicTypeDatatypeValidator::newInstance
(
      XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf<XERCES_CPP_NAMESPACE_QUALIFIER KVStringPair>* const facets
    , XERCES_CPP_NAMESPACE_QUALIFIER RefArrayVectorOf<XMLCh>* const      enums
    , const int                           finalSet
    , XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const                manager
)
{
  return new (manager) AnyAtomicTypeDatatypeValidator(manager);
}
