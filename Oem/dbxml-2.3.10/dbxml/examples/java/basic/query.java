//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: query.java,v 1.2 2006/10/30 17:45:46 bostic Exp $
//
// query
//
// A very simple Berkeley DB XML program
// that does a query and handles results.
//
// This program demonstrates:
//  Basic initialization
//  Container creation
//  Document insertion
//  Query creation and execution
//  Use of a variable in a query and context
//  Results handling
//
//  Usage: query
//

package dbxml.basic;

import java.io.*;
import com.sleepycat.dbxml.*;
import com.sleepycat.db.*;

class query {
    // This function is used to ensure that databases are
    // properly closed, even on exceptions
    private static void cleanup(XmlManager mgr, XmlContainer cont) {
	try {
	    if (cont != null)
		cont.delete();
	    if (mgr != null)
		mgr.delete();
	} catch (Exception e) {
	    // ignore exceptions in cleanup
	}
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

	// declare these here for cleanup
	XmlManager mgr = null;
	XmlContainer cont = null;
	try {
	    // All BDB XML programs require an XmlManager instance
	    mgr = new XmlManager();

	    // Because the container will exist on disk, remove it
	    // first if it exists
	    if (mgr.existsContainer(containerName) != 0)
		mgr.removeContainer(containerName);

	    // Now it is safe to create the container
	    cont = mgr.createContainer(containerName);

	    // All Container modification operations need XmlUpdateContext
	    XmlUpdateContext uc = mgr.createUpdateContext();
	    cont.putDocument(docName, content, uc);

	    // Querying requires an XmlQueryContext
	    XmlQueryContext qc = mgr.createQueryContext();
	    
	    // Add a variable to the query context, used by the query
	    qc.setVariableValue("name", new XmlValue("mary"));
	    
	    // Note: these two calls could be replaced by a shortcut
	    // mgr.query(queryString, qc), but under the covers,
	    // this is what is happening.
	    XmlQueryExpression expr = mgr.prepare(queryString, qc);
	    XmlResults res = expr.execute(qc);
	    
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

	    // Explicitly delete other objects to release resources
	    res.delete();
	    expr.delete();
	} catch (XmlException xe) {
	    System.err.println("XmlException during helloWorld: " + 
			       xe.getMessage());
	    throw xe;
	}
	finally {
	    cleanup(mgr, cont);
	}
    }
}

