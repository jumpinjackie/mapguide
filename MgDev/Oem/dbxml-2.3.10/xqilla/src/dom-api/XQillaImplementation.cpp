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
 * $Id: XQillaImplementation.cpp,v 1.6 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include "XQillaImplementation.hpp"
#include <xqilla/utils/XPath2Utils.hpp>
#include "impl/XQillaDocumentImpl.hpp"
#include "impl/XQillaBuilderImpl.hpp"
#include "impl/XQillaXMLGrammarPoolImpl.hpp"

#include "XPath2NodeSerializer.hpp"

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/dom/DOMException.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLStringTokenizer.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

const XMLCh XQillaImplementation::gXQilla[] =   // Points to "XPath2"
{XERCES_CPP_NAMESPACE_QUALIFIER chLatin_X, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_P, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2, XERCES_CPP_NAMESPACE_QUALIFIER chNull};
 
const XMLCh XQillaImplementation::g3_0[] =      // Points to "3.0"
        {XERCES_CPP_NAMESPACE_QUALIFIER chDigit_3, XERCES_CPP_NAMESPACE_QUALIFIER chPeriod, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, XERCES_CPP_NAMESPACE_QUALIFIER chNull};

XQillaImplementation::XQillaImplementation() {
    domImpl = XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation::getImplementation();
}
  
XQillaImplementation::~XQillaImplementation() {
  // nothing to do
}

// -----------------------------------------------------------------------
//  Singleton DOMImplementationImpl
// -----------------------------------------------------------------------
XQillaImplementation * XQillaImplementation::gDomimp = 0;

//static
void XQillaImplementation::initialize()
{
	if (gDomimp)
		delete gDomimp;
	gDomimp = new XQillaImplementation();
	XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationRegistry::addSource(
		gDomimp);
}

void XQillaImplementation::terminate()
{
	if (gDomimp)
		delete gDomimp;
	gDomimp = 0;
}

XQillaImplementation* XQillaImplementation::getDOMImplementationImpl() {
	// initialized by XQillaImplementation::initialize()
	return gDomimp;
}

bool XQillaImplementation::hasFeature(const  XMLCh * feature,  const  XMLCh * version) const
{
  if (!feature)
    return false;
    
  if (XERCES_CPP_NAMESPACE_QUALIFIER XMLString::compareIString(feature, gXQilla) == 0
      && (version == 0 || !*version || XERCES_CPP_NAMESPACE_QUALIFIER XMLString::equals(version, g3_0)))
      return true;
  
  return domImpl->hasFeature(feature, version);
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* XQillaImplementation::createDocument(const XMLCh *namespaceURI,
	const XMLCh *qualifiedName, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentType *doctype, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const manager)
{
    return new (manager) XQillaDocumentImpl(namespaceURI, qualifiedName, doctype, manager);
}


XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* XQillaImplementation::createDocument(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const manager)
{
  return new (manager) XQillaDocumentImpl(manager);
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMWriter* XQillaImplementation::createDOMWriter(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const manager)
{
    return new (manager) XPath2NodeSerializer(manager);
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMBuilder* XQillaImplementation::createDOMBuilder(const short mode,
                                                                                  const XMLCh* const schemaType,
                                                                                  XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const manager,
                                                                                  XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool*  const gramPool)
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool *temp = 0;
  
  if(!gramPool) {
    temp = new (manager) XQillaXMLGrammarPoolImpl(manager);
  }
  else {
    temp = gramPool;
  }


    if (mode == XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationLS::MODE_ASYNCHRONOUS)
        throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::NOT_SUPPORTED_ERR, 0);

    XERCES_CPP_NAMESPACE_QUALIFIER DOMBuilder *tmp = new (manager) XQillaBuilderImpl(0, manager, temp);
    tmp->setProperty(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXercesParserUseDocumentFromImplementation, (void*)gXQilla);
    tmp->setFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXercesCacheGrammarFromParse, true);

    return tmp;
}



XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation* XQillaImplementation::getDOMImplementation(const XMLCh* features) const
{
  XERCES_CPP_NAMESPACE_QUALIFIER  DOMImplementation* impl = XQillaImplementation::getDOMImplementationImpl();

  XERCES_CPP_NAMESPACE_QUALIFIER XMLStringTokenizer tokenizer(features);
  const XMLCh* feature = 0;

  while (feature || tokenizer.hasMoreTokens()) {

    if (!feature)
      feature = tokenizer.nextToken();
    
    const XMLCh* version = 0;
    const XMLCh* token = tokenizer.nextToken();
    
    if (token && XERCES_CPP_NAMESPACE_QUALIFIER XMLString::isDigit(token[0]))
      version = token;
    
    if (!impl->hasFeature(feature, version))
      return 0;
    
    if (!version)
      feature = token;
  }
  return impl;
}


XERCES_CPP_NAMESPACE_QUALIFIER DOMInputSource* XQillaImplementation::createDOMInputSource()
{
    throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::NOT_SUPPORTED_ERR, 0);
    return 0;
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentType *XQillaImplementation::createDocumentType(const XMLCh *qualifiedName,
	const XMLCh * publicId, const XMLCh *systemId)
{
    return domImpl->createDocumentType(qualifiedName, publicId, systemId); 
}

//Introduced in DOM Level 3
XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation* XQillaImplementation::getInterface(const XMLCh* feature){
    throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::NOT_SUPPORTED_ERR, 0);
    return 0;
}
