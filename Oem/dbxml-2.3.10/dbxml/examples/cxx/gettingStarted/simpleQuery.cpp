//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: simpleQuery.cpp,v 1.12 2006/10/30 17:45:45 bostic Exp $
//

#include <fstream>
#include <cstdio>

#include "dbxml/DbXml.hpp"

using namespace DbXml;

void usage()
{
        std::cerr <<   "This program performs various XPath queries against a DBXML container. You should\n"
                  << "pre-load the container using loadExamplesData.[sh|cmd] before running this example. You\n"
                  << "are only required to pass this command the path location of the database environment\n"
                  << "that you specified when you pre-loaded the examples data:\n\n"

                  << "\t-h <dbenv directory>" << std::endl;
        exit( -1 );
}

void doQuery( XmlManager &db, const XmlContainer &container, const std::string &query )
{
	////////////////////////////////////////////////////////////////////////
	//////  Performs a simple query (no context) against the   ///////
	//////  provided container.                                      ///////
	////////////////////////////////////////////////////////////////////////

	///// some defensive code eliminated for clarity //

	// Perform the query. Result type is by default Result Document
	std::string fullQuery = "collection('" + container.getName() + "')" + query;
	try {
		std::cout << "Exercising query '" << fullQuery << "' " << std::endl;
		std::cout << "Return to continue: ";
		getc(stdin);

		XmlQueryContext context = db.createQueryContext();

		XmlResults results( db.query(fullQuery, context) );
		XmlValue value;
		while( results.next(value) ) {
			// Obtain the value as a string and print it to stdout
			std::cout << value.asString() << std::endl;
		}

		std::cout << results.size() << " objects returned for expression '"
			  << fullQuery << "'\n" << std::endl;

	}
	catch(XmlException &e) {
		std::cerr << "Query " << fullQuery << " failed\n";
		std::cerr << e.what() << "\n";
		exit( -1 );
	}
	catch(std::exception &e) {
		std::cerr << "Query " << fullQuery << " failed\n";
		std::cerr << e.what() << "\n";
		exit( -1 );
	}
}

int main(int argc, char **argv)
{
  std::string path2DbEnv;
  std::string theContainer = "simpleExampleData.dbxml";
  for ( int i=1; i<argc; i++ )
    {
      if ( argv[i][0] == '-' )
        {
          switch(argv[i][1])
            {
            case 'h':
              path2DbEnv = argv[++i];
              break;
            default:
              usage();
            }
        }
    }

  if (! path2DbEnv.length() )
    usage();

  //open a container in the db environment
  DbEnv env(0);
  env.set_cachesize(0, 64 * 1024 * 1024, 1);
  env.open(path2DbEnv.c_str(),
	   DB_INIT_MPOOL|DB_CREATE|DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_TXN, 0);
  XmlManager db(&env);

  XmlTransaction containerTxn = db.createTransaction();
  XmlContainer container = db.openContainer(containerTxn, theContainer);
  containerTxn.commit();

  //perform the queries

  //find all the Vendor documents in the database
  doQuery( db, container, "/vendor" );

  //find all the vendors that are wholesale shops
  doQuery( db, container, "/vendor[@type=\"wholesale\"]");

  //find the product document for "Lemon Grass"
  doQuery( db, container, "/product/item[.=\"Lemon Grass\"]");

  //find all the products where the price is less than or equal to 0.11
  doQuery( db, container, "/product/inventory[price<=0.11]");

  //find all the vegetables where the price is less than or equal to 0.11
  doQuery( db, container, "/product[inventory/price<=0.11 and category=\"vegetables\"]");

  return 0;
}
