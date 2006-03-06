//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: TestResolver.cpp,v 1.4 2005/04/05 16:44:41 bostic Exp $
//

// System includes
#include <string>
#include <fstream>

// DB XML includes (external)
#include "dbxml/DbXml.hpp"

// Local (test) includes
#include "TestResolver.hpp"

using namespace DbXml;
using namespace DbXmlTest;

TestResolver::TestResolver()
  : dir_("./")
{
}

TestResolver::~TestResolver()
{
}

XmlInputStream *TestResolver::resolveEntity(XmlTransaction *txn, XmlManager &mgr,
					    const std::string &systemId,
					    const std::string &publicId) const
{
  // verify the existance of the DTD in the filesystem
  std::string file = dir_ + systemId;
  std::ifstream ifs(file.c_str(), std::ios::in|std::ios::binary);
  if(ifs) {
    ifs.close();
    return mgr.createLocalFileInputStream(file);
  }

  return 0;
}

void TestResolver::setDir(const std::string &dir)
{ 
  dir_ = dir;

  // ensure that directory has a trailing slash
  size_t pos = dir_.rfind("/");
  if(pos != dir_.length()-1) //maybe on windows?
    pos = dir_.rfind("\\");

  if (pos != dir_.length()-1) //just add a slash
    dir_.append("/");
}

std::string TestResolver::getDir() const
{
  return dir_;
}

