//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DocumentDatabase.cpp,v 1.47.2.1 2007/01/09 22:32:49 gmf Exp $
//

#include <sstream>

#include <dbxml/XmlException.hpp>
#include "Container.hpp"
#include "DocumentDatabase.hpp"
#include "Manager.hpp"
#include "ScopedDbt.hpp"
#include "OperationContext.hpp"
#include "Document.hpp"
#include "Cursor.hpp"
#include "UpdateContext.hpp"
#include "NameID.hpp"
#include "db_utils.h"
#include "nodeStore/NsEvent.hpp"

using namespace DbXml;
using namespace std;
static const char *document_name = "document";

DocumentDatabase::DocumentDatabase(DbEnv *env, Transaction *txn,
				   const std::string &name,
				   XmlContainer::ContainerType type,
				   u_int32_t pageSize, u_int32_t flags,
				   int mode)
	: environment_(env),
	  name_(name),
	  type_(type),
	  content_(env, name, "content_", document_name,
		   pageSize, flags & DB_XA_CREATE),
	  secondary_(env, name, document_name, pageSize,
		     flags & DB_XA_CREATE)
{
	flags &= ~DB_XA_CREATE;
	open(txn, flags, mode);
}

DocumentDatabase::DocumentDatabase(DbEnv *env, const std::string &name,
				   XmlContainer::ContainerType type)
	: environment_(env),
	  name_(name),
	  type_(type),
	  content_(env, name, "content_", document_name, 0, 0),
	  secondary_(env, name, document_name, 0, 0)
{
	open(0, 0, 0);
}

