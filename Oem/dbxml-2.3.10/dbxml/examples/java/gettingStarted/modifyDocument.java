//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: modifyDocument.java,v 1.14.2.2 2007/01/30 15:34:40 gmf Exp $
//

package dbxml.gettingStarted;

import java.io.*;

import com.sleepycat.dbxml.*;

class modifyDocument
{

    private static String theContainer = "namespaceExampleData.dbxml";
    private static String collection = "collection('namespaceExampleData.dbxml')";
    private static void usage() {
	String usageMessage = "\nThis program modifies documents found in a DBXML container.\n";
	usageMessage += "You should run exampleLoadContainer before running this example.\n";
	usageMessage += "You are only required to pass this command the path location of the\n";
	usageMessage += "database environment that you specified when you loaded the examples data:\n\n";
	usageMessage += "\t-h <dbenv directory>\n";
	usageMessage += "For example:\n";
	usageMessage += "\tjava com.sleepycat.dbxml.examples.gettingStarted.modifyDocument -h examplesEnvironment\n";
	System.out.println(usageMessage);
	System.exit( -1 );
    }

    // display documents matching the query
    private static void dumpDocuments(XmlResults results)
	throws Throwable {
	System.out.println("Found " + results.size() + " documents ");

	results.reset();
	XmlValue value = results.next();
	while(value != null) {
            XmlDocument document = value.asDocument();
            String docString =document.getContentAsString();
            System.out.println("Document content: \n" + docString);
	    document.delete();
	    value.delete();
            value = results.next();
	}
    }

    //Utility function to clean up objects, exceptions or not,
    // containers and environments must be closed.
    private static void cleanup(myDbEnv env, XmlContainer openedContainer) {
	try {
	    if (openedContainer != null)
		openedContainer.delete();
	    if (env != null)
		env.cleanup();
	} catch (Exception e) {
	    // ignore exceptions on close
	}
    }

    public static void main(String args[])
	throws Throwable {
	File path2DbEnv = null;
	for(int i = 0; i < args.length; ++i) {
            if (args[i].startsWith("-")) {
		switch(args[i].charAt(1)) {
		case 'h':
		    path2DbEnv = new File( args[++i] );
		    break;
		default:
                    usage();
		}
            }
	}

	if (path2DbEnv == null || ! path2DbEnv.isDirectory()) {
            usage();
	}

	myDbEnv env = null;
	XmlContainer openedContainer = null;
	XmlTransaction txn = null;
	try {
	    // Open an environment
	    env = new myDbEnv(path2DbEnv);
	    XmlManager theMgr = env.getManager();

	    // open a transactional container
	    XmlContainerConfig config = new XmlContainerConfig();
	    config.setTransactional(true);
	    openedContainer =
		theMgr.openContainer(theContainer, config);

	    // Start atransaction
	    txn = theMgr.createTransaction();

	    //Create a context and declare the namespaces
	    XmlQueryContext context =
		theMgr.createQueryContext(XmlQueryContext.LiveValues,
					  XmlQueryContext.Eager);

	    context.setNamespace( "fruits", "http://groceryItem.dbxml/fruits");
	    context.setNamespace( "vegetables",
				  "http://groceryItem.dbxml/vegetables");
	    context.setNamespace( "desserts", "http://groceryItem.dbxml/desserts");

	    String query = collection +
		"/fruits:item/product[. = 'Zapote Blanco']";
	    XmlQueryExpression expression = theMgr.prepare(txn,
							   query, context);

	    System.out.println("Updating document for the expression: '" +
			       query + "' ");
	    System.out.println("Return to continue: ");
	    System.in.read();

	    //Print the document(s) to be updated -- those that describe
	    // Zapote Blanco (a fruit). The document strings are only being
	    // printed to show before/after results.
	    // Most modification programs would not perform the additional queries.
	    XmlResults results = expression.execute(txn, context, null);
	    dumpDocuments(results);
	    results.reset();
	    System.out.println("About to update the document(s) above.\n" +
			       "Look for a new element after the target " +
			       "element, named 'description' ...\n" +
			       "Return to continue: ");
	    System.in.read();

	    //The modification is a new element in the target node, called
	    // "descripton, which goes immediately after the <product> element.
	    // if this program is run more than once, and committed, additional
	    // identical elements are added.  It is easy to modify this program
	    // to change the modification.
	    XmlModify modify = theMgr.createModify();
	    modify.addInsertAfterStep(theMgr.prepare(txn, ".", context),
				      XmlModify.Element,
				      "description", "very nice");

	    XmlUpdateContext uc = theMgr.createUpdateContext();
	    results.reset();
	    long numMod = modify.execute(txn, results, context, uc);

	    System.out.println("Peformed " + numMod + " modification operations");
	    dumpDocuments(results);

	    System.out.println("If committed, this program will add a new element each time it is run.\n" + "Press 'c' to commit this change:");
	    int c = System.in.read();
	    if (c == 'c' || c =='C')
		txn.commit();
	    else
		txn.abort();
	    txn.delete();
	    expression.delete();
	    modify.delete();
	    results.delete();
	    context.delete();
	    uc.delete();

	} catch(Exception e) {
	    System.err.println("Error performing document modification against "
			       + theContainer);
	    System.err.println("   Message: " + e.getMessage());
	    // In the event of an error, we abort the operation
	    // The database is left in the same state as it was in before
	    // we started this operation.
	    if ( txn != null ) {
		txn.abort();
		txn.delete();
	    }
	    throw e;
	}
	finally {
	    cleanup(env, openedContainer);
	}
    } //End main
}
