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
 * $Id: AnyAtomicTypeDatatypeValidator.hpp,v 1.8 2006/11/01 16:37:17 jpcs Exp $
 */

#include <xqilla/framework/XQillaExport.hpp>

#include <xercesc/validators/datatype/AnySimpleTypeDatatypeValidator.hpp>
#include <xercesc/validators/datatype/DatatypeValidator.hpp>
#include <xercesc/util/RefHashTableOf.hpp>
#include <xercesc/util/RefArrayVectorOf.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/framework/MemoryManager.hpp>
#include <xercesc/util/PlatformUtils.hpp>

class KVStringPair;

class XQILLA_API AnyAtomicTypeDatatypeValidator : public XERCES_CPP_NAMESPACE_QUALIFIER AnySimpleTypeDatatypeValidator {

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

  virtual bool isAtomic() const { return true; }

};

