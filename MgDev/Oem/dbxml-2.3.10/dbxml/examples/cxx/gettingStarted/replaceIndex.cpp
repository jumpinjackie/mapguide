//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: replaceIndex.cpp,v 1.11 2006/10/30 17:45:45 bostic Exp $
//

#include <fstream>
#include <cstdio>
#include <stdlib.h>

#include "dbxml/DbXml.hpp"

using namespace DbXml;

void usage()
{
     std::cerr <<    "This program replaces the index for a document node.  You should pre-load the\n"
               <<    "container using loadExamplesData.[sh|cmd]  before running this example. You\n"
               <<    "are only required to pass this command the path on of the database environment\n"
               <<    "that you specified when you pre-loaded the examples data:\n\n"

               <<     "\t-h <dbenv directory>\n\n"

               <<    "Also, for best results run addIndex before running this program.\n" << std::endl;
        exit( -1 );
}

void replaceIndex( XmlContainer &container, const std::string &URI,
                   const std::string &nodeName, const std::string &indexType,
                   XmlTransaction &txn, XmlUpdateContext &uc)
{
    std::cout << "Replacing index on node: " << nodeName << std::endl;
    try
    {
        //Retrieve the XmlIndexSpecification from the container
        XmlIndexSpecification idxSpec = container.getIndexSpecification( txn );

        //Lets see what indexes exist on this container
        std::string uri, name, index;
        int count = 0;
        std::cout << "Before index add." << std::endl;
        while( idxSpec.next(uri, name, index) )
        {
            // Obtain the value as a string and print it to the console
            std::cout << "\tFor node '" << name << "', found index: '" << index << "'." << std::endl;
            count ++;
        }

        std::cout << count << " indexes found." << std::endl;

        //Replace the indexes for the specified node
        idxSpec.replaceIndex( URI, nodeName, indexType );

        //Set the specification back to the container
        container.setIndexSpecification( txn, idxSpec, uc );

        //Look at the indexes again to make sure our replacement took.
        count = 0;
        idxSpec.reset();
        std::cout << "After index add." << std::endl;
        while( idxSpec.next(uri, name, index) )
        {
            // Obtain the value as a string and print it to the console
            std::cout << "\tFor node '" << name << "', found index: '" << index << "'." << std::endl;
            count ++;
        }

        std::cout << count << " indexes found." << std::endl;

    }
    //Catches XmlException
    catch(std::exception &e)
    {
        std::cerr << "Index replace failed: \n";
        std::cerr << e.what() << "\n";

        exit( -1 );
    }
    std::cout << "Index replaced successfully." << std::endl;

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

    //replace the index on the "product" node.
    replaceIndex( container, "", "product",
                  "node-attribute-substring-string node-element-equality-string",
                  txn, uc );

    txn.commit();

    return 0;
}

