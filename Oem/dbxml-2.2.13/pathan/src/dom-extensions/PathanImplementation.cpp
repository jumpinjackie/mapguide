/*
 * Copyright (c) 2003, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/PathanImplementation.hpp>
#include <pathan/XPath2Utils.hpp>
#include "./impl/PathanDocumentImpl.hpp"
#include "./impl/PathanBuilderImpl.hpp"
#include "./impl/PathanXMLGrammarPoolImpl.hpp"

#include <pathan/XPath2NodeSerializer.hpp>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/dom/DOMException.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLStringTokenizer.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

const XMLCh PathanImplementation::gPathan[] =   // Points to "XPath2"
{XERCES_CPP_NAMESPACE_QUALIFIER chLatin_X, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_P, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2, XERCES_CPP_NAMESPACE_QUALIFIER chNull};
 
const XMLCh PathanImplementation::g3_0[] =      // Points to "3.0"
        {XERCES_CPP_NAMESPACE_QUALIFIER chDigit_3, XERCES_CPP_NAMESPACE_QUALIFIER chPeriod, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, XERCES_CPP_NAMESPACE_QUALIFIER chNull};

PathanImplementation::PathanImplementation() {
    domImpl = XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation::getImplementation();
}
  
PathanImplementation::~PathanImplementation() {
  // nothing to do
}

// -----------------------------------------------------------------------
//  Singleton DOMImplementationImpl
// -----------------------------------------------------------------------
PathanImplementation * PathanImplementation::gDomimp = 0;

//static
void PathanImplementation::initialize()
{
	if (gDomimp)
		delete gDomimp;
	gDomimp = new PathanImplementation();
	XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationRegistry::addSource(
		gDomimp);
}

void PathanImplementation::terminate()
{
	if (gDomimp)
		delete gDomimp;
	gDomimp = 0;
}

PathanImplementation* PathanImplementation::getDOMImplementationImpl() {
	// initialized by PathanImplementation::initialize()
	return gDomimp;
}

bool PathanImplementation::hasFeature(const  XMLCh * feature,  const  XMLCh * version) const
{
  if (!feature)
    return false;
    
  if (XERCES_CPP_NAMESPACE_QUALIFIER XMLString::compareIString(feature, gPathan) == 0
      && (version == 0 || !*version || XERCES_CPP_NAMESPACE_QUALIFIER XMLString::equals(version, g3_0)))
      return true;
  
  return domImpl->hasFeature(feature, version);
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* PathanImplementation::createDocument(const XMLCh *namespaceURI,
	const XMLCh *qualifiedName, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentType *doctype, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const manager)
{
    return new (manager) PathanDocumentImpl(namespaceURI, qualifiedName, doctype, manager);
}


XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* PathanImplementation::createDocument(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const manager)
{
  return new (manager) PathanDocumentImpl(manager);
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMWriter* PathanImplementation::createDOMWriter(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const manager)
{
    return new (manager) XPath2NodeSerializer(manager);
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMBuilder* PathanImplementation::createDOMBuilder(const short mode,
                                                                                  const XMLCh* const schemaType,
                                                                                  XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const manager,
                                                                                  XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool*  const gramPool)
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool *temp = 0;
  
  if(!gramPool) {
    temp = new (manager) PathanXMLGrammarPoolImpl(manager);
  }
  else {
    temp = gramPool;
  }


    if (mode == XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationLS::MODE_ASYNCHRONOUS)
        throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::NOT_SUPPORTED_ERR, 0);

    XERCES_CPP_NAMESPACE_QUALIFIER DOMBuilder *tmp = new (manager) PathanBuilderImpl(0, manager, temp);
    tmp->setProperty(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXercesParserUseDocumentFromImplementation, (void*)gPathan);
    tmp->setFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXercesCacheGrammarFromParse, true);

    return tmp;
}



XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation* PathanImplementation::getDOMImplementation(const XMLCh* features) const
{
  XERCES_CPP_NAMESPACE_QUALIFIER  DOMImplementation* impl = PathanImplementation::getDOMImplementationImpl();

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


XERCES_CPP_NAMESPACE_QUALIFIER DOMInputSource* PathanImplementation::createDOMInputSource()
{
    throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::NOT_SUPPORTED_ERR, 0);
    return 0;
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentType *PathanImplementation::createDocumentType(const XMLCh *qualifiedName,
	const XMLCh * publicId, const XMLCh *systemId)
{
    return domImpl->createDocumentType(qualifiedName, publicId, systemId); 
}

//Introduced in DOM Level 3
XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation* PathanImplementation::getInterface(const XMLCh* feature){
    throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::NOT_SUPPORTED_ERR, 0);
    return 0;
}
