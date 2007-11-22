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
 * $Id: TestSuiteParser.hpp,v 1.9 2006/11/01 16:37:17 jpcs Exp $
 */

#ifndef _TESTSUITEPARSERHANDLER_HPP
#define _TESTSUITEPARSERHANDLER_HPP

#include <string>
#include <list>
#include <map>

#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLURL.hpp>

#include <xqilla/framework/XQillaExport.hpp>

class TestSuiteRunner;

class XQILLA_API TestCase
{
public:
  std::string name;
  std::string queryURL;
  std::string query;
  std::string contextItem;
  std::string defaultCollection;
  std::map<std::string, std::string> inputURIVars;
  std::map<std::string, std::string> inputVars;
  std::map<std::string, std::string> extraVars;
  std::map<std::string, std::string> outputFiles;
  std::list<std::pair<std::string, std::string> > moduleFiles;
  std::list<std::string> expectedErrors;
};

class XQILLA_API TestSuiteParser : private XERCES_CPP_NAMESPACE_QUALIFIER DefaultHandler
{
public:
  TestSuiteParser(const std::string &pathToTestSuite, TestSuiteRunner *runner);

  void run();

private:
  virtual void startElement
  (
    const XMLCh* const uri,
    const XMLCh* const localname,
    const XMLCh* const qname,
    const XERCES_CPP_NAMESPACE_QUALIFIER Attributes&	attrs
  );
  virtual void endElement
  (
	const XMLCh* const uri,
	const XMLCh* const localname,
	const XMLCh* const qname
   );
  virtual void characters
  (
    const XMLCh* const chars,
    const unsigned int length
   );

  virtual void error(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& exc);
  virtual void fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& exc);

private:
  TestSuiteRunner *m_runner;

  XERCES_CPP_NAMESPACE_QUALIFIER XMLURL m_urlXQTSDirectory, m_urlXQTSQueriesDirectory, m_urlXQTSResultsDirectory, 
    m_urlBasePath, m_urlBasePathReferenceFiles, m_urlQuery;

  bool m_bReadingChars;
  std::string m_szChars;

  std::string m_szVariableBoundToInput, m_szCompareMethod, m_szNamespace, m_szCollectionID;

  TestCase m_testCase;
};

#endif
