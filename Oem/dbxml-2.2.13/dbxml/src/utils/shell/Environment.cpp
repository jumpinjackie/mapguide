//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: Environment.cpp,v 1.6 2005/04/05 16:44:28 bostic Exp $
//

#include "Environment.hpp"
#include "ShellCommand.hpp"

using namespace DbXml;
using namespace std;

Environment::Environment(XmlManager &db)
	: db_(db),
	  txn_(0),
	  context_(db.createQueryContext()),
	  uc_(db.createUpdateContext()),
	  container_(0),
	  results_(0),
	  lineNo_(0),
	  interactive_(false),
	  verbose_(false),
	  quit_(false),
	  shell_(0)
{
}

Environment::~Environment()
{
	delete results_;
	delete txn_;
	delete container_;
}

XmlManager &Environment::db()
{
	return db_;
}

XmlTransaction *&Environment::txn()
{
	return txn_;
}

XmlQueryContext &Environment::context()
{
	return context_;
}

XmlUpdateContext &Environment::uc()
{
	return uc_;
}

XmlContainer *&Environment::container()
{
	return container_;
}

XmlResults *&Environment::results()
{
	return results_;
}

bool &Environment::interactive()
{
	return interactive_;
}

bool &Environment::transactions()
{
	return transactions_;
}

bool &Environment::verbose()
{
	return verbose_;
}

bool &Environment::quit()
{
	return quit_;
}

string &Environment::streamName()
{
	return streamName_;
}

int &Environment::lineNo()
{
	return lineNo_;
}

void Environment::testContainer()
{
	if(!container()) {
		throw CommandException("There is no default container");
	}
}

void Environment::deleteContainer()
{
	delete container();
	container() = 0;
}

void Environment::testResults()
{
	if(!results()) {
		throw CommandException("The last query returned no results");
	}
}

void Environment::deleteResults()
{
	delete results();
	results() = 0;
}

void Environment::testTransaction()
{
	if(!txn()) {
		throw CommandException("There is no current transaction");
	}
}

void Environment::commitTransaction()
{
	txn()->commit();
	delete txn();
	txn() = 0;
}

void Environment::abortTransaction()
{
	txn()->abort();
	delete txn();
	txn() = 0;
}

DbXml::XmlTransaction Environment::childTransaction()
{
	return txn()->createChild();
}

const Shell *&Environment::shell()
{
	return shell_;
}
