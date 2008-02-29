//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: retrieveDB.cpp,v 1.13 2006/10/30 17:45:45 bostic Exp $
//

#include <cassert>

#include "dbxml/DbXml.hpp"
#include "myDb.hpp"

using namespace DbXml;
//some exception handling omitted for clarity

void usage()
{
        std::cerr << "This program retrieves data from a Berkeley DB data based on information\n"
                  << "retrieved from an XML document. The document is first retrieved from an \n"
                  << "XML container. Both the database and the container are stored in the same\n"
                  << "database environment, and all the necessary queries are performed under the\n"
                  << "protection of a transaction.\n\n"

                  << "Use the 'retrieveDB' sample program to retrieve the stored data.\n\n"

                  << "Before running this program, run loadExamplesData.[sh|cmd] to pre-populate\n"
                  << "the DBXML container with the appropriate data. Then, run buildDB to populate\n"
                  << "the database with the appropriate data.\n\n"

                  << "When you run this program, identify the directory where you told loadExamplesData\n"
                  << "to place the sample data:\n\n"

                  << "\t-h <dbenv directory>" << std::endl;
        exit( -1 );
}

int main(int argc, char **argv)
{
    std::string path2DbEnv;
    std::string theDB = "testBerkeleyDB";
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

    if ( ! path2DbEnv.length() )
    {
        usage();
    }

    //Open a database environment
    DbEnv env(0);
    env.set_cachesize(0, 64 * 1024 * 1024, 1);
    env.open(path2DbEnv.c_str(),
	     DB_INIT_MPOOL|DB_CREATE|DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_TXN, 0);
    XmlManager mgr(&env);

    //Open a database in the environment
    myDb openedDatabase( theDB, mgr );

    //Open a container in the environment
    XmlTransaction containerTxn = mgr.createTransaction();
    XmlContainer container = mgr.openContainer(containerTxn, theContainer);
    containerTxn.commit();

    try
    {
      //Get a transaction
      XmlTransaction txn = mgr.createTransaction();

            XmlQueryContext resultsContext = mgr.createQueryContext();

            //Find all the vendor documents.
	    std::string fullQuery = "collection('" + container.getName() + "')/vendor";
            XmlResults results( mgr.query(txn, fullQuery, resultsContext) );

            resultsContext.setReturnType( XmlQueryContext::DeadValues );

            //Loop through the result set, retrieving the data we are interested in for each document.
            XmlValue value;
            while( results.next(value) )
            {
                //Query the retrieved document for the salesrep's name
                XmlQueryExpression doc_expr = mgr.prepare("fn:string(salesrep/name)", resultsContext);
                XmlResults docResult = doc_expr.execute(value, resultsContext);

                //The result set from the document query should be of size 1. If not, the
                //document schema has changed and the query must be modified..
                assert( docResult.size() == 1 );

                //Pull the value out of the document query result set.
                XmlValue docValue;
                docResult.next(docValue);
                std::string theSalesRepKey = docValue.asString();

                //Reconstruct the key used to store the information relevant to this document
                Dbt theKey( (void *)theSalesRepKey.c_str(), theSalesRepKey.length() + 1 );

                //This next structure is used to hold the data that is
                // retrieved from the db.
                Dbt theData;

                //Get the data and report the results
                openedDatabase.getDatabase().get(txn.getDbTxn(), &theKey, &theData, 0 );
                std::cout << "For key: '" << (char *)theKey.get_data() << "', retrieved:" << std::endl;
                std::cout << "\t" << (char *)theData.get_data() << std::endl;
            }

            txn.commit();
    }
    catch(DbException &e)
    {
        std::cerr << "Error writing to database: "<< std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << "Aborting transaction and exiting."<< std::endl;

        exit( -1 );
    }
    //Catches XmlException
    catch(std::exception &e)
    {
        std::cerr << "Error writing to database: "<< std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << "Aborting transaction and exiting."<< std::endl;

        exit( -1 );
    }

    return 0;
}
