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
 * $Id: XQillaPlatformUtils.cpp,v 1.9.2.2 2007/01/12 18:04:46 gmfeinberg Exp $
 */

#include "../config/xqilla_config.h"

#include <xqilla/framework/XPath2MemoryManagerImpl.hpp>
#include <xqilla/utils/XQillaPlatformUtils.hpp>
#include <xqilla/items/impl/ATDecimalOrDerivedImpl.hpp>
#include <xqilla/items/impl/ATDoubleOrDerivedImpl.hpp>
#include <xqilla/items/impl/ATFloatOrDerivedImpl.hpp>
#include <xqilla/schema/AnyAtomicTypeDatatypeValidator.hpp>
#include <xqilla/mapm/m_apm.h>

#include "../dom-api/XQillaImplementation.hpp"
#include "../functions/FunctionLookupImpl.hpp"
#include "DateUtils.hpp"

#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/validators/datatype/DatatypeValidatorFactory.hpp>

XERCES_CPP_NAMESPACE_USE;

static int gInitFlag = 0;

void XQillaPlatformUtils::initialize(MemoryManager *memMgr) {
  //  Make sure we haven't already been initialized. Note that this is not
  //  thread safe and is not intended for that.
  if(gInitFlag++ == 0) {
    XMLPlatformUtils::Initialize(XMLUni::fgXercescDefaultLocale, 0, 0, memMgr);
    XQillaImplementation::initialize();
    m_apm_mt_initialize();
    DateUtils::initialize();
    FunctionLookupImpl::initialize();

    // Trigger the creation of the built-in datatype registry
    DatatypeValidatorFactory dvf;
    dvf.expandRegistryToFullSchemaSet();

    // Expand the Xerces Built-in registry to include xs:anyAtomicType
    DatatypeValidator* dv = new AnyAtomicTypeDatatypeValidator();
    dv->setTypeName(AnyAtomicType::fgDT_ANYATOMICTYPE_XERCESHASH);
    DatatypeValidatorFactory::getBuiltInRegistry()->
      put((void*) AnyAtomicType::fgDT_ANYATOMICTYPE_XERCESHASH, dv);
  }
}

void XQillaPlatformUtils::terminate() {
  if(gInitFlag == 0) {
    return;
  }

  if(--gInitFlag == 0) {
    m_apm_free_all_mem_mt();
    DateUtils::terminate();
    m_apm_mt_terminate();
    FunctionLookupImpl::terminate();
    XQillaImplementation::terminate();
    XMLPlatformUtils::Terminate();
  }
}

void XQillaPlatformUtils::enableExtendedPrecision(bool bEnable)
{
    if(bEnable)
    {
        ATDecimalOrDerivedImpl::g_nSignificantDigits=50;
        ATFloatOrDerivedImpl::g_nSignificantDigits=25;
        ATDoubleOrDerivedImpl::g_nSignificantDigits=25;
    }
    else
    {
        ATDecimalOrDerivedImpl::g_nSignificantDigits=18;
        ATFloatOrDerivedImpl::g_nSignificantDigits=7;
        ATDoubleOrDerivedImpl::g_nSignificantDigits=16;
    }
}
