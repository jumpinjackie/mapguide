//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: helloWorld.java,v 1.2.2.1 2007/01/17 23:26:37 gmf Exp $
//
//
// helloWorld
//
// The simplest possible Berkeley DB XML program
// that does something.
//
//  Usage: java helloWorld
//
// This program demonstrates:
//  Basic initialization
//  Container creation
//  Document insertion
//  Document retrieval by name
//

package dbxml.basic;

import java.io.*;
import com.sleepycat.dbxml.*;
import com.sleepycat.db.*;


class helloWorld
{
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
	
	// An empty string means an in-memory container, which
	// will not be persisted
	String containerName = "";
	String content = "<hello>Hello World</hello>";
	String docName = "doc";
	XmlManager mgr = null;
	XmlContainer cont = null;
	try {
		// All BDB XML programs require an XmlManager instance
		mgr = new XmlManager();
		cont = mgr.createContainer(containerName);

		// All Container modification operations need XmlUpdateContext
		XmlUpdateContext uc = mgr.createUpdateContext();
		cont.putDocument(docName, content, uc);

		// Now, get the document
		XmlDocument doc = cont.getDocument(docName);
		String name = doc.getName();
		String docContent = doc.getContentAsString();
		
		// Explicitly delete the document, or it holds
		// resources that will cause cleanup problems
		doc.delete();

		// print it
		System.out.println("Document name: " + name + "\nContent: " +
				   docContent);

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

