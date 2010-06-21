//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: simpleAdd.cpp,v 1.14 2006/10/30 17:45:45 bostic Exp $
//

#include <stdlib.h>

#include "dbxml/DbXml.hpp"

using namespace DbXml;
//some exception handling omitted for clarity

void usage()
{
        std::cerr << "This program adds a few simple XML documents to container 'simpleExampleData.dbxml'\n"
                  << "You are only required to pass this program the path to where you want the\n"
                  << "database environment to be located: \n\n"
                  << "\t-h <dbenv directory>" << std::endl;
        exit( -1 );
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

    // Get an XmlUpdateContext. Useful from a performance perspective.
    XmlUpdateContext updateContext = db.createUpdateContext();

    //myDbEnv and myXmlContainer open with transactions. All subsequent
    //writes to them must also be performed inside a transaction.

    //Get a transaction
    XmlTransaction txn = db.createTransaction();

    std::string document1 = "<aDoc><title>doc1</title><color>green</color></aDoc>";
    std::string document2 = "<aDoc><title>doc2</title><color>yellow</color></aDoc>";

    //Add the documents
    XmlDocument myXMLDoc = db.createDocument();

    //Set the XmlDocument to the relevant string and then put it
    // into the container.
    myXMLDoc.setContent( document1 );
    container.putDocument(txn, myXMLDoc, updateContext, DBXML_GEN_NAME);

    //do it again for the second document
    myXMLDoc.setContent( document2 );
    container.putDocument(txn, myXMLDoc, updateContext, DBXML_GEN_NAME);

    //Normally we would use a try/catch block to trap any exceptions.
    // In the catch, we should call txn->abort() to avoid leaving the
    // database in an indeterminate state in the event of an error.
    // However, this simple example avoids error handling so as to
    // highlite basic concepts, so that step if omitted here as well.

    //Commit the writes. This causes the container write operations
    //  to be saved to the container.
    txn.commit();

    return 0;
}
