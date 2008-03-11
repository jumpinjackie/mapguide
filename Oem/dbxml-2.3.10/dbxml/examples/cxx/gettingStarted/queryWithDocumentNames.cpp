//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: queryWithDocumentNames.cpp,v 1.15 2006/10/30 17:45:45 bostic Exp $
//

#include <fstream>
#include <cstdio>
#include <stdlib.h>

#include "dbxml/DbXml.hpp"

using namespace DbXml;

void usage()
{
       std::cerr <<  "This program retrieves DBXML documents using their document names. You should\n"
                 <<  "pre-load the container using loadExamplesData.[sh|cmd] before running this\n"
                 <<  "example.  You are only required to pass this command the path location of the\n"
                 <<  "database environment that you specified when you pre-loaded the examples\n"
                 <<  "data:\n\n"

                 <<  "\t-h <dbenv directory>" << std::endl;
        exit( -1 );
}

void doContextQuery( XmlManager &mgr, const std::string cname,
		     const std::string &query, XmlQueryContext &context )
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
		std::cout << "\n";

		XmlResults results( mgr.query(fullQuery, context ) );
		XmlValue value;
		while(results.next(value)) {
			// Get the document's name and print it to the console
			XmlDocument theDocument = value.asDocument();
			std::cout << "Document name: " << theDocument.getName() << std::endl;
			std::cout << value.asString() << std::endl;
		}

		std::cout << results.size() << " objects returned for expression '" << fullQuery << "'\n" << std::endl;

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

    //open a container in the db environment
    DbEnv env(0);
    env.set_cachesize(0, 64 * 1024 * 1024, 1);
    env.open(path2DbEnv.c_str(),
	     DB_INIT_MPOOL|DB_CREATE|DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_TXN, 0);
    XmlManager mgr(&env);

    XmlTransaction containerTxn = mgr.createTransaction();
    XmlContainer container = mgr.openContainer(containerTxn, theContainer);
    containerTxn.commit();

    //create a context and declare the namespaces
    XmlQueryContext context = mgr.createQueryContext();
    context.setNamespace( "fruits", "http://groceryItem.dbxml/fruits");
    context.setNamespace( "vegetables", "http://groceryItem.dbxml/vegetables");
    context.setNamespace( "desserts", "http://groceryItem.dbxml/desserts");

    //Query for documents by their document names.
    doContextQuery( mgr, container.getName(),
		    "/*[dbxml:metadata('dbxml:name')='ZuluNut.xml']", context );
    doContextQuery( mgr, container.getName(),
		    "/*[dbxml:metadata('dbxml:name')='TrifleOrange.xml']", context );
    doContextQuery( mgr, container.getName(),
		    "/*[dbxml:metadata('dbxml:name')='TriCountyProduce.xml']", context );

    return 0;
}
