//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: exampleLoadContainer.cpp,v 1.20 2006/10/30 17:45:44 bostic Exp $
//

#include "dbxml/DbXml.hpp"

#include <vector>
#include <fstream>
#include <time.h>

using namespace DbXml;
//some exception handling omitted for clarity

void usage()
{
        std::cerr << "This program loads XML data into an identified container and environment.\n"
                  << "Provide the directory where you want to place your database environment, the\n"
                  << "name of the container to use, and the xml files you want inserted into the\n"
                  << "container.\n\n"

                  << "\t-h <dbenv directory> -c <container> -f <filelist> -p <filepath> file1.xml file2.xml file3.xml .... " << std::endl;
        exit( -1 );
}

int main(int argc, char **argv)
{
    //Used for metadata example
    const std::string mdURI = "http://dbxmlExamples/timestamp";
    const std::string mdName = "timeStamp";

    std::string path2DbEnv;
    std::string theContainer;
    std::string filePath;
    std::vector<std::string> files2add;

    for ( int i=1; i<argc; i++ )
    {
        if ( argv[i][0] == '-' )
        {
            switch(argv[i][1])
            {
              case 'h':
                path2DbEnv = argv[++i];
                break;
              case 'c':
                theContainer = argv[++i];
                break;
              case 'p':
                filePath = argv[++i];
                break;
              case 'f':
               {
                    std::ifstream inFile(argv[++i],std::ios::in);
                    if ( !inFile )
                    {
                        std::cerr << "Could not open file " << argv[++i] << ". Giving up." << std::endl;
                        exit( -1 );
                    }
                    while ( !inFile.eof() )
                    {
                        std::string stringBuf;
                        std::getline( inFile, stringBuf );
                        if(!stringBuf.empty())
                        {
                            files2add.push_back( filePath + stringBuf );
                        }
                    }
                }
                break;
              default:
                usage();
            }

        } else {
            files2add.push_back( argv[i] );
        }
     }

    //You must provide a container name, a path to a database environment,
    //and at least one file to add to the container.
    if ( ! path2DbEnv.length() || ! theContainer.length() || files2add.empty() )
    {
        usage();
    }

    // Open a container in the db environment
    DbEnv env(0);
    // Boost some default resource sizes (locks only work
    //   at environment creation time)
    env.set_lk_max_lockers(10000);
    env.set_lk_max_locks(20000);
    env.set_lk_max_objects(20000);
    env.set_cachesize(0, 64 * 1024 * 1024, 1);
    env.open(path2DbEnv.c_str(),
	      DB_INIT_MPOOL|DB_CREATE|DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_TXN, 0);
    XmlManager db(&env);

    XmlTransaction containerTxn = db.createTransaction();
    XmlContainer container = db.openContainer(containerTxn, theContainer, DB_CREATE);
    containerTxn.commit();

    // Get an update context.
    XmlUpdateContext updateContext = db.createUpdateContext();

    //myDbEnv and myXmlContainer open with transactions. All subsequent
    //writes to them must also be performed inside a transaction.

    //Iterate through the list of files to add, read each into a string and
    //  put that string into the now opened container.
    try {
      //Get a transaction
      XmlTransaction txn = db.createTransaction();

        std::vector<std::string>::iterator theFile;
        for ( theFile = files2add.begin(); theFile != files2add.end(); ++theFile )
        {
            /* Load the xml document into a string */
            std::string xmlString;
            /* Open as binary to preserve whitespace */
            std::ifstream inFile( (*theFile).c_str(), std::ios::in | std::ios::binary );
            if ( !inFile )
            {
                std::cerr << "Could not open file '" << *theFile << "'. Giving up." << std::endl;
                throw std::exception();
            }

            while ( !inFile.eof() )
            {
                std::string stringBuf;
                std::getline( inFile, stringBuf );
                // use \xA for the eol given that these are XML files.
                stringBuf += '\xA';
                xmlString += stringBuf;
            }

            inFile.close();

            /* declare an xml document */
            XmlDocument xmlDoc = db.createDocument();

            /* Set the xml document's content to be the xmlString we just obtained. */
            xmlDoc.setContent( xmlString );

            // Get the document name. this strips off any path information.
            std::string tmpName = *theFile;
            std::string theName;
            unsigned int pos = tmpName.rfind("/");
            if (pos == std::string::npos ) //maybe on windows?
            {
                pos = tmpName.rfind("\\");
            }
            if ( pos == std::string::npos ) //giving up
            {
                theName = tmpName;
            } else {
                theName.assign( tmpName, pos+1, tmpName.length() );
            }

            //Set the document name
            xmlDoc.setName( theName );

            //Set sample metadata who's value is the current date and time.
            //the variables used here are defined in gettingStarted.hpp

            //Get the local time
            time_t now = time( 0 );
            char timeString[100];
            strftime(timeString, 100, "%d/%m/%Y:%H:%M:%S", localtime( &now ) );

            //Set the localtime onto the timeStamp metadata attribute
	    xmlDoc.setMetaData( mdURI, mdName, (std::string)timeString );

            /* place that document into the container */
            container.putDocument(txn, xmlDoc, updateContext);

            std::cout << "\tAdded " << *theFile << " to container " << theContainer << "." << std::endl;

        } //end files2add iterator

        txn.commit();
    }
    catch(std::exception &e)
    {
        std::cerr << "Error adding XML data to container " << theContainer << std::endl;
        std::cerr << e.what() << std::endl;

        exit( -1 );
    }

    return 0;
}
