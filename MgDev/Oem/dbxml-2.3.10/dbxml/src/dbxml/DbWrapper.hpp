//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbWrapper.hpp,v 1.31 2006/10/30 17:45:52 bostic Exp $
//

#ifndef __DBWRAPPER_HPP
#define	__DBWRAPPER_HPP

#include <string>
#include <db_cxx.h>
#include <errno.h>
#include "ScopedPtr.hpp"
#include "SharedPtr.hpp"
#include "Transaction.hpp"
#include "dbxml/XmlException.hpp"

extern "C" {
int index_duplicate_compare(DB *db, const DBT *dbt1, const DBT *dbt2);
};

// provide consistent handling of deadlock and DB errors
#define HANDLE_DB_ERRORS \
catch (DbException &de) { \
	dberr = de.get_errno(); \
	if (dberr == DB_LOCK_DEADLOCK) \
		throw XmlException(de); \
} \
return dberr;

namespace DbXml
{

class Key;
class PrimaryCursor;
class IndexCursor;
class NameID;
class Syntax;
class OperationContext;
class DbtOut;
class IndexEntry;
class NsIterator;

/// Wraps a Db.
class DbWrapper
{
public:
	// this class is used to run a given function on *all* databases
	// in a container (e.g. truncate(), compact()).  Not used
	// for upgrade because upgrade is order-dependent.
	class DbFunctionRunner {
	public:
		virtual ~DbFunctionRunner() {}
		virtual void run(Transaction *txn, DbWrapper &database) = 0;
	};
	
	enum Operation { NONE, ALL, EQUALITY, LTX, LTE, GTX, GTE, RANGE, PREFIX, SUBSTRING }; // Cursor Operation
	typedef int(*bt_compare_fn)(Db *, const Dbt *, const Dbt *);

	DbWrapper(DbEnv *environment, const std::string &name,
		  const std::string &prefixName,
		  const std::string &databaseName,
		  u_int32_t pageSize, u_int32_t flags);
	virtual ~DbWrapper();

	int open(Transaction *txn, DBTYPE type, u_int32_t flags, int mode);
	int close(u_int32_t flags);
	int dump(std::ostream *out);
	int load(std::istream *in, unsigned long *lineno);
	int verify(std::ostream *out, u_int32_t flags);
	int sync()
	{
		int dberr;
		try {
			dberr = db_.sync(0);
		}
		HANDLE_DB_ERRORS;
	}
	int put(Transaction *txn, Dbt *key, Dbt *data, u_int32_t flags)
	{
		int dberr;
		try {
			dberr = db_.put(Transaction::toDbTxn(txn), key,
				       data, flags);
		}
		HANDLE_DB_ERRORS;
	}
	int get(Transaction *txn, Dbt *key, Dbt *data, u_int32_t flags) const
	{
		int dberr;
		try {
			dberr = const_cast<Db&>(db_).
				get(Transaction::toDbTxn(txn), key,
				    data, flags);
		}
		catch(DbMemoryException &) {
			dberr = DB_BUFFER_SMALL;
		}
		HANDLE_DB_ERRORS;
	}
	int del(Transaction *txn, Dbt *key, u_int32_t flags)
	{
		int dberr;
		try {
			dberr = db_.del(Transaction::toDbTxn(txn), key, flags);
		}
		HANDLE_DB_ERRORS;
	}
	int truncate(Transaction *txn, u_int32_t *countp, u_int32_t flags)
	{
		int dberr;
		try {
			dberr = db_.truncate(Transaction::toDbTxn(txn),
					    countp, flags);
		}
		HANDLE_DB_ERRORS;
	}
	int key_range(Transaction *txn, Dbt *key, DB_KEY_RANGE *key_range,
		      u_int32_t flags) const
	{
		int dberr;
		try {
			dberr = const_cast<Db&>(db_).
				key_range(Transaction::toDbTxn(txn),
					  key, key_range, flags);
		}
		HANDLE_DB_ERRORS;
	}

	/// Returns the number of pages in the database.
	unsigned long getNumberOfPages() const;

	u_int32_t getPageSize() const;

	Db &getDb() {
		return db_;
	}

	DbEnv *getEnvironment() const {
		return environment_;
	}

	std::string getDatabaseName() const {
		return prefixName_ + databaseName_;
	}

