//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: ReindexCommand.cpp,v 1.3 2006/10/30 17:46:08 bostic Exp $
//

#include "ReindexCommand.hpp"

using namespace DbXml;
using namespace std;

string ReindexCommand::getCommandName() const
{
	return "reindexContainer";
}

string ReindexCommand::getBriefHelp() const
{
	return "Reindex a container, optionally changing index type";
}

string ReindexCommand::getMoreHelp() const
{
	return
		string("Usage: reindexContainer <containerName> <d|n>\n")+
		string("This command uses the XmlManager::reindexContainer() method.\n")+
		string("to recreate the indexes on the container.  It will also\n")+
		string("change the indexing type of the container, based on the\n")+
		string("second argument.  'n' means node indexes and 'd' means\n")+
		string("document level indexes\n")+
		string("This command can take a long time on large containers.\n")+
		string("Containers must be closed, and should be backed up\n")+
		string("before running this command.\n");
}

void ReindexCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 3) {
		throw CommandException("Wrong number of arguments");
	}

	if (env.container()) {
		const std::string &cname = env.container()->getName();
		if (cname == args[1]) {
			// close current container if it's the one
			env.deleteResults(); // might ref container
			env.deleteContainer();
		}
	}

	u_int32_t flags = (args[2] == "n") ? DBXML_INDEX_NODES : 0;
	if(env.txn()) {
		env.db().reindexContainer(*env.txn(), args[1], env.uc(), flags);
	} else {
		env.db().reindexContainer(args[1], env.uc(), flags);
	}
	
	if(env.verbose()) cout << "Container reindexed: " << args[1] << endl;
}

