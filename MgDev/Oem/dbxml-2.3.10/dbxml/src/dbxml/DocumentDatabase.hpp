//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DocumentDatabase.hpp,v 1.28 2006/10/31 23:39:51 jsnelson Exp $
//

#ifndef __DOCUMENTDATABASE_HPP
#define	__DOCUMENTDATABASE_HPP

#include <string>
#include <sstream>
#include <db_cxx.h>
#include "SharedPtr.hpp"
#include "DbWrapper.hpp"
#include "Cursor.hpp"
#include "dbxml/XmlContainer.hpp"

namespace DbXml
{

class Document;
class DocID;
class NameID;
class Name;
class DictionaryDatabase;
class UpdateContext;
class KeyStash;
class DocumentCursor;
class NsDocumentDatabase;

/**
 * Implements the DocID generator, content and
 * metadata store for a container, and handles
 * indexing of content and metadata.
 */
class DocumentDatabase
{
public:
	typedef SharedPtr<DocumentDatabase> Ptr;

	DocumentDatabase(DbEnv *env, Transaction *txn,
			 const std::string &name,
			 XmlContainer::ContainerType type,
			 u_int32_t pageSize, u_int32_t flags,
			 int mode);
	// used for defaulted, read-only access to existing database
	DocumentDatabase(DbEnv *env, const std::string &name,
			 XmlContainer::ContainerType type);

	virtual ~DocumentDatabase();

	virtual u_int32_t getPageSize() const; // page size of content db
	virtual unsigned long getNumberOfPages() const;
	virtual int deleteID(OperationContext &context, const DocID& oldId);
	virtual int createDocumentCursor(Transaction *txn,
					 ScopedPtr<DocumentCursor> &cursor,
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

	virtual int reindex(const IndexSpecification &is, const Document &document,
		UpdateContext &context, KeyStash &stash);


	virtual int getAllMetaData(OperationContext &context,
				   DictionaryDatabase *dictionary,
				   Document *document, u_int32_t flags) const;
	virtual int getMetaData(OperationContext &context,
		DictionaryDatabase *dictionary,
		const Name &name, const DocID &did,
		XmlValue::Type &type, Dbt *metadata,
		u_int32_t flags) const;
	virtual int addMetaData(OperationContext &oc,
				DictionaryDatabase *dictionary,
				Document &document);
	virtual int updateMetaData(OperationContext &oc,
				   DictionaryDatabase *dictionary,
				   Document &document);
	virtual int removeMetaData(OperationContext &oc, const DocID &id,
				   std::vector<NameID> *toRemove);

	virtual void sync() {
		content_.sync();
		secondary_.sync();
	}

	virtual NsDocumentDatabase *getNsDocumentDatabase() {
		return NULL;
	}

	virtual int addContent(Document &document, UpdateContext &context);
	
	virtual void addContentException(Document &document,
					 UpdateContext &context, bool isDbExc) {}

	virtual void run(Transaction *txn,
			 DbWrapper::DbFunctionRunner &runner) {
		runner.run(txn, content_);
		runner.run(txn, secondary_);
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

	const std::string &getContainerName() const {
		return name_;
	}
	const XmlContainer::ContainerType getContainerType() const {
		return type_;
	}

private:
	void open(Transaction *txn, u_int32_t flags, int mode);

protected:
	// no need for copy and assignment
	DocumentDatabase(const DocumentDatabase&);
	DocumentDatabase &operator=(const DocumentDatabase&);
	
	DbEnv *environment_;
	std::string name_;
	XmlContainer::ContainerType type_;
	/**
	   TBD: factor content_ out into a document level
	   storage specific class, DsDocumentDatabase.cpp
	   - jpcs
	 */
	mutable DbWrapper content_;
	SecondaryDatabase secondary_;
};

// Iterate through DocumentDatabase content databases,
// pulling out keys (IDs)
class DocumentDatabaseCursor : public DocumentCursor
{
public:
	DocumentDatabaseCursor(Transaction *txn, DbWrapper &db, u_int32_t flags);
	virtual ~DocumentDatabaseCursor() {}
	virtual int first(DocID &id) { return next(id); }
	virtual int next(DocID &id);
private:
	Cursor cursor_;
	u_int32_t cursorFlags_;
	bool done_;
	DbtOut key_;
	DbtOut data_;
};

}

#endif
