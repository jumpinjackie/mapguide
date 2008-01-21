//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: queryWithContext.cpp,v 1.13.2.1 2007/01/18 20:12:33 gmf Exp $
//

#include <fstream>
#include <cstdio>

#include "dbxml/DbXml.hpp"

using namespace DbXml;

void usage()
{
      std::cerr  <<   "This program illustrates how to query for documents that require namespace\n"
                 <<   "usage in the query.  You should pre-load the container using\n"
                 <<   "loadExamplesData.[sh|cmd] before running this example.  You are only required\n"
                 <<   "to pass this command the path location of the database environment that you\n"
                 <<   "specified when you pre-loaded the examples data:\n\n"

                 <<   "\t-h <dbenv directory>" << std::endl;
        exit( -1 );
}

void doContextQuery( XmlManager &mgr,
		     const std::string &cname, const std::string &query,
                     XmlQueryContext &context )
{
	////////////////////////////////////////////////////////////////////////
	//////  Performs a simple query (with context) against the ///////
	//////  provided container.                                      ///////
	////////////////////////////////////////////////////////////////////////

	///// some defensive code eliminated for clarity //

	// Perform the query. Result type is by default Result Document
	std::string fullQuery = "collection('" + cname + "')" + query;
	try {
		std::cout << "Exercising query '" << fullQuery << "' " << std::endl;
		std::cout << "Return to continue: ";
		getc(stdin);

		XmlResults results( mgr.query(fullQuery, context ) );
		XmlValue value;
		while( results.next(value) )
		{
			// Obtain the value as a string and print it to the console
			std::cout << value.asString() << std::endl;
		}

		std::cout << results.size() << " objects returned for expression '"
			  << fullQuery << "'\n" << std::endl;

	}
	//Catches XmlException
	catch(std::exception &e) {
		std::cerr << "Query " << fullQuery << " failed\n";
		std::cerr << e.what() << "\n";
		exit( -1 );
	}
}

int main(int argc, char **argv)
{
  std::string path2DbEnv;
  std::string theContainer = "namespaceExampleData.dbxml";
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

  //Open a container in the db environment
  DbEnv env(0);
  env.set_cachesize(0, 64 * 1024 * 1024, 1);
  env.open(path2DbEnv.c_str(),
	   DB_INIT_MPOOL|DB_CREATE|DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_TXN, 0);
  XmlManager mgr(&env);

  XmlTransaction containerTxn = mgr.createTransaction();
  XmlContainer container = mgr.openContainer(containerTxn, theContainer);
  containerTxn.commit();

  //Create a context and declare the namespaces
  XmlQueryContext context = mgr.createQueryContext();
  context.setNamespace( "fruits", "http://groceryItem.dbxml/fruits");
  context.setNamespace( "vegetables", "http://groceryItem.dbxml/vegetables");
  context.setNamespace( "desserts", "http://groceryItem.dbxml/desserts");

  //Set a variable
  context.setVariableValue( (std::string)"aDessert", (std::string)"Blueberry Boy Bait");

  //Perform the queries

  //Find all the Vendor documents in the database. Vendor documents do
  // not use namespaces, so this query returns documents.
  doContextQuery( mgr, container.getName(), "/vendor", context );

  //Find the product document for "Lemon Grass". This query returns no documents
  // because a namespace prefix is not identified for the 'item' node.
  doContextQuery( mgr, container.getName(),
		  "/item/product[.=\"Lemon Grass\"]", context);

  //Find the product document for "Lemon Grass" using the namespace prefix 'fruits'. This
  // query successfully returns a document.
  doContextQuery( mgr, container.getName(),
		  "/fruits:item/product[.=\"Lemon Grass\"]", context);

  //Find all the vegetables
  doContextQuery( mgr, container.getName(), "/vegetables:item", context);

  //Find the  dessert called Blueberry Boy Bait
  // Note the use of a variable
  doContextQuery( mgr, container.getName(),
		  "/desserts:item/product[.=$aDessert]", context);

  return 0;
}
