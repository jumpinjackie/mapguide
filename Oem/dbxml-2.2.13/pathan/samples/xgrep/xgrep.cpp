/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include <pathan/internal/DOMutils/XStr.hpp>
#include "../common/DOMTreeErrorReporter.hpp"

//System includes

#if defined(WIN32) && !defined(__CYGWIN__)
#include <iostream.h>
#endif

#include <stdio.h>

//Xerces includes

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMBuilder.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMXPathNSResolver.hpp>
#include <xercesc/framework/StdInInputSource.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/XMLFormatter.hpp>

//Pathan includes

#include <pathan/PathanEngine.hpp>
#include <pathan/XPath2Result.hpp>
#include <pathan/PathanException.hpp>
#include <pathan/PathanExpression.hpp>
#include <pathan/PathanImplementation.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/PathanPlatformUtils.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/Node.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/PathanNSResolver.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>

#define XERCES XERCES_CPP_NAMESPACE_QUALIFIER

// functions
void welcome();
void usage();
int init(bool doValidate);
void term();
XERCES DOMDocument *loadXMLFile(char *filename);
int runExpression(XERCES DOMDocument *document, const XMLCh* expression, const std::string filename, bool bXPath1Compatible);
void printResultNodes(const XPath2Result* setSnapshot, const std::string filename);
void printResultPaths(const XPath2Result* setSnapshot, const std::string filename, const DynamicContext* context);
XMLCh* getPath(const XERCES DOMNode *node, const DynamicContext* context);

//global variables
XERCES DOMImplementation* _DOMImpl;
XERCES DOMBuilder* _xmlparser;
DOMTreeErrorReporter *_errHandler;
bool opt_p = false;

int main(int argc, char *argv[])
{
  bool doValidate = false;
  bool debug = false;
  bool bXPath1Compatible = false;

  // parse the command line arguments
  char *expression = NULL;
  int newArgC = 0;
  char **newArgV = (char **)malloc(argc * sizeof(char *));
  for(int i = 1; i < argc; ++i) {
    if(*argv[i] == '-' && argv[i][2] == '\0') {
      if(argv[i][1] == 'd') {
        debug = true;
      }

      else if(argv[i][1] == 'f') {
        bXPath1Compatible = true;
      }

      else if(argv[i][1] == 'v') {
        doValidate = true;
      }

      // -p option : path
      else if(argv[i][1] == 'p') {
        opt_p = true;
      }
      // -h option : help
      else if(argv[i][1] == 'h') {
        welcome();
        usage();
        return 0;
      }
      else if(isdigit(argv[i][1])) {
        //expression is -1, -2 etc, so is XPath
        expression = argv[i];
      }
      else {
        welcome();
        usage();
        return 1;
      }
    }
    else if(expression == NULL) {
      expression = argv[i];
    }
    else {
      newArgV[newArgC++] = argv[i];
    }
  }

  // Check for bad command line arguments
  if(expression == NULL) {
    welcome();
    usage();
    return 1;
  }


  // initialisation
  if(init(doValidate)) {
    return 1;
  }

  PathanException::setDebug(debug);

  // run the parsed expression on the document(s)
  if(newArgC == 0) {
    XERCES DOMDocument *doc = _DOMImpl->createDocument(0,X("test"), 0);
    
    if(runExpression(doc, X(expression) , "stdin", bXPath1Compatible) != 0) {
      return 1;
    }
  }
  else {
    for(int i = 0; i < newArgC; ++i) {
      // Load the XML file
      XERCES DOMDocument *doc = loadXMLFile(newArgV[i]);

      if(runExpression(doc, X(expression), newArgV[i], bXPath1Compatible) != 0) {
        return 1;
      }

    }
  }

  // clean up and exit
  term();

  free(newArgV);

  return 0;
}

