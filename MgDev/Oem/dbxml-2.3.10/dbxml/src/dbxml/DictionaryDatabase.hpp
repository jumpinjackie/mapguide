//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DictionaryDatabase.hpp,v 1.20 2006/11/17 18:18:25 jsnelson Exp $
//

#ifndef __DICTIONARYDATABASE_HPP
#define	__DICTIONARYDATABASE_HPP

#include <string>
#include <db_cxx.h>
#include "DbWrapper.hpp"
#include "NameID.hpp"
#include "ScopedPtr.hpp"

namespace DbXml
{

class Name;

/**
 * Implements the name dictionary for a container
 */
class DictionaryDatabase
{
public:
	typedef SharedPtr<DictionaryDatabase> Ptr;

	DictionaryDatabase(DbEnv *env, Transaction *txn,
			   const std::string &name, u_int32_t pageSize,
			   u_int32_t flags, int mode);
	~DictionaryDatabase();

	// lookups in primary
	int lookupNameFromID(OperationContext &context, const NameID &id,
			     Name &name) const; ///< lookup by id

	// return goes into the OperationContext data DBT, and is
	// only valid until that is deleted, or overwritten by another
	// call
	int lookupStringNameFromID(OperationContext &context, const NameID &id,
				   const char **name) const; ///< lookup by id

	// return goes into the DBT passed in
	int lookupStringNameFromID(OperationContext &context, const NameID &id,
				   DbtOut &dbt) const; ///< lookup by id

	// const lookups in secondary
	int lookupIDFromStringName(OperationContext &context,
				   const char *name, u_int32_t namelen,
				   NameID &id) const; ///< lookup by name{:uri}

	// lookups in secondary, optionally define new name
	int lookupIDFromName(OperationContext &context, const Name &name, NameID &id,
			     bool define); ///< lookup by uri:name and define
	int lookupIDFromQName(OperationContext &context, const char *uriname,
			      NameID &id, bool define); ///< lookup by name:uri and define
	int lookupIDFromStringName(OperationContext &context, const char *name,
				   u_int32_t namelen,
				   NameID &id, bool define); ///< lookup by name and define

	// addition to dictionary
	int defineName(OperationContext &context, const Name &name,
		       NameID &id); ///< define from Name
	int defineQName(OperationContext &context, const char *uriname,
			NameID &id); ///< define from uri:name
	int defineStringName(OperationContext &context, const char *name,
			     u_int32_t namelen, NameID &id); ///< define from name

	//
	// internals
	//
	const NameID &getNIDForName() const { return nidName_; }
	const NameID &getNIDForRoot() const { return nidRoot_; }
	void sync() {
		primary_->sync();
		secondary_->sync();
	}

	void run(Transaction *txn, DbWrapper::DbFunctionRunner &runner) {
		runner.run(txn, *primary_);
		runner.run(txn, *secondary_);
	}

	static int dump(DbEnv *env, const std::string &name,
			std::ostream *out);
	static int load(DbEnv *env, const std::string &name,
			std::istream *in, unsigned long *lineno);
	static int verify(DbEnv *env, const std::string &name,
			  std::ostream *out, u_int32_t flags);
	// upgrade
	static void upgrade(const std::string &name, Manager &mgr,
			    int old_version, int current_version);

private:
	// no need for copy and assignment
	DictionaryDatabase(const DictionaryDatabase&);
	DictionaryDatabase &operator=(const DictionaryDatabase&);

	// internal lookups
	int lookupFromID(OperationContext &context,
		DbtOut &dbt, const NameID &id) const;

	void preloadDictionary(Transaction *txn, bool rdonly);

	DbEnv *environment_;
	std::string name_;

	PrimaryDatabase::Ptr primary_;
	SecondaryDatabase::Ptr secondary_;
	NameID nidName_;
	NameID nidRoot_;
	bool usePreloads_;
};

}

#endif

