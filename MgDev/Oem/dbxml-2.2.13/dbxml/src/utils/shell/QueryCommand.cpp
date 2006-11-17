//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: QueryCommand.cpp,v 1.7 2005/04/05 16:44:29 bostic Exp $
//

#include "QueryCommand.hpp"

using namespace DbXml;
using namespace std;

string QueryCommand::getCommandName() const
{
	return "query";
}

string QueryCommand::getBriefHelp() const
{
	return "Execute an expression in the context of the XmlManager";
}

string QueryCommand::getMoreHelp() const
{
	return
		string("Usage: query <queryExpression>\n") +
		string("This command uses the XmlManager::query() method.");
}

void QueryCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}
	env.deleteResults();

	if(env.txn()) {
		env.results() = new XmlResults(env.db().query(*env.txn(), args[1], env.context(), DBXML_LAZY_DOCS));
	}
	else {
		env.results() = new XmlResults(env.db().query(args[1], env.context(), DBXML_LAZY_DOCS));
	}

	if(env.context().getEvaluationType() == XmlQueryContext::Eager) {
		if(env.verbose()) cout << env.results()->size() << " objects returned for eager expression '"
				       << args[1] << "'" << endl << endl;
	}
	else {
		if(env.verbose()) cout << "Lazy expression '" << args[1] << "' completed" << endl << endl;
	}
}