int runExpression(XERCES DOMDocument *document, const XMLCh* expression, const std::string filename, bool bXPath1Compatible)
{
  if(document != 0) {

    try {
      //Evaluate expression on document
      AutoRelease<DynamicContext> context(PathanEngine::createContext(document));
      context->setXPath1CompatibilityMode(bXPath1Compatible);

      PathanNSResolver *nsR = PathanEngine::createNSResolver(document->getDocumentElement(), context->getMemoryManager());
      context->setNSResolver(nsR);

      AutoRelease<PathanExpression> parsedExpression(PathanEngine::createExpression(expression, context));

      DynamicContext *eval_context = parsedExpression->createContext(); // Memory managed by parsedExpression
      eval_context->setExternalContextNode(document->getDocumentElement());

      //Evaluate expression
      AutoRelease<XPath2Result> result(parsedExpression->evaluate(eval_context, XPath2Result::SNAPSHOT_RESULT));
      
      if(opt_p) {
        printResultPaths(result, filename, context);
      }
      else {
        printResultNodes(result, filename);
      }
    }
    catch(const PathanException &e) {
      std::cerr << XERCES XMLString::transcode(e.getString()) << std::endl;
      return 1;
    }
  }
  else {
    std::cerr << "Cannot open file " << filename << ".\n";
  }

  return 0;
}

void printResultNodes(const XPath2Result* result, const std::string filename)
{
  // serializer for nodes only
  XERCES DOMWriter* serializer = _DOMImpl->createDOMWriter();
  XERCES StdOutFormatTarget* target = new XERCES StdOutFormatTarget();
    
  for(unsigned int i = 0; i < result->getSnapshotLength(); i++) {
    std::cout << "<?xgrep-match [" << filename;
    std::cout << "]--------------------------------?>\n";
    result->snapshotItem(i);
    if(result->isNode()) {
      serializer->writeNode(target, *(result->asNode()));
      std::cout<<std::endl; // separate nodes with \n
    }
  }
}

void printResultPaths(const XPath2Result* result, const std::string filename, const DynamicContext* context)
{

  for(unsigned int i = 0; i < result->getSnapshotLength(); i++) {
    result->snapshotItem(i);
    const XERCES DOMNode *node = result->asNode();
  
    std::cout << filename << ": ";
    XMLCh *path = getPath(node, context);
    char *tPath = XERCES XMLString::transcode(path); 
    std::cout << tPath << std::endl;
    delete[] path;
    delete[] tPath;
  }
}

XERCES DOMNode *getParent(const XERCES DOMNode *node)
{
  if(node->getNodeType() == XERCES DOMNode::ATTRIBUTE_NODE) {
    return (static_cast<const XERCES DOMAttr *>(node))->getOwnerElement();
  }
  else {
    return node->getParentNode();
  }
}

XMLCh* getPath(const XERCES DOMNode *node, const DynamicContext* context)
{
  XERCES XMLBuffer buf;

  // preceding path
  XERCES DOMNode *parent = getParent(node);
  if((parent != 0) && parent->getNodeType() != XERCES DOMNode::DOCUMENT_NODE) {
    XMLCh *parPath = getPath(parent, context);
    buf.append(parPath);
    delete[] parPath;
  }

  // slash then name
  buf.append(XERCES chForwardSlash);

  switch(node->getNodeType()) {
  case XERCES DOMNode::TEXT_NODE: {
    buf.append(Node::text_string);
    buf.append(XERCES chOpenParen);
    buf.append(XERCES chCloseParen);
    break;
  }
  case XERCES DOMNode::COMMENT_NODE: {
    buf.append(Node::comment_string);
    buf.append(XERCES chOpenParen);
    buf.append(XERCES chCloseParen);
    break;
  }
  case XERCES DOMNode::ATTRIBUTE_NODE: {
    buf.append(XERCES chAmpersand);
    // break left out
  }
  case XERCES DOMNode::ELEMENT_NODE: {
    if(node->getPrefix() != 0) {
      buf.append(node->getPrefix());
      buf.append(XERCES chColon);
    }
    buf.append(node->getLocalName());
    break;
  }
  default: {
    //REVISIT should throw an exception
    abort();
  }
  }

  // index
  if(node->getNodeType() != XERCES DOMNode::ATTRIBUTE_NODE) {
    buf.append(XERCES chOpenSquare);
    char b[32];
    sprintf(b, "%d", XPath2Utils::indexNode(node));
    buf.append(X(b));
    buf.append(XERCES chCloseSquare);
  }

  return XERCES XMLString::replicate(buf.getRawBuffer());
}

