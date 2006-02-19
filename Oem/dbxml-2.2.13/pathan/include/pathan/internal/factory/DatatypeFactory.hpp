/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  Factory base class
*/

#ifndef _DATATYPEFACTORY_HPP
#define _DATATYPEFACTORY_HPP

#include <pathan/Pathan.hpp>
#include <pathan/internal/m_apm.h>
#include "../../ATAnySimpleType.hpp"
#include "../../ATAnyURIOrDerived.hpp"
#include "../../ATBase64BinaryOrDerived.hpp"
#include "../../ATBooleanOrDerived.hpp"
#include "../../ATDateOrDerived.hpp"
#include "../../ATDateTimeOrDerived.hpp"
#include "../../ATDecimalOrDerived.hpp"
#include "../../ATDoubleOrDerived.hpp"
#include "../../ATDurationOrDerived.hpp"
#include "../../ATFloatOrDerived.hpp"
#include "../../ATGDayOrDerived.hpp"
#include "../../ATGMonthOrDerived.hpp"
#include "../../ATGMonthDayOrDerived.hpp"
#include "../../ATGYearMonthOrDerived.hpp"
#include "../../ATGYearOrDerived.hpp"
#include "../../ATHexBinaryOrDerived.hpp"
#include "../../ATNotationOrDerived.hpp"
#include "../../ATQNameOrDerived.hpp"
#include "../../ATStringOrDerived.hpp"
#include "../../ATTimeOrDerived.hpp"
#include "../../ATUntypedAtomic.hpp"
#include "../../Node.hpp"

#include <xercesc/validators/datatype/DatatypeValidator.hpp>

class StaticContext;
class XPath2MemoryManager;
class DocumentCache;

class PATHAN_EXPORT DatatypeFactory
{
public:
  DatatypeFactory(const DocumentCache* dc, XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *baseValidator);
  virtual ~DatatypeFactory();

  ///Create instance of the base datatype
  AnyAtomicType::Ptr createInstance(const XMLCh* value,
                                    const DynamicContext* context) const;

  ///Create instance of a datatype, using DatatypeFactoryTemplate to determine type
  AnyAtomicType::Ptr createInstance(const XMLCh* typeURI,
                                    const XMLCh* typeName,
                                    const XMLCh* value,
                                    const DynamicContext* context) const;

  bool checkInstance(const XMLCh* value,
                     const StaticContext* context) const;

  bool checkInstance(const XMLCh* typeURI,
                     const XMLCh* typeName,
                     const XMLCh* value,
                     const StaticContext* context) const;

  ///Create instance of a datatype, using DatatypeFactoryTemplate to determine type
  virtual AnyAtomicType::Ptr createInstanceNoCheck(const XMLCh* typeURI,
                                                   const XMLCh* typeName,
                                                   const XMLCh* value,
                                                   const DynamicContext* context) const = 0;

  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const = 0;

  virtual const XMLCh* getPrimitiveTypeURI() const = 0;
  virtual const XMLCh* getPrimitiveTypeName() const = 0;

private:
  XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *fBaseValidator;
  const DocumentCache *fDocumentCache;
};

#endif  // DATATYPEFACTORY