void
DocumentDatabase::open(Transaction *txn, u_int32_t flags, int mode)
{
	int err = 0;
	try {
		// Open the Db objects
		if (type_ == XmlContainer::WholedocContainer) {
			err = content_.open(txn, DB_BTREE, flags, mode);
		}
		if (err == 0)
			err = secondary_.open(txn, /*duplicates*/false,
					      flags, mode);
	} catch (DbException &e) {
		err = e.get_errno();
		// fall through
	}
	catch (XmlException &xe) {
		if (txn)
			txn->abort();
		throw;
	}
	if (err != 0) {
		if (txn)
			txn->abort();
		string msg = name_;
		if (err == EEXIST) {
			msg += ": container exists";
			throw XmlException(
				XmlException::CONTAINER_EXISTS, msg);
		} else if (err == ENOENT) {
			msg += ": container file not found, or not a container";
			throw XmlException(XmlException::CONTAINER_NOT_FOUND,
					   msg);
		}
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
	Transaction *txn, ScopedPtr<DocumentCursor> &cursor, u_int32_t flags) const
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
int DocumentDatabase::deleteID(OperationContext &context, const DocID& oldId)
{
	return 0; // this is a no-op; IDs are never reused
}


int DocumentDatabase::addContent(Document &document, UpdateContext &context)
{
	OperationContext &oc = context.getOperationContext();
	
	Dbt *dbt = (Dbt*) document.getContentAsDbt();
	// Allow no-content documents
	int err = 0;
	if (dbt && (dbt->get_size() != 0)) {
		document.getID().setDbtFromThis(oc.key());
		err = content_.put(oc.txn(), &oc.key(),
			dbt, 0);
	}
	return err;
}

int DocumentDatabase::updateContentAndIndex(Document &new_document,
					    UpdateContext &context,
					    KeyStash &stash)
{
	OperationContext &oc = context.getOperationContext();
	DocID id = new_document.getID();
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
		ScopedPtr<NsPushEventSource>
			source(new_document.
			       getContentAsEventSource(
				       oc.txn(), /*needsValidation*/true,
				       indexer.getContainer()->getIndexNodes()));
		if (source.get()) {
			indexer.initIndexContent(index, id,
						 source.get(), stash,
						 true);
			source.get()->start();
		}
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
		ScopedPtr<NsPushEventSource>
			source(((Document&)old_document).
			       getContentAsEventSource(
				       oc.txn(),
				       /*needsValidation*/false,
				       indexer.getContainer()->getIndexNodes()));
		if (source.get()) {
			indexer.initIndexContent(delete_index, id,
						 source.get(), stash,
						 false);
			source.get()->start();
		}
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
	ScopedPtr<NsPushEventSource>
		source(document.getContentAsEventSource(
			       oc.txn(), /*needsValidation*/false,
			       indexer.getContainer()->getIndexNodes()));
	if (source.get()) {
		indexer.initIndexContent(index, document.getID(),
					 source.get(), stash,
					 false);
		source.get()->start();
	}

	// Delete the content
	deleteID(oc, document.getID()); // a no-op
	document.getID().setDbtFromThis(oc.key());
	int err = content_.del(oc.txn(), &oc.key(), 0);
	if (err == DB_NOTFOUND)
		err = 0; // no-content doc
	
	return err;
}

int DocumentDatabase::reindex(const IndexSpecification &is, const Document &document,
	UpdateContext &context, KeyStash &stash)
{
	Indexer &indexer = context.getIndexer();
	OperationContext &oc = context.getOperationContext();

	// Index the document
	indexer.indexMetaData(is, document, stash, /*checkModified*/false);
	ScopedPtr<NsPushEventSource> source(document.
		getContentAsEventSource(oc.txn(), /*needsValidation*/false,
					indexer.getContainer()->getIndexNodes()));
	if (source.get()) {
		indexer.initIndexContent(is, document.getID(),
					 source.get(), stash, false);
		source.get()->start();
	}

	return 0;
}

int DocumentDatabase::getAllMetaData(OperationContext &context,
				     DictionaryDatabase *dictionary,
				     Document *document,
				     u_int32_t flags) const
{
	int err = 0;
	u_int32_t orig_flags = flags;
	const DocID &did = document->getID();

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
			DocID db_did;
			NameID db_nid;
			XmlValue::Type type;
			MetaDatum::decodeKeyDbt(context.key(), db_did,
						db_nid, type);
			if (did == db_did) {
				Name name;
				err = dictionary->lookupNameFromID(context, db_nid, name);
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
	const DocID &did, XmlValue::Type &type, Dbt *metadata,
	u_int32_t flags) const
{
	NameID nid;
	int err = dictionary->lookupIDFromName(context, name,
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
			DocID db_did;
			NameID db_nid;
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
		NameID nid;
		err = dictionary->lookupIDFromName(oc,
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
	std::vector<NameID> toRemove;
	for(i = document.metaDataBegin(); err == 0 && i != end; ++i) {
		if((*i)->isModified()) {
			NameID nid;
			err = dictionary->lookupIDFromName(oc,
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
idInList(const std::vector<NameID> &list, const NameID &id)
{
	std::vector<NameID>::const_iterator it = list.begin();
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
				     const DocID &id,
				     std::vector<NameID> *toRemove)
{
	Cursor cursor(const_cast<SecondaryDatabase&>(secondary_), oc.txn(),
		      CURSOR_WRITE);

	DbtIn none;
	none.set_flags(DB_DBT_PARTIAL); // Don't pull back the data.

	id.setDbtFromThis(oc.key());

	DocID db_id;
	int err = cursor.get(&oc.key(), &none, DB_SET_RANGE);
	while(err == 0) {
		if (toRemove) {
			NameID nm_id;
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

int DocumentDatabaseCursor::next(DocID &id) {
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
		DocID did;
		NameID nid;
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
			if (err != 0) {
				// ignore ENOENT
				if (err == ENOENT)
					err = 0;
				return err;
			}
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
				DocID id(id1);
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

// determine if the name ID needs swapping.
// If the container was created big-endian, then yes.
// This is detected by finding *little-endian* values
// that are very large. Note that this could have issues
// if there is a *HUGE* number of metadata names, which
// is very unlikely
static bool needsSwap(u_int32_t val)
{
	bool be = Manager::isBigendian();
	if (be) {
		if (val <= 0x00010000)
			return true;
	} else {
		if (val >= 0x00010000)
			return true;
	}
	return false;
	
}

// static
void DocumentDatabase::upgrade(const std::string &name, Manager &mgr,
			       int old_version, int current_version)
{
	int err = 0;
	DBXML_ASSERT(old_version < current_version);
	// 2.0 -> 2.1 upgrade, if necessary, must be first
	if (old_version == VERSION_20){
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
			++p;
			// swap happens on second int, not first
			if (!needsSwap(*p))
				break;
			M_32_SWAP(*p);
			curs.del(0);
			err = curs.put(&key, &data, DB_KEYFIRST);
		}
	}
	if (old_version < VERSION_22 && err == 0)
		err = upgrade22(name, mgr);
	if (err != 0)
		throw XmlException(
			XmlException::DATABASE_ERROR,
			"Unexpected error upgrading Document DB");
}
