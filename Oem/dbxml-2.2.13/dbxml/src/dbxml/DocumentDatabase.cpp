//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DocumentDatabase.cpp,v 1.34 2005/11/22 16:39:49 jsnelson Exp $
//

#include <sstream>

#include <dbxml/XmlException.hpp>
#include "DocumentDatabase.hpp"
#include "Manager.hpp"
#include "ScopedDbt.hpp"
#include "Container.hpp"
#include "OperationContext.hpp"
#include "Document.hpp"
#include "Cursor.hpp"
#include "UpdateContext.hpp"
#include "db_utils.h"

using namespace DbXml;
using namespace std;
static const char *document_name = "document";

DocumentDatabase::DocumentDatabase(DbEnv *env, Transaction *txn,
				   const std::string &name,
				   XmlContainer::ContainerType type,
				   u_int32_t pageSize, u_int32_t flags,
				   int mode)
	: environment_(env),
	  containerName_(name),
	  content_(env, name, "content_", document_name,
		   pageSize, flags & DB_XA_CREATE),
	  secondary_(env, name, document_name, pageSize,
		     flags & DB_XA_CREATE),
	  type_(type)
{
	flags &= ~DB_XA_CREATE;
	int err = 0;
	try {
		// Open the Db objects
		if (type == XmlContainer::WholedocContainer) {
			err = content_.open(txn, DB_BTREE, flags, mode);
		}
		if (err == 0)
			err = secondary_.open(txn, /*duplicates*/false, flags, mode);
	} catch (DbException &e) {
		if(e.get_errno() == EEXIST) {
			throw XmlException(XmlException::CONTAINER_EXISTS,
					   e.what());
		} else {
			throw XmlException(e);
		}
	}
	if(err == EEXIST) {
		throw XmlException(XmlException::CONTAINER_EXISTS,
				   db_strerror(err));
	} else if(err != 0) {
		throw XmlException(err);
	}
}

DocumentDatabase::~DocumentDatabase()
{
	// nothing to do
	
}

int DocumentDatabase::getContent(OperationContext &context,
				 Document *document, u_int32_t flags) const
{
	DbtOut *data = new DbtOut();
	int err = 0;
	try {
		document->getID().setDbtFromThis(context.key());
		// NOTE: db_.get() does not throw on DB_NOTFOUND, but
		// try/catch anyway...
		err = content_.get(context.txn(),
			&context.key(), data, flags);
	} catch (...) {
		delete data;
		throw; // re-throw
	}

	if(err == 0 && (data->get_size() != 0)) {
		document->setContentAsDbt(&data); // Note: consumes data
	} else {
		delete data;
	}
	if (err == DB_NOTFOUND)
		err = 0; // allow no-content documents
	return err;
}

// object is donated to caller
int DocumentDatabase::createDocumentCursor(
	Transaction *txn, scoped_ptr<DocumentCursor> &cursor, u_int32_t flags) const
{
	DocumentDatabaseCursor *newCursor =
		new DocumentDatabaseCursor(txn, content_, flags);
	cursor.reset(newCursor);
	return 0;
}

u_int32_t DocumentDatabase::getPageSize() const
{
	return content_.getPageSize();
}

unsigned long DocumentDatabase::getNumberOfPages() const
{
	return content_.getNumberOfPages();
}

// Remove the ID -- cannot "remove" an ID.  They are not
// reusable, but implement the call, just in case that ever
// changes.
int DocumentDatabase::deleteID(OperationContext &context, const ID& oldId)
{
	return 0; // this is a no-op; IDs are never reused
}

int DocumentDatabase::addContentAndIndex(Document &document,
					 UpdateContext &context,
					 Container &container,
					 KeyStash &stash)
{
	int err = 0;
	OperationContext &oc = context.getOperationContext();
	Indexer &indexer = context.getIndexer();

	// If this document's content is a stream, convert it to a dbt,
	// because streams can only be used once... - jpcs
	if(document.getDefinitiveContent() == Document::INPUTSTREAM) {
		document.getContentAsDbt();
	}

	// Generate a unique ID for the Document, if not done by caller
	if (document.getID() == 0) {
		err = container.getConfigurationDB()->
			generateID(oc.txn(), document.getIDToSet());
		if (err)
			return err;
	}
	
	// Index the document
	indexer.indexMetaData(context.getIndexSpecification(), document, stash, /*checkModified*/false);

	scoped_ptr<NsPushEventSource8> source(document.
		getContentAsSAX(oc.txn(), /*needsValidation*/true,
			container.getIndexNodes(), /*NsDocumentDatabase*/0));
	indexer.indexContent(context.getIndexSpecification(), document.getID(), source.get(), stash);


	Dbt *dbt = (Dbt*) document.getContentAsDbt();
	// Allow no-content documents; they have no entry in
	// the content database
	if (dbt && (dbt->get_size() != 0)) {
		document.getID().setDbtFromThis(oc.key());
		err = content_.put(oc.txn(), &oc.key(),
			dbt, 0);
	}
	if(err == 0) document.setContentModified(false);
	return err;
}

