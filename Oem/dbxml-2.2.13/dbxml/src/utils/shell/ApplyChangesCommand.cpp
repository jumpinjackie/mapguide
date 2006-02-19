//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: ApplyChangesCommand.cpp,v 1.4 2005/04/05 16:44:27 bostic Exp $
//

#include "ApplyChangesCommand.hpp"

using namespace DbXml;
using namespace std;

string ApplyChangesCommand::getCommandName() const
{
	return "setApplyChanges";
}

string ApplyChangesCommand::getCommandNameCompat() const
{
	return "applychanges";
}

string ApplyChangesCommand::getBriefHelp() const
{
	return "Modifies \"apply changes\" state in the default update context";
}

string ApplyChangesCommand::getMoreHelp() const
{
	return
		string("Usage: setApplyChanges <on | off>\n") +
		string("Calls XmlUpdateContext::setApplyChangesToContainers().");
}

void ApplyChangesCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}
	
	if(args[1] == "on") {
		env.uc().setApplyChangesToContainers(true);
		if(env.verbose()) cout << "Apply changes on" << endl;
	}
	else {
		env.uc().setApplyChangesToContainers(false);
		if(env.verbose()) cout << "Apply changes off" << endl;
	}
}

