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
 * $Id: TestSuiteParser.cpp,v 1.9 2006/11/01 16:37:23 jpcs Exp $
 */

#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

#include <xqilla/xqts/TestSuiteParser.hpp>
#include <xqilla/xqts/TestSuiteRunner.hpp>
#include <xqilla/xqts/TestSuiteResultListener.hpp>

#include <iostream>

#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/URLInputSource.hpp>
#include <xercesc/util/BinFileInputStream.hpp>

#include <xqilla/utils/XStr.hpp>
#include <xqilla/utils/UTF8Str.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif
using namespace std;

TestSuiteParser::TestSuiteParser(const string &pathToTestSuite, TestSuiteRunner *runner)
  : m_runner(runner)
{
  string szXQTSLocation=string("file:///")+pathToTestSuite;

  for(string::iterator i=szXQTSLocation.begin();i!=szXQTSLocation.end();i++) {
    if(*i==' ') {
      i=szXQTSLocation.erase(i);
      szXQTSLocation=szXQTSLocation.insert(i-szXQTSLocation.begin(),"%20");
      i=szXQTSLocation.begin();
    }
    else if(*i=='\\') *i='/';
  }

  if(*(szXQTSLocation.end()-1)!='/')
    szXQTSLocation+='/';

  m_urlXQTSDirectory.setURL(X(szXQTSLocation.c_str()));
  m_bReadingChars=false;
}

void TestSuiteParser::run()
{
  SAX2XMLReader* parser=XMLReaderFactory::createXMLReader();
  try {
    parser->setContentHandler(this);
    parser->setErrorHandler(this);
    XMLURL url(m_urlXQTSDirectory,"XQTSCatalog.xml");
    URLInputSource is(url);
    parser->parse(is);
  }
  catch(const XMLException& e) {
    cerr << "Error while reading test catalog: " << UTF8(e.getMessage()) << endl;
  }
  delete parser;
}

void TestSuiteParser::error(const SAXParseException& exc)
{
  cerr << "ERROR parsing catalog: " << UTF8(exc.getMessage()) << endl;
}

void TestSuiteParser::fatalError(const SAXParseException& exc)
{
  cerr << "FATAL ERROR parsing catalog: " << UTF8(exc.getMessage()) << endl;
}

void TestSuiteParser::startElement
(
    const XMLCh* const uri,
    const XMLCh* const localname,
    const XMLCh* const qname,
    const Attributes&  attributes
)
{
    static XMLCh g_szName[]  = { chLatin_n, chLatin_a, chLatin_m, chLatin_e, chNull };
    static XMLCh g_szPath[]  = { chLatin_F, chLatin_i, chLatin_l, chLatin_e, chLatin_P, chLatin_a, chLatin_t, chLatin_h, chNull };
    static XMLCh g_szVar[]   = { chLatin_v, chLatin_a, chLatin_r, chLatin_i, chLatin_a, chLatin_b, chLatin_l, chLatin_e, chNull };
    static XMLCh g_szDotXQ[] = { chPeriod, chLatin_x, chLatin_q, chNull };
    static XMLCh g_szComp[]  = { chLatin_c, chLatin_o, chLatin_m, chLatin_p, chLatin_a, chLatin_r, chLatin_e, chNull };
    string szName=UTF8(localname);
    string szURI=UTF8(uri);
    if(szURI=="http://www.w3.org/2005/02/query-test-XQTSCatalog")
    {
      if(szName=="test-suite") {
        m_runner->getResultListener()->reportVersion(UTF8(attributes.getValue(X("version"))));

        m_urlXQTSQueriesDirectory.setURL(m_urlXQTSDirectory, attributes.getValue(X("XQueryQueryOffsetPath")));
        m_urlXQTSResultsDirectory.setURL(m_urlXQTSDirectory, attributes.getValue(X("ResultOffsetPath")));
      }
      else if(szName=="test-group") {
        m_runner->startTestGroup(UTF8(attributes.getValue(g_szName)));
      }
      else if(szName=="test-case")
      {
        m_testCase.name = UTF8(attributes.getValue(g_szName));
        m_testCase.queryURL = "";
        m_testCase.contextItem = "";
        m_testCase.defaultCollection = "";
        m_testCase.inputURIVars.clear();
        m_testCase.inputVars.clear();
        m_testCase.extraVars.clear();
        m_testCase.expectedErrors.clear();
        m_testCase.outputFiles.clear();
        m_testCase.moduleFiles.clear();

        m_szVariableBoundToInput=m_szCompareMethod="";
        m_urlQuery=XMLURL();
        XMLBuffer buff;
        buff.set(attributes.getValue(g_szPath));
        buff.append('/');
        m_urlBasePath.setURL(m_urlXQTSQueriesDirectory, buff.getRawBuffer());
        m_urlBasePathReferenceFiles.setURL(m_urlXQTSResultsDirectory, buff.getRawBuffer());
      }
      else if(szName=="query")
      {
        XMLBuffer buff;
        const XMLCh* name=attributes.getValue(g_szName);
        buff.set(name);
        if(!XMLString::endsWith(name,g_szDotXQ))
            buff.append(g_szDotXQ);
        m_urlQuery.setURL(m_urlBasePath,buff.getRawBuffer());
        m_testCase.queryURL = UTF8(m_urlQuery.getURLText());

        Janitor<BinFileInputStream> stream((BinFileInputStream*)URLInputSource(m_urlQuery).makeStream());
        unsigned int dwSize=stream->getSize();
        m_testCase.query.resize(dwSize);
        stream->readBytes((XMLByte*)m_testCase.query.c_str(), dwSize);
      }
      else if(szName=="input-query")
      {
        XMLBuffer buff;
        const XMLCh* name=attributes.getValue(g_szName);
        buff.set(name);
        if(!XMLString::endsWith(name,g_szDotXQ))
            buff.append(g_szDotXQ);
        XMLURL urlQuery(m_urlBasePath,buff.getRawBuffer());
        string varQuery = UTF8(urlQuery.getURLText());

        m_testCase.extraVars[UTF8(attributes.getValue(g_szVar))]=varQuery;
      }
      else if(szName=="input-file")
      {
        m_bReadingChars=true;
        m_szChars="";
        m_szVariableBoundToInput=UTF8(attributes.getValue(g_szVar));
      }
      else if(szName=="input-URI")
      {
        m_bReadingChars=true;
        m_szChars="";
        m_szVariableBoundToInput=UTF8(attributes.getValue(g_szVar));
      }
      else if(szName=="contextItem")
      {
        m_bReadingChars=true;
        m_szChars="";
      }
      else if(szName=="defaultCollection")
      {
        m_bReadingChars=true;
        m_szChars="";
      }
      else if(szName=="output-file")
      {
        m_bReadingChars=true;
        m_szChars="";
        m_szCompareMethod=UTF8(attributes.getValue(g_szComp));
      }
      else if(szName=="expected-error")
      {
        m_bReadingChars=true;
        m_szChars="";
      }
      else if(szName=="source") {
        XMLURL realFile(m_urlXQTSDirectory, attributes.getValue(X("FileName")));
        m_runner->addSource(UTF8(attributes.getValue(X("ID"))), UTF8(realFile.getURLText()), UTF8(attributes.getValue(X("schema"))));
      }
      else if(szName=="schema") {
        XMLURL realFile(m_urlXQTSDirectory, attributes.getValue(X("FileName")));
        m_runner->addSchema(UTF8(attributes.getValue(X("ID"))), UTF8(realFile.getURLText()), UTF8(attributes.getValue(X("uri"))));
      }
      else if(szName=="module") {
        if(attributes.getValue(X("FileName"))!=NULL)
        {
            XMLURL realFile(m_urlXQTSDirectory, attributes.getValue(X("FileName")));
            m_runner->addModule(UTF8(attributes.getValue(X("ID"))), UTF8(realFile.getURLText()));
        }
        else
        {
            m_bReadingChars=true;
            m_szChars="";
            m_szNamespace=UTF8(attributes.getValue(X("namespace")));
        }
      }
      else if(szName=="collection") {
        m_szCollectionID = UTF8(attributes.getValue(X("ID")));
      }
      else if(!m_szCollectionID.empty() && szName=="input-document") {
        m_bReadingChars=true;
        m_szChars="";
      }
    }
}

