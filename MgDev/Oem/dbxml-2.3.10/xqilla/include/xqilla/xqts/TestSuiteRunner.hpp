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
 * $Id: TestSuiteRunner.hpp,v 1.7 2006/11/01 16:37:17 jpcs Exp $
 */

#ifndef _TESTSUITERUNNER_HPP
#define _TESTSUITERUNNER_HPP

#include <string>
#include <list>
#include <map>

#include <xqilla/framework/XQillaExport.hpp>

class TestSuiteResultListener;
class TestCase;

class XQILLA_API TestSuiteRunner
{
public:
  virtual ~TestSuiteRunner() {}

  TestSuiteResultListener *getResultListener() { return m_results; }

  virtual void addSource(const std::string &id, const std::string &filename, const std::string &schema) = 0;
  virtual void addSchema(const std::string &id, const std::string &filename, const std::string &uri) = 0;
  virtual void addModule(const std::string &id, const std::string &filename) = 0;
  virtual void addCollection(const std::string &id, const std::string &filename) = 0;

  virtual void startTestGroup(const std::string &name) = 0;
  virtual void endTestGroup() = 0;

  virtual void runTestCase(const TestCase &testCase) = 0;

protected:
  TestSuiteRunner(TestSuiteResultListener *results) : m_results(results) {}

  void testResults(const TestCase &testCase, const std::string &results) const;
  void testErrors(const TestCase &testCase, const std::string &actualError) const;

protected:
  TestSuiteResultListener *m_results;
};

#endif
