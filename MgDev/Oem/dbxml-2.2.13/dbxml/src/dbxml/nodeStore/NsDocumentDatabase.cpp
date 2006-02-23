//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: NsDocumentDatabase.cpp,v 1.46 2005/11/22 16:39:49 jsnelson Exp $
//

#include <sstream>

#include <dbxml/XmlException.hpp>
#include "NsDocumentDatabase.hpp"
#include "NsDocument.hpp"
#include "NsUtil.hpp"
#include "NsWriter.hpp"
#include "NsEventGenerator.hpp"
#include "NsDom.hpp"
#include "NsConstants.hpp"
#include "../ScopedDbt.hpp"
#include "../Container.hpp"
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
	  nodeStorage_(env, name, "node_", nodestorage_name,
		       pageSize, flags & DB_XA_CREATE)
{
	flags &= ~DB_XA_CREATE;
	int err = 0;
	try {
		// Open the Db object
		nodeStorage_.getDb().set_dup_compare(nsCompareNodes);
		nodeStorage_.getDb().set_flags(DB_DUP|DB_DUPSORT);
		err = nodeStorage_.open(txn, DB_BTREE, flags, mode);
	}
	catch (DbException &e) {
		if(e.get_errno() == EEXIST) {
			throw XmlException(XmlException::CONTAINER_EXISTS, e.what());
		}
		else {
			throw XmlException(e);
		}
	}
	if(err == EEXIST) {
		throw XmlException(XmlException::CONTAINER_EXISTS, db_strerror(err));
	}
	else if(err != 0) {
		throw XmlException(err);
	}
}

NsDocumentDatabase::~NsDocumentDatabase()
{
	// nothing to do
}

// object is donated to caller
int NsDocumentDatabase::createDocumentCursor(
	Transaction *txn, scoped_ptr<DocumentCursor> &cursor, u_int32_t flags) const
{
	DocumentDatabaseCursor *newCursor =
		new DocumentDatabaseCursor(txn, nodeStorage_, flags);
	cursor.reset(newCursor);
	return 0;
}

int NsDocumentDatabase::getContent(OperationContext &context,
				   Document *document, u_int32_t flags) const
{
	// Make output writer
	BufferNsStream8 output;
	NsWriter writer;
	writer.setStream(&output);

	// Make event generator
	NsEventGenerator event_gen(context.txn(),
		const_cast<NsDocumentDatabase*>(this),
		document->getID(), flags, Globals::defaultMemoryManager);

	while(event_gen.nextEvent(&writer)) {}

	DbtOut *data = new DbtOut(output.buffer.getBuffer(),
				  output.buffer.getOccupancy());
	// pass setOnly arg -- this tells Document to not change
	// state, other than to set the contentDbt_
	document->setContentAsDbt(&data, true); // Note: consumes data

	return 0;
}

