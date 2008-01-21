//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: UpdateCommand.cpp,v 1.9 2006/10/30 17:46:08 bostic Exp $
//

#include "UpdateCommand.hpp"

using namespace DbXml;
using namespace std;

string UpdateCommand::getCommandName() const
{
	return "updateNodes";
}

string UpdateCommand::getCommandNameCompat() const
{
	return "update";
}

string UpdateCommand::getBriefHelp() const
{
	return "Update node content based on query expression and new content";
}

string UpdateCommand::getMoreHelp() const
{
	return
		string("Usage: updateNodes <queryExpression> <newContent>\n") +
		string("This command calls XmlModify::addUpdateStep() and\n") +
		string("XmlModify::execute() to replace the content of the\n") +
		string("nodes specified by the current results and the query\n") +
		string("expression with the specified content");
}

void UpdateCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 3) {
		throw CommandException("Wrong number of arguments");
	}
	env.testResults();
	
	if(env.verbose()) cout << "Updating nodes: " << args[1] <<
		" to have content: " << args[2] << endl;
	
	XmlModify modify = env.db().createModify();
	env.results()->reset();
	
	unsigned int modifications;
	if(env.txn()) {
		XmlTransaction myTxn = env.childTransaction();
		modify.addUpdateStep(env.db().prepare(myTxn, args[1],
						      env.context()), args[2]);
		modifications = modify.execute(myTxn, *env.results(),
					       env.context(), env.uc());
		myTxn.commit();
	} else {
		modify.addUpdateStep(env.db().prepare(args[1], env.context()),
				     args[2]);
		modifications = modify.execute(*env.results(), env.context(),
					       env.uc());
	}
	
	
	if (env.verbose()) cout << modifications << " modifications made."
				<< endl;
}

