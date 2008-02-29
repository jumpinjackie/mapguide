//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: RenameCommand.cpp,v 1.9 2006/10/30 17:46:08 bostic Exp $
//

#include "RenameCommand.hpp"

using namespace DbXml;
using namespace std;

string RenameCommand::getCommandName() const
{
	return "renameNodes";
}

string RenameCommand::getCommandNameCompat() const
{
	return "rename";
}

string RenameCommand::getBriefHelp() const
{
	return "Rename nodes specified by the query expression";
}

string RenameCommand::getMoreHelp() const
{
	return
		string("Usage: renameNodes <queryExpression> <newName>\n") +
		string("This command uses XmlModify::addRenameStep() and\n") +
		string("XmlModify::execute() to rename the nodes specified by\n") +
		string("current results and the query expression using the\n") +
		string("specified name.");
}

void RenameCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 3) {
		throw CommandException("Wrong number of arguments");
	}
	env.testResults();
	
	if(env.verbose()) cout << "Renaming nodes: " << args[1] << " to: "
			       << args[2] << endl;
	
	XmlModify modify = env.db().createModify();
	env.results()->reset();
	
	unsigned int modifications;
	if(env.txn()) {
		XmlTransaction myTxn = env.childTransaction();
		modify.addRenameStep(env.db().prepare(myTxn, args[1],
						      env.context()), args[2]);
		modifications = modify.execute(myTxn, *env.results(),
					       env.context(),
					       env.uc());
		myTxn.commit();
	} else {
		modify.addRenameStep(env.db().prepare(args[1], env.context()),
				     args[2]);
		modifications = modify.execute(*env.results(), env.context(),
					       env.uc());
	}
	
	if(env.verbose()) cout << modifications << " modifications made." << endl;
}

