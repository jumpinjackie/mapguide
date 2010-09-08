//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: AppendCommand.cpp,v 1.9 2006/10/30 17:46:07 bostic Exp $
//

#include "AppendCommand.hpp"

using namespace DbXml;
using namespace std;

string AppendCommand::getCommandName() const
{
	return "append";
}

string AppendCommand::getBriefHelp() const
{
	return "Append to nodes specified in the query expression";
}

string AppendCommand::getMoreHelp() const
{
	return
		string("Usage: append <queryExpr> <objectType> <objectName> <objectContent>\n") +
		string("Uses XmlModify::addAppendStep() and XmlModify::execute()\n") +
		string("to append new content (element, attribute, comment, PI)\n") +
		string("to the nodes specified by the current results and the query\n") +
		string("expression.\n") +
		string("\tqueryExpr -- valid XQuery expression\n") +
		string("\tobjectType -- \"element\", \"attribute\", \"text\"\n") +
		string("\t\t\"comment\", \"processinginstruction\"\n") +
		string("\tobjectContent -- text content or value.");
}

void AppendCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 5) {
		throw CommandException("Wrong number of arguments");
	}
	env.testResults();
	
	XmlModify::XmlObject o = getObject(args[2]);
	if(o == (XmlModify::XmlObject)-1) {
		throw CommandException("Unknown object type: " + args[2]);
	}
	
	if(env.verbose()) cout << "Appending into nodes: " << args[1] <<
		" an object of type: " << args[2] << " with name: " <<
		args[3] << " and content: " << args[4] << endl;
	
	XmlModify modify = env.db().createModify();
	env.results()->reset();
	
	unsigned int modifications;
	if(env.txn()) {
		XmlTransaction myTxn = env.childTransaction();
		modify.addAppendStep(env.db().prepare(myTxn, args[1],
						      env.context()),
				     o, args[3], args[4]);
		modifications = modify.execute(myTxn, *env.results(),
					       env.context(),
					       env.uc());
		myTxn.commit();
	} else { 
		modify.addAppendStep(env.db().prepare(args[1], env.context()),
				     o, args[3], args[4]);
		modifications = modify.execute(*env.results(), env.context(),
					       env.uc());
	}
	
	if(env.verbose()) cout << modifications << " modifications made." << endl;
}
