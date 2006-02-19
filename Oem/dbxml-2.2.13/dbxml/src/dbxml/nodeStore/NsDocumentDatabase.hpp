//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: NsDocumentDatabase.hpp,v 1.12 2005/08/17 21:15:36 gmf Exp $
//

#ifndef __NSDOCUMENTDATABASE_HPP
#define	__NSDOCUMENTDATABASE_HPP

#include "../DocumentDatabase.hpp"
#include "NsTypes.hpp"

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
		return nodeStorage_;
	}

	virtual u_int32_t getPageSize() const {
		return nodeStorage_.getPageSize();
	}
	virtual unsigned long getNumberOfPages() const {
		return nodeStorage_.getNumberOfPages();
	}

	virtual int createDocumentCursor(
		Transaction *txn, scoped_ptr<DocumentCursor> &cursor,
		u_int32_t flags) const;
	
	virtual int getContent(OperationContext &context, Document *document,
			       u_int32_t flags) const;
	/// Also allocates an ID for the document
	virtual int addContentAndIndex(Document &document,
				       UpdateContext &context,
				       Container &container,
				       KeyStash &stash);
	virtual int updateContentAndIndex(Document &new_document,
					  UpdateContext &context,
					  KeyStash &stash);
	/// Also deallocates the ID
	virtual int removeContentAndIndex(const Document &document,
					  UpdateContext &context,
					  KeyStash &stash);

	/// Replaces if it already exists
	int putNodeRecord(OperationContext &context, const ID &id,
			  const Dbt *data);
	/// Replaces if it already exists
	int putNodeRecord(OperationContext &context, const ID &id,
			  const nsNode_t *node);
	int delNodeRecord(OperationContext &context, const ID &id,
			  const Dbt *data);
	int getNodeRecord(OperationContext &context, const ID &id, Dbt *data,
			  u_int32_t flags) const;
	virtual void sync() {
		nodeStorage_.sync();
		secondary_.sync();
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
	mutable DbWrapper nodeStorage_;
};

}

#endif
