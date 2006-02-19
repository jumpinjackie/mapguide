//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DeleteDocumentCommand.cpp,v 1.5 2005/04/05 16:44:28 bostic Exp $
//

#include "DeleteDocumentCommand.hpp"

using namespace DbXml;
using namespace std;

string DeleteDocumentCommand::getCommandName() const
{
	return "removeDocument";
}

string DeleteDocumentCommand::getCommandNameCompat() const
{
	return "deldocument";
}

string DeleteDocumentCommand::getBriefHelp() const
{
	return "Remove a document from the default container";
}

string DeleteDocumentCommand::getMoreHelp() const
{
	return
		string("Usage: removeDocument <docName>\n")+
		string("This command uses the XmlContainer::deleteDocument() method.\n");
}

void DeleteDocumentCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}
	env.testContainer();
	
	if(env.txn()) {
		XmlTransaction myTxn = env.childTransaction();
		env.container()->deleteDocument(myTxn, args[1], env.uc());
		myTxn.commit();
	} else {
		env.container()->deleteDocument(args[1], env.uc());
	}
	
	if(env.verbose()) cout << "Document deleted, name = " << args[1] << endl;
}

