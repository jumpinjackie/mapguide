//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: queryForMetaData.cpp,v 1.13 2006/10/30 17:45:45 bostic Exp $
//

#include <fstream>
#include <cstdio>
#include <stdlib.h>

#include "dbxml/DbXml.hpp"

using namespace DbXml;

void usage()
{
      std::cerr   << "This program retrieves DBXML documents and then retrieves the day and time the\n"
                  << "document was stored in the container. The date/time information was set as\n"
                  << "metadata information when the document was loaded. See the\n"
                  << "exampleLoadContainer.cpp example for how this metadata is set.\n\n"

                  << "Before running this example, use loadExamplesData.[sh|cmd] to pre-load the\n"
                  << "appropriate container with documents and metadata. \n\n"

                  << "You are only required to pass this command the path location of the database\n"
                  << "environment that you specified when you pre-loaded the examples data:\n\n"

                  << "\t-h <dbenv directory>" << std::endl;
        exit( -1 );
}

void showTimeStamp( XmlManager &mgr,
		    const XmlContainer &container,
		    const std::string &query, XmlQueryContext &context )
{
    ////////////////////////////////////////////////////////////////////////
    //////  Performs a simple query (with context) against the ///////
    //////  provided container. The timestamp metadata attribute     ///////
    //////  value is then displayed.                                 ///////
    ////////////////////////////////////////////////////////////////////////

    ///// some defensive code eliminated for clarity //

    //Used for metadata query
    const std::string mdURI = "http://dbxmlExamples/timestamp";
    const std::string mdPrefix = "time";
    const std::string mdName = "timeStamp";

    // Perform the query. Result type is by default Result Document
    std::string fullQuery = "collection('" + container.getName() + "')" + query;
    try {
        std::cout << "Exercising query '" << fullQuery << "' " << std::endl;
        std::cout << "Return to continue: ";
        getc(stdin);

        XmlResults results( mgr.query(fullQuery, context ) );
        XmlValue value;
        while( results.next(value) )
        {
            XmlDocument theDocument = value.asDocument();

            //Get this document's name
            const std::string docName( theDocument.getName() );

            //Get the timestamp on the document (stored as metadata)
            // and print it to the console.
            XmlValue metaValue;
            theDocument.getMetaData( mdURI, mdName, metaValue );
            std::cout << "Document '" << docName << "' stored on " << metaValue.asString() << std::endl;
        }

        std::cout << results.size() << " objects returned for expression '" << fullQuery << "'\n" << std::endl;

    }
    //Catches XmlException
    catch(std::exception &e)
    {
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

    //Find all the vegetables
    showTimeStamp( mgr, container, "/vegetables:item", context);

    return 0;
}
