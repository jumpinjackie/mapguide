//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: queryTxn.java,v 1.2 2006/10/30 17:45:46 bostic Exp $
//
// queryTxn
//
// A very simple Berkeley DB XML program
// that does a query and handles results, using
// transactions.
//
// This program demonstrates:
//  Transactional initialization
//  Transactional container creation
//  Transactional document insertion
//  Transactional query creation and execution
//  Use of a variable in a query and context
//  Transactional results handling
//
//  Usage: queryTxn [-h environmentDirectory]
//
//
package dbxml.basic;

import java.io.*;
import com.sleepycat.dbxml.*;
import com.sleepycat.db.*;


class queryTxn
{
    static void usage()
    {
	System.err.println("Usage: java dbxml.basic.queryTxn [-h environmentDirectory]");
	System.exit(-1);
    }

    // This function is used to ensure that databases are
    // properly closed, even on exceptions
    private static void cleanup(Environment env,
				XmlManager mgr, 
				XmlContainer cont) {
	try {
	    if (cont != null)
		cont.delete();
	    if (mgr != null)
		mgr.delete();
	    if (env != null)
		env.close();
	} catch (Exception e) {
	    // ignore exceptions in cleanup
	}
    }

    private static Environment createEnvironment(String home)
	throws Throwable {
	EnvironmentConfig config = new EnvironmentConfig();
	config.setTransactional(true);
	config.setAllowCreate(true);
	config.setRunRecovery(true);
	config.setInitializeCache(true);
	config.setCacheSize(25 * 1024 * 1024); // 25MB cache
	config.setInitializeLocking(true);
	config.setInitializeLogging(true);
	config.setErrorStream(System.err);
	File f = new File(home);
	return new Environment(f, config);
    }

    public static void main(String args[])
	throws Throwable {

	// This program uses a named container, which will apear
	// on disk
	String containerName = "people.dbxml";
	String content = "<people><person><name>joe</name></person><person><name>mary</name></person></people>";
	String docName = "people";
	// Note that the query uses a variable, which must be set
	// in the query context
	String queryString =
		"collection('people.dbxml')/people/person[name=$name]";
	String environmentDir = ".";

	if (args.length == 2) {
	    environmentDir = args[1];
	} else if (args.length != 0)
	    usage();

	XmlManager mgr = null;
	XmlContainer cont = null;
	Environment env = null;
	try {
	    // Create and open a Berkeley DB Transactional Environment.
	    env = createEnvironment(environmentDir);

	    // All BDB XML programs require an XmlManager instance.
	    // Create it from the DB Environment, but do not adopt the
	    // Environment
	    XmlManagerConfig mconfig = new XmlManagerConfig();
	    mconfig.setAllowExternalAccess(true);
	    mgr = new XmlManager(env, mconfig);

	    
	    // Because the container will exist on disk, remove it
	    // first if it exists
	    if (mgr.existsContainer(containerName) != 0)
		mgr.removeContainer(containerName);
		
	    // Create a container that is transactional.  Specify
	    // that it is also a Node Storage container, with nodes
	    // indexed
	    XmlContainerConfig cconfig = new XmlContainerConfig();
	    cconfig.setNodeContainer(true);
	    cconfig.setIndexNodes(true);
	    cconfig.setTransactional(true);
	    cont = mgr.createContainer(containerName, cconfig);

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
	    qc.setVariableValue("name", new XmlValue("mary"));
	    
	    // Create a new transaction for the query
	    txn = mgr.createTransaction();
	    
	    // Note the passing of txn to both methods
	    XmlQueryExpression expr = mgr.prepare(txn, queryString, qc);
	    XmlResults res = expr.execute(txn, qc);
	    
	    // Note use of XmlQueryExpression::getQuery() and
	    // XmlResults::size()
	    System.out.println("The query, '" + expr.getQuery() +
			       "'\n\t returned " + res.size() + " result(s)");
	    
	    // Process results -- just print them
	    XmlValue value = new XmlValue();
	    System.out.print("Result: ");
	    while ((value = res.next()) != null) {
		System.out.println("\t" + value.asString());
		value.delete();
	    }
	    
	    // done with the transaction
	    txn.commit();
	    
	    // explicitly delete objects to release resources
	    res.delete();
	    expr.delete();
	    qc.delete();
	} 
	catch (XmlException xe) {
	    System.err.println("XmlException during helloWorldTxn: " + 
			       xe.getMessage());
	    throw xe;
	}
	catch (DatabaseException de) {
	    System.err.println("DatabaseException during helloWorldTxn: " + 
			       de.getMessage());
	    throw de;
	}
	finally {
	    cleanup(env, mgr, cont);
	}
    }
}

