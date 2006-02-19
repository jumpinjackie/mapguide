/*
 * Copyright (c) 2003, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include <pathan/Pathan.hpp>

#include <xercesc/validators/datatype/AnySimpleTypeDatatypeValidator.hpp>
#include <xercesc/validators/datatype/DatatypeValidator.hpp>
#include <xercesc/util/RefHashTableOf.hpp>
#include <xercesc/util/RefArrayVectorOf.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/framework/MemoryManager.hpp>
#include <xercesc/util/PlatformUtils.hpp>

class KVStringPair;

class PATHAN_EXPORT AnyAtomicTypeDatatypeValidator : public XERCES_CPP_NAMESPACE_QUALIFIER AnySimpleTypeDatatypeValidator {

public:
  // constructor
  AnyAtomicTypeDatatypeValidator(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const manager  = XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager);

  // destructor
  virtual ~AnyAtomicTypeDatatypeValidator();

  virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator* newInstance(
      XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf<XERCES_CPP_NAMESPACE_QUALIFIER KVStringPair>* const facets,
      XERCES_CPP_NAMESPACE_QUALIFIER RefArrayVectorOf<XMLCh>* const enums, 
      const int finalSet, 
      XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const manager);

};

