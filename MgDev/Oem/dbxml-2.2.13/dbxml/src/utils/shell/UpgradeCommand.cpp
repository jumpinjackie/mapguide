//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: UpgradeCommand.cpp,v 1.1 2005/03/23 21:03:56 gmf Exp $
//

#include "UpgradeCommand.hpp"

using namespace DbXml;
using namespace std;

string UpgradeCommand::getCommandName() const
{
	return "upgradeContainer";
}

string UpgradeCommand::getBriefHelp() const
{
	return "Upgrade a container to the current container format";
}

string UpgradeCommand::getMoreHelp() const
{
	return
		string("Usage: upgradeContainer <containerName>\n")+
		string("This command uses the XmlManager::upgradeContainer() method.\n")+
		string("This command can take a long time on large containers.\n")+
		string("Containers should be backed up before running this command.\n");
}

void UpgradeCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}
	if(env.txn()) {
		throw CommandException("upgradeContainer cannot be run in a transaction");
	} else {
		env.db().upgradeContainer(args[1], env.uc());
	}
	
	if(env.verbose()) cout << "Container upgraded: " << args[1] << endl;
}

