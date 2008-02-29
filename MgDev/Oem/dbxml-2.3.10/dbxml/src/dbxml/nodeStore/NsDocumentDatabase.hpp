//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsDocumentDatabase.hpp,v 1.23 2006/11/01 15:57:28 jsnelson Exp $
//

#ifndef __NSDOCUMENTDATABASE_HPP
#define	__NSDOCUMENTDATABASE_HPP

#include "../DocumentDatabase.hpp"
#include "NsNode.hpp"

namespace DbXml
{
class Transaction;

/**
 * Implements the ID generator, content and
 * metadata store for a container
 */
class NsDocumentDatabase : public DocumentDatabase
{
public:
	NsDocumentDatabase(DbEnv *env, Transaction *txn,
			   const std::string &name,
			   u_int32_t pageSize, u_int32_t flags, int mode);
	virtual ~NsDocumentDatabase();

	/** @name Interface to Databases */
	DbWrapper &getNodeStorageDatabase()
	{
		return *nodeStorage_;
	}

	virtual u_int32_t getPageSize() const {
		return nodeStorage_->getPageSize();
	}
	virtual unsigned long getNumberOfPages() const {
		return nodeStorage_->getNumberOfPages();
	}

	virtual int createDocumentCursor(
		Transaction *txn, ScopedPtr<DocumentCursor> &cursor,
		u_int32_t flags) const;
	
	virtual int getContent(OperationContext &context, Document *document,
			       u_int32_t flags) const;
	virtual int updateContentAndIndex(Document &new_document,
					  UpdateContext &context,
					  KeyStash &stash);
	/// Also deallocates the ID
	virtual int removeContentAndIndex(const Document &document,
					  UpdateContext &context,
					  KeyStash &stash);

	/// Replaces if it already exists
	int putNodeRecord(OperationContext &context, const DocID &did,
		const NsNid *nid, const Dbt *data);
	/// Replaces if it already exists
	int putNodeRecord(OperationContext &context, const DocID &id,
		const NsNode *node);
	int delNodeRecord(OperationContext &context, const DocID &id,
		const NsNid *nid);
	int getNodeRecord(OperationContext &context, const DocID &id,
		const NsNid *nid, Dbt *data, u_int32_t flags) const;
	int getNextNodeRecord(OperationContext &context, const DocID &did,
		const NsNid *nid, Dbt *data, u_int32_t flags) const;
	virtual void sync() {
		nodeStorage_->sync();
		secondary_.sync();
	}

	virtual NsDocumentDatabase *getNsDocumentDatabase() {
		return this;
	}

	virtual int addContent(Document &document, UpdateContext &context) {
		return 0;
	}

	virtual void addContentException(Document &document,
					 UpdateContext &context, bool isDbExc);
	
	virtual void run(Transaction *txn,
			 DbWrapper::DbFunctionRunner &runner) {
		runner.run(txn, secondary_);
		runner.run(txn, *nodeStorage_);
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
	int deleteAllNodes(OperationContext &context, const DocID &did);

	// Private methods for upgrade from 2.2 to 2.3.  This is non-trivial
	NsDocumentDatabase(DbEnv *env, const std::string &name,
			   DbWrapper *db);
	static void upgrade23(const std::string &name, Manager &mgr);
	
	mutable DbWrapper *nodeStorage_;
	bool dbIsOwned_;
};

}

#endif
