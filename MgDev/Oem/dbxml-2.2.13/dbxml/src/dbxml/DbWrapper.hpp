//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbWrapper.hpp,v 1.20 2005/12/14 17:57:07 jsnelson Exp $
//

#ifndef __DBWRAPPER_HPP
#define	__DBWRAPPER_HPP

#include <string>
#include <db_cxx.h>
#include <errno.h>
#include "scoped_ptr.hpp"
#include "shared_ptr.hpp"
#include "Transaction.hpp"

extern "C" {
int index_duplicate_compare(DB *db, const DBT *dbt1, const DBT *dbt2);
};


namespace DbXml
{

class Key;
class PrimaryCursor;
class IndexCursor;
class ID;
class Syntax;
class OperationContext;
class DbtOut;
class IndexEntry;

/// Wraps a Db.
class DbWrapper
{
public:
	enum Operation { NONE, ALL, EQUALITY, LTX, LTE, GTX, GTE, RANGE, PREFIX, SUBSTRING }; // Cursor Operation
	typedef int(*bt_compare_fn)(Db *, const Dbt *, const Dbt *);

	DbWrapper(DbEnv *environment, const std::string &containerName, const std::string &prefixName, const std::string &databaseName, u_int32_t pageSize, u_int32_t flags);
	virtual ~DbWrapper();

	int open(Transaction *txn, DBTYPE type, u_int32_t flags, int mode);
	int close(u_int32_t flags);
	int dump(std::ostream *out);
	int load(std::istream *in, unsigned long *lineno);
	int verify(std::ostream *out, u_int32_t flags);
	int sync()
	{
		try {
			return db_.sync(0);
		}
		catch(DbException &de) {
			return de.get_errno();
		}
	}
	int put(Transaction *txn, Dbt *key, Dbt *data, u_int32_t flags)
	{
		try {
			return db_.put(Transaction::toDbTxn(txn), key, data, flags);
		}
		catch(DbException &de) {
			return de.get_errno();
		}
	}
	int get(Transaction *txn, Dbt *key, Dbt *data, u_int32_t flags) const
	{
		try {
			return const_cast<Db&>(db_).get(Transaction::toDbTxn(txn), key, data, flags);
		}
		catch(DbException &de) {
			return de.get_errno();
		}
	}
	int del(Transaction *txn, Dbt *key, u_int32_t flags)
	{
		try {
			return db_.del(Transaction::toDbTxn(txn), key, flags);
		}
		catch(DbException &de) {
			return de.get_errno();
		}
	}
	int truncate(Transaction *txn, u_int32_t *countp, u_int32_t flags)
	{
		try {
			return db_.truncate(Transaction::toDbTxn(txn), countp, flags);
		}
		catch(DbException &de) {
			return de.get_errno();
		}
	}
	int key_range(Transaction *txn, Dbt *key, DB_KEY_RANGE *key_range, u_int32_t flags) const
	{
		try {
			return const_cast<Db&>(db_).key_range(Transaction::toDbTxn(txn), key, key_range, flags);
		}
		catch(DbException &de) {
			return de.get_errno();
		}
	}

	/// Returns the number of pages in the database.
	unsigned long getNumberOfPages() const;

	u_int32_t getPageSize() const;

	Db &getDb()
	{
		return db_;
	} // jcm - this isn't so good
	DbEnv *getEnvironment() const
	{
		return environment_;
	}

	std::string getDatabaseName() const
	{
		return prefixName_ + databaseName_;
	}

	static const char *operationToString(Operation operation);

protected:
	bool needsToBeClosed_;
	std::string containerName_;
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
	typedef scoped_ptr<PrimaryDatabase> Ptr;

	PrimaryDatabase(DbEnv *environment, const std::string &containerName, const std::string &databaseName, u_int32_t pageSize, u_int32_t flags);
	virtual ~PrimaryDatabase();

	int open(Transaction *txn, u_int32_t flags, int mode);

	int getPrimary(OperationContext &context, const ID &id, DbtOut *data, u_int32_t flags) const;
	/// put some data using the next available id as the key.
	int putPrimary(OperationContext &context, const ID &id, const Dbt *data, u_int32_t flags);
	int appendPrimary(OperationContext &context, ID &id, const Dbt *data, u_int32_t flags);
	int deletePrimary(OperationContext &context, const ID &id, u_int32_t flags);

private:
	// no need for copy and assignment
	PrimaryDatabase(const PrimaryDatabase&);
	PrimaryDatabase& operator = (const PrimaryDatabase&);
};

/// Wraps a Secondary Db.
class SecondaryDatabase : public DbWrapper
{
public:
	typedef shared_ptr<SecondaryDatabase> Ptr;

	SecondaryDatabase(DbEnv *environment, const std::string &containerName, const std::string &databaseName, u_int32_t pageSize, u_int32_t flags);

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
	typedef shared_ptr<IndexDatabase> Ptr;

	IndexDatabase(DbEnv *environment, const std::string &containerName, const std::string &databaseName,
		const Syntax *syntax, u_int32_t pageSize, u_int32_t flags);

	int open(Transaction *txn, bool duplicates, bool nodesIndexed, u_int32_t flags, int mode);

	// We assume that key has the correct endianness.
	int getIndexEntry(OperationContext &context, const Dbt &key, IndexEntry &ie) const;

	// We assume that key has the correct endianness.
	int putIndexEntry(OperationContext &context, const Dbt &key, const Dbt &data);

	// We assume that key has the correct endianness.
	int updateIndexEntry(OperationContext &context, const Dbt &key, const Dbt &data);

	// We assume that key has the correct endianness.
	int delIndexEntry(OperationContext &context, const Dbt &key, const Dbt &data);

	// We assume that key has the correct endianness.
	bool exists(Transaction *txn, const Dbt &key) const;

	// Returns the % of keys in the index that will be returned by the operation.
	double percentage(OperationContext &context, Operation operation,
			  Operation gto, Operation lto, const Key &key1,
			  const Key &key2) const;

	// Create cursors for index databases
	IndexCursor *createCursor(Transaction *txn, Operation operation,
				  const Key *key, bool reverse);
	IndexCursor *createCursor(Transaction *txn, Operation gto,
				  const Key *gtk, Operation lto,
				  const Key *ltk, bool reverse);

private:
	// no need for copy and assignment
	IndexDatabase(const IndexDatabase&);
	IndexDatabase &operator = (const IndexDatabase&);

	void getMinKeyDbt(const Key &key, DbtOut &dbt) const;
	void getMaxKeyDbt(const Key &key, DbtOut &dbt) const;
	void getNextKeyDbt(const Key &key, DbtOut &dbt) const;

	const Syntax *syntax_; // The syntax for the database. Describes how keys are compared.
};

}

#endif

