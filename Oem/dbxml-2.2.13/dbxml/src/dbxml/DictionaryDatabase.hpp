//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DictionaryDatabase.hpp,v 1.12 2005/08/17 21:15:33 gmf Exp $
//

#ifndef __DICTIONARYDATABASE_HPP
#define	__DICTIONARYDATABASE_HPP

#include <string>
#include <db_cxx.h>
#include "DbWrapper.hpp"
#include "ID.hpp"
#include "scoped_ptr.hpp"

namespace DbXml
{

class Name;

/**
 * Implements the name dictionary for a container
 */
class DictionaryDatabase
{
public:
	typedef shared_ptr<DictionaryDatabase> Ptr;

	DictionaryDatabase(DbEnv *env, Transaction *txn,
			   const std::string &name, u_int32_t pageSize,
			   u_int32_t flags, int mode);
	~DictionaryDatabase();

	int lookupName(OperationContext &context, const ID &id,
		       Name &name) const; ///< lookup by id
	int lookupName(OperationContext &context,
		       const std::string &uriname, ID &id) const; ///< lookup by uri:name

	int defineName(OperationContext &context, const Name &name,
		       ID &id); ///< define from { uri, prefix, name }
	int defineName(OperationContext &context, const char *uriname,
		       ID &id); ///< define from uri:name

	int lookupName(OperationContext &context, const Name &name, ID &id,
		       bool define); ///< lookup by uri:name and define
	int lookupName(OperationContext &context, const char *uriname,
		       ID &id, bool define); ///< lookup by uri:name and define
	const ID &getNIDForName() const { return nidName_; }
	const ID &getNIDForRoot() const { return nidRoot_; }
	void sync() {
		primary_->sync();
		secondary_->sync();
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

	DbEnv *environment_;
	std::string containerName_;

	PrimaryDatabase::Ptr primary_;
	SecondaryDatabase::Ptr secondary_;
	ID nidName_;
	ID nidRoot_;
};

}

#endif