// Set up xerces and the xml parser
int init(bool doValidate)
{
  try{
    PathanPlatformUtils::initialize();
  }
  catch (const XERCES XMLException& eXerces){
    char *pMsg = XERCES XMLString::transcode(eXerces.getMessage());
    std::cerr << "Error during Xerces-C initialisation.\n"
              << "Xerces exception message: "
              << pMsg << std::endl;
    delete pMsg;
    return 1;
  }

  XERCES XMLBuffer implementation_name;
  implementation_name.set(PathanImplementation::gPathan);
  implementation_name.append(XERCES chSpace);
  implementation_name.append(PathanImplementation::g3_0);

  _DOMImpl = XERCES DOMImplementationRegistry::getDOMImplementation(implementation_name.getRawBuffer());

  _xmlparser = _DOMImpl->createDOMBuilder(XERCES DOMImplementationLS::MODE_SYNCHRONOUS, 0);
  _xmlparser->setFeature(XERCES XMLUni::fgDOMNamespaces, true);
  _xmlparser->setFeature(XERCES XMLUni::fgXercesSchema, true);
  _xmlparser->setFeature(XERCES XMLUni::fgDOMValidation, doValidate);


  //  _xmlparser->setProperty(XERCES XMLUni::fgXercesScannerName, (void*)XERCES XMLUni::fgSGXMLScanner);
    


  _errHandler = new DOMTreeErrorReporter();
  _xmlparser->setErrorHandler(_errHandler);

  return 0;
}

// kill xerces and the xml parser
void term()
{
  delete _xmlparser; //parser must be deleted before calling Terminate
  delete _errHandler;
  PathanPlatformUtils::terminate();
}

// parse an XML file to memory
XERCES DOMDocument *loadXMLFile(char *filename)
{
  XERCES DOMDocument *result= 0;
  try {
    result = _xmlparser->parseURI(filename);
  }
  catch(const XERCES XMLException& eParse) {
    std::cerr << "Error during script parsing, in file "
              << filename << ".\nMessage: "
              << XERCES XMLString::transcode(eParse.getMessage())
              << std::endl;

    return 0;
  }
  catch(const XERCES SAXParseException& eParse) {
    std::cerr << "Error during script parsing, in file "
              << filename << " at line " << eParse.getLineNumber()
              << ", column " << eParse.getColumnNumber()
              << ".\nMessage: "
              << XERCES XMLString::transcode(eParse.getMessage())
              << std::endl;

    return 0;
  }

  return result;
}

// print the welcome message
void welcome()
{
  std::cerr << "X-Grep 2.0 Beta." << std::endl;
  std::cerr << "(c) 2001 DecisionSoft Limited.\n";
  std::cerr << "User documentation and licence at http://www.xmlscript.org\n\n";
}

// print the usage message
void usage()
{
  std::cerr << "\nUsage: xgrep [-h] [-p] [-v] [-n] \"XPath Expression\" data1.xml [data2.xml] ..." << std::endl << std::endl;
  std::cerr << "-f : Enable XPath 1.0 compatibility mode (default is off)" << std::endl;
  std::cerr << "-h : Show this display" << std::endl;
  std::cerr << "-p : Display an absolute xpath expression to query match" << std::endl;
  std::cerr << "-d : Enable debug" << std::endl;
  std::cerr << "-v : Enable validation" << std::endl;
  std::cerr << "-n : Disable namespaces" << std::endl;
}

