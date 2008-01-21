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
 * $Id: DatatypeFactory.hpp,v 1.8 2006/11/01 16:37:14 jpcs Exp $
 */

/*
  Factory base class
*/

#ifndef _DATATYPEFACTORY_HPP
#define _DATATYPEFACTORY_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/mapm/m_apm.h>
#include <xqilla/items/ATAnySimpleType.hpp>
#include <xqilla/items/ATAnyURIOrDerived.hpp>
#include <xqilla/items/ATBase64BinaryOrDerived.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/items/ATDateOrDerived.hpp>
#include <xqilla/items/ATDateTimeOrDerived.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/ATDoubleOrDerived.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include <xqilla/items/ATFloatOrDerived.hpp>
#include <xqilla/items/ATGDayOrDerived.hpp>
#include <xqilla/items/ATGMonthOrDerived.hpp>
#include <xqilla/items/ATGMonthDayOrDerived.hpp>
#include <xqilla/items/ATGYearMonthOrDerived.hpp>
#include <xqilla/items/ATGYearOrDerived.hpp>
#include <xqilla/items/ATHexBinaryOrDerived.hpp>
#include <xqilla/items/ATNotationOrDerived.hpp>
#include <xqilla/items/ATQNameOrDerived.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/ATTimeOrDerived.hpp>
#include <xqilla/items/ATUntypedAtomic.hpp>
#include <xqilla/items/Node.hpp>

#include <xercesc/validators/datatype/DatatypeValidator.hpp>

class StaticContext;
class XPath2MemoryManager;
class DocumentCache;

class XQILLA_API DatatypeFactory
{
public:
  DatatypeFactory(const DocumentCache* dc, XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *baseValidator);
  virtual ~DatatypeFactory();

  ///Create instance of the base datatype
  virtual AnyAtomicType::Ptr createInstance(const XMLCh* value,
                                            const DynamicContext* context) const;

  ///Create instance of a datatype, using DatatypeFactoryTemplate to determine type
  virtual AnyAtomicType::Ptr createInstance(const XMLCh* typeURI,
                                            const XMLCh* typeName,
                                            const XMLCh* value,
                                            const DynamicContext* context) const;

  virtual bool checkInstance(const XMLCh* value,
                             XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm) const;

  virtual bool checkInstance(const XMLCh* typeURI,
                             const XMLCh* typeName,
                             const XMLCh* value,
                             XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm) const;

  ///Create instance of a datatype, using DatatypeFactoryTemplate to determine type
  virtual AnyAtomicType::Ptr createInstanceNoCheck(const XMLCh* typeURI,
                                                   const XMLCh* typeName,
                                                   const XMLCh* value,
                                                   const DynamicContext* context) const = 0;

  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const = 0;

  virtual const XMLCh* getPrimitiveTypeURI() const = 0;
  virtual const XMLCh* getPrimitiveTypeName() const = 0;

protected:
  XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *fBaseValidator;
  const DocumentCache *fDocumentCache;
};

#endif  // DATATYPEFACTORY

