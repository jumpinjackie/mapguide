//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: simpleContainerInEnv.cpp,v 1.10 2006/10/30 17:45:45 bostic Exp $
//

#include "dbxml/DbXml.hpp"
#include "db_cxx.h"

using namespace DbXml;
//some exception handling omitted for clarity

int main(void)
{
  //The path the directory where you want to place the environment
  // must exist!!
  const char *environmentPath = "/path/to/environment/directory";

  //Create and open a database environment. This is a very simple example.
  DbEnv env(0);
  env.set_cachesize(0, 64 * 1024 * 1024, 1);
  env.open(environmentPath,
	   DB_INIT_MPOOL|DB_CREATE|DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_TXN, 0);
  XmlManager db(&env);

  //multiple containers can be opened in the same database environment
  XmlContainer container1 = db.createContainer("myContainer1");

  XmlContainer container2 = db.createContainer("myContainer2");

  XmlContainer container3 = db.createContainer("myContainer3");

  // do work here //

  return 0;
}
