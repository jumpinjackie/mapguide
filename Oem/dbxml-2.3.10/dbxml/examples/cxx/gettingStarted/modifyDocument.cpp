//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: modifyDocument.cpp,v 1.10.2.1 2007/01/18 20:12:33 gmf Exp $
//

#include <fstream>
#include <cstdio>
#include <cassert>

#include "dbxml/DbXml.hpp"

using namespace DbXml;

void usage()
{
	std::cerr  <<  "This program modifies documents found in a DBXML container. You should\n"
		   <<  "pre-load the container using loadExamplesData.[sh|cmd] before running this\n"
		   <<  "example. You are only required to pass this command the path location of the\n"
		   <<  "database environment that you specified when you pre-loaded the examples\n"
		   <<  "data:\n\n"

		   <<   "\t-h <dbenv directory>" << std::endl;
        exit( -1 );
}

// print out the string values of matching documents
void dumpDocuments(XmlResults &results)
{
	////////////////////////////////////////////////////////////////////////
	//////  Updates a document stored in a DBXML container.          ///////
	////////////////////////////////////////////////////////////////////////

	///// some defensive code eliminated for clarity //

	try {
    results.reset();
		std::cout << "Found " << results.size() << " documents. " << std::endl;
		XmlValue value;
		std::cout << "\n";
		while( results.next(value) ) {
			XmlDocument theDocument = value.asDocument();
			std::string docString;
			//Retrieve the document's value as a string.
			theDocument.getContent(docString);
			std::cout << "Document content: \n" << docString << std::endl;
		}
	}
	//Catches XmlException
	catch(std::exception &e)
	{
		std::cerr << "Document query failed. ";
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
	XmlQueryContext context = mgr.createQueryContext(XmlQueryContext::LiveValues, XmlQueryContext::Eager);
	context.setNamespace( "fruits", "http://groceryItem.dbxml/fruits");
	context.setNamespace( "vegetables", "http://groceryItem.dbxml/vegetables");
	context.setNamespace( "desserts", "http://groceryItem.dbxml/desserts");
	std::string query = "collection('" + container.getName() + "')/fruits:item/product[. = 'Zapote Blanco']";
	try {
		//myDbEnv and myXmlContainer open with transactions. All subsequent
		//writes to them must also be performed inside a transaction.
		//Get a transaction
		XmlTransaction txn = mgr.createTransaction();

		XmlQueryExpression expression = mgr.prepare(txn, query, context);

		std::cout << "Updating document for the expression: '" << query << "' " << std::endl;
		std::cout << "Return to continue: ";
		getc(stdin);

		// print the document(s) to be updated -- those that describe Zapote Blanco (a fruit)
		// The document strings are only being printed to show before/after results.
		// Most modification programs would not perform the additional queries.
		XmlResults results = expression.execute(txn, context);
		dumpDocuments(results);

		std::cout << "About to update the document(s) above.\nLook for a new element after the target element, named 'description' ..." << std::endl;
		std::cout << "Return to continue: ";
		getc(stdin);

		// the modification is a new element in the target node, called "descripton,
		// which goes immediately after the <product> element.
		// if this program is run more than once, and committed, additional
		// identical elements are added.  It is easy to modify this program
		// to change the modification.
		XmlModify modify = mgr.createModify();
		modify.addInsertAfterStep(mgr.prepare(txn, ".", context), XmlModify::Element, "description", "very nice");

		XmlUpdateContext uc = mgr.createUpdateContext();
		results.reset();
		unsigned int noOfModifications = modify.execute(txn, results, context, uc);
		std::cout << noOfModifications << " modifications made." << std::endl;

		// print the modified documents
		dumpDocuments(results);

		std::cout << "If committed, this program will add a new element each time it is run." << std::endl;
		std::cout << "Press 'c' to commit this change:";
		char cmt = getc(stdin);
		if (cmt == 'c' || cmt == 'C') {
			txn.commit();
		}
		else {
			txn.abort();
		}
	}
	//Catches XmlException
	catch(std::exception &e) {
		std::cerr << "Document modification failed. ";
		std::cerr << e.what() << "\n";
		exit( -1 );
	}

	return 0;
}
