//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: ConfigurationDatabase.hpp,v 1.15 2005/08/31 17:14:20 gmf Exp $
//

#ifndef __CONFIGURATIONDATABASE_HPP
#define	__CONFIGURATIONDATABASE_HPP

#include <string>
#include <db_cxx.h>
#include "shared_ptr.hpp"
#include "DbWrapper.hpp"
#include "dbxml/XmlContainer.hpp"

namespace DbXml
{

class Buffer;
class IndexSpecification;
class Transaction;

/**
 * Wraps the configuration database, which is part of a container
 */
class ConfigurationDatabase
{
public:
	typedef shared_ptr<ConfigurationDatabase> Ptr;

	ConfigurationDatabase(DbEnv *env, Transaction *txn,
			      const std::string &name, u_int32_t pageSize,
			      u_int32_t seqIncr, u_int32_t flags, int mode,
			      XmlContainer::ContainerType &type,
			      bool &indexNodes,
			      bool doVersionCheck);
	~ConfigurationDatabase();
	int generateID(Transaction *txn, ID &newId);
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
	XmlContainer::ContainerType checkContainerType(
		Transaction *txn, XmlContainer::ContainerType type,
		bool isReadonly);
	int checkIndexSpecification(Transaction *txn);
	bool checkIndexNodes(Transaction *txn, bool indexNodes,
			     bool isReadonly);
	
	DbEnv *environment_;
	std::string containerName_;
	mutable DbWrapper database_;
	mutable DbWrapper seqDatabase_;
	DbSequence *seq_;
	u_int32_t seqIncr_;
	Transaction *creatingTxn_;
};

}

#endif

