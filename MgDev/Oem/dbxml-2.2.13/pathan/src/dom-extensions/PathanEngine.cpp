/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/PathanEngine.hpp>

#include <pathan/internal/dom-extensions/PathanExpressionImpl.hpp>
#include <pathan/PathanException.hpp>
#include <pathan/PathanExpression.hpp>

#include <pathan/PathanNSResolver.hpp>
#include <pathan/XPath2Result.hpp>
#include <pathan/internal/context/XPath2ContextImpl.hpp>
#include "impl/PathanDocumentImpl.hpp"
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/internal/XPath2MemoryManagerImpl.hpp>

#include <simpleVariables/VarStoreImpl.hpp>
#include <parser/XPath2ParserControl.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/exceptions/ContextException.hpp>
#include <pathan/exceptions/NamespaceLookupException.hpp>

#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMXPathException.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMXPathEvaluator.hpp>
#include <xercesc/dom/impl/DOMDocumentImpl.hpp>
#include <xercesc/dom/DOMXPathException.hpp>
#include <xercesc/framework/XMLGrammarPool.hpp>
#include <xercesc/framework/MemoryManager.hpp>
#include <xercesc/dom/DOMException.hpp>

#include <pathan/XPath2Utils.hpp>
#include <iostream>

#define yyFlexLexer xxFlexLexer
#include <lexer/XPathFlexLexer.hpp>
#undef yyFlexLexer

//Bison hooks
extern int XPathyyparse(void *);
extern int XPathyydebug;

//Remove leading and trailing whitespace of the given string
static const XMLCh* stripWhitespace(const XMLCh* inputString, XPath2MemoryManager* memMgr)
{
  if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(inputString) == 0) {
    return memMgr->getPooledString(inputString);
  }
  unsigned int start = 0, end = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(inputString) - 1;

  //Get the position of the first non-whitespace character
  while (isspace ((char) inputString[start]) && start < XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(inputString)) {
    start++;
  }//while

  //Get the position of the last non-whitespace character
  while (isspace ((char) inputString[end]) && end > 0) {
    end--;
  }//while

  if(start <= end) {
    return XPath2Utils::subString(inputString, start, end - start + 1, memMgr);
  }
  return memMgr->getPooledString(inputString);
}//stripWhitespace

PathanExpression* PathanEngine::createExpression(const XMLCh* expression,
                                                 DynamicContext* context,
                                                 XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr)
  throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException)
{
  return new (memMgr) PathanExpressionImpl(expression, context, memMgr);
}

DataItem* PathanEngine::createDataItem(const XMLCh* expression,
                                       DynamicContext* context,
                                       XPath2MemoryManager *memMgr,
                                       bool staticallyResolve)
  throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException) {

  if(memMgr == 0) {
    return 0;
  }

  //Lex expression
  const XMLCh *exprStrip = stripWhitespace(expression, memMgr);
  LexerDOMStringStream lexerStream(exprStrip);
  xxFlexLexer lexer(&lexerStream, &std::cout);

  //Parse expression
  XPathParserControl pcontrol;
  pcontrol.lexer = &lexer;
  pcontrol.context = context;
  pcontrol.memMgr = memMgr;

  XPathyydebug = 0;

  try {
    XPathyyparse((void *) &pcontrol);
    //Complete and return expression object
    if(staticallyResolve) {
      pcontrol.result = pcontrol.result->staticResolution(context);
    }
    return pcontrol.result;
  }
  catch(const ContextException &e) {
    if(PathanException::getDebug()) {
      e.printDebug( X("Caught exception at Interface") );
    }
    throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::NAMESPACE_ERR, e.getError());
  }
  catch(const NamespaceLookupException &e) {
    if(PathanException::getDebug()) {
      e.printDebug( X("Caught exception at Interface") );
    }
    throw PathanException(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::INVALID_EXPRESSION_ERR, e.getError());
  }
  catch(const FunctionException &fe) {
    if(PathanException::getDebug()) {
      fe.printDebug( X("Caught exception at Interface") );
    }
    const XMLCh* errString = fe.getError();
    throw PathanException(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::INVALID_EXPRESSION_ERR, errString);
  }
  catch(const DSLException &e) {
    if(PathanException::getDebug()) {
      e.printDebug( X("Caught exception at Interface") );
    }
    throw PathanException(XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::INVALID_EXPRESSION_ERR, e.getError());
  }

  return 0;
}

/** Create an NSResolver */
PathanNSResolver *PathanEngine::createNSResolver(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* nodeResolver,
                                                 XPath2MemoryManager *memMgr)
{
  if(memMgr == 0) {
    return 0;
  }
  return memMgr->createNSResolver(nodeResolver);
}

DynamicContext* PathanEngine::createContext(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *document, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr)
{
  if(memMgr == 0) {
    return 0;
  }

  XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool* xmlgr = 0;
  if(document != 0) {
    xmlgr = static_cast<PathanDocumentImpl*>(document)->getGrammarPool();
  }

  return new (memMgr) XPath2ContextImpl(memMgr, xmlgr, 0);
}

DynamicContext *PathanEngine::createContext(XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool *xmlGP,
					    XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr)
{
  if(memMgr == 0) {
    return 0;
  }

  return new (memMgr) XPath2ContextImpl(memMgr, xmlGP, 0);
}

DynamicContext *PathanEngine::createContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr)
{
  if(memMgr == 0) {
    return 0;
  }

  return new (memMgr) XPath2ContextImpl(memMgr, 0, 0);
}

XPath2MemoryManager *PathanEngine::createMemoryManager() {
  return new XPath2MemoryManagerImpl;
}
