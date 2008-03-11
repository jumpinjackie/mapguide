//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: deleteIndex.cpp,v 1.11 2006/10/30 17:45:44 bostic Exp $
//

#include <fstream>
#include <cstdio>
#include <stdlib.h>

#include "dbxml/DbXml.hpp"

using namespace DbXml;

void usage()
{
      std::cerr   << "This program deletes an index from a document node in a DBXML container.  You\n"
                  << "should pre-load the container using loadExamplesData.[sh|cmd] before running\n"
                  << "this example. You are only required to pass this command the path location of\n"
                  << "the database environment that you specified when you pre-loaded the examples\n"
                  << "data:\n\n"

                  <<  "\t-h <dbenv directory>\n"

                  <<    "Also, for best results run addIndex before running this program.\n" << std::endl;
        exit( -1 );
}

void deleteIndex( XmlContainer &container, const std::string &URI,
                  const std::string &nodeName, const std::string &indexType,
                  XmlTransaction &txn, XmlUpdateContext &uc )
{
    std::cout << "Deleting index type: '" << indexType << ""
              << " from node: '" << nodeName << "'." << std::endl;
    try
    {
        //Retrieve the XmlIndexSpecification from the container
        XmlIndexSpecification idxSpec = container.getIndexSpecification( txn );

        std::cout << "Before the delete, the following indexes are maintained for the container:" << std::endl;
        std::string uri, name, index;
        while( idxSpec.next(uri, name, index) )
        {
            // Obtain the value as a string and print it to the console
            std::cout << "\tFor node '" << name << "', found index: '" << index << "'." << std::endl;
        }
        std::cout << "\n" << std::endl;

        //Delete the indexes from the specification.
        idxSpec.deleteIndex( URI, nodeName, indexType );

        //Set the specification back to the container
        container.setIndexSpecification( txn, idxSpec, uc );

        //Show the remaining indexes in the container, if any.
        std::cout << "After the delete, the following indexes exist for the container:" << std::endl;
        idxSpec.reset();
        while( idxSpec.next(uri, name, index) )
        {
            // Obtain the value as a string and print it to the console
            std::cout << "\tFor node '" << name << "', found index: '" << index << "'." << std::endl;
        }
        std::cout << "\n" << std::endl;
    }
    //Catches XmlException.
    catch(std::exception &e)
    {
        std::cerr << "Index delete failed: \n";
        std::cerr << e.what() << "\n";

        exit( -1 );
    }
    std::cout << "Index deleted successfully.\n" << std::endl;

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

    //Open the environment and the container
    DbEnv env(0);
    env.set_cachesize(0, 64 * 1024 * 1024, 1);
    env.open(path2DbEnv.c_str(),
	      DB_INIT_MPOOL|DB_CREATE|DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_TXN, 0);
    XmlManager db(&env);

    XmlTransaction containerTxn = db.createTransaction();
    XmlContainer container = db.openContainer(containerTxn, theContainer);
    containerTxn.commit();

    //myDbEnv and myXmlContainer open with transactions. All subsequent
    //writes to them must also be performed inside a transaction.

    //Get a transaction
    XmlTransaction txn = db.createTransaction();
    XmlUpdateContext uc = db.createUpdateContext();

    //add an string equality index for the "product" element node.
    deleteIndex( container, "", "product", "node-element-equality-string", txn, uc );

    //Do these deletes in two different transactions
    // for no particular reason.
    txn.commit();
    txn = db.createTransaction();

    //add an edge presence index for the product node
    deleteIndex( container, "", "product", "edge-element-presence-none", txn, uc );

    txn.commit();

    return 0;
}

