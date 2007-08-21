//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: UnitTest.cpp,v 1.6 2006/10/30 17:46:14 bostic Exp $
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