int DocumentDatabase::updateContentAndIndex(Document &new_document,
					    UpdateContext &context,
					    KeyStash &stash)
{
	OperationContext &oc = context.getOperationContext();
	ID id = new_document.getID();
	Indexer &indexer = context.getIndexer();
	IndexSpecification &index = context.getIndexSpecification();
	int err = 0;
	bool resetId = false;

	// NOTE: consider adding DB_RMW if transacted.
	
	// Check to see if the old document exists, first
	// If ID is non-zero, let's trust it.  If not, get by name.
	// Retrieve the old document
	XmlDocument old_document;

	if (id == 0) {
		// will throw if name is bad or doc doesn't exist
		err = indexer.getContainer()->getDocument(
			oc, new_document.getName(), old_document, DBXML_LAZY_DOCS);
		if (err == 0) {
			id = ((Document&)old_document).getID();
			new_document.getIDToSet() = id;
			resetId = true;
			// clear modified flag if set on name
			const_cast<Document*>(&new_document)->
				clearModified(Name(metaDataName_uri_name));
		}
	} else {
		err = indexer.getContainer()->getDocument(
			oc, id, old_document, DBXML_LAZY_DOCS);
	}
	if(err != 0) return err;
	
	// If this document's content is a stream, convert it to a dbt,
	// because streams can only be used once... - jpcs
	if(new_document.getDefinitiveContent() == Document::INPUTSTREAM) {
		new_document.getContentAsDbt();
	}

	// Index the new document
	indexer.indexMetaData(index, new_document, stash, true);
	if(new_document.isContentModified()) {
		scoped_ptr<NsPushEventSource8> source(new_document.
			getContentAsSAX(oc.txn(), /*needsValidation*/true,
				indexer.getContainer()->getIndexNodes(),
				/*NsDocumentDatabase*/0));
		indexer.indexContent(index, id,
				     source.get(), stash);
	}

	// Set the modified flags of the old document to the same as the
	// new document, so that when we index, we only generate keys for
	// things that have actually changed.
	MetaData::const_iterator end = new_document.metaDataEnd();
	for(MetaData::const_iterator i = new_document.metaDataBegin();
	    i != end; ++i) {
		if((*i)->isModified()) {
			const MetaDatum *md = ((Document&)old_document)
				.getMetaDataPtr((*i)->getName());
			if(md != 0) const_cast<MetaDatum*>(md)->setModified(true);
		}
	}

	// Remove the index keys for the old document
	IndexSpecification delete_index(index);
	delete_index.set(Index::INDEXER_DELETE);
	indexer.indexMetaData(delete_index, old_document, stash, true);
	if(new_document.isContentModified()) {
		scoped_ptr<NsPushEventSource8> source(((Document&)old_document).
			getContentAsSAX(oc.txn(), /*needsValidation*/false,
				indexer.getContainer()->getIndexNodes(),
				/*NsDocumentDatabase*/0));
		indexer.indexContent(delete_index, id,
				     source.get(), stash);
	}

	// Update the content
	if(new_document.isContentModified()) {
		OperationContext &oc = context.getOperationContext();
		id.setDbtFromThis(oc.key());
		err = content_.put(oc.txn(), &oc.key(),
			(Dbt*)new_document.getContentAsDbt(), 0);
	}

	if(err == 0) new_document.setContentModified(false);
	if (resetId)
		new_document.getIDToSet() = 0;
	return err;
}

