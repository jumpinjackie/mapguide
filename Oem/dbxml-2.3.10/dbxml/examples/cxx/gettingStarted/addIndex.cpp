//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: addIndex.cpp,v 1.12 2006/10/30 17:45:44 bostic Exp $
//

#include <fstream>
#include <cstdio>
#include <stdlib.h>

#include "dbxml/DbXml.hpp"

using namespace DbXml;

void usage()
{
	std::cerr << "This program adds an index to a DBXML container and then runs queries that use\n"
		  << "the index. You should pre-load the container using loadExamplesData.[sh|cmd]\n"
		  << "before running this example. You are only required to pass this command the\n"
		  << "path of the database environment that you specified when you pre-loaded the\n"
		  << "examples data:\n\n"

		  << "\t-h <dbenv directory>" << std::endl;
        exit( -1 );
}

void addIndex( XmlContainer &container, const std::string &URI,
               const std::string &nodeName, const std::string &indexType,
               XmlTransaction &txn, XmlUpdateContext &uc)
{
	std::cout << "Adding index type: " << indexType << " to node: " << nodeName << std::endl;
	try {
		//Retrieve the XmlIndexSpecification from the container
		XmlIndexSpecification idxSpec = container.getIndexSpecification(txn);

		//Lets see what indexes exist on this container
		std::string uri, name, index;
		int count = 0;
		std::cout << "Before index add." << std::endl;
		while( idxSpec.next(uri, name, index) )
		{
			// Obtain the value as a string and print it to the console
			std::cout << "\tFor node '" << name << "', found index: '"
				  << index << "'." << std::endl;
			count ++;
		}

		std::cout << count << " indexes found." << std::endl;

		//Add the index to the specification.
		//If it already exists, then this does nothing.
		idxSpec.addIndex( URI, nodeName, indexType );

		//Set the specification back to the container
		container.setIndexSpecification( txn, idxSpec, uc );

		//Look at the indexes again to make sure our replacement took.
		count = 0;
		idxSpec.reset();
		std::cout << "After index add." << std::endl;
		while( idxSpec.next(uri, name, index) )
		{
			// Obtain the value as a string and print it to the console
			std::cout << "\tFor node '" << name << "', found index: '" <<
				index << "'." << std::endl;
			count ++;
		}

		std::cout << count << " indexes found." << std::endl;
	}
	//catches XmlException
	catch(std::exception &e) {
		std::cerr << "Add index failed: \n";
		std::cerr << e.what() << "\n";

		exit( -1 );
	}
	std::cout << "Index added successfully." << std::endl;
}

int main(int argc, char **argv)
{
	std::string path2DbEnv;
	std::string theContainer = "namespaceExampleData.dbxml";
	for ( int i=1; i<argc; i++ ) {
		if ( argv[i][0] == '-' ) {
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
	XmlManager db(&env);

	XmlTransaction containerTxn = db.createTransaction();
	XmlContainer container = db.openContainer(containerTxn, theContainer);
	containerTxn.commit();

	//Get a transaction
	XmlTransaction txn = db.createTransaction();
	XmlUpdateContext uc = db.createUpdateContext();

	//Add an string equality index for the "product" element node.
	addIndex( container, "", "product", "node-element-equality-string", txn, uc );
	//Add an edge presence index for the product node
	addIndex( container, "", "product", "edge-element-presence", txn, uc );

	//commit the index adds
	txn.commit();

	return 0;
}

