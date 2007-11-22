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
 * $Id: TestSuiteResultListener.hpp,v 1.12 2006/11/01 16:37:17 jpcs Exp $
 */

#ifndef _TESTSUITERESULTLISTENER_HPP
#define _TESTSUITERESULTLISTENER_HPP

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <list>

#include <xqilla/framework/XQillaExport.hpp>

class TestCase;

class XQILLA_API TestSuiteResultListener
{
public:
  virtual ~TestSuiteResultListener() {}

  virtual void reportVersion(const std::string &version) {}

  virtual void startTestGroup(const std::string &name) = 0;
  virtual void endTestGroup() = 0;

  virtual void reportPass(const TestCase &testCase, const std::string &comment) = 0;
  virtual void reportInspect(const TestCase &testCase, const std::string &actualResult,
                          const std::list<std::string> &expectedResult, const std::string &comment) = 0;
  virtual void reportSkip(const TestCase &testCase, const std::string &comment) = 0;
  virtual void reportFail(const TestCase &testCase, const std::string &actualResult,
                          const std::list<std::string> &expectedResult, const std::string &comment) = 0;
  virtual void reportFailNoError(const TestCase &testCase, const std::string &actualResult,
                                 const std::string &comment) = 0;
  virtual void reportFailUnexpectedError(const TestCase &testCase, const std::string &unexpectedError,
                                         const std::string &comment) = 0;

protected:
  TestSuiteResultListener() {}
};

class XQILLA_API KnownErrorChecker : public TestSuiteResultListener
{
public:
  KnownErrorChecker(TestSuiteResultListener *results);

  virtual void startTestGroup(const std::string &name);
  virtual void endTestGroup();

  virtual void reportPass(const TestCase &testCase, const std::string &comment);
  virtual void reportInspect(const TestCase &testCase, const std::string &actualResult,
                             const std::list<std::string> &expectedResult, const std::string &comment);
  virtual void reportSkip(const TestCase &testCase, const std::string &comment);
  virtual void reportFail(const TestCase &testCase, const std::string &actualResult,
                          const std::list<std::string> &expectedResult, const std::string &comment);
  virtual void reportFailNoError(const TestCase &testCase, const std::string &actualResult,
                                 const std::string &comment);
  virtual void reportFailUnexpectedError(const TestCase &testCase, const std::string &unexpectedError,
                                         const std::string &comment);

  bool printReport() const;

  bool loadErrors(const std::string &errorFile);
  bool saveErrors(const std::string &errorFile) const;

  class Error {
  public:
    std::string reason;
    std::string comment;
    std::string action;
  };

private:
  TestSuiteResultListener *results_;

  // testName -> (reason, comment, action)
  std::map<std::string, Error> errors_;
  std::vector<std::string> nowPass_;
  std::vector<std::string> nowFail_;  
};

class XQILLA_API ConsoleResultListener : public TestSuiteResultListener
{
public:
  ConsoleResultListener();

  virtual void startTestGroup(const std::string &name);
  virtual void endTestGroup();

  virtual void reportPass(const TestCase &testCase, const std::string &comment);
  virtual void reportInspect(const TestCase &testCase, const std::string &actualResult,
                             const std::list<std::string> &expectedResult, const std::string &comment);
  virtual void reportSkip(const TestCase &testCase, const std::string &comment);
  virtual void reportFail(const TestCase &testCase, const std::string &actualResult,
                          const std::list<std::string> &expectedResult, const std::string &comment);
  virtual void reportFailNoError(const TestCase &testCase, const std::string &actualResult,
                                 const std::string &comment);
  virtual void reportFailUnexpectedError(const TestCase &testCase, const std::string &unexpectedError,
                                         const std::string &comment);

  bool printReport() const;

private:
  void testCaseToErrorStream(const TestCase &testCase);

private:
  std::string m_szFullTestName;

  unsigned int m_nTotalTests, m_nPassedTests, m_nSkippedTests, m_nInspectTests;

  int testDepth_;
  bool needNewline_;

  std::ostringstream errorStream_;
};

class XQILLA_API XMLReportResultListener : public TestSuiteResultListener
{
public:
  XMLReportResultListener();

  virtual void reportVersion(const std::string &version) { version_ = version; }

  virtual void startTestGroup(const std::string &name);
  virtual void endTestGroup();

  virtual void reportPass(const TestCase &testCase, const std::string &comment);
  virtual void reportInspect(const TestCase &testCase, const std::string &actualResult,
                             const std::list<std::string> &expectedResult, const std::string &comment);
  virtual void reportSkip(const TestCase &testCase, const std::string &comment);
  virtual void reportFail(const TestCase &testCase, const std::string &actualResult,
                          const std::list<std::string> &expectedResult, const std::string &comment);
  virtual void reportFailNoError(const TestCase &testCase, const std::string &actualResult,
                                 const std::string &comment);
  virtual void reportFailUnexpectedError(const TestCase &testCase, const std::string &unexpectedError,
                                         const std::string &comment);

  void setImplementation(const std::string &name, const std::string &version = "", const std::string &description = "");
  void setOrganization(const std::string &name, const std::string &website = "");
  void setSubmittor(const std::string &name, const std::string &email = "");
  void setTestRun(const std::string &transformation, const std::string &comments = "");
  void setAnonymous(bool anon);
  void addImplementationDefinedItem(const std::string &name, const std::string &value);
  void addFeature(const std::string &name, bool supported);
  void addContextProperty(const std::string &name, const std::string &contextType, const std::string &value);

  void printReport() const;

private:
  std::string version_;
  std::string implName_, implVersion_, implDescription_;
  std::string orgName_, orgWebsite_;
  std::string submittorName_, submittorEmail_;
  std::string testTransformation_, testComments_;
  bool anonymous_;

  std::map<std::string, std::string> implDefinedItems_;
  std::map<std::string, bool> features_;
  std::map<std::string, std::pair<std::string, std::string> > contextProperties_;

  std::ostringstream outputStream_;
};

#endif
