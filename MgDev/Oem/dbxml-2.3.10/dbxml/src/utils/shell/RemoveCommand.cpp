//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: RemoveCommand.cpp,v 1.9 2006/10/30 17:46:08 bostic Exp $
//

#include "RemoveCommand.hpp"

using namespace DbXml;
using namespace std;

string RemoveCommand::getCommandName() const
{
	return "removeNodes";
}

string RemoveCommand::getCommandNameCompat() const
{
	return "remove";
}

string RemoveCommand::getBriefHelp() const
{
	return "Remove content from documents specified by the query expression";
}

string RemoveCommand::getMoreHelp() const
{
	return
		string("Usage: removeNodes <queryExpression>\n") +
		string("This command uses XmlModify::addRemoveStep() and\n") +
		string("XmlModify::execute() to remove the nodes specified by\n") +
		string("the current results and the query expression.");
}

void RemoveCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}
	env.testResults();
	
	if(env.verbose()) cout << "Removing nodes: " << args[1] << endl;
	
	XmlModify modify = env.db().createModify();
	env.results()->reset();
	
	unsigned int modifications;
	if(env.txn()) {
		XmlTransaction myTxn = env.childTransaction();
		modify.addRemoveStep(env.db().prepare(myTxn, args[1],
						      env.context()));
		modifications = modify.execute(myTxn, *env.results(),
					       env.context(), env.uc());
		myTxn.commit();
	} else {
		modify.addRemoveStep(env.db().prepare(args[1], env.context()));
		modifications = modify.execute(*env.results(), env.context(),
					       env.uc());
	}
	
	if(env.verbose()) cout << modifications << " modifications made." << endl;
}

