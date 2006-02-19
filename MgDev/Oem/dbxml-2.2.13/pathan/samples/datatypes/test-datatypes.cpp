/* * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

//System includes

#if defined(WIN32) && !defined(__CYGWIN__)
#include <iostream.h>
#endif

//STL includes
#include <vector>

//Xerces includes

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/framework/StdInInputSource.hpp>
#include <xercesc/validators/schema/SchemaValidator.hpp>
#include <xercesc/validators/schema/SchemaGrammar.hpp>

//Pathan includes
#include <pathan/PathanException.hpp>
#include <pathan/PathanEngine.hpp>
#include <pathan/PathanNSResolver.hpp>

#include <pathan/Sequence.hpp>
#include <pathan/PathanPlatformUtils.hpp>
#include <pathan/XPath2NSUtils.hpp>
#include <utils/NumUtils.hpp>
#include <pathan/internal/context/DynamicContextImpl.hpp>
#include <pathan/Node.hpp>
#include <pathan/exceptions/DSLException.hpp>
#include <pathan/internal/DOMutils/XStr.hpp>

//Local includes
#include "../common/DOMTreeErrorReporter.hpp"

#include "ATAnyURITester.hpp"
#include "ATBase64BinaryTester.hpp"
#include "ATBooleanTester.hpp"
#include "ATDateTester.hpp"
#include "ATDateTimeTester.hpp"
#include "ATDecimalTester.hpp"
#include "ATDoubleTester.hpp"
#include "ATDurationTester.hpp"
#include "ATFloatTester.hpp"
#include "ATGDayTester.hpp"
#include "ATGMonthTester.hpp"
#include "ATGMonthDayTester.hpp"
#include "ATGYearMonthTester.hpp"
#include "ATGYearTester.hpp"
#include "ATHexBinaryTester.hpp"
//#include "ATNotationTester.hpp"
#include "ATQNameTester.hpp"
#include "ATStringTester.hpp"
#include "ATTimeTester.hpp"
#include "ATUntypedAtomicTester.hpp"
#include "CastAsTester.hpp"
#include "TypePromotionTester.hpp"

// functions
void usage();

int main(int argc, char *argv[])
{
  bool fullExceptionDebug = false;
  
  if(argc == 2 && *argv[1] == '-' && argv[1][2] == '\0' ){
      if(argv[1][1] == 'x') {
        fullExceptionDebug = true;
      }
      else {
        usage();
      }
  } else if (argc != 1) {
    usage();
  }
  
  ///////////////////////////////////////////////////////////////////
  // initialisation                                                //
  ///////////////////////////////////////////////////////////////////
  try{
    PathanPlatformUtils::initialize();
  }
  catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& eXerces){
    char *pMsg = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(eXerces.getMessage());
    std::cerr << "Error during Xerces-C initialisation.\n"
              << "Xerces exception message: "
              << pMsg << std::endl;
    delete pMsg;
    return 1;
  }

  XPath2MemoryManager* memMgr = PathanEngine::createMemoryManager();

  XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser *xmlparser = new XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser();

  xmlparser->setValidationScheme(XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser::Val_Auto);

  xmlparser->setDoNamespaces(true);
  xmlparser->setCreateEntityReferenceNodes(false);
  xmlparser->setDoSchema(true);

  DOMTreeErrorReporter *errHandler = new DOMTreeErrorReporter();
  xmlparser->setErrorHandler(errHandler);

  //Initialise the Pathan memoryManager
  PathanException::setDebug(false);
  DSLException::setDebug(fullExceptionDebug);

  //no XML file specified
  XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation *factory = XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationRegistry::getDOMImplementation(X("Core"));;
  XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = factory->createDocument(0,X("test"), 0);
  
  DynamicContext* context = PathanEngine::createContext(memMgr);
  context->setBaseURI(doc->getBaseURI());

  PathanNSResolver* resolver = context->getMemoryManager()->createNSResolver(doc->getDocumentElement());
  resolver->addNamespaceBinding(X("xs"),X("http://www.w3.org/2001/XMLSchema"));
  context->setNSResolver(resolver);

  /////////////////////////////////////////////////////////////////
  // Create test vector                                          //
  /////////////////////////////////////////////////////////////////
  std::vector<DatatypeTester*> tests;

  tests.push_back(new ATStringTester(memMgr));
  tests.push_back(new ATAnyURITester(memMgr));
  tests.push_back(new ATBase64BinaryTester(memMgr));
  tests.push_back(new ATBooleanTester(memMgr));
  tests.push_back(new ATDateTester(memMgr));
  tests.push_back(new ATDateTimeTester(memMgr));
  tests.push_back(new ATDecimalTester(memMgr));
  tests.push_back(new ATDoubleTester(memMgr));
  tests.push_back(new ATDurationTester(memMgr));
  tests.push_back(new ATFloatTester(memMgr));
  tests.push_back(new ATGDayTester(memMgr));
  tests.push_back(new ATGMonthTester(memMgr));
  tests.push_back(new ATGMonthDayTester(memMgr));
  tests.push_back(new ATGYearMonthTester(memMgr));
  tests.push_back(new ATGYearTester(memMgr));
  tests.push_back(new ATHexBinaryTester(memMgr));
//  tests.push_back(new ATNotationTester(memMgr));
  tests.push_back(new ATQNameTester(memMgr));
  tests.push_back(new ATTimeTester(memMgr));
  tests.push_back(new ATUntypedAtomicTester(memMgr));
  tests.push_back(new CastAsTester(memMgr));
  tests.push_back(new TypePromotionTester(memMgr));

  bool failed = false;
  //////////////////////////////////////////////////////////////////
  // test!                                                        //
  //////////////////////////////////////////////////////////////////
  for(std::vector<DatatypeTester*>::iterator test = tests.begin(); test != tests.end(); test++) {
    (*test)->init();
    
    try {
      (*test)->run(context);
    } catch(const PathanException &e) {
      std::cerr << std::endl << "PathanException: " << XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(e.getString()) << std::endl;
      failed = true;
    } catch(const DSLException &e) {
      std::cerr << std::endl << "DSLException: " << XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(e.getError()) << std::endl;
      failed = true;
    } catch( ... ) {
      std::cerr << "Caught unknown exception!"<<std::endl;
      failed = true;
    }
    
    (*test)->wrapUp();
  }

  //////////////////////////////////////////////////////////////////
  // clean up and exit                                            //
  //////////////////////////////////////////////////////////////////
  context->release();
	delete xmlparser; //parser must be deleted before calling Terminate
  delete errHandler;
  PathanPlatformUtils::terminate();
  delete memMgr;

  if (failed)
    return 1;
  else
    return 0;
}

void usage()
{
  std::cerr << "\nUsage: test-datatypes [-x]" << std::endl
            << "-x : Enable full exception debugging (all exception throws shown)"<< std::endl;
}