int DocumentDatabase::removeContentAndIndex(const Document &document,
					    UpdateContext &context,
					    KeyStash &stash)
{
	Indexer &indexer = context.getIndexer();
	OperationContext &oc = context.getOperationContext();

	// Index the document
	IndexSpecification &index = context.getIndexSpecification();
	index.set(Index::INDEXER_DELETE);

	indexer.indexMetaData(index, document, stash, /*checkModified*/false);
	scoped_ptr<NsPushEventSource8> source(document.
		getContentAsSAX(oc.txn(), /*needsValidation*/false,
			indexer.getContainer()->getIndexNodes(),
			/*NsDocumentDatabase*/0));
	indexer.indexContent(index, document.getID(), source.get(), stash);

	// Delete the content
	deleteID(oc, document.getID()); // a no-op
	document.getID().setDbtFromThis(oc.key());
	int err = content_.del(oc.txn(), &oc.key(), 0);
	
	return err;
}

int DocumentDatabase::reindex(const IndexSpecification &is, const Document &document,
	UpdateContext &context, KeyStash &stash)
{
	Indexer &indexer = context.getIndexer();
	OperationContext &oc = context.getOperationContext();

	// Index the document
	indexer.indexMetaData(is, document, stash, /*checkModified*/false);
	scoped_ptr<NsPushEventSource8> source(document.
		getContentAsSAX(oc.txn(), /*needsValidation*/false,
			indexer.getContainer()->getIndexNodes(),
			/*NsDocumentDatabase*/0));
	indexer.indexContent(is, document.getID(), source.get(), stash);

	return 0;
}

int DocumentDatabase::getAllMetaData(OperationContext &context,
				     DictionaryDatabase *dictionary,
				     Document *document,
				     u_int32_t flags) const
{
	int err = 0;
	u_int32_t orig_flags = flags;
	const ID &did = document->getID();

	//
	// Read each of the meta-data items from the document secondary
	// database.  Content is no longer considered metadata
	//
	Cursor cursor(const_cast<SecondaryDatabase&>(secondary_),
		context.txn(), CURSOR_READ, flags);
	flags = DB_CURSOR_GET_MASK(flags);
	orig_flags = DB_CURSOR_GET_MASK(flags);
	flags |= DB_SET_RANGE;
	bool done = false;
	while (!done) {
		did.setDbtFromThis(context.key());
		DbtIn none;
		none.set_flags(DB_DBT_PARTIAL); // Don't pull back the data.
		// if DB_RMW set, don't get locks on this get, wait for a match
		err = cursor.get(&context.key(), &none, flags & (~DB_RMW));
		if (err == 0) {
			ID db_did, db_nid;
			XmlValue::Type type;
			MetaDatum::decodeKeyDbt(context.key(), db_did,
						db_nid, type);
			if (did == db_did) {
				Name name;
				err = dictionary->lookupName(context, db_nid, name);
				if(err == 0 && !document->containsMetaData(name)) {
					DbtOut *data = new DbtOut();
					err = cursor.get(&context.key(), data,
							 DB_CURRENT);
					if(err == 0) document->setMetaData(
						name, type, &data,
						/*modified*/false);
					delete data; // If not consumed by setThis..
				}
				flags = orig_flags | DB_NEXT;
			} else {
				err = 0;
				done = true;
			}
		} else if (err == DB_NOTFOUND) {
			err = 0;
			done = true;
		} else {
			done = true;
		}
	}
	return err;
}

int DocumentDatabase::getMetaData(OperationContext &context,
	DictionaryDatabase *dictionary, const Name &name,
	const ID &did, XmlValue::Type &type, Dbt *metadata,
	u_int32_t flags) const
{
	ID nid;
	int err = dictionary->lookupName(context, name,
					 nid, /*define=*/false);
	if(err == 0) {
		Cursor cursor(const_cast<SecondaryDatabase&>(secondary_),
			context.txn(), CURSOR_READ, flags);
		u_int32_t origFlags = DB_CURSOR_GET_MASK(flags);
		MetaDatum::setKeyDbt(did, nid, XmlValue::NONE, context.key());
		DbtIn none;
		none.set_flags(DB_DBT_PARTIAL); // Don't pull back the data.
		err = cursor.get(&context.key(), &none,
				 (flags | DB_SET_RANGE) & ~DB_RMW);
		if (err == 0) {
			ID db_did, db_nid;
			MetaDatum::decodeKeyDbt(context.key(), db_did,
						db_nid, type);
			if(db_did == did && db_nid == nid) {
				err = cursor.get(&context.key(), metadata,
						 origFlags|DB_CURRENT);
			} else {
				return DB_NOTFOUND;
			}
		}
	}

	return err;
}