	static const char *operationToString(Operation operation);
	static const char *operationToWord(Operation operation);

protected:
	bool needsToBeClosed_;
	std::string name_;
	std::string prefixName_;
	std::string databaseName_;
	u_int32_t pageSize_;

	Db db_;
	DbEnv *environment_;

private:
	// no need for copy and assignment
	DbWrapper(const DbWrapper&);
	DbWrapper & operator = (const DbWrapper &);
};

/// Wraps a Primary Db.
class PrimaryDatabase : public DbWrapper
{
public:
	typedef ScopedPtr<PrimaryDatabase> Ptr;

	PrimaryDatabase(DbEnv *environment, const std::string &name,
			const std::string &databaseName, u_int32_t pageSize,
			u_int32_t flags);
	virtual ~PrimaryDatabase();

	int open(Transaction *txn, u_int32_t flags, int mode);

	int getPrimary(OperationContext &context, const NameID &id,
		       DbtOut *data, u_int32_t flags) const;
	/// put some data using the next available id as the key.
	int putPrimary(OperationContext &context, const NameID &id,
		       const Dbt *data, u_int32_t flags);
	int appendPrimary(OperationContext &context, NameID &id,
			  const Dbt *data, u_int32_t flags);
	int deletePrimary(OperationContext &context, const NameID &id,
			  u_int32_t flags);

private:
	// no need for copy and assignment
	PrimaryDatabase(const PrimaryDatabase&);
	PrimaryDatabase& operator = (const PrimaryDatabase&);
};

/// Wraps a Secondary Db.
class SecondaryDatabase : public DbWrapper
{
public:
	typedef SharedPtr<SecondaryDatabase> Ptr;

	SecondaryDatabase(DbEnv *environment, const std::string &name,
			  const std::string &databaseName,
			  u_int32_t pageSize, u_int32_t flags);

	int open(Transaction *txn, bool duplicates, u_int32_t flags, int mode);

private:
	// no need for copy and assignment
	SecondaryDatabase(const SecondaryDatabase&);
	SecondaryDatabase &operator = (const SecondaryDatabase&);
};

/// Wraps an Index Db.
class IndexDatabase : public DbWrapper
{
public:
	typedef SharedPtr<IndexDatabase> Ptr;

	IndexDatabase(DbEnv *environment, const std::string &name,
		      const std::string &databaseName,
		      const Syntax *syntax, u_int32_t pageSize,
		      u_int32_t flags);

	int open(Transaction *txn, bool duplicates, bool nodesIndexed,
		 u_int32_t flags, int mode);

	// We assume that key has the correct endianness.
	int getIndexEntry(OperationContext &context, const Dbt &key,
			  IndexEntry &ie) const;

	// We assume that key has the correct endianness.
	int putIndexEntry(OperationContext &context, const Dbt &key,
			  const Dbt &data);

	// We assume that key has the correct endianness.
	int updateIndexEntry(OperationContext &context, const Dbt &key,
			     const Dbt &data);

	// We assume that key has the correct endianness.
	int delIndexEntry(OperationContext &context, const Dbt &key,
			  const Dbt &data);

	// We assume that key has the correct endianness.
	bool exists(Transaction *txn, const Dbt &key) const;

	// Returns the % of keys in the index that will be returned
	// by the operation.
	double percentage(OperationContext &context, Operation operation,
			  Operation gto, Operation lto, const Key &key1,
			  const Key &key2) const;

	// Create cursors for index databases
	IndexCursor *createCursor(Transaction *txn, Operation operation,
				  const Key *key, bool reverse);
	IndexCursor *createCursor(Transaction *txn, Operation gto,
				  const Key *gtk, Operation lto,
				  const Key *ltk, bool reverse);

	NsIterator *createIterator(Transaction *txn, const Key *key);

private:
	// no need for copy and assignment
	IndexDatabase(const IndexDatabase&);
	IndexDatabase &operator = (const IndexDatabase&);

	void getMinKeyDbt(const Key &key, DbtOut &dbt) const;
	void getMaxKeyDbt(const Key &key, DbtOut &dbt) const;
	void getNextKeyDbt(const Key &key, DbtOut &dbt) const;
	// The syntax for the database. Describes how keys are compared.
	const Syntax *syntax_;
};

}

#endif

