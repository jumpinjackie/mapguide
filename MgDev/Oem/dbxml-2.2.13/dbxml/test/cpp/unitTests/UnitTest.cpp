//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: UnitTest.cpp,v 1.4 2005/04/05 16:44:37 bostic Exp $
//

#include "dbxml/DbXml.hpp"

#include "TestEnvironment.hpp"
#include "UnitTest.hpp"

using namespace DbXmlTest;

UnitTest::UnitTest(TestEnvironment &env, const std::string &name)
  : env_(env), name_(name)
{
}

UnitTest::~UnitTest()
{
}

// preparation is optional
void UnitTest::prepare()
{
}
