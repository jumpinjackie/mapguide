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
 * $Id: basicXQillaUsage.cpp,v 1.9 2006/11/01 16:37:22 jpcs Exp $
 */

//System includes


#include <iostream>

//Xerces includes

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>

//XQilla includes

#include <xqilla/xqilla-dom3.hpp>

// IO include
#include <iostream>

XERCES_CPP_NAMESPACE_USE;
using namespace std;

int main(int argc, char *argv[])
{
  if(argc != 2) {
    cerr << "Data file not specified." << endl;
    return 1;
  }

  ///////////////////////
  // Initialise XQilla //
  ///////////////////////

  // 1. Initialize XQilla
  //
  // Note that this initialisation takes care of initialising
  // xerces-c as well
  try {
    
    XQillaPlatformUtils::initialize();
    
  } catch (const XMLException& eXerces) {
    cerr << "Error during Xerces-C initialisation.\n"
              << "Xerces exception message: "
              << UTF8(eXerces.getMessage()) << endl;
    return 1;
  }

  try {

    // 2. Obtain a DOM3 XPath2 implementation.  This is a XQilla-specific 
    //    implementation that overrides the standard DOMDocument, the standard 
    //    DOMWriter and the standard DOMBuilder
    DOMImplementation* xqillaImplementation = 
      DOMImplementationRegistry::getDOMImplementation(X("XPath2 3.0"));

    // 3. Obtain a parser and set 'do namespaces', 'use schema' and 'validate' to 
    //    true.
    DOMBuilder* xmlParser = xqillaImplementation->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    xmlParser->setFeature(X("namespaces"), true);
    xmlParser->setFeature(X("http://apache.org/xml/features/validation/schema"), true);
    xmlParser->setFeature(X("validation"), true);

    ////////////////////////////
    // Parse our XML document //
    ////////////////////////////
  
    DOMDocument* document = xmlParser->parseURI(argv[1]);
    if(document == 0) {
      cerr << "Document not found: " << argv[1] << endl;
      return 1;
    }

    ////////////////////////////////
    // Create a parsed expression //
    // (compiled) and evaluate it //
    ////////////////////////////////


    // 1. Create a Namespace Resolver.  This holds a map of namespace prefixes 
    //    to namespace URIs.
  
    const DOMXPathNSResolver* resolver = document->createNSResolver(document->getDocumentElement());

    //    For a more advanced interface (like to have the ability to add 
    //    namespace bindings, this NSResolver can be cast to a XQillaNSResolver
    XQillaNSResolver* xqillaResolver = (XQillaNSResolver*)resolver;
    xqillaResolver->addNamespaceBinding(X("my"), X("http://example.com/myURI"));
  
    // **************** Example 1: max() function ****************** //
  
    // 2. Create a parsed expression
    char* expression = "max(/Catalogue/Book/Price)";
    const DOMXPathExpression* parsedExpression = document->createExpression(X(expression), resolver); 

    // 3. Evaluate the expression. We choose to have a first result, since we 
    //    know the answer will have only one item. Could also choose 
    //    XPath2Result::SNAPSHOT_RESULT and XPath2Result::ITERATOR_RESULT
    XPath2Result* firstResult = (XPath2Result*)parsedExpression->evaluate(document->getDocumentElement(), XPath2Result::FIRST_RESULT, 0);

    // 4. Work with the result: output it to the screen in this case
    cout << "The answer for expression '" << expression << "' is: " << firstResult->asDouble() << endl;

    // **************** Example 2: output of nodes ****************** //
    
    // 2. Create a parsed expression
    char* expression2 = "//Magazine";
    parsedExpression = document->createExpression(X(expression2), resolver);

    // 3. Evaluate the expression. We choose to have an iterator result
    XPath2Result* iteratorResult = (XPath2Result*)parsedExpression->evaluate(document->getDocumentElement(), XPath2Result::ITERATOR_RESULT, 0);

    // 4. Work with the result: output it to the screen in this case
  
    // create a DOMWriter (serializer) to output the nodes
    DOMWriter* serializer = xqillaImplementation->createDOMWriter();
    StdOutFormatTarget* target = new StdOutFormatTarget();

    cout << "The answer for expression '" << expression2 << "' is: " <<endl;

    int i = 0;
    while(iteratorResult->iterateNext()) {
      if(iteratorResult->isNode()) {
        cout << "Node " << i++ << ": "<< flush;
        serializer->writeNode(target, *(iteratorResult->asNode()));
        cout << endl;
      } else {
        cerr << "Expected a node but received an atomic value!"<< endl;
      }
    }

    serializer->release();
  }
  catch(DOMException &e) {
    cerr << "DOMException: " << UTF8(e.getMessage()) << endl;
    return 1;
  }
  catch(DOMXPathException &e) {
    cerr << "DOMException: " << UTF8(e.msg) << endl;
    return 1;
  }

  ///////////////
  // Terminate //
  ///////////////

  XQillaPlatformUtils::terminate();
  
  return 0;
}
