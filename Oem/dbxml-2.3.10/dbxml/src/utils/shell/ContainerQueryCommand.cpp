//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: ContainerQueryCommand.cpp,v 1.10 2006/10/30 17:46:07 bostic Exp $
//

#include "ContainerQueryCommand.hpp"

using namespace DbXml;
using namespace std;

string ContainerQueryCommand::getCommandName() const
{
	return "cquery";
}

string ContainerQueryCommand::getBriefHelp() const
{
	return "Execute an expression in the context of the default container";
}

string ContainerQueryCommand::getMoreHelp() const
{
	return
		string("Takes one argument - the query to be run. This is run using the\n") +
		string("XmlContainer::query() method, on the default container, and the\n") +
		string("results are returned.\n")+
		string("NOTE: cquery can produce unexpected results for some queries, as it\n")+
		string("rewrites 'queryString' into:\n")+
		string("\tcollection('defaultContainer')/('queryString')\n");
}

void ContainerQueryCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}
	env.testContainer();
	env.deleteResults();

	std::string fullQuery = "collection('" + env.container()->getName() + "')/(" + args[1] + ")";
	if(env.txn()) {
		XmlQueryExpression expr = env.db().prepare(*env.txn(), fullQuery, env.context());
		env.results() = new XmlResults(expr.execute(*env.txn(), env.context(), DBXML_LAZY_DOCS));
	}
	else {
		XmlQueryExpression expr = env.db().prepare(fullQuery, env.context());
		env.results() = new XmlResults(expr.execute(env.context(), DBXML_LAZY_DOCS));
	}

	if(env.context().getEvaluationType() == XmlQueryContext::Eager) {
		if(env.verbose()) cout << env.results()->size() << " objects returned for eager expression '"
				       << args[1] << "'" << endl << endl;
	}
	else {
		if(env.verbose()) cout << "Lazy expression '" << args[1] << "' completed" << endl << endl;
	}
}

