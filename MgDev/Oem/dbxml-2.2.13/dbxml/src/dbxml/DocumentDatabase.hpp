//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DocumentDatabase.hpp,v 1.20 2005/12/14 17:57:07 jsnelson Exp $
//

#ifndef __DOCUMENTDATABASE_HPP
#define	__DOCUMENTDATABASE_HPP

#include <string>
#include <sstream>
#include <db_cxx.h>
#include "shared_ptr.hpp"
#include "DbWrapper.hpp"
#include "Cursor.hpp"
#include "dbxml/XmlContainer.hpp"

namespace DbXml
{

class Document;
class ID;
class Name;
class DictionaryDatabase;
class UpdateContext;
class KeyStash;
class DocumentCursor;

/**
 * Implements the ID generator, content and
 * metadata store for a container, and handles
 * indexing of content and metadata.
 */
class DocumentDatabase
{
public:
	typedef shared_ptr<DocumentDatabase> Ptr;

	DocumentDatabase(DbEnv *env, Transaction *txn,
			 const std::string &name,
			 XmlContainer::ContainerType type,
			 u_int32_t pageSize, u_int32_t flags, int mode);
	virtual ~DocumentDatabase();

	virtual u_int32_t getPageSize() const; // page size of content db
	virtual unsigned long getNumberOfPages() const;
	virtual int deleteID(OperationContext &context, const ID& oldId);
	virtual int createDocumentCursor(Transaction *txn,
					 scoped_ptr<DocumentCursor> &cursor,
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

	virtual int reindex(const IndexSpecification &is, const Document &document,
		UpdateContext &context, KeyStash &stash);


	virtual int getAllMetaData(OperationContext &context,
				   DictionaryDatabase *dictionary,
				   Document *document, u_int32_t flags) const;
	virtual int getMetaData(OperationContext &context,
		DictionaryDatabase *dictionary,
		const Name &name, const ID &did,
		XmlValue::Type &type, Dbt *metadata,
		u_int32_t flags) const;
	virtual int addMetaData(OperationContext &oc,
				DictionaryDatabase *dictionary,
				Document &document);
	virtual int updateMetaData(OperationContext &oc,
				   DictionaryDatabase *dictionary,
				   Document &document);
	virtual int removeMetaData(OperationContext &oc, const ID &id,
				   std::vector<ID> *toRemove);

	virtual void sync() {
		content_.sync();
		secondary_.sync();
	}
	static int dump(DbEnv *env, const std::string &name,
			XmlContainer::ContainerType type,
			std::ostream *out);
	static int load(DbEnv *env, const std::string &name,
			XmlContainer::ContainerType type,
			std::istream *in, unsigned long *lineno);
	static int verify(DbEnv *env, const std::string &name,
			  XmlContainer::ContainerType type,
			  std::ostream *out, u_int32_t flags);
	// upgrade
	static void upgrade(const std::string &name, Manager &mgr,
			    int old_version, int current_version);

	DbEnv *getDbEnv() { return environment_; }
	const std::string &getContainerName() const { return containerName_; }

protected:
	// no need for copy and assignment
	DocumentDatabase(const DocumentDatabase&);
	DocumentDatabase &operator=(const DocumentDatabase&);
	
	DbEnv *environment_;
	std::string containerName_;
	/**
	   TBD: factor content_ out into a document level
	   storage specific class, DsDocumentDatabase.cpp
	   - jpcs
	 */
	mutable DbWrapper content_;
	SecondaryDatabase secondary_;
	XmlContainer::ContainerType type_;
};

// Iterate through (Ns)DocumentDatabase content databases,
// pulling out keys (IDs)
class DocumentDatabaseCursor : public DocumentCursor
{
public:
	DocumentDatabaseCursor(Transaction *txn, DbWrapper &db, u_int32_t flags);
	virtual ~DocumentDatabaseCursor() {}
	virtual int first(ID &id) { return next(id); }
	virtual int next(ID &id);
private:
	Cursor cursor_;
	u_int32_t cursorFlags_;
	bool done_;
	DbtOut key_;
	DbtOut data_;
};

}

#endif
