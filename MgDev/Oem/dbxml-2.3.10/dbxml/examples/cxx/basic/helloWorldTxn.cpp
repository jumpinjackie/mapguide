/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2004,2006 Oracle.  All rights reserved.
 *
 * $Id: helloWorldTxn.cpp,v 1.2 2006/10/30 17:45:44 bostic Exp $
 *
 *******
 *
 * helloWorldTxn
 *
 * The simplest possible Berkeley DB XML program
 * that does something, but using transactions.
 * Transactions require an initialized Berkeley DB environment.
 * There is a single, optional argument, which allows the
 * environment directory to be specified:
 *
 *  Usage: helloWorldTxn [-h environmentDirectory]
 *
 * This program demonstrates:
 *  Berkeley DB transactional environment creation
 *  Transactional container creation
 *  Transactional document insertion
 *  Transactional document retrieval by name
 *
 * After running this program, you will notice a number of files
 * in the environment directory:
 *   __db.* files are the BDB environment, including cache
 *   log.* files are BDB log files for transactions
 *
 * The actual BDB XML container is not present, since it's only
 * created in-memory, and will disappear at the end of the program.
 *
 */

#include <stdlib.h>

#include <dbxml/DbXml.hpp>

using namespace DbXml;

static void usage(const char *progname)
{
	std::cerr << "Usage:" << progname << " [-h environmentDirectory]"
		  << std::endl;
	exit(1);
}

int
main(int argc, char **argv)
{
	// An empty string means an in-memory container, which
	// will not be persisted
	std::string containerName = "";
	std::string content = "<hello>Hello World</hello>";
	std::string docName = "doc";
	std::string environmentDir = ".";

	// Berkeley DB environment flags
	u_int32_t envFlags = DB_RECOVER|DB_CREATE|DB_INIT_MPOOL|
		DB_INIT_LOCK|DB_INIT_TXN|DB_INIT_LOG;
	// Berkeley DB cache size (25 MB).  The default is quite small
	u_int32_t envCacheSize = 25*1024*1024;

	// argument parsing should really use getopt(), but
	// avoid it for platform-independence
	if (argc == 3) {
		if (std::string(argv[1]) != std::string("-h"))
			usage(argv[0]);
		environmentDir = argv[2];
	} else if (argc != 1)
		usage(argv[0]);
	
	try {

		// Create and open a Berkeley DB Transactional Environment.
		DbEnv dbEnv(0);
		dbEnv.set_cachesize(0, envCacheSize, 1);
		dbEnv.open(environmentDir.c_str(), envFlags, 0);


		// All BDB XML programs require an XmlManager instance.
		// Create it from the DbEnv
		XmlManager mgr(&dbEnv);


		// Create a container that is transactional.  Specify
		// that it is also a Node Storage container, with nodes
		// indexed
		XmlContainer cont = mgr.createContainer(
			containerName,
			DBXML_TRANSACTIONAL|DBXML_INDEX_NODES,
			XmlContainer::NodeContainer);


		// All Container modification operations need XmlUpdateContext
		XmlUpdateContext uc = mgr.createUpdateContext();

		// Perform the putDocument in a transaction, created
		// from the XmlManager
		XmlTransaction txn = mgr.createTransaction();

		cont.putDocument(txn, docName, content, uc);

		// commit the Transaction
		txn.commit();
		

		// Now, get the document, in a new transaction.
		// It is OK to reuse the XmlTransaction object -- it
		// is just a handle.
		txn = mgr.createTransaction();
		XmlDocument doc = cont.getDocument(txn, docName);
		std::string docContent;
		std::string name = doc.getName();
		docContent = doc.getContent(docContent);

		// commit the transaction.  Note that this is done
		// after getting the document content.  This is necessary
		// as getting the content will touch the database.
		// Also, given that this is a read-only operation, abort()
		// would be just as appropriate.

		txn.commit();

		std::cout << "Document name: " << name << "\nContent: " <<
			docContent << std::endl;

		// In C++, resources are released as objects go out
		// of scope.
		
	}
	catch (XmlException &xe) {
		std::cout << "XmlException: " << xe.what() << std::endl;
	}

	// Creation/open of the DbEnv object may throw DbException
	catch (DbException &de) {
		std::cout << "DbException: " << de.what() << std::endl;
	}
	return 0;
}