int NsDocumentDatabase::addContentAndIndex(Document &document,
					   UpdateContext &context,
					   Container &container,
					   KeyStash &stash)
{
	OperationContext &oc = context.getOperationContext();
	Indexer &indexer = context.getIndexer();
	IndexSpecification &index = context.getIndexSpecification();
	int err = 0;

	// If the document does not parse cleanly, it's necessary to
	// perform some cleanup, to keep the document DBs clean.  It's
	// not possible to count on transactions, so do it manually.
	// Cleanup: 1. remove ID from primary. 2.  remove from secondary
	// NOTE: if transactions are being used, a sub-transaction here
	// might be helpful/more efficient.  Think about it.

	// Generate a unique ID for the Document, if not done by caller
	if (document.getID() == 0) {
		err = container.getConfigurationDB()->
			generateID(oc.txn(), document.getIDToSet());
		if(err != 0) return err;
	}

	// Index the document metadata
	indexer.indexMetaData(index, document, stash, false);

	// no-content documents; have ID, but no content
	if (document.getDefinitiveContent() == Document::NONE)
		return 0;

	// Index the document content, and add it to the db
	// deleted by Document::getContentAsSAX()...
	scoped_ptr<NsPushEventSource8> source(document.
		getContentAsSAX(oc.txn(), /*needsValidation*/true, container.getIndexNodes(), this));
	// no-content doc will have null return from getContentAsSAX
	if (source.get() != 0) {
		try {
			indexer.indexContent(index, document.getID(),
					     source.get(), stash);
		}
		catch(XmlException &xe) {
			// Don't clean up a database exception
			if(xe.getExceptionCode() != XmlException::DATABASE_ERROR) {
				// cleanup
				// Unfortunately, once an ID is allocated,it can't be
				// undone. This means if no transactions, bad documents will
				// consume an ID in the DbSequence
				//
				// Remove the content
				source.reset(0); // need to delete cursor in event src
				document.getID().setDbtFromThis(oc.key());
				nodeStorage_.del(oc.txn(), &oc.key(), 0);
			}
			throw;
		}
		catch(DbException &) {
			// Don't clean up a database exception
			throw;
		}
		catch(...) {
			// Remove the content
			source.reset(0); // need to delete cursor in event src
			document.getID().setDbtFromThis(oc.key());
			nodeStorage_.del(oc.txn(), &oc.key(), 0);
			throw;
		}
	}
	if(err == 0)
		document.setContentModified(false);

	return err;
}

