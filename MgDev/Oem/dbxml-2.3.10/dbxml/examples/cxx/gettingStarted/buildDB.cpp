//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: buildDB.cpp,v 1.15 2006/10/30 17:45:44 bostic Exp $
//

#include <cassert>

#include "myDb.hpp"

using namespace DbXml;
//some exception handling omitted for clarity

void usage()
{
        std::cerr << "This program retrieves node information from a specific set of XML \n"
                     "documents stored in a DBXML container, and then saves relevant sample\n"
                     "data to a Berkeley DB database. Both the DBXML container and the Berkeley\n"
                     "DB database are stored in the same database environment. The value of\n"
                     "the text node found on the XML document is used as the Berkeley DB key for the\n"
                     "sample data stored in the database.\n\n"

                   << "Use the 'retrieveDB' sample program to retrieve the stored data.\n\n"

                   << "Before running this program, run loadExamplesData.[sh|cmd] to pre-populate\n"
                   << "the DBXML container with the appropriate data.\n\n"

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

    //Open a database in the db environment
    myDb openedDatabase( theDB, mgr );

    //Open a container in the db environment
    XmlTransaction containerTxn = mgr.createTransaction();
    XmlContainer container = mgr.openContainer(containerTxn, theContainer);
    containerTxn.commit();

    try
    {
	    //Get a transaction
	    XmlTransaction txn = mgr.createTransaction();

	    XmlQueryContext resultsContext = mgr.createQueryContext();

            //Find all the vendor documents.
	    std::string query = "collection('" + container.getName() + "')/vendor";
            XmlResults results = mgr.query(txn, query, resultsContext);

            //Loop through the result set, setting data in the db for each document
            XmlValue value;
            while( results.next(value) ) {
                //Query on the document for the salesrep's name
                XmlQueryExpression doc_expr = mgr.prepare("fn:string(/vendor/salesrep/name)", resultsContext);
                XmlResults docResult = doc_expr.execute(value, resultsContext);

                //The result set from the document query should be of size 1. If not, the
                //document schema has changed and the query must be modified..
                assert( docResult.size() == 1 );

                //Pull the value out of the document query result set.
                XmlValue docValue;
                docResult.next(docValue);
                std::string theSalesRepKey = docValue.asString();
                std::cout << "theSalesRepKey : " << theSalesRepKey << std::endl;

                //This is the data that we're putting into the database. A real world example would probably
                //include a BLOB, such as a jpeg image of the salereps, a public key, or some other bit of
                //information that doesn't fit into the XML document schema. In our case, we'll just put in
                //a descriptive string so we can see what is going on when we retrieve this data.
                std::string theSalesRepData = "This is the data stored in the database for " + docValue.asString() + ".";
                Dbt theKey( (void *)theSalesRepKey.c_str(), theSalesRepKey.length() + 1 );
                Dbt theData( (void *)theSalesRepData.c_str(), theSalesRepData.length() + 1 );

                openedDatabase.getDatabase().put(txn.getDbTxn(), &theKey, &theData, DB_NOOVERWRITE );
            } //end results loop

            txn.commit();
    }
    //Catch any exceptions thrown by the database operations.
    catch(DbException &e)
    {
        std::cerr << "Error writing to database: "<< std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << "Aborting transaction and exiting."<< std::endl;

        exit( -1 );

    }
    //Catch standard exceptions and XmlException
    catch(std::exception &e)
    {
        std::cerr << "Error writing to database: "<< std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << "Aborting transaction and exiting."<< std::endl;

        exit( -1 );
    }

    return 0;
}
