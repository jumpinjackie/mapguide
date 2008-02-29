//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: queryForDocumentValue.cpp,v 1.14 2006/10/30 17:45:45 bostic Exp $
//

#include <fstream>
#include <cstdio>
#include <cassert>

#include "dbxml/DbXml.hpp"

using namespace DbXml;

void usage()
{
      std::cerr   <<  "This program performs various XPath queries against a DBXML container and then\n"
                  <<  "retrieves information of interest from the retrieved document(s).  You should\n"
                  <<  "pre-load the container using loadExamplesData.[sh|cmd] before running this\n"
                  <<  "example.  You are only required to pass this command the path location of the\n"
                  <<  "database environment that you specified when you pre-loaded the examples data:\n\n"

                  <<  "\t-h <dbenv directory>" << std::endl;
        exit( -1 );
}

std::string getValue( XmlManager &mgr, const XmlDocument &document,
		      const std::string &XPath, XmlQueryContext &context )
{
    /////////////////////////////////////////////////////////////////////////////////
    ///////////    Return specific information from a document. /////////////////////
    ///////////   !!!!!! Assumes a result set of size 1 !!!!!!! /////////////////////
    /////////////////////////////////////////////////////////////////////////////////

    // Exception handling omitted....

    //Perform the query
    XmlQueryExpression doc_expr = mgr.prepare(XPath, context);
    XmlResults result = doc_expr.execute(XmlValue(document), context);

    //We require a result set size of 1.
    assert( result.size() == 1 );

    //Get the value. If we allowed the result set to be larger than size 1,
    //we would have to loop through the results, processing each as is
    //required by our application.
    XmlValue value;
    result.next(value);

    return value.asString();

}

void getDetails( XmlManager &mgr, const XmlContainer &container, const std::string &query, XmlQueryContext &context )
{
	////////////////////////////////////////////////////////////////////////
	//////  Performs an query (in context) against the         ///////
	//////  provided container.                                      ///////
	////////////////////////////////////////////////////////////////////////

	///// some defensive code eliminated for clarity //

	// Perform the query. Result type is by default Result Document
   	std::string fullQuery = "collection('" + container.getName() + "')" + query;
	try {
		std::cout << "Exercising query '" << fullQuery << "' " << std::endl;
		std::cout << "Return to continue: ";
		getc(stdin);

		XmlResults results( mgr.query(fullQuery, context ) );
		XmlValue value;
		std::cout << "\n\tProduct : Price : Inventory Level\n";
		while( results.next(value) ) {
			/// Retrieve the value as a document
			XmlDocument theDocument = value.asDocument();

			/// Obtain information of interest from the document. Note that the
			//  wildcard in the query expression allows us to not worry about what
			//  namespace this document uses.
			std::string item = getValue( mgr, theDocument, "fn:string(/*/product)", context);
			std::string price = getValue( mgr, theDocument, "fn:string(/*/inventory/price)", context);
			std::string inventory = getValue( mgr, theDocument, "fn:string(/*/inventory/inventory)", context);

			std::cout << "\t" << item << " : " << price << " : " << inventory << std::endl;

		}
		std::cout << "\n";
		std::cout << results.size() << " objects returned for expression '" << fullQuery << "'\n" << std::endl;
	}
	//Catches XmlException
	catch(std::exception &e) {
		std::cerr << "Query " << fullQuery << " failed\n";
		std::cerr << e.what() << "\n";
		exit(-1);
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

    //get details on Zulu Nuts
    getDetails( mgr, container, "/fruits:item[fn:string(product) = 'Zulu Nut']", context);

    //get details on all fruits that start with 'A'
    getDetails( mgr, container, "/vegetables:item[starts-with(fn:string(product),'A')]", context);

    return 0;
}
