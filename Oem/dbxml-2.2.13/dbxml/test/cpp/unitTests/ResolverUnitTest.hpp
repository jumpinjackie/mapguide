//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: ResolverUnitTest.hpp,v 1.2 2005/04/05 16:44:37 bostic Exp $
//

#ifndef __RESOLVERUNITTEST_HPP
#define __RESOLVERUNITTEST_HPP

#include <string>

#include "TestEnvironment.hpp"
#include "UnitTest.hpp"

namespace DbXmlTest
{
  class ResolverUnitTest : public UnitTest
  {
  public:
    ResolverUnitTest(TestEnvironment &env, const Iterator<std::string> &args = EMPTY(std::string));
    virtual ~ResolverUnitTest();

    virtual void execute();

  private:
    ResolverUnitTest(const ResolverUnitTest &);
    ResolverUnitTest &operator=(const ResolverUnitTest &);

  };
}

#endif
