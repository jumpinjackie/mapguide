/* 
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
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
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/framework/StdInInputSource.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/XMLBuffer.hpp>

//Pathan includes

#include <pathan/PathanEngine.hpp>
#include <pathan/PathanNSResolver.hpp>
#include <pathan/PathanException.hpp>
#include <pathan/PathanExpression.hpp>
#include <pathan/PathanImplementation.hpp>
#include <pathan/XPath2Result.hpp>
#include <pathan/PathanPlatformUtils.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/Node.hpp>
#include <pathan/AnyAtomicType.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>

#include "../common/DOMTreeErrorReporter.hpp"
#include <pathan/internal/DOMutils/XStr.hpp>

// structures and classes
struct XMLNamespace {
  char* prefix;
  char* uri;
};

#define XERCES XERCES_CPP_NAMESPACE_QUALIFIER

////////////////////////////
// function declarations  //
////////////////////////////

/** Print welcome message */
void welcome();

/** Print usage */
void usage();

/** Initialize Pathan. @param doValidate Tell the parser wether to validate @param showErrors show errors*/
int init(bool doValidate, bool showErros);

/** Terminate pathan (cleanup) */
void term();

/** Load an xml file */
XERCES DOMDocument *loadXMLFile(char *filename);

/** Evaluate 'expression' with specified context, document, XPath2 Compatibility mode and namespace bindings.
 * @returns 0 if expression was run successfully, non-zero otherwise */
int runExpression(XERCES DOMDocument* document, const XMLCh *expression, bool bXPath1Compatible, std::vector<XMLNamespace> &nspaces);

/** Output the result to the screen */
void printResultItems(const XPath2Result* result);

//////////////////////
// Global Variables //
//////////////////////

// The 'DOM-XPath 3.0' implementation
XERCES DOMImplementation* _DOMImpl;

// The xml parser
XERCES DOMBuilder *_xmlparser;

// The error handler - not currently used
DOMTreeErrorReporter *_errHandler;