int DocumentDatabase::addMetaData(OperationContext &oc,
				  DictionaryDatabase *dictionary,
				  Document &document)
{
	int err = 0;
	MetaData::const_iterator end = document.metaDataEnd();
	MetaData::const_iterator i;
	for (i = document.metaDataBegin(); err == 0 && i != end; ++i) {
		ID nid;
		err = dictionary->lookupName(oc,
					     (*i)->getName(),
					     nid, /*define=*/true);
		if(err == 0) {
			DbtIn value;
			MetaDatum::setKeyDbt(document.getID(),
					     nid, (*i)->getType(),
					     oc.key());
			(*i)->setValueDbtFromThis(value);
			// could throw on error
			err = secondary_.put(oc.txn(), &oc.key(),
					     &value, 0);
		}
	}
	if(err == 0)
		for(i = document.metaDataBegin(); i != end; ++i)
			(*i)->setModified(false);
	return err;
}

int DocumentDatabase::updateMetaData(OperationContext &oc,
				     DictionaryDatabase *dictionary,
				     Document &document)
{
	int err = 0;
	MetaData::const_iterator end = document.metaDataEnd();
	MetaData::const_iterator i;
	std::vector<ID> toRemove;
	for(i = document.metaDataBegin(); err == 0 && i != end; ++i) {
		if((*i)->isModified()) {
			ID nid;
			err = dictionary->lookupName(oc,
						     (*i)->getName(),
						     nid, /*define=*/true);
			if(err == 0) {
				if ((*i)->isRemoved())
					toRemove.push_back(nid);
				else {
					DbtIn value;
					MetaDatum::setKeyDbt(document.getID(),
							     nid,
							     (*i)->getType(),
							     oc.key());
					(*i)->setValueDbtFromThis(value);
					// could throw on error
					err = secondary_.put(oc.txn(),
							     &oc.key(),
							     &value, 0);
				}
			}
		}
	}
	if (toRemove.size() > 0) {
		err = removeMetaData(oc, document.getID(),
				     &toRemove);
	}
	if(err == 0)
		for(i = document.metaDataBegin(); i != end; ++i)
			(*i)->setModified(false);
	return err;
}

static bool
idInList(const std::vector<ID> &list, const ID &id)
{
	std::vector<ID>::const_iterator it = list.begin();
	while (it != list.end()) {
		if (*it == id)
			return true;
		it++;
	}
	
	return false;
}

// if toRemove is non-null, it specifies a list of Name IDs
// to remove; otherwise remove all metadata for the target document
int DocumentDatabase::removeMetaData(OperationContext &oc,
				     const ID &id,
				     std::vector<ID> *toRemove)
{
	Cursor cursor(const_cast<SecondaryDatabase&>(secondary_), oc.txn(), CURSOR_WRITE);

	DbtIn none;
	none.set_flags(DB_DBT_PARTIAL); // Don't pull back the data.

	id.setDbtFromThis(oc.key());

	ID db_id;
	int err = cursor.get(&oc.key(), &none, DB_SET_RANGE);
	while(err == 0) {
		if (toRemove) {
			ID nm_id;
			XmlValue::Type type;
			MetaDatum::decodeKeyDbt(oc.key(), db_id, nm_id, type);
			if ((id == db_id) && idInList(*toRemove, nm_id))
				cursor.del(0);
		} else {
			db_id.setThisFromDbt(oc.key());
			if (id == db_id)
				cursor.del(0);
		}
		if (id != db_id) // done with document?
			break;
		err = cursor.get(&oc.key(), &none, DB_NEXT);
	}

	if(err == DB_NOTFOUND) {
		err = 0;
	}

	return err;
}

int DocumentDatabase::dump(DbEnv *env, const std::string &name,
			   XmlContainer::ContainerType type,
			   std::ostream *out)
{
	DbWrapper content(env, name, "content_", document_name, 0, 0);
	SecondaryDatabase secondary(env, name, document_name, 0, 0);
	int err = 0;
	if (type == XmlContainer::WholedocContainer) {
		err = Container::writeHeader(content.getDatabaseName(), out);
		if(err == 0)
			err = content.dump(out);
	}
	if(err == 0)
		err = Container::writeHeader(secondary.getDatabaseName(), out);
	if(err == 0)
		err = secondary.dump(out);

	return err;
}

