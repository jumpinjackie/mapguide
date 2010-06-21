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
 * $Id: runner.cpp,v 1.27 2006/11/01 16:37:23 jpcs Exp $
 */

#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

#include <xqilla/xqts/TestSuiteParser.hpp>
#include <xqilla/xqts/TestSuiteResultListener.hpp>
#include <xqilla/xqts/TestSuiteRunner.hpp>

#include <iostream>

#include <xercesc/framework/URLInputSource.hpp>
#include <xercesc/util/XMLEntityResolver.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/dom/DOMException.hpp>

#include <xqilla/xqilla-simple.hpp>
#include <xqilla/context/VariableStore.hpp>
#include <xqilla/context/ModuleResolver.hpp>
#include <xqilla/context/URIResolver.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/utils/XQillaPlatformUtils.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif
using namespace std;

class XQillaTestSuiteRunner : public TestSuiteRunner, private XMLEntityResolver, private ModuleResolver, private URIResolver
{
public:
  XQillaTestSuiteRunner(const string &singleTest, TestSuiteResultListener *results);

  virtual void addSource(const string &id, const string &filename, const string &schema);
  virtual void addSchema(const string &id, const string &filename, const string &uri);
  virtual void addModule(const string &id, const string &filename);
  virtual void addCollection(const string &id, const string &filename);

  virtual void startTestGroup(const string &name);
  virtual void endTestGroup();

  virtual void runTestCase(const TestCase &testCase);

private:
  virtual InputSource* resolveEntity(XMLResourceIdentifier* resourceIdentifier);
  virtual bool resolveModuleLocation(VectorOfStrings* result, const XMLCh* nsUri, const StaticContext* context);
  virtual bool resolveDocument(Sequence &result, const XMLCh* uri, DynamicContext* context);
  virtual bool resolveCollection(Sequence &result, const XMLCh* uri, DynamicContext* context);
  virtual bool resolveDefaultCollection(Sequence &result, DynamicContext* context);

private:
  string m_szSingleTest;
  string m_szFullTestName;
  const TestCase* m_pCurTestCase;

  // id -> filename
  map<string, string> m_inputFiles;
  // schemaURL -> filename
  map<string, string> m_schemaFiles;
  // id -> filename
  map<string, string> m_moduleFiles;
  // id -> list of inputFiles ID
  map<string, list<string> > m_collections;
};

void usage(const char *progname)
{
  const char *name = progname;
  while(*progname != 0) {
    if(*progname == '/' || *progname == '\\') {
      ++progname;
      name = progname;
    } else {
      ++progname;
    }
  }

  cout << "Usage: " << name << " [options] <location of the XQTS suite> (<test group or case name>)?" << endl << endl;
  cout << "-e <file>      : Use the given file as a known error file" << endl;
  cout << "-E <file>      : Output an error file" << endl;
  cout << "-h             : Show this display" << endl;
  cout << "-x             : Output results as XML" << endl;
}

