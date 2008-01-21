//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: SyntaxDatabase.hpp,v 1.23 2006/11/17 18:18:27 jsnelson Exp $
//

#ifndef __SYNTAXDATABASE_HPP
#define	__SYNTAXDATABASE_HPP

#include <string>
#include <vector>
#include "SharedPtr.hpp"
#include "DbWrapper.hpp"
#include "IDS.hpp"
#include "IndexEntry.hpp"

namespace DbXml
{

class KeyStatistics;
class Syntax;
class DbtIn;

/**
 * Wraps an index and statistics database for a syntax.
 * Container have one for each syntax available.
 */
class SyntaxDatabase
{
public:
	typedef SharedPtr<SyntaxDatabase> Ptr;
	typedef std::vector<Ptr> Vector;

	SyntaxDatabase(const Syntax *syntax, DbEnv *env, Transaction *txn, const std::string &name,
		bool nodesIndexed, u_int32_t pageSize, u_int32_t flags, int mode);
	~SyntaxDatabase();

	IndexDatabase *getIndexDB() { return index_.get(); }
	const IndexDatabase *getIndexDB() const { return index_.get(); }

	IndexDatabase *getStatisticsDB() { return statistics_.get(); }
	const IndexDatabase *getStatisticsDB() const { return statistics_.get(); }

	int getIndexData(OperationContext &context, IndexData::Ptr &data,
			 SecondaryDatabase::Operation op1, const Key &k1,
			 SecondaryDatabase::Operation op2 = SecondaryDatabase::NONE,
			 const Key &k2 = Key(0)) const;

	int removeIndex(OperationContext &context, const Index &index,
			const NameID &id);
	int updateStatistics(OperationContext &context, DbtIn &key,
			     const KeyStatistics &statistics);

	void sync() {
		index_->sync();
		statistics_->sync();
	}

	void run(Transaction *txn, DbWrapper::DbFunctionRunner &runner) {
		runner.run(txn, *index_);
		runner.run(txn, *statistics_);
	}

	static int dump(const Syntax *syntax, DbEnv *env, const std::string &name,
		std::ostream *out);
	static int load(const Syntax *syntax, DbEnv *env, const std::string &name,
		std::istream *in, unsigned long *lineno);
	static int verify(const Syntax *syntax, DbEnv *env, const std::string &name,
		std::ostream *out, u_int32_t flags);

private:
	// no need for copy and assignment
	SyntaxDatabase(const SyntaxDatabase&);
	SyntaxDatabase &operator=(const SyntaxDatabase&);
	int removeIndexEntries(OperationContext &context, DbWrapper &db,
			       void *buf, u_int32_t bufsize);

	const Syntax *syntax_;
	DbEnv *environment_;
	std::string containerName_;

	IndexDatabase::Ptr index_;
	IndexDatabase::Ptr statistics_;
};

}

#endif

