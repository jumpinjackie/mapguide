//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: helloWorldTxn.java,v 1.2 2006/10/30 17:45:46 bostic Exp $
//
// helloWorldTxn
//
// The simplest possible Berkeley DB XML program
// that does something, but using transactions.
// Transactions require an initialized Berkeley DB environment.
// There is a single, optional argument, which allows the
// environment directory to be specified:
//
//  Usage: java helloWorldTxn [-h environmentDirectory]
//
// This program demonstrates:
//  Berkeley DB transactional environment creation
//  Transactional container creation
//  Transactional document insertion
//  Transactional document retrieval by name
//
// The length of this program is due to the use of configuration
// objects, and the need for explicit deletion/cleanup of Java
// objects in order to release resources.
//
// After running this program, you will notice a number of files
// in the environment directory:
//   __db.* files are the BDB environment, including cache
//   log.* files are BDB log files for transactions
//
// The actual BDB XML container is not present, since it's only
// created in-memory, and will disappear at the end of the program.
//

package dbxml.basic;

import java.io.*;
import com.sleepycat.dbxml.*;
import com.sleepycat.db.*;


class helloWorldTxn
{
    static void usage()
    {
	System.err.println("Usage: java dbxml.basic.helloWorldTxn [-h environmentDirectory]");
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

	// An empty string means an in-memory container, which
	// will not be persisted
	String containerName = "";
	String content = "<hello>Hello World</hello>";
	String docName = "doc";
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

	    // Now, get the document
	    txn = mgr.createTransaction();
	    XmlDocument doc = cont.getDocument(txn, docName);
	    String name = doc.getName();
	    String docContent = doc.getContentAsString();

	    // Explicitly delete the document, or it holds
	    // resources that will cause cleanup problems
	    doc.delete();

	    // commit the transaction.  Note that this is done
	    // after getting the document content.  This is necessary
	    // as getting the content will touch the database.
	    // Also, given that this is a read-only operation, abort()
	    // would be just as appropriate.
	    txn.commit();
	    
	    // print it
	    System.out.println("Document name: " + name + "\nContent: " +
			       docContent);
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