void TestSuiteParser::characters
(
    const   XMLCh* const    chars
    , const unsigned int    length
)
{
    XMLBuffer buff;
    buff.set(chars,length);
    if(m_bReadingChars)
        m_szChars+=UTF8(buff.getRawBuffer());
}

void TestSuiteParser::endElement
(
	const XMLCh* const uri,
	const XMLCh* const localname,
	const XMLCh* const qname
)
{
    string szName=UTF8(localname);
    string szURI=UTF8(uri);
    if(szURI=="http://www.w3.org/2005/02/query-test-XQTSCatalog")
    {
      if(szName == "test-group") {
        m_runner->endTestGroup();
      }
      else if(szName == "test-case") {
        m_runner->runTestCase(m_testCase);
      }
      else if(szName == "input-file")
      {
        m_bReadingChars=false;
        m_testCase.inputVars[m_szVariableBoundToInput]=m_szChars;
        m_szVariableBoundToInput="";
      }
      else if(szName=="input-URI")
      {
        m_bReadingChars=false;
        m_testCase.inputURIVars[m_szVariableBoundToInput]=m_szChars;
        m_szVariableBoundToInput="";
      }
      else if(szName=="contextItem")
      {
        m_bReadingChars=false;
        m_testCase.contextItem=m_szChars;
      }
      else if(szName=="defaultCollection")
      {
        m_bReadingChars=false;
        m_testCase.defaultCollection=m_szChars;
      }
      else if(szName == "output-file")
      {
        m_bReadingChars=false;
        m_szChars=UTF8(XMLURL(m_urlBasePathReferenceFiles, m_szChars.c_str()).getURLText());
        m_testCase.outputFiles[m_szChars]=m_szCompareMethod;
        m_szCompareMethod="";
      }
      else if(szName == "expected-error")
      {
        m_bReadingChars=false;
        m_testCase.expectedErrors.push_back(m_szChars);
      }
      else if(szName == "module")
      {
        m_bReadingChars=false;
        m_testCase.moduleFiles.push_back(std::pair<std::string, std::string>(m_szNamespace, m_szChars));
      }
      else if(szName=="collection") {
        m_szCollectionID = "";
      }
      else if(!m_szCollectionID.empty() && szName=="input-document") {
        m_bReadingChars=false;
        m_runner->addCollection(m_szCollectionID, m_szChars);
      }
    }
}