int _main(int argc, char *argv[])
{


  // First we parse the command line arguments 
  bool doValidate =  true;
  bool debug = false;
  bool showErrors = false;
  std::vector<XMLNamespace> nspaces;
  bool compatible=false;

  char *expression = NULL;
  int newArgC = 0;
  char **newArgV = (char **)malloc(argc * sizeof(char *));
  for(int i = 1; i < argc; ++i) {
    if(*argv[i] == '-' && argv[i][2] == '\0' ){
      
      // -N option, add a namespace binding
      if(argv[i][1] =='N') {
        XMLNamespace nspace;
        if (i > argc - 3) { 
          std::cerr << "Not enough arguments for namespace option" << std::endl;
        }
        nspace.prefix = argv[++i];
        nspace.uri = argv[++i];
        nspaces.push_back(nspace);
      }

      // -d option, set DSLException error reporting to true
      else if(argv[i][1] == 'd') {
        debug = true;
      }

      // -e option, show error reporting
      else if(argv[i][1] == 'e') {
        showErrors = true;
      }
         

      // -h option, print usage
      else if(argv[i][1] == 'h') {
        welcome();
        usage();
        return 0;
      }

      // -f option, set typeExceptionPolicy to flexible (XPath2Compatibility mode is true)
      else if(argv[i][1] == 'f') {
        compatible = true;
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
    // we've parsed all commandline arguments, the rest must be filenames
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


  // initialisation of Pathan
  
  if(init(doValidate, showErrors)) {
    return 1;
  }

  PathanException::setDebug(debug);

  if(newArgC == 0) {
    // If no XML file was specified
    if(runExpression(0, X(expression), compatible, nspaces) != 0) {
      return 1;
    }
  }
  else {
    for(int i = 0; i < newArgC; ++i) {
      // Load the XML file 
      XERCES DOMDocument *doc = loadXMLFile(newArgV[i]);
      
      if(doc == 0) {
        std::cerr << "Cannot open file " << newArgV[i] << ".\n";
        return 1;
      }

      if(runExpression(doc, X(expression), compatible, nspaces) != 0) {
        return 1;
      }
    }
  }

  // clean up and exit
  term();

  free(newArgV);

  return 0;
}

int runExpression(XERCES DOMDocument *document, const XMLCh *expression, bool bXPath1Compatible, std::vector<XMLNamespace> &nspaces)
{
  //Evaluate expression on document
  try {
    AutoRelease<DynamicContext> context(PathanEngine::createContext(document));
    if(document != 0) context->setBaseURI(document->getBaseURI());
    //Set exception policy to strict by default, or flexible if -f is set
    context->setXPath1CompatibilityMode(bXPath1Compatible);

    // Add bindings to the NSResolver
    PathanNSResolver* resolver = PathanEngine::createNSResolver(document == 0 ? 0 : document->getDocumentElement(), context->getMemoryManager());
    resolver->addNamespaceBinding(X("xs"),X("http://www.w3.org/2001/XMLSchema"));
    for (std::vector<XMLNamespace>::iterator i = nspaces.begin();i != nspaces.end(); i++) {
      resolver->addNamespaceBinding(X(i->prefix),X(i->uri));
    }
    context->setNSResolver(resolver);

    AutoRelease<PathanExpression> parsedExpression(PathanEngine::createExpression(expression, context));
    
    DynamicContext *eval_context = parsedExpression->createContext(); // Memory managed by parsedExpression
    if(document != 0) eval_context->setExternalContextNode(document->getDocumentElement());

    //Evaluate expression
    AutoRelease<XPath2Result> result((XPath2Result*)parsedExpression->evaluate(eval_context, XPath2Result::ITERATOR_RESULT));

    printResultItems(result);
  }
  catch(PathanException &e) {
    std::cerr << XERCES XMLString::transcode(e.getString()) << std::endl;
    return 1;
  }
  catch(XERCES DOMException &e) {
    std::cerr << XERCES XMLString::transcode(e.msg) << std::endl;
    return 1;
  }

  return 0;
}

void printResultItems(const XPath2Result* result)
{

  // serializer used for nodes only
  XERCES DOMWriter* serializer = _DOMImpl->createDOMWriter();
  XERCES StdOutFormatTarget* target = new XERCES StdOutFormatTarget();
  
  while (result->iterateNext()) {
    if(result->isNode()) {
      serializer->writeNode(target, *(result->asNode()));
      std::cout<<std::endl; // separate nodes with \n
    } else {
      char* str=XERCES XMLString::transcode(result->asString());
      std::cout << str << std::endl;
      XERCES XMLString::release(&str);
    }
  }

  serializer->release();
  delete target;
  
}

// Set up pathan/xerces and the xml parser
int init(bool doValidate, bool showErrors)
{ 
  try {
    PathanPlatformUtils::initialize();
  } catch (const XERCES XMLException& eXerces) {
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
  _xmlparser->setFeature(XERCES XMLUni::fgXercesDOMHasPSVIInfo, true);

  _errHandler = new DOMTreeErrorReporter();
  if(showErrors) {
    _xmlparser->setErrorHandler(_errHandler);
  }
  
  return 0;
}

// Terminate Pathan
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
  std::cerr << "Eval 1.0 Beta." << std::endl;
  std::cerr << "(c) 2003 DecisionSoft Limited.\n";
  std::cerr << "User documentation and licence at http://software.decisionsoft.com\n\n";
}

// print the usage message
void usage()
{
  std::cerr << "\nUsage: eval [options] \"XPath Expression\" [ data1.xml ... ]" << std::endl << std::endl;
  std::cerr << "-f : Enable XPath 1.0 compatibility mode (default is off)" << std::endl;
  std::cerr << "-h : Show this display" << std::endl;
  std::cerr << "-e : Enable schema validation error reporting" << std::endl;
  std::cerr << "-d : Enable debug" << std::endl;
  std::cerr << "-N prefix uri : bind prefix to the namespace uri" << std::endl;
}

#include <xercesc/util/XMLStringTokenizer.hpp>

int main(int argc, char *argv[])
{
  // check if we are asked to run an entire test case ("-t" option)
  if(argc==3 && strcmp(argv[1],"-t")==0)
    {
      std::vector<char*> testStrings;
      XERCES XMLPlatformUtils::Initialize();
      XERCES DOMImplementation* impl = XERCES DOMImplementationRegistry::getDOMImplementation(X("core"));
      XERCES DOMBuilder* xmlparser = ((XERCES DOMImplementationLS*)impl)->createDOMBuilder(XERCES DOMImplementationLS::MODE_SYNCHRONOUS, 0);
      xmlparser->setFeature(XERCES XMLUni::fgDOMNamespaces, true);
      xmlparser->setFeature(XERCES XMLUni::fgXercesSchema, true);
      xmlparser->setFeature(XERCES XMLUni::fgDOMValidation, false);

      XERCES DOMDocument *tests=xmlparser->parseURI(argv[2]);
      XERCES DOMElement* pTests=tests->getDocumentElement();
      XERCES DOMElement* pPolicies=(XERCES DOMElement*)pTests->getFirstChild();
      XERCES DOMNode* pNode=(XERCES DOMElement*)pPolicies->getNextSibling();
      int numArguments=2;
      char* args[40];
      args[0]="eval";
      args[1]="placeholder for query";
      XMLCh* policies=XERCES XMLString::transcode("policies");
      XMLCh* policy=XERCES XMLString::transcode("policy");
      XMLCh* addArg=XERCES XMLString::transcode("addArg");
      XMLCh* base=XERCES XMLString::transcode("base");
      XMLCh* type=XERCES XMLString::transcode("type");
      XMLCh* fail=XERCES XMLString::transcode("fail");
      XMLCh* arg=XERCES XMLString::transcode("arg");
      while(pNode)
        {
          if(pNode->getNodeType()==XERCES DOMNode::ELEMENT_NODE && 
             XERCES XMLString::equals(pNode->getNodeName(),policies))
            {
              XERCES DOMNode* elt=pNode->getFirstChild();
              while(elt)
                {
                  if(elt->getNodeType()==XERCES DOMNode::ELEMENT_NODE && 
                     XERCES XMLString::equals(elt->getNodeName(),policy))
                    {
                      XERCES DOMNode* arg=elt->getFirstChild();
                      while(arg)
                        {
                          if(arg->getNodeType()==XERCES DOMNode::ELEMENT_NODE && 
                             XERCES XMLString::equals(arg->getNodeName(),addArg))
                            {
                              static const XMLCh space[]={ XERCES chSpace, XERCES chNull };
                              const XMLCh* argument=arg->getFirstChild()->getNodeValue();
                              XERCES XMLStringTokenizer tokenizer(argument,space);
                              while(tokenizer.hasMoreTokens())
                                args[numArguments++]=XERCES XMLString::transcode(tokenizer.nextToken());
                            }
                          arg=arg->getNextSibling();
                        }
                    }
                  elt=elt->getNextSibling();
                }
            }
          else if(pNode->getNodeType()==XERCES DOMNode::ELEMENT_NODE && 
                  XERCES XMLString::equals(pNode->getNodeName(),base))
            {
              XERCES DOMElement* elt=(XERCES DOMElement*)pNode;
              if(!XERCES XMLString::equals(elt->getAttribute(type),fail))
                {
                  XERCES DOMNode* pArg=pNode->getFirstChild();
                  while(pArg)
                    {
                      if(pArg->getNodeType()==XERCES DOMNode::ELEMENT_NODE && 
                         XERCES XMLString::equals(pArg->getNodeName(),arg))
                        {
                          const XMLCh* arg=pArg->getFirstChild()->getNodeValue();
                          testStrings.push_back(XERCES XMLString::transcode(arg));
                          break;
                        }
                      pArg=pArg->getNextSibling();
                    }
                }
            }
          pNode=pNode->getNextSibling();
        }
      delete xmlparser;
      XERCES XMLString::release(&policies);
      XERCES XMLString::release(&policy);
      XERCES XMLString::release(&addArg);
      XERCES XMLString::release(&base);
      XERCES XMLString::release(&type);
      XERCES XMLString::release(&fail);
      XERCES XMLString::release(&arg);

      std::vector<char*>::iterator it;
      for(it=testStrings.begin();it!=testStrings.end();it++)
        {
          args[1]=*it;
          _main(numArguments,args);
          XERCES XMLString::release(&args[1]);
        }
      for(int i=2;i<numArguments;i++)
        XERCES XMLString::release(&args[i]);

      XERCES XMLPlatformUtils::Terminate();
      return 0;
    }
  return _main(argc,argv);
}

