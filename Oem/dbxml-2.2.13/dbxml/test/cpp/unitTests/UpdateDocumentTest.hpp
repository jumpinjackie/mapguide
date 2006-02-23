//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: UpdateDocumentTest.hpp,v 1.2 2005/04/05 16:44:37 bostic Exp $
//

#ifndef __UPDATEDOCUMENTTEST_HPP
#define __UPDATEDOCUMENTTEST_HPP

#include <map>
#include <string>

#include "TestEnvironment.hpp"
#include "UnitTest.hpp"

namespace DbXml
{
  class XmlDocument;
}

namespace DbXmlTest
{
  class Functor;

  class UpdateDocumentTest : public UnitTest
  {
  public:

    // avoid inclusion of XmlQueryContext
    enum EvaluationType {
      EVAL_NOT_SET,
      EAGER,
      LAZY
    };

    UpdateDocumentTest(TestEnvironment &env, const Iterator<std::string> &args = EMPTY(std::string));
    virtual ~UpdateDocumentTest();

    virtual void execute();

  private:
    UpdateDocumentTest(const UpdateDocumentTest &);
    UpdateDocumentTest &operator=(const UpdateDocumentTest &);

    void prepare();
    void resetContainer();
    void doTest(const std::string &method, const std::string &xpath);

    typedef std::map<std::string, Functor*> UpdateMethodMap;
    UpdateMethodMap updateMethods_;

    std::string docName_;
    EvaluationType evalType_;
  };

}

#endif
