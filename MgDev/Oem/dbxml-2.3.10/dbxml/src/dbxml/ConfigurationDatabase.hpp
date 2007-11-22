//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: ConfigurationDatabase.hpp,v 1.21 2006/10/30 17:45:51 bostic Exp $
//

#ifndef __CONFIGURATIONDATABASE_HPP
#define	__CONFIGURATIONDATABASE_HPP

#include <string>
#include <db_cxx.h>
#include "SharedPtr.hpp"
#include "DbWrapper.hpp"
#include "dbxml/XmlContainer.hpp"

namespace DbXml
{

class Buffer;
class IndexSpecification;
class Transaction;
class DocID;
	
/**
 * Wraps the configuration database, which is part of a container
 */
class ConfigurationDatabase
{
public:
	typedef SharedPtr<ConfigurationDatabase> Ptr;

	ConfigurationDatabase(DbEnv *env, Transaction *txn,
			      const std::string &name,
			      XmlContainer::ContainerType &type, // in/out
			      u_int32_t pageSize,
			      u_int32_t seqIncr, u_int32_t flags, int mode,
			      bool &indexNodes,
			      bool doVersionCheck);
	~ConfigurationDatabase();
	int generateID(Transaction *txn, DocID &newId);
	void resetSequenceCache();
	int getConfigurationItem(Transaction *txn, const char *key,
				 size_t keyLength, Buffer &b, bool lock) const;
	int putConfigurationItem(Transaction *txn, const char *key,
				 const Buffer &b);

	int getVersion(Transaction *txn, unsigned int &version) const;
	int getVersions(Transaction *txn, unsigned int &current_version,
			unsigned int &save_version) const;

	int putIndexSpecification(Transaction *txn,
				  const IndexSpecification &index);
	int getIndexSpecification(Transaction *txn, IndexSpecification &index,
				  bool lock = false) const;

	void setIndexNodes(Transaction *txn, bool indexNodes);

	void sync() {
		database_.sync();
	}

	void run(Transaction *txn, DbWrapper::DbFunctionRunner &runner) {
		runner.run(txn, database_);
		runner.run(txn, seqDatabase_);
	}

	static XmlContainer::ContainerType readContainerType(
		DbEnv *env, const std::string &name);

	static int dump(DbEnv *env, const std::string &name,
			std::ostream *out);
	static int load(DbEnv *env, const std::string &name,
			std::istream *in, unsigned long *lineno);
	static int verify(DbEnv *env, const std::string &name,
			  std::ostream *out, u_int32_t flags);
	// upgrade
	static int putVersion(Transaction *txn, DbWrapper &db,
			      unsigned int version);
	static void upgrade(const std::string &name, Manager &mgr,
			    int old_version, int current_version);
	
private:
	// no need for copy and assignment
	ConfigurationDatabase(const ConfigurationDatabase&);
	ConfigurationDatabase &operator=(const ConfigurationDatabase&);
	
	int checkVersion(Transaction *txn, bool rdonly);
	int checkIndexSpecification(Transaction *txn);
	bool checkIndexNodes(Transaction *txn, bool indexNodes,
			     bool isReadonly);
	static XmlContainer::ContainerType checkContainerType(
		Transaction *txn, DbWrapper &db,
		XmlContainer::ContainerType type, bool isReadonly);
	
	DbEnv *environment_;
	std::string name_;
	mutable DbWrapper database_;
	mutable DbWrapper seqDatabase_;
	DbSequence *seq_;
	u_int32_t seqIncr_;
	Transaction *creatingTxn_;
};

}

#endif

