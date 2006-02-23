/*
 * Copyright (c) 2003, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include "PathanBuilderImpl.hpp"
#include "PathanDocumentImpl.hpp"
#include "PathanXMLGrammarPoolImpl.hpp"
#include <pathan/internal/context/DocumentCacheImpl.hpp>

#include <xercesc/framework/XMLGrammarPool.hpp>
#include <xercesc/framework/Wrapper4DOMInputSource.hpp>
#include <xercesc/framework/XMLValidator.hpp>
#include <xercesc/validators/common/GrammarResolver.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/dom/impl/DOMDocumentImpl.hpp>
#include <xercesc/framework/XMLGrammarDescription.hpp>


#include <xercesc/framework/XMLSchemaDescription.hpp>

const XMLCh PathanBuilderImpl::gPathan[] =   // Points to "XPath2"
{XERCES_CPP_NAMESPACE_QUALIFIER chLatin_X, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_P, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2, XERCES_CPP_NAMESPACE_QUALIFIER chNull};




PathanBuilderImpl::PathanBuilderImpl( XERCES_CPP_NAMESPACE_QUALIFIER XMLValidator* const   valToAdopt
                              , XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const  manager
                              , XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool* const gramPool) :

DOMBuilderImpl(valToAdopt, manager, gramPool)
{
}

// ---------------------------------------------------------------------------
//  DOMBuilderImpl: Parsing methods
// ---------------------------------------------------------------------------
XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* PathanBuilderImpl::parse(const XERCES_CPP_NAMESPACE_QUALIFIER DOMInputSource& source)
{
    initParser();
    
    XERCES_CPP_NAMESPACE_QUALIFIER Wrapper4DOMInputSource isWrapper((XERCES_CPP_NAMESPACE_QUALIFIER DOMInputSource*) &source, false, getMemoryManager());

    AbstractDOMParser::parse(isWrapper);

    return getDocumentAndAddGrammar();

}

void PathanBuilderImpl::initParser() {
    //set it here in case someone has messed it up.
    setProperty(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXercesParserUseDocumentFromImplementation, (void*)gPathan);
 
    //add our specific ones in
    XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool *xmlGr = getGrammarResolver()->getGrammarPool();
    xmlGr->cacheGrammar(DocumentCacheParser::createXQueryTypes(getMemoryManager(), getGrammarResolver()));

}


XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* PathanBuilderImpl::getDocumentAndAddGrammar() {
    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = 0;

    if (getFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXercesUserAdoptsDOMDocument))
        doc = adoptDocument();
    else
        doc = getDocument();
    if(doc == 0) {
      return 0;
    }

    //we copy this gramamr and reset the parser one in the process.
    PathanXMLGrammarPoolImpl *gr = new (getMemoryManager()) PathanXMLGrammarPoolImpl(getMemoryManager());

    XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOfEnumerator< XERCES_CPP_NAMESPACE_QUALIFIER Grammar> enumerator
        = getGrammarResolver()->getCachedGrammarEnumerator();

    XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool *oldGrPool = getGrammarResolver()->getGrammarPool();
    while(enumerator.hasMoreElements()) {
        XERCES_CPP_NAMESPACE_QUALIFIER Grammar &g = enumerator.nextElement();
        gr->cacheGrammar(oldGrPool->orphanGrammar(g.getGrammarDescription()->getGrammarKey()));
    }

    ((PathanDocumentImpl*)doc)->setGrammarPool(gr);
    return doc;
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* PathanBuilderImpl::parseURI(const XMLCh* const systemId)
{
    initParser();
    AbstractDOMParser::parse(systemId);
    return getDocumentAndAddGrammar();
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* PathanBuilderImpl::parseURI(const char* const systemId)
{
    initParser();
    AbstractDOMParser::parse(systemId);
    return getDocumentAndAddGrammar();
}