int DocumentDatabase::load(DbEnv *env, const std::string &name,
			   XmlContainer::ContainerType type,
			   std::istream *in, unsigned long *lineno)
{
	DbWrapper content(env, name, "content_", document_name, 0, 0);
	SecondaryDatabase secondary(env, name, document_name, 0, 0);

	int err = 0;
	if (type == XmlContainer::WholedocContainer) {
		// Load primary
		err = Container::verifyHeader(content.getDatabaseName(), in);
		if(err != 0) {
			ostringstream oss;
			oss << "DocumentDatabase::load() invalid database dump file loading '" << name << "'";
			Log::log(env, Log::C_CONTAINER, Log::L_ERROR, oss.str().c_str());
		} else {
			err = content.load(in, lineno);
		}
	}

	// Load secondary
	if(err == 0) {
		err = Container::verifyHeader(secondary.getDatabaseName(), in);
		if(err != 0) {
			ostringstream oss;
			oss << "DocumentDatabase::load() invalid database dump file loading '" << name << "'";
			Log::log(env, Log::C_CONTAINER, Log::L_ERROR, oss.str().c_str());
		} else {
			err = secondary.load(in, lineno);
		}
	}

	return err;
}

int DocumentDatabase::verify(DbEnv *env, const std::string &name,
			     XmlContainer::ContainerType type,
			     std::ostream *out, u_int32_t flags)
{
	DbWrapper content(env, name, "content_", document_name, 0, 0);
	SecondaryDatabase secondary(env, name, document_name, 0, 0);

	int err = 0;
	if (type == XmlContainer::WholedocContainer) {
		if(flags & DB_SALVAGE)
			err = Container::writeHeader(content.getDatabaseName(),
						     out);
		if(err == 0)
			err = content.verify(out, flags);
	}
	if(err == 0 && (flags & DB_SALVAGE))
		err = Container::writeHeader(secondary.getDatabaseName(), out);
	if(err == 0)
		err = secondary.verify(out, flags);
	return err;
}

DocumentDatabaseCursor::DocumentDatabaseCursor(Transaction *txn,
					       DbWrapper &db, u_int32_t flags) :
	cursor_(db, txn, CURSOR_READ, flags),
	cursorFlags_(DB_CURSOR_GET_MASK(flags)),
	done_(false) {
	data_.set_flags(DB_DBT_PARTIAL); // only want keys (for now)
}

int DocumentDatabaseCursor::next(ID &id) {
	int err = 0;
	if (done_) {
		id = 0;
	} else {
		err = cursor_.get(&key_, &data_,
				  cursorFlags_|DB_NEXT_NODUP); // no throw
		if (err == 0)
			id.setThisFromDbt(key_);
		if (err == DB_NOTFOUND || err == DB_KEYEMPTY) {
			err = 0;
			done_ = true;
			id = 0;
		}
	}
	return err;
}

