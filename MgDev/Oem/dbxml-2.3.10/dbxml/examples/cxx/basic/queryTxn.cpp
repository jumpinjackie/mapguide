/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2004,2006 Oracle.  All rights reserved.
 *
 * $Id: queryTxn.cpp,v 1.2 2006/10/30 17:45:44 bostic Exp $
 *
 *******
 *
 * queryTxn
 *
 * A very simple Berkeley DB XML program
 * that does a query and handles results, using
 * transactions.
 *
 * This program demonstrates:
 *  Transactional initialization
 *  Transactional container creation
 *  Transactional document insertion
 *  Transactional query creation and execution
 *  Use of a variable in a query and context
 *  Transactional results handling
 *
 *  Usage: queryTxn [-h environmentDirectory]
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
	// This program uses a named container, which will apear
	// on disk
	std::string containerName = "people.dbxml";
	std::string content = "<people><person><name>joe</name></person><person><name>mary</name></person></people>";
	std::string docName = "people";
	// Note that the query uses a variable, which must be set
	// in the query context
	std::string queryString =
		"collection('people.dbxml')/people/person[name=$name]";
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

		// Because the container will exist on disk, remove it
		// first if it exists
		if (mgr.existsContainer(containerName))
			mgr.removeContainer(containerName);
		
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

		// Querying requires an XmlQueryContext
		XmlQueryContext qc = mgr.createQueryContext();

		// Add a variable to the query context, used by the query
		qc.setVariableValue("name", "mary");

		// Create a new transaction for the query
		txn = mgr.createTransaction();
		
		// Note the passing of txn to both methods
		XmlQueryExpression expr = mgr.prepare(txn, queryString, qc);
		XmlResults res = expr.execute(txn, qc);

		// Note use of XmlQueryExpression::getQuery() and
		// XmlResults::size()
		std::cout << "The query, '" << expr.getQuery() << "' returned " <<
		res.size() << " result(s)" << std::endl;

		// Process results -- just print them
		XmlValue value;
		std::cout << "Result: " << std::endl;
		while (res.next(value)) {
			std::cout << "\t" << value.asString() << std::endl;
		}

		// done with the transaction
		txn.commit();
		
		// In C++, resources are released as objects go out
		// of scope.

		
	} catch (XmlException &xe) {
		std::cout << "XmlException: " << xe.what() << std::endl;
	}

	// Creation/open of the DbEnv object may throw DbException
	catch (DbException &de) {
		std::cout << "DbException: " << de.what() << std::endl;
	}

	return 0;
}

