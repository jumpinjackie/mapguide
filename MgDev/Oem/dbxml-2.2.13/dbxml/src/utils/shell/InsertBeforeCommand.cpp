//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: InsertBeforeCommand.cpp,v 1.7 2005/10/17 11:30:55 jsnelson Exp $
//

#include "InsertBeforeCommand.hpp"

using namespace DbXml;
using namespace std;

string InsertBeforeCommand::getCommandName() const
{
	return "insertBefore";
}

string InsertBeforeCommand::getBriefHelp() const
{
	return "Insert new content before nodes selected by the query expression";
}

string InsertBeforeCommand::getMoreHelp() const
{
	return
		string("Usage: insertBefore <queryExpr> <objectType> <objectName> <objectContent>\n") +
		string("Uses XmlModify::addInsertBeforeStep() and XmlModify::execute()\n") +
		string("to place new content (element, attribute, comment, PI)\n") +
		string("before the nodes specified by the current results and the query\n") +
		string("expression.\n") +
		string("\tqueryExpr -- valid XQuery expression\n") +
		string("\tobjectType -- \"element\", \"attribute\", \"text\"\n") +
		string("\t\t\"comment\", \"processinginstruction\"\n") +
		string("\tobjectContent -- text content or value.");
}

void InsertBeforeCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 5) {
		throw CommandException("Wrong number of arguments");
	}
	env.testResults();
	
	XmlModify::XmlObject o = getObject(args[2]);
	if(o == (XmlModify::XmlObject)-1) {
		throw CommandException("Unknown object type: " + args[2]);
	}
	
	if(env.verbose()) cout << "Inserting before nodes: " << args[1] <<
		" an object of type: " << args[2] <<
		" with name: " << args[3] <<
		" and content: " << args[4] << endl;
	
	XmlModify modify = env.db().createModify();
	env.results()->reset();
	
	unsigned int modifications;
	if(env.txn()) {
		XmlTransaction myTxn = env.childTransaction();
		modify.addInsertBeforeStep(env.db().prepare(myTxn, args[1],
							    env.context()), o,
					   args[3], args[4]);
		modifications = modify.execute(myTxn, *env.results(),
					       env.context(), env.uc());
		myTxn.commit();
	} else {
		modify.addInsertBeforeStep(env.db().prepare(args[1],
							    env.context()),
					   o, args[3], args[4]);
		modifications = modify.execute(*env.results(), env.context(),
					       env.uc());
	}
	
	if(env.verbose()) cout << modifications << " modifications made." << endl;
}