int main(int argc, char *argv[])
{
  string testSuitePath;
  string singleTest;
  string errorFile;
  string outputErrorFile;
  bool xmlResults = false;

  for(int i = 1; i < argc; ++i) {
    if(*argv[i] == '-' && argv[i][2] == '\0' ){

      switch(argv[i][1]) {
      case 'h': {
        usage(argv[0]);
        return 0;
      }
      case 'e': {
        i++;
        if(i == argc) {
          cout << "Missing argument to option 'e'" << endl;
          return 1;
        }
        errorFile = argv[i];
        break;
      }
      case 'E': {
        i++;
        if(i == argc) {
          cout << "Missing argument to option 'E'" << endl;
          return 1;
        }
        outputErrorFile = argv[i];
        break;
      }
      case 'x': {
        xmlResults = true;
        break;
      }
      default: {
        cout << "Unknown option: " << argv[i] << endl;
        usage(argv[0]);
        return 1;
      }
      }
    }
    else if(testSuitePath == "") {
      testSuitePath = argv[i];
    }
    else if(singleTest == "") {
      singleTest = argv[i];
    }
    else {
      usage(argv[0]);
      return 1;
    }
  }

  if(testSuitePath == "") {
    cout << "Test suite path not specified!" << endl;
    usage(argv[0]);
    return 1;
  }

  XQillaPlatformUtils::enableExtendedPrecision(false);
  XQilla xqilla;

  Janitor<TestSuiteResultListener> results(0);
  if(xmlResults) {
    results.reset(new XMLReportResultListener());
    XMLReportResultListener *xmlreport = (XMLReportResultListener*)results.get();
    xmlreport->setImplementation("XQilla", "1.0");
    xmlreport->setOrganization("XQilla", "http://xqilla.sourceforge.net");

    xmlreport->addImplementationDefinedItem("expressionUnicode", "UTF-16");
    xmlreport->addImplementationDefinedItem("implicitTimezone", "Defined by the system clock");
    xmlreport->addImplementationDefinedItem("XMLVersion", "1.1");
    xmlreport->addImplementationDefinedItem("axes", "Full axis support");
    xmlreport->addImplementationDefinedItem("defaultOrderEmpty", "empty least");
    xmlreport->addImplementationDefinedItem("normalizationForms", "NFC, NFD, NFKC, NFKD");
    xmlreport->addImplementationDefinedItem("docProcessing", "schema validation");

    xmlreport->addFeature("Minimal Conformance", true);
    xmlreport->addFeature("Schema Import", true);
    xmlreport->addFeature("Schema Validation", true);
    xmlreport->addFeature("Static Typing", false);
    xmlreport->addFeature("Static Typing Extensions", false);
    xmlreport->addFeature("Full Axis", true);
    xmlreport->addFeature("Module", true);
    xmlreport->addFeature("Serialization", false);
    xmlreport->addFeature("Trivial XML Embedding", false);
  }
  else {
    results.reset(new ConsoleResultListener());
  }

  KnownErrorChecker knownErrors(results.get());
  if(errorFile != "" && !knownErrors.loadErrors(errorFile)) {
    return 1;
  }

  XQillaTestSuiteRunner runner(singleTest, &knownErrors);
  TestSuiteParser parser(testSuitePath, &runner);

  parser.run();

  bool passed = true;
  if(xmlResults) {
    ((XMLReportResultListener*)results.get())->printReport();
  }
  else {
    passed = ((ConsoleResultListener*)results.get())->printReport();
  }

  if(errorFile != "") {
    passed = knownErrors.printReport();
  }

  if(outputErrorFile != "" && !knownErrors.saveErrors(outputErrorFile)) {
    cout << "Unable to open error file: " << outputErrorFile << endl;
    return 1;
  }

  return passed ? 0 : 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

string serializeXMLResults(const Sequence &result, const DynamicContext *context);

XQillaTestSuiteRunner::XQillaTestSuiteRunner(const string &singleTest, TestSuiteResultListener *results)
  : TestSuiteRunner(results),
    m_szSingleTest(singleTest),
    m_pCurTestCase(NULL)
{
}

void XQillaTestSuiteRunner::startTestGroup(const string &name)
{
  if(m_szFullTestName != "")
    m_szFullTestName += ":";
  m_szFullTestName += name;

  m_results->startTestGroup(name);
}

void XQillaTestSuiteRunner::endTestGroup()
{
  int nColonPos = m_szFullTestName.find_last_of(":");
  if(nColonPos != -1)
    m_szFullTestName = string(m_szFullTestName.c_str(), nColonPos);
  else
    m_szFullTestName = "";

  m_results->endTestGroup();
}

void XQillaTestSuiteRunner::addSource(const string &id, const string &filename, const string &schema)
{
  m_inputFiles[id] = filename;
}

void XQillaTestSuiteRunner::addSchema(const string &id, const string &filename, const string &uri)
{
  m_schemaFiles[uri] = filename;
}

void XQillaTestSuiteRunner::addModule(const string &id, const string &filename)
{
  m_moduleFiles[id] = filename;
}

void XQillaTestSuiteRunner::addCollection(const string &id, const string &filename)
{
  m_collections[id].push_back(filename);
}

void XQillaTestSuiteRunner::runTestCase(const TestCase &testCase)
{
  if(m_szSingleTest != "" &&
     testCase.name.find(m_szSingleTest) == string::npos &&
     m_szFullTestName.find(m_szSingleTest) == string::npos) {
    m_results->reportSkip(testCase, "Not run");
    return;
  }

  if(m_szFullTestName.substr(0,21)=="Optional:StaticTyping") {
    m_results->reportSkip(testCase, "Static typing not supported");
    return;
  }
  if(m_szFullTestName.substr(0,26)=="Optional:TrivialEmbedding") {
    m_results->reportSkip(testCase, "TrivialEmbedding not supported");
    return;
  }

  XQilla xqilla;

  m_pCurTestCase=&testCase;
  Janitor<DynamicContext> context(xqilla.createContext());
  try {
    context->setImplicitTimezone(context->getItemFactory()->
                                 createDurationOrDerived(FunctionConstructor::XMLChXPath2DatatypesURI,
                                                         ATDurationOrDerived::fgDT_DAYTIMEDURATION,
                                                         X("PT0S"), context.get()));
    context->setXMLEntityResolver(this);
    context->setModuleResolver(this);
    context->registerURIResolver(this);

    Janitor<XQQuery> pParsedQuery(xqilla.parseFromURI(X(testCase.queryURL.c_str()), XQilla::XQUERY, context.get(), XQilla::NO_ADOPT_CONTEXT));

    map<string, string>::const_iterator v;
    for(v=testCase.extraVars.begin();v!=testCase.extraVars.end();v++) {
      XQQuery* pInnerQuery = xqilla.parseFromURI(X(v->second.c_str()), XQilla::XQUERY, context.get());
      Sequence doc=pInnerQuery->execute(context.get())->toSequence(context.get());
      context->getVariableStore()->setGlobalVar(X(v->first.c_str()),doc,context.get(), 0);
    }
    for(v=testCase.inputVars.begin();v!=testCase.inputVars.end();v++) {
      Sequence doc=context->resolveDocument(X(v->second.c_str()), 0);
      context->getVariableStore()->setGlobalVar(X(v->first.c_str()),doc,context.get(), 0);
    }
    for(v=testCase.inputURIVars.begin();v!=testCase.inputURIVars.end();v++) {
      Item::Ptr uri = context->getItemFactory()->createString(X(v->second.c_str()),context.get());
      context->getVariableStore()->setGlobalVar(X(v->first.c_str()), uri, context.get(), 0);
    }
    if(!testCase.contextItem.empty())
    {
      Sequence doc=context->resolveDocument(X(testCase.contextItem.c_str()), 0);
      context->setContextItem(doc.first());
    }
    context->setContextPosition(1);
    context->setContextSize(1);
    time_t curTime;
    context->setCurrentTime(time(&curTime));

    Sequence result=pParsedQuery->execute(context.get())->toSequence(context.get());

    testResults(testCase, serializeXMLResults(result, context.get()));
  }
  catch(XQException& e) {
    ostringstream oss;
//     if(e.getXQueryLine() == 0) {
//       oss << "No line number:" << std::endl << UTF8(e.getError()) << std::endl;
//       oss << "at " << UTF8(e.getXQueryFile()) << ":" << e.getXQueryLine() << ":" << e.getXQueryColumn() << std::endl;
//       oss << "at " << e.getCppFile() << ":" << e.getCppLine() << std::endl;
//       m_results->reportFailUnexpectedError(testCase, oss.str(), "XXX");
//     }
//     else if(e.getXQueryColumn() == 0) {
//       oss << "No column number:" << std::endl << UTF8(e.getError()) << std::endl;
//       oss << "at " << UTF8(e.getXQueryFile()) << ":" << e.getXQueryLine() << ":" << e.getXQueryColumn() << std::endl;
//       oss << "at " << e.getCppFile() << ":" << e.getCppLine() << std::endl;
//       m_results->reportFailUnexpectedError(testCase, oss.str(), "XXX");
//     }
//     else if(e.getXQueryFile() == 0) {
//       oss << "No file name:" << std::endl << UTF8(e.getError()) << std::endl;
//       oss << "at " << UTF8(e.getXQueryFile()) << ":" << e.getXQueryLine() << ":" << e.getXQueryColumn() << std::endl;
//       oss << "at " << e.getCppFile() << ":" << e.getCppLine() << std::endl;
//       m_results->reportFailUnexpectedError(testCase, oss.str(), "XXX");
//     }
//     else {
      oss << UTF8(e.getError()) << std::endl;
      oss << "at " << UTF8(e.getXQueryFile()) << ":" << e.getXQueryLine() << ":" << e.getXQueryColumn() << std::endl;
      oss << "at " << e.getCppFile() << ":" << e.getCppLine() << std::endl;
      testErrors(testCase, oss.str());
//     }
  }
  catch(DOMException &de) {
    testErrors(testCase, string("DOMException: ") + UTF8(de.getMessage()));
  }
  catch(...) {
    testErrors(testCase, "[Unknown exception]");
  }
  m_pCurTestCase=NULL;
}

InputSource* XQillaTestSuiteRunner::resolveEntity(XMLResourceIdentifier* resourceIdentifier)
{
    const XMLCh* systemId=resourceIdentifier->getSystemId();
    if((systemId==NULL || *systemId==0) && 
       resourceIdentifier->getResourceIdentifierType()==XMLResourceIdentifier::SchemaGrammar) {
	    
      map<string, string>::const_iterator i =
        m_schemaFiles.find(UTF8(resourceIdentifier->getNameSpace()));
      if(i != m_schemaFiles.end()) {
        return new URLInputSource(X(i->second.c_str()));
      }
    }
    else if(resourceIdentifier->getResourceIdentifierType()==XMLResourceIdentifier::UnKnown) {
      list<std::pair<string, string> >::const_iterator i;
      for(i=m_pCurTestCase->moduleFiles.begin(); i!=m_pCurTestCase->moduleFiles.end(); i++)
      {
        if(i->first == UTF8(resourceIdentifier->getNameSpace()) && 
           i->second == UTF8(resourceIdentifier->getSystemId()))
        {
          map<string, string>::const_iterator i2 = m_moduleFiles.find(i->second);
          if(i2 != m_moduleFiles.end()) {
            string file=i2->second+".xq";
            return new URLInputSource(X(file.c_str()));
          }
        }
      }
    }
    return NULL;
}

bool XQillaTestSuiteRunner::resolveModuleLocation(VectorOfStrings* result, const XMLCh* nsUri, const StaticContext* context)
{
  bool bFound=false;
  list<std::pair<string, string> >::const_iterator i;
  for(i=m_pCurTestCase->moduleFiles.begin(); i!=m_pCurTestCase->moduleFiles.end(); i++)
  {
    if(i->first == UTF8(nsUri))
    {
      result->push_back(context->getMemoryManager()->getPooledString(i->second.c_str()));
      bFound=true;
    }
  }
  return bFound;
}

bool XQillaTestSuiteRunner::resolveDocument(Sequence &result, const XMLCh* uri, DynamicContext* context)
{
  std::map<std::string, std::string>::iterator it=m_inputFiles.find(UTF8(uri));
  if(it!=m_inputFiles.end())
  {
    result=context->resolveDocument(X(it->second.c_str()), 0);
    return true;
  }
  return false;
}

bool XQillaTestSuiteRunner::resolveCollection(Sequence &result, const XMLCh* uri, DynamicContext* context)
{
  std::map<std::string, std::list<std::string> >::iterator it=m_collections.find(UTF8(uri));
  if(it!=m_collections.end())
  {
    for(std::list<std::string>::iterator s=it->second.begin();s!=it->second.end();s++)
    {
      result.joinSequence(context->resolveDocument(X(s->c_str()), 0));
    }
    return true;
  }
  return false;
}

bool XQillaTestSuiteRunner::resolveDefaultCollection(Sequence &result, DynamicContext* context)
{
  if(!m_pCurTestCase->defaultCollection.empty())
    return resolveCollection(result, X(m_pCurTestCase->defaultCollection.c_str()), context);
  return false;
}

string serializeXMLResults(const Sequence &result, const DynamicContext *context) {
  MemBufFormatTarget strTarget;
  {
    XMLFormatter formatter("UTF-8", "1.0", &strTarget, XMLFormatter::NoEscapes, XMLFormatter::UnRep_CharRef);
    for(unsigned int i=0;i<result.getLength();i++) {
      const Item* item=result.item(i);
      if(i>0 && !item->isNode() && !result.item(i-1)->isNode())
        formatter << X(" ");
      if(item->isNode())
        formatter << XMLFormatter::NoEscapes;
      else
        formatter << XMLFormatter::CharEscapes;
      formatter << item->asString(context);
    }
  }
  return (char*)strTarget.getRawBuffer();
}