int NsDocumentDatabase::updateContentAndIndex(
	Document &new_document, UpdateContext &context,
	KeyStash &stash)
{
	OperationContext &oc = context.getOperationContext();
	ID id = new_document.getID();
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
	if(err != 0) return err;

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
	if(new_document.isContentModified()) {
		scoped_ptr<NsPushEventSource8> source(((Document&)old_document).
			getContentAsSAX(oc.txn(), /*needsValidation*/false,
				indexer.getContainer()->getIndexNodes(),
				/*NsDocumentDatabase*/0));
		indexer.indexContent(delete_index, id, source.get(), stash);
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
				const nid_t *nid = (*it)->getNID();
				oc.data().set(0, nid->idLen + 1);
				unsigned char *idptr = (unsigned char*)oc.data().get_data();
				idptr[0] = NS_PROTOCOL_VERSION;
				NsUtil::nsMarshId(idptr + 1, nid);

				err = delNodeRecord(oc, id, &oc.data());
				if(err != 0) return err;
				break;
			}
			}
		}
		if (!new_document.getNsDocument()->getDb())
			new_document.initNsObjects();
		new_document.getNsDocument()->updateSpecialNodes(oc);

		// Index the new document
		indexer.indexMetaData(index, new_document, stash, true);
		if(new_document.isContentModified()) {
			// Don't specify the NsDocumentDatabase - we aren't adding content
			try {
				scoped_ptr<NsPushEventSource8>
					source(new_document.
					       getContentAsSAX(
						       oc.txn(),
						       /*needsValidation*/false,
						       indexer.getContainer()->getIndexNodes(),
						       /*NsDocumentDatabase*/0)
						);
				indexer.indexContent(index, id,
						     source.get(), stash);
			}
			catch(XmlException &xe) {
				// Don't clean up a database exception
				if(xe.getExceptionCode() != XmlException::DATABASE_ERROR) {
					// Remove the content
					id.setDbtFromThis(oc.key());
					nodeStorage_.del(oc.txn(), &oc.key(), 0);
				}
				throw;
			}
			catch(DbException &) {
				// Don't clean up a database exception
				throw;
			}
			catch(...) {
				// Remove the content
				id.setDbtFromThis(oc.key());
				nodeStorage_.del(oc.txn(), &oc.key(), 0);
				throw;
			}
		}

		// Clear the modifications from the NsDocument
		new_document.getNsDocument()->clearModifications();
	} else {
		//
		// This routine has arranged this way because the old document content
		// has to be removed before we add the new content. Otherwise, we have
		// no way of removing the old content... - jpcs
		//

		// Remove the old content, but only if it's being replaced
		if (new_document.isContentModified()) {
			id.setDbtFromThis(oc.key());
			err = nodeStorage_.del(oc.txn(), &oc.key(), 0);
			if(err != 0) {
				if (err == DB_NOTFOUND)
					err = 0;
				else
					return err;
			}
		}

		// Index the new document, and add the content, metadata first
		indexer.indexMetaData(index, new_document, stash, true);

		if(new_document.isContentModified()) {
			try {
				// deleted by Document::getContentAsSAX()...
				scoped_ptr<NsPushEventSource8>
					source(new_document.
					       getContentAsSAX(
						       oc.txn(),
						       /*needsValidation*/true,
						       indexer.getContainer()->
						       getIndexNodes(), this)
						);
				indexer.indexContent(index, id,
						     source.get(), stash);
			}
			catch(XmlException &xe) {
				// Don't clean up a database exception
				if(xe.getExceptionCode() != XmlException::DATABASE_ERROR) {
					// Remove the content
					id.setDbtFromThis(oc.key());
					nodeStorage_.del(oc.txn(), &oc.key(), 0);
				}
				throw;
			}
			catch(DbException &) {
				// Don't clean up a database exception
				throw;
			}
			catch(...) {
				// Remove the content
				id.setDbtFromThis(oc.key());
				nodeStorage_.del(oc.txn(), &oc.key(), 0);
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
	{
		scoped_ptr<NsPushEventSource8>
			source(document.getContentAsSAX(
				       oc.txn(), /*needsValidation*/false,
				       indexer.getContainer()->getIndexNodes(),
				       /*NsDocumentDatabase*/0)
				);
		indexer.indexContent(index, document.getID(),
				     source.get(), stash);
	}

	// Remove the ID -- cannot "remove" an ID.  They are not
	// reusable, but make the call, just in case that ever
	// changes.
	int err = deleteID(oc, document.getID());
	if(err != 0) return err;

	// Remove the content
	document.getID().setDbtFromThis(oc.key());
	err = nodeStorage_.del(oc.txn(), &oc.key(), 0);

	return err;
}

int NsDocumentDatabase::putNodeRecord(OperationContext &context,
				      const ID &id, const Dbt *data)
{
	// add DB_RMW if transacted
	u_int32_t flags = (context.txn() ? DB_RMW : 0);
	if(Log::isLogEnabled(Log::C_NODESTORE, Log::L_DEBUG)) {
		ostringstream oss;
		int nid_length = NsUtil::nsStringLen((const xmlbyte_t*)data->get_data());
		oss << "updating node did: " << id << ", nid: 0x";
		NsUtil::displayNID(oss, (const char*)data->get_data() + 1, nid_length - 1);
		oss << ", node: ";
		Buffer b(((const xmlbyte_t*)data->get_data()) + nid_length + 1,
			 data->get_size() - nid_length - 1, /*wrapper*/true);
		oss << b.asString();

		Log::log(environment_, Log::C_NODESTORE, Log::L_DEBUG,
			 containerName_.c_str(), oss.str().c_str());
	}

	Cursor cursor(nodeStorage_, context.txn(), CURSOR_WRITE);
	int err = cursor.error();
	if(err != 0) return err;

	//marshId(id, context.key());
	id.setDbtFromThis(context.key());

	// set to the Node ID section of the data
	context.data().set(data->get_data(),
			   NsUtil::nsStringLen((const xmlbyte_t*)data->get_data()) + 1);

	err = cursor.get(&context.key(), &context.data(),
			 flags | DB_GET_BOTH);
	if(err == DB_NOTFOUND) {
		err = cursor.put(&context.key(), const_cast<Dbt*>(data),
				 DB_KEYLAST);
	} else if(err == 0) {
		err = cursor.put(&context.key(), const_cast<Dbt*>(data),
				 DB_CURRENT);
	}

	return err;
}

int NsDocumentDatabase::delNodeRecord(
	OperationContext &context, const ID &id, const Dbt *data)
{
	if(Log::isLogEnabled(Log::C_NODESTORE, Log::L_DEBUG)) {
		ostringstream oss;
		int nid_length = NsUtil::nsStringLen((const xmlbyte_t*)data->get_data());
		oss << "deleting node did: " << id << ", nid: ";
		NsUtil::displayNID(oss, (const char*)data->get_data() + 1 , nid_length - 1);
		Log::log(environment_, Log::C_NODESTORE, Log::L_DEBUG,
			 containerName_.c_str(), oss.str().c_str());
	}

	Cursor cursor(nodeStorage_, context.txn(), CURSOR_WRITE);
	int err = cursor.error();
	if(err != 0) return err;
	// add DB_RMW if transacted
	u_int32_t flags = (context.txn()) ? DB_RMW|DB_GET_BOTH : DB_GET_BOTH;

	id.setDbtFromThis(context.key());
	err = cursor.get(&context.key(), const_cast<Dbt*>(data), flags);
	if(err != 0) return err;

	err = cursor.del(0);

	return err;
}

#define STACK_BUF_SIZE 512

int NsDocumentDatabase::putNodeRecord(
	OperationContext &context, const ID &id, const nsNode_t *node)
{
	unsigned char stackBuf[STACK_BUF_SIZE]; // avoid malloc/free

	unsigned char *buf = stackBuf;
	int bufsize = NsUtil::nsMarshalNode(node, NULL, true);
	if (bufsize > STACK_BUF_SIZE)
		buf = (unsigned char *) malloc(bufsize);
	NsUtil::nsMarshalNode(node, buf, false);

	Dbt data(buf, bufsize);
	int err = putNodeRecord(context, id, &data);

	if (bufsize > STACK_BUF_SIZE)
		free(buf);

	return err;
}

int NsDocumentDatabase::getNodeRecord(
	OperationContext &context, const ID &id,
	Dbt *data, u_int32_t flags) const
{
	//marshId(id, context.key());
	id.setDbtFromThis(context.key());
	return const_cast<DbWrapper&>(nodeStorage_).get(context.txn(),
		&context.key(), data, flags | DB_GET_BOTH);
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
		nodeStorage.getDb().set_dup_compare(nsCompareNodes);
		nodeStorage.getDb().set_flags(DB_DUP|DB_DUPSORT);

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
	if (current_version == VERSION_22) {
		try {
			DbWrapper nodeStorage(mgr.getDbEnv(), name, "node_",
					      nodestorage_name, 0, 0);
			nodeStorage.getDb().set_dup_compare(nsCompareNodes);
			nodeStorage.getDb().set_flags(DB_DUP|DB_DUPSORT);
			err = nodeStorage.open(0, DB_BTREE, 0, 0);
			DbWrapper newNs(mgr.getDbEnv(), name,
					"newns_", nodestorage_name,
					nodeStorage.getPageSize(), 0);
			newNs.getDb().set_dup_compare(nsCompareNodes);
			newNs.getDb().set_flags(DB_DUP|DB_DUPSORT);
			err = newNs.open(0, DB_BTREE,
					 DB_CREATE|DB_EXCL, 0);
			Dbt key;
			Dbt data;
			Cursor curs(nodeStorage, (Transaction*)0, DbXml::CURSOR_READ);
			int ret = 0;
			while ((err == 0) &&
			       (ret = curs.get(&key, &data, DB_NEXT)) == 0) {
				// decode/encode key, rewrite
				u_int32_t *p = (u_int32_t *)key.get_data();
				u_int32_t id1;
				id1 = *p;
				if (Manager::isBigendian())
					M_32_SWAP(id1);
				ID id(id1);
				DbtOut newKey;
				id.setDbtFromThis(newKey);
				err = newNs.put(0, &newKey, &data, 0);
			}
			curs.close();
			newNs.close(0);
			nodeStorage.close(0);
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
	if (err != 0)
		throw XmlException(
			XmlException::DATABASE_ERROR,
			"Unexpected error upgrading NsDocument DB");
}
