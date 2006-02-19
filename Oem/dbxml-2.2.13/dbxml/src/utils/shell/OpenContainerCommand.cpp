//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: OpenContainerCommand.cpp,v 1.10 2005/08/16 11:56:33 jsnelson Exp $
//

#include "OpenContainerCommand.hpp"

using namespace DbXml;
using namespace std;

string OpenContainerCommand::getCommandName() const
{
	return "openContainer";
}

string OpenContainerCommand::getCommandNameCompat() const
{
	return "open";
}

string OpenContainerCommand::getBriefHelp() const
{
	return "Opens a container, and uses it as the default container";
}

string OpenContainerCommand::getMoreHelp() const
{
	return
		string("Usage: openContainer <container> [[no]validate]\n")+
		string("This command uses the XmlManager::openContainer() method. It also sets\n") +
		string("the default collection, using the XmlQueryContext::setDefaultCollection()\n") +
		string("method.");
}

void OpenContainerCommand::execute(Args &args, Environment &env)
{
	if (args.size() < 2 || args.size() > 3){
		throw CommandException("Wrong number of arguments");
	}
	u_int32_t flags = 0;
	if (args.size() == 3) {
		if (args[2] == "validate")
			flags |= DBXML_ALLOW_VALIDATION;
	}
	if (env.transactions())
		flags |= DBXML_TRANSACTIONAL;
	env.deleteContainer();
	env.container() = new XmlContainer(
		env.db().openContainer(args[1], flags));
	env.context().setDefaultCollection(env.container()->getName());
}

