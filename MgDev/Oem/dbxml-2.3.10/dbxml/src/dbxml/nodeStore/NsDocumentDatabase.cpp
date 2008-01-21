//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsDocumentDatabase.cpp,v 1.67.2.1 2007/01/09 22:32:48 gmf Exp $
//

#include <sstream>

#include <dbxml/XmlException.hpp>
#include "../Container.hpp"
#include "NsDocumentDatabase.hpp"
#include "NsDocument.hpp"
#include "NsUtil.hpp"
#include "NsWriter.hpp"
#include "NsEventReader.hpp"
#include "NsEventWriter.hpp"
#include "EventReaderToWriter.hpp"
#include "NsDom.hpp"
#include "NsConstants.hpp"
#include "NsFormat.hpp"
#include "NsUpgrade.hpp"
#include "../ScopedDbt.hpp"
#include "../OperationContext.hpp"
#include "../Document.hpp"
#include "../Cursor.hpp"
#include "../UpdateContext.hpp"
#include "../KeyStash.hpp"
#include "../Manager.hpp"
#include "../Globals.hpp"
#include "../db_utils.h"

using namespace DbXml;
using namespace std;

static const char *nodestorage_name = "nodestorage";

NsDocumentDatabase::NsDocumentDatabase(
	DbEnv *env, Transaction *txn, const std::string &name,
	u_int32_t pageSize, u_int32_t flags, int mode)
	: DocumentDatabase(env, txn, name, XmlContainer::NodeContainer,
			   pageSize, flags, mode),
	  dbIsOwned_(true)
{
	flags &= ~DB_XA_CREATE;
	int err = 0;
	try {
		nodeStorage_ = new DbWrapper(
			env, name, "node_", nodestorage_name,
			pageSize, flags & DB_XA_CREATE);
		if (!nodeStorage_)
			throw XmlException(XmlException::NO_MEMORY_ERROR,
					   "Error opening container");
		// Open the Db object
		nodeStorage_->getDb().set_bt_compare(lexicographical_bt_compare);
		err = nodeStorage_->open(txn, DB_BTREE, flags, mode);
	}
	catch (DbException &e) {
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
		string msg = name;
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

// this is a private method, only used for read-only access for upgrade
NsDocumentDatabase::NsDocumentDatabase(
	DbEnv *env, const std::string &name, DbWrapper *db)
	: DocumentDatabase(env, name, XmlContainer::NodeContainer),
	  nodeStorage_(db), // use DbWrapper that's passed in
	  dbIsOwned_(false)
{
	// nothing to do
}

NsDocumentDatabase::~NsDocumentDatabase()
{
	if (dbIsOwned_)
		delete nodeStorage_;
}

class NsDocumentDatabaseCursor : public DocumentCursor
{
public:
	NsDocumentDatabaseCursor(Transaction *txn, DbWrapper &db, u_int32_t flags)
		: cursor_(db, txn, CURSOR_READ, flags),
		  cursorFlags_(DB_CURSOR_GET_MASK(flags)),
		  done_(false)
	{
		data_.set_flags(DB_DBT_PARTIAL); // only want keys (for now)
	}
	virtual int first(DocID &id)
	{
		return next(id);
	}
	virtual int next(DocID &id)
	{
		int err = 0;
		if(done_) {
			id = 0;
		} else {
			while((err = cursor_.get(&key_, &data_, cursorFlags_|DB_NEXT)) == 0) {
				id.setThisFromDbt(key_);
				if(id != last_) {
					last_ = id;
					break;
				}
			}
			if (err == DB_NOTFOUND || err == DB_KEYEMPTY) {
				err = 0;
				done_ = true;
				id = 0;
			}
		}
		return err;
	}
private:
	Cursor cursor_;
	u_int32_t cursorFlags_;
	bool done_;
	DocID last_;
	DbtOut key_;
	DbtOut data_;
};

// object is donated to caller
int NsDocumentDatabase::createDocumentCursor(
	Transaction *txn, ScopedPtr<DocumentCursor> &cursor, u_int32_t flags) const
{
	NsDocumentDatabaseCursor *newCursor =
		new NsDocumentDatabaseCursor(txn, *nodeStorage_, flags);
	cursor.reset(newCursor);
	return 0;
}

int NsDocumentDatabase::getContent(OperationContext &context,
				   Document *document, u_int32_t flags) const
{
	// Make output writer
	BufferNsStream output;
	NsWriter writer(&output);

	// Make an event reader and serialize from it
	NsEventReader reader(context.txn(),
			     const_cast<NsDocumentDatabase*>(this),
			     document->getContainer()->getDictionaryDB(),
			     document->getID(), flags,
			     Globals::defaultMemoryManager);

	writer.writeFromReader(reader);

	// donate memory from buffer to avoid copy into DbtOut
	DbtOut *data = new DbtOut();
	data->setNoCopy(output.buffer.donateBuffer(),
			output.buffer.getOccupancy());
	
	// pass setOnly arg -- this tells Document to not change
	// state, other than to set the contentDbt_
	document->setContentAsDbt(&data, true); // Note: consumes data

	return 0;
}

int NsDocumentDatabase::deleteAllNodes(OperationContext &context,
	const DocID &did)
{
	Cursor cursor(*nodeStorage_, context.txn(), CURSOR_WRITE, 0);
	if(cursor.error() != 0) return cursor.error();

	DbtOut data;
	data.set_flags(DB_DBT_PARTIAL); // only want keys

	NsFormat::marshalNodeKey(did, 0, context.key());
	int err = cursor.get(&context.key(), &data, DB_SET_RANGE);

	DocID tmp;
	while(err == 0) {
		tmp.setThisFromDbt(context.key());
		if(tmp != did) {
			break;
		}

		err = cursor.del(0);
		if(err == 0)
			err = cursor.get(&context.key(), &data, DB_NEXT);
	}

	if(err == DB_NOTFOUND)
		err = 0;

	return err;
}

void NsDocumentDatabase::addContentException(Document &document,
					      UpdateContext &context,
					      bool isDbExc)
{
	if (!isDbExc) {
		deleteAllNodes(context.getOperationContext(), document.getID());
	}
}


int NsDocumentDatabase::updateContentAndIndex(
	Document &new_document, UpdateContext &context,
	KeyStash &stash)
{
	OperationContext &oc = context.getOperationContext();
	DocID id = new_document.getID();
	Indexer &indexer = context.getIndexer();
	IndexSpecification &index = context.getIndexSpecification();
	int err = 0;
	bool resetId = false;

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

	if (err != 0) return err;

	// Set the modified flags of the old document to the same as the
	// new document, so that when we index, we only generate keys for
	// things that have actually changed.
	MetaData::const_iterator end = new_document.metaDataEnd();
	for(MetaData::const_iterator i = new_document.metaDataBegin(); i != end; ++i) {
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
	if(new_document.isContentModified() && index.isContentIndexed()) {
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

	if(new_document.getDefinitiveContent() == Document::DOM &&
	   new_document.getNsDocument() != 0) {
		NodeModifications &mods = new_document.getNsDocument()->getModifications();

		// Apply the content modifications to the database
		NodeModifications::iterator end = mods.end();
		NodeModifications::iterator begin = mods.begin();
		NodeModifications::iterator it;
		for(it = begin; it != end; ++it) {
			switch((*it)->getAction()) {
			case NodeModification::UPDATE:
			case NodeModification::ADD: {
				err = putNodeRecord(oc, id, (*it)->getNode()->getNsNode());
				if(err != 0) return err;
				break;
			}
			case NodeModification::REMOVE: {
				err = delNodeRecord(oc, id, (*it)->getNID());
				if(err != 0) return err;
				break;
			}
			}
		}
		if (!new_document.getNsDocument()->getDb())
			new_document.initNsObjects();
		new_document.getNsDocument()->updateSpecialNodes(oc);

		// Clear the modifications from the NsDocument
		new_document.getNsDocument()->clearModifications();
		
		indexer.indexMetaData(index, new_document, stash, true);
		if(new_document.isContentModified() &&
		   index.isContentIndexed()) {
			// Don't specify the NsDocumentDatabase,
			// we aren't adding content.  Also, force the use
			// of the ID for the indexer, rather than using
			// definitiveContent_, which is DOM.  This is more
			// efficient, and less memory-intensive.  See
			// "true" argument to Document.getContentAsEventSource.
			try {
				ScopedPtr<NsPushEventSource>
					source(new_document.
					       getContentAsEventSource(
						       oc.txn(),
						       /*needsValidation*/false,
						       indexer.getContainer()->getIndexNodes(), true));
				if (source.get()) {
					indexer.initIndexContent(
						index, id,
						source.get(),
						stash, false);
					source.get()->start();
				}
			}
			catch(XmlException &xe) {
				// Don't clean up a database exception
				if(xe.getExceptionCode() != XmlException::DATABASE_ERROR) {
					// Remove the content
					deleteAllNodes(oc, id);
				}
				throw;
			}
			catch(DbException &) {
				// Don't clean up a database exception
				throw;
			}
			catch(...) {
				// Remove the content
				deleteAllNodes(oc, id);
				throw;
			}
		}
	} else {
		//
		// This routine has arranged this way because the old document
		// content has to be removed before we add the new content.
		// Otherwise, we have no way of removing the
		// old content... - jpcs
		//

		// Remove the old content, but only if it's being replaced
		if (new_document.isContentModified()) {
			err = deleteAllNodes(oc, id);
			if(err != 0) {
				return err;
			}
		}

		// Index the new document, and add the content, metadata first
		indexer.indexMetaData(index, new_document, stash, true);

		// If the there are no content indexes, the parse still
		// needs to happen for unparsed content (DBT or INPUTSTREAM)
		if (new_document.isContentModified() &&
		    (index.isContentIndexed() ||
		     ((new_document.getDefinitiveContent() == Document::DBT) ||
		      (new_document.getDefinitiveContent() == Document::INPUTSTREAM)))) {
			try {
				// deleted by Document::getContentAsEventSource()...
				ScopedPtr<NsPushEventSource>
					source(new_document.
					       getContentAsEventSource(
						       oc.txn(),
						       /*needsValidation*/true,
						       indexer.getContainer()->
						       getIndexNodes()));
				if (source.get()) {
					indexer.initIndexContent(
						index, id,
						source.get(),
						stash, false);
					source.get()->start();
				}
			}
			catch(XmlException &xe) {
				// Don't clean up a database exception
				if(xe.getExceptionCode() != XmlException::DATABASE_ERROR) {
					// Remove the content
					deleteAllNodes(oc, id);
				}
				throw;
			}
			catch(DbException &) {
				// Don't clean up a database exception
				throw;
			}
			catch(...) {
				// Remove the content
				deleteAllNodes(oc, id);
				throw;
			}
		}
	}

	if(err == 0) new_document.setContentModified(false);

	if (resetId)
		new_document.getIDToSet() = 0;

	return err;
}

int NsDocumentDatabase::removeContentAndIndex(
	const Document &document, UpdateContext &context,
	KeyStash &stash)
{
	Indexer &indexer = context.getIndexer();
	OperationContext &oc = context.getOperationContext();

	// Index the document
	IndexSpecification &index = context.getIndexSpecification();
	index.set(Index::INDEXER_DELETE);

	indexer.indexMetaData(index, document, stash, /*checkModified*/false);

	// put this in its own block to destroy the event source in a
	// timely manner.  It holds a cursor to the document.
	if (index.isContentIndexed()) {
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
	}

	// Remove the ID -- cannot "remove" an ID.  They are not
	// reusable, but make the call, just in case that ever
	// changes.
	int err = deleteID(oc, document.getID());
	if(err != 0) return err;

	// Remove the content
	err = deleteAllNodes(oc, document.getID());
	
	return err;
}

int NsDocumentDatabase::putNodeRecord(OperationContext &context,
	const DocID &did, const NsNid *nid, const Dbt *data)
{
	if(Log::isLogEnabled(Log::C_NODESTORE, Log::L_DEBUG)) {
		ostringstream oss;
		oss << "updating node did: " << did.asString() << ", nid: 0x";
		nid->displayNid(oss);
		oss << ", node: ";
		Buffer b((const xmlbyte_t*)data->get_data(), data->get_size(), /*wrapper*/true);
		oss << b.asString();

		Log::log(environment_, Log::C_NODESTORE, Log::L_DEBUG,
			 getContainerName().c_str(), oss.str().c_str());
	}

	NsFormat::marshalNodeKey(did, nid, context.key());
	return nodeStorage_->put(context.txn(), &context.key(), const_cast<Dbt*>(data), 0);
}

int NsDocumentDatabase::delNodeRecord(OperationContext &context,
	const DocID &did, const NsNid *nid)
{
	if(Log::isLogEnabled(Log::C_NODESTORE, Log::L_DEBUG)) {
		ostringstream oss;
		oss << "deleting node did: " << did.asString() << ", nid: ";
		nid->displayNid(oss);
		Log::log(environment_, Log::C_NODESTORE, Log::L_DEBUG,
			getContainerName().c_str(), oss.str().c_str());
	}

	NsFormat::marshalNodeKey(did, nid, context.key());
	return nodeStorage_->del(context.txn(), &context.key(), 0);
}

#define STACK_BUF_SIZE 512

class NodeMarshalBuffer {
public:
	NodeMarshalBuffer(const NsNode *node)
	{
		const NsFormat &fmt = *node->getFormat();

		data.set_data(stackBuf);
		data.set_size(fmt.marshalNodeData(node, NULL, true));

		if(data.get_size() > STACK_BUF_SIZE) {
			data.set_data(malloc(data.get_size()));
			if(!data.get_data())
				NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					"NodeMarshalBuffer::NodeMarshalBuffer",
					__FILE__, __LINE__);
		}
		fmt.marshalNodeData(node, (unsigned char*)data.get_data(), false);
	}

	~NodeMarshalBuffer()
	{
		if(data.get_size() > STACK_BUF_SIZE)
			free(data.get_data());
	}

	Dbt data;
	unsigned int stackBuf[STACK_BUF_SIZE];
};

int NsDocumentDatabase::putNodeRecord(
	OperationContext &context, const DocID &id, const NsNode *node)
{
	NodeMarshalBuffer nmb(node);
	return putNodeRecord(context, id, const_cast<NsNode*>(node)->getNid(),
		&nmb.data);
}

int NsDocumentDatabase::getNodeRecord(OperationContext &context,
	const DocID &did, const NsNid *nid, Dbt *data, u_int32_t flags) const
{
	NsFormat::marshalNodeKey(did, nid, context.key());

	int err = nodeStorage_->get(context.txn(), &context.key(), data, flags);
	if (err == DB_LOCK_DEADLOCK) {
		throw XmlException(DB_LOCK_DEADLOCK, __FILE__, __LINE__);
	}
	return err;
}

int NsDocumentDatabase::getNextNodeRecord(OperationContext &context,
	const DocID &did, const NsNid *nid, Dbt *data, u_int32_t flags) const
{
	Cursor cursor(*nodeStorage_, context.txn(), CURSOR_READ, 0);
	if(cursor.error() != 0) return cursor.error();

	// Marshal a key that comes directly after
	// the given node ID
	NsFormat::marshalNextNodeKey(did, nid, context.key());

	// Find the next node record
	int err = cursor.get(&context.key(), data, DB_SET_RANGE);

	if (err == DB_LOCK_DEADLOCK) {
		throw XmlException(DB_LOCK_DEADLOCK, __FILE__, __LINE__);
	}
	return err;
}

int NsDocumentDatabase::dump(DbEnv *env, const std::string &name,
			     std::ostream *out)
{
	int err = DocumentDatabase::dump(env, name,
					 XmlContainer::NodeContainer, out);

	if(err == 0) {
		DbWrapper nodeStorage(env, name, "node_",
				      nodestorage_name, 0, 0);
		err = Container::writeHeader(nodeStorage.getDatabaseName(),
					     out);
		if(err == 0) err = nodeStorage.dump(out);
	}

	return err;
}

int NsDocumentDatabase::load(DbEnv *env, const std::string &name,
			     std::istream *in, unsigned long *lineno)
{
	int err = DocumentDatabase::load(env, name,
					 XmlContainer::NodeContainer,
					 in, lineno);

	if(err == 0) {
		DbWrapper nodeStorage(env, name, "node_",
				      nodestorage_name, 0, 0);
		nodeStorage.getDb().set_bt_compare(lexicographical_bt_compare);

		err = Container::verifyHeader(
			nodeStorage.getDatabaseName(), in);
		if(err != 0) {
			ostringstream oss;
			oss << "NsDocumentDatabase::load() invalid database dump file loading '" << name << "'";
			Log::log(env, Log::C_CONTAINER, Log::L_ERROR, oss.str().c_str());
		}
		else {
			err = nodeStorage.load(in, lineno);
		}
	}

	return err;
}

int NsDocumentDatabase::verify(DbEnv *env, const std::string &name,
			       std::ostream *out, u_int32_t flags)
{
	int err = DocumentDatabase::verify(env, name,
					   XmlContainer::NodeContainer,
					   out, flags);

	if(err == 0) {
		DbWrapper nodeStorage(env, name, "node_", nodestorage_name, 0, 0);

		if(flags & DB_SALVAGE)
			err = Container::writeHeader(
				nodeStorage.getDatabaseName(), out);
		if(err == 0) err = nodeStorage.verify(out, flags);
	}

	return err;
}

// upgrade from 2.0 (version 3) or 2.1 (version 4) to 2.2 (version 5)
// Modify all keys for node storage database
// content key: docId
// 1.  open old DB and create a new database
// 2.  for each record:
//    o unmarshal old format; marshal in new, and put into new DB
// 3.  remove old DB
// 4.  rename new DB to correct name
// static
void NsDocumentDatabase::upgrade(const std::string &name, Manager &mgr,
				 int old_version, int current_version)
{
	DBXML_ASSERT(old_version < current_version);
	int err = 0;
	if (old_version < VERSION_22) {
		// do 2.2 upgrade
		try {
			DbWrapper oldDb(mgr.getDbEnv(), name, "node_",
					      nodestorage_name, 0, 0);
			oldDb.getDb().set_dup_compare(nsCompareNodes);
			oldDb.getDb().set_flags(DB_DUP|DB_DUPSORT);
			err = oldDb.open(0, DB_BTREE, 0, 0);
			DbWrapper newDb(mgr.getDbEnv(), name,
					"newns_", nodestorage_name,
					oldDb.getPageSize(), 0);
			newDb.getDb().set_dup_compare(nsCompareNodes);
			newDb.getDb().set_flags(DB_DUP|DB_DUPSORT);
			err = newDb.open(0, DB_BTREE,
					 DB_CREATE|DB_EXCL, 0);
			Dbt key;
			Dbt data;
			Cursor curs(oldDb, (Transaction*)0, DbXml::CURSOR_READ);
			int ret = 0;
			while ((err == 0) &&
			       (ret = curs.get(&key, &data, DB_NEXT)) == 0) {
				// decode/encode key, rewrite
				u_int32_t *p = (u_int32_t *)key.get_data();
				u_int32_t id1;
				id1 = *p;
				if (Manager::isBigendian())
					M_32_SWAP(id1);
				DocID id(id1);
				DbtOut newKey;
				id.setDbtFromThis(newKey);
				err = newDb.put(0, &newKey, &data, 0);
			}
			curs.close();
			newDb.close(0);
			oldDb.close(0);
			if (err == 0) {
				std::string dbname("node_");
				std::string tname("newns_");
				dbname += nodestorage_name;
				tname += nodestorage_name;
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
	if (old_version < VERSION_23) {
		upgrade23(name, mgr);
	}
	if (err != 0)
		throw XmlException(
			XmlException::DATABASE_ERROR,
			"Unexpected error upgrading NsDocument DB");
}

//
// 2.2->2.3 upgrade
// This looks a lot like the function above, but with a more
// complex interior:
// 1.  open old db (using old dup compare, if different)
// 2.  create new db (using new dup compare, if different)
// 3.  move all old docs into new database (this is the tricky part)
// 4.  remove old database
// 5.  rename new db to old name
// 6.  In caller: run compaction to squeeze out unused space, after reindexing
static void copyAllDocs(NsDocumentDatabase &oldDb,
			NsDocumentDatabase  &newDb, Manager &mgr);

void NsDocumentDatabase::upgrade23(const std::string &name, Manager &mgr)
{
	DbWrapper oldDb(mgr.getDbEnv(), name, "node_",
			      nodestorage_name, 0, 0);
	oldDb.getDb().set_dup_compare(nsCompareNodes);
	oldDb.getDb().set_flags(DB_DUP|DB_DUPSORT);
	int err = oldDb.open(0, DB_BTREE, 0, 0);

	NsDocumentDatabase oldNs(mgr.getDbEnv(), name, &oldDb);

	DbWrapper newDb(mgr.getDbEnv(), name,
			"newns_", nodestorage_name,
			oldDb.getPageSize(), 0);

	newDb.getDb().set_bt_compare(lexicographical_bt_compare);
	err = newDb.open(0, DB_BTREE, DB_CREATE|DB_EXCL, 0);

	NsDocumentDatabase newNs(mgr.getDbEnv(), name, &newDb);
	//
	// copy all old docs to new db
	//
	copyAllDocs(oldNs, newNs, mgr);
	
	newDb.close(0);
	oldDb.close(0);
	if (err == 0) {
		std::string dbname("node_");
		std::string tname("newns_");
		dbname += nodestorage_name;
		tname += nodestorage_name;
		// remove old db, rename new one
		err = mgr.getDbEnv()->dbremove(
			0, name.c_str(), dbname.c_str(), 0);
		if (err == 0)
			err = mgr.getDbEnv()->dbrename(
				0, name.c_str(), tname.c_str(),
				dbname.c_str(), 0);
	}
	
	if (err != 0)
		throw XmlException(
			XmlException::DATABASE_ERROR,
			"upgrade23: unexpected error upgrading NsDocument DB");
}

static
void copyAllDocs(NsDocumentDatabase &oldDb,
		 NsDocumentDatabase &newDb, Manager &mgr)
{
	ScopedPtr<DocumentCursor> cursor;
	oldDb.createDocumentCursor(0, cursor, 0);
	DocID did;
	
	// 2.3 node storage databases require a dictionary, which
	// *must* be already upgraded.  Default everything, no txn.
	DictionaryDatabase ddb(mgr.getDbEnv(), 0, newDb.getContainerName(),
			       0, 0, 0);

	int err = cursor->first(did);
	DBXML_ASSERT(err == 0 || did == 0);
	while (did != 0) {
		NsUpgradeReader reader(oldDb.getNodeStorageDatabase(), did,
				       Globals::defaultMemoryManager);
		NsEventWriter writer(&newDb, &ddb, did,
				     Globals::defaultMemoryManager);
		EventReaderToWriter r2w(reader, writer, false);
		r2w.start();
		cursor->next(did);
	}
}
