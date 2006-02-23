//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: Environment.hpp,v 1.6 2005/04/05 16:44:28 bostic Exp $
//

#ifndef __ENVIRONMENT_HPP
#define	__ENVIRONMENT_HPP

#include "dbxml/DbXml.hpp"
#include <vector>
#include <string>

class Shell;

class Environment
{
public:
	Environment(DbXml::XmlManager &db);
	virtual ~Environment();

	virtual DbXml::XmlManager &db();
	virtual DbXml::XmlTransaction *&txn();
	virtual DbXml::XmlQueryContext &context();
	virtual DbXml::XmlUpdateContext &uc();
	virtual DbXml::XmlContainer *&container();
	virtual DbXml::XmlResults *&results();

	virtual std::string &streamName();
	virtual int &lineNo();

	virtual bool &interactive();
	virtual bool &transactions();
	virtual bool &verbose();
	virtual bool &quit();

	virtual const Shell *&shell();

	void testContainer();
	void deleteContainer();

	void testResults();
	void deleteResults();

	void testTransaction();
	void commitTransaction();
	void abortTransaction();
	DbXml::XmlTransaction childTransaction();

private:
	Environment(const Environment &);
	Environment &operator=(const Environment &);

	DbXml::XmlManager db_;
	DbXml::XmlTransaction *txn_;
	DbXml::XmlQueryContext context_;
	DbXml::XmlUpdateContext uc_;
	DbXml::XmlContainer *container_;
	DbXml::XmlResults *results_;

	std::string streamName_;
	int lineNo_;

	bool interactive_;
	bool transactions_;
	bool verbose_;
	bool quit_;

	const Shell *shell_;
};

#endif