// upgrade from 2.1 (version 4) to 2.2 (version 5)
// Modify keys for both document secondary and content (if wholedoc storage)
// secondary key: docId, nameId, type
// content key: docId
// for both:
// 1.  open old DB and create a new database
// 2.  for each record:
//    o unmarshal old format; marshal in new, and put into new DB
// 3.  remove old DB
// 4.  rename new DB to correct name
static int upgrade22(const std::string &name, Manager &mgr)
{
	int err = 0;
	SecondaryDatabase secondary(mgr.getDbEnv(), name,
				    document_name, 0, 0);
	err = secondary.open(0, /*duplicates*/false, 0, 0);
	SecondaryDatabase newSec(mgr.getDbEnv(), name,
				 "newdoc", secondary.getPageSize(), 0);
	err = newSec.open(0, /*duplicates*/false, DB_CREATE|DB_EXCL, 0);
	// the key is: docId,nodeId,type
	Dbt key;
	Dbt data;
	Cursor curs(secondary, (Transaction*)0, DbXml::CURSOR_READ);
	int ret = 0;
	while ((err == 0) &&
	       (ret = curs.get(&key, &data, DB_NEXT)) == 0) {
		// decode key using pre-2.2 code
		ID did, nid;
		XmlValue::Type type;
		u_int32_t *p = (u_int32_t *)key.get_data();
		u_int32_t id1, id2;
		id1 = *p++;
		id2 = *p++;
		if (Manager::isBigendian()) {
			M_32_SWAP(id1);
			M_32_SWAP(id2);  
		}
		did = id1;            
		nid = id2;
		type = (XmlValue::Type) * ((unsigned char*)p);
		// encode using 2.2. code
		DbtOut newKey;
		MetaDatum::setKeyDbt(did, nid, type, newKey);
		err = newSec.put(0, &newKey, &data, 0);
	}
	curs.close();
	secondary.close(0);
	newSec.close(0);
	if (err == 0) {
		// remove old db, rename new one
		std::string dbname("secondary_");
		std::string tname("secondary_");
		dbname += document_name;
		tname += "newdoc";
		err = mgr.getDbEnv()->dbremove(0, name.c_str(),
					       dbname.c_str(), 0);
		if (err == 0)
			err = mgr.getDbEnv()->dbrename(0, name.c_str(),
						       tname.c_str(),
						       dbname.c_str(), 0);
		if (err != 0)
			return err;
		try {
			DbWrapper content(mgr.getDbEnv(), name,
					  "content_",
					  document_name, 0, 0);
			err = content.open(0, DB_BTREE, 0, 0);
			if (err != 0)
				return err;
			DbWrapper newContent(mgr.getDbEnv(), name,
					     "newcontent_", document_name,
					     content.getPageSize(), 0);
			err = newContent.open(0, DB_BTREE,
					      DB_CREATE|DB_EXCL, 0);
			if (err != 0)
				return err;
			// the key is: docId
			Dbt key;
			Dbt data;
			Cursor curs1(content, (Transaction*)0, DbXml::CURSOR_READ);
			int ret = 0;
			while ((err == 0) &&
			       (ret = curs1.get(&key, &data, DB_NEXT)) == 0) {
				// decode/encode key, rewrite
				u_int32_t *p = (u_int32_t *)key.get_data();
				u_int32_t id1;
				id1 = *p;
				if (Manager::isBigendian())
					M_32_SWAP(id1);
				ID id(id1);
				DbtOut newKey;
				id.setDbtFromThis(newKey);
				err = newContent.put(0, &newKey, &data, 0);
			}
			curs1.close();
			content.close(0);
			newContent.close(0);
			if (err == 0) {
				std::string dbname("content_");
				std::string tname("newcontent_");
				dbname += document_name;
				tname += document_name;
				// remove old db, rename new one
				err = mgr.getDbEnv()->dbremove(
					0, name.c_str(), dbname.c_str(), 0);
				if (err == 0)
					err = mgr.getDbEnv()->dbrename(
						0, name.c_str(), tname.c_str(),
						dbname.c_str(), 0);
			}
		}
		catch (DbException &de) {} // ignore
	}
	return err;
}
// static
void DocumentDatabase::upgrade(const std::string &name, Manager &mgr,
			       int old_version, int current_version)
{
	int err = 0;
	DBXML_ASSERT(old_version < current_version);
	// 2.0 -> 2.1 upgrade, if necessary, must be first
	if ((old_version == VERSION_20) && Manager::isBigendian()) {
		SecondaryDatabase secondary(mgr.getDbEnv(), name,
					    document_name, 0, 0);
		err = secondary.open(0, /*duplicates*/false, 0, 0);
		// byte-swap name ids in document metadata keys.
		// iterate through all records in secondary.
		// the key is: docId,nodeId,type
		Dbt key;
		Dbt data;
		Cursor curs(secondary, (Transaction*)0, DbXml::CURSOR_WRITE);
		int ret = 0;
		while ((err == 0) &&
		       (ret = curs.get(&key, &data, DB_NEXT)) == 0) {
			u_int32_t *p = reinterpret_cast<u_int32_t*>(key.get_data());
			curs.del(0);
			++p;
			M_32_SWAP(*p);
			err = curs.put(&key, &data, DB_KEYFIRST);
		}
	}
	if (current_version == VERSION_22 && err == 0)
		err = upgrade22(name, mgr);
	if (err != 0)
		throw XmlException(
			XmlException::DATABASE_ERROR,
			"Unexpected error upgrading Document DB");
}
