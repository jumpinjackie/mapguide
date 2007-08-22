//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Container.cpp,v 1.281.2.3 2007/01/18 19:34:20 gmf Exp $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlContainer.hpp"
#include "dbxml/XmlDocument.hpp"
#include "dbxml/XmlIndexSpecification.hpp"
#include "Container.hpp"
#include "SyntaxManager.hpp"
#include "Cursor.hpp"
#include "OperationContext.hpp"
#include "Results.hpp"
#include "Indexer.hpp"
#include "IndexLookup.hpp"
#include "UpdateContext.hpp"
#include "Name.hpp"
#include "Document.hpp"
#include "Value.hpp"
#include "TransactedContainer.hpp"
#include "QueryContext.hpp"
#include "dbxml/XmlManager.hpp"
#include "Manager.hpp"
#include "IndexEntry.hpp"
#include "nodeStore/NsDocumentDatabase.hpp"
#include "nodeStore/NsEvent.hpp"
#include "nodeStore/NsEventWriter.hpp"
#include <dbxml/XmlEventReaderToWriter.hpp>

using namespace DbXml;
using namespace std;

// default to 8k pages for node storage, 16k for whole-doc
#define DBXML_NODE_PAGESIZE (8 * 1024)
#define DBXML_DOC_PAGESIZE (16 * 1024)

// forward
static void doUpgrade(const std::string &name, Manager &mgr,
		      unsigned int old_version, unsigned int current_version);
static void throwUnique(const XmlException &xe, const std::string name,
			bool isDuplicateMD);

namespace DbXml {

/*
 * Transaction::Notify instance to track index DBs created
 * on demand, during transactions.  Must be lock protected by
 * lock on container.
 */
class IndexDbNotify : public Transaction::Notify
{
public:
	IndexDbNotify(Container *cont) : cont_(cont) {}
	void add(int idx) { dbs_.push_back(idx); }
	virtual void postNotify(bool commit);
private:
	vector<int> dbs_;
	Container *cont_;
};
}

/*
 * This is the version of the container format
 * supported by this release.  If it does not
 * match that of an opened container, upgrade is required,
 * if supported
 * release 1.2.1 -- version 2
 * release 2.0.9 -- version 3
 * release 2.1.x -- version 4
 * release 2.2.x -- version 5
 * Upgrades supported:
 *  o version 3->[4,5]
 *  o version 4->5
 * Upgrades not supported:
 *  o version 2->*
 */

const unsigned int Container::version = CURRENT_VERSION;

static const string document_name("document");

static void checkSameContainer(const Container &cont,
			       const Document &doc)
{
	if (doc.getID() != DocID(0)) {
		TransactedContainer *c = doc.getContainer();
		if (c && (c->getName() != cont.getName())) {
			string msg = "Cannot perform operation on document from a different container.  The XmlDocument is from container ";
			msg += c->getName();
			msg += ", and this container is ";
			msg += cont.getName();
			throw XmlException(XmlException::INVALID_VALUE,
					   msg);
		}
	}
}

Container::Container(Manager &mgr, const string &name,
		     u_int32_t pagesize, u_int32_t seqIncr,
		     XmlContainer::ContainerType type)
	: mgr_(mgr),
	  flags_(0),
	  environment_(mgr.getDbEnv()),
	  openingTransaction_(0),
	  name_(name),
	  pageSize_(pagesize),
	  seqIncr_(seqIncr),
	  contType_(type), // will be reset for open of existing containers
	  indexNodes_(false),
	  doValidation_(true),
	  hasAlias_(false),
	  indexDbNotify_(0)
{
	if (pagesize != 0 && (pagesize < 512 || pagesize > 65536)) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "Container expects a page size between 512 bytes and 64k");
	}
}

Container::~Container()
{
	if(openingTransaction_) {
		openingTransaction_->unregisterNotify(this);
		openingTransaction_->release();
	}

	ostringstream oss;
	oss << "container closed.";
	log(C_CONTAINER, L_INFO, oss);
}

int Container::open(Transaction *txn, u_int32_t flags,
		    int mode, bool doVersionCheck)
{
	if (flags & DBXML_INDEX_NODES && flags & DBXML_NO_INDEX_NODES)
		throw XmlException(XmlException::INVALID_VALUE,
				   "Cannot specify both DBXML_INDEX_NODES and DBXML_NO_INDEX_NODES");

	flags_ = flags;
	// Remove flags that aren't passed to DB
	indexNodes_ = (flags & DBXML_INDEX_NODES) != 0;
	doValidation_ = (flags & DBXML_ALLOW_VALIDATION) != 0;
	flags &= ~(DBXML_INDEX_NODES|DBXML_NO_INDEX_NODES|
		   DBXML_ALLOW_VALIDATION);

	if(txn) {
		openingTransaction_ = txn;
		openingTransaction_->acquire();
		openingTransaction_->registerNotify(this);
	}

	// set default page sizes if creating, and not already set
	//
	// [#13781],[#13782]: DB does not currently ignore pagesize
	// for opening existing databases with the DB_CREATE flag.
	// This means, if the container exists, and DB_CREATE is
	// set, and pagesize should be left as the default.  Alternatively,
	// DB_CREATE could be stripped, but the result is the same.
	// TBD: remove this code when 13781 is fixed.
	if (flags & DB_CREATE) {
		// this check is the code to remove.  A non-zero return
		// means the container exists
		if (Container::checkContainer(name_.c_str(),
					      *mgr_.getDbEnv()) != 0)
			pageSize_ = 0;
		else if (pageSize_ == 0) {
			if (contType_ == XmlContainer::WholedocContainer)
				pageSize_ = DBXML_DOC_PAGESIZE;
			else
				pageSize_ = DBXML_NODE_PAGESIZE;
		}
	} else
		pageSize_ = 0; // irrelevant if not creating

	// Create/Open Databases
	configuration_.reset(new ConfigurationDatabase(environment_, txn, name_,
						       contType_,
						       pageSize_, seqIncr_,
						       flags, mode, indexNodes_,
						       doVersionCheck));

	dictionary_.reset(new DictionaryDatabase(environment_, txn, name_,
						 pageSize_, flags, mode));

	switch(contType_) {
	case XmlContainer::WholedocContainer: {
		documentDb_.reset(new DocumentDatabase(
					  environment_, txn, name_,
					  contType_, pageSize_, flags, mode));
		break;
	}
	case XmlContainer::NodeContainer: {
		documentDb_.reset(new NsDocumentDatabase(environment_, txn,
							 name_, pageSize_,
							 flags, mode));
		break;
	}
	default: DBXML_ASSERT(false);
	}

	openIndexDbs(txn, flags, mode);
	// Set pageSize_ to the correct value, based on existing databases.
	// This works whether or not they were just created.
	pageSize_ = documentDb_->getPageSize();
	// Log the opening
	if(isLogEnabled(C_CONTAINER, L_INFO)) {
		ostringstream oss;
		switch(contType_) {
		case XmlContainer::WholedocContainer: {
			oss << "Document";
			break;
		}
		case XmlContainer::NodeContainer: {
			oss << "Node";
			break;
		}
		default: DBXML_ASSERT(false);
		}
		oss << " storage container opened.";
		log(C_CONTAINER, L_INFO, oss);
	}

	return 0;
}

void Container::close()
{
	if(openingTransaction_) {
		openingTransaction_->unregisterNotify(this);
		openingTransaction_->release();
		openingTransaction_ = 0;
	}
	((Manager &)mgr_).openContainers_.closeContainer((TransactedContainer*)this, 0);
	configuration_.reset(0);
	dictionary_.reset(0);
	documentDb_.reset(0);
	closeIndexes();
}

void Container::closeIndexes(int idx)
{
	if (idx == -1) {
		int j = 0;
		const Syntax *syntax = SyntaxManager::getInstance()->getNextSyntax(j);
		while (syntax != 0) {
			indexes_[syntax->getType()].reset(0);
			syntax = SyntaxManager::getInstance()->getNextSyntax(j);
		}
	} else {
		indexes_[idx].reset(0);
	}
}

bool Container::addAlias(const std::string &alias)
{
	if ((alias.find('/') != std::string::npos) || (alias.find('\\') != std::string::npos)) {
		throw XmlException(
			XmlException::INVALID_VALUE,
			"XmlContainer::addAlias: aliases cannot include a path separator character ('/' or '\\')");
	}
	bool retval = ((Manager &)mgr_).openContainers_.addAlias(alias, (TransactedContainer*)this);
	if (retval)
		hasAlias_ = true;
	return retval;
}

bool Container::removeAlias(const std::string &alias)
{
	return ((Manager &)mgr_).openContainers_.removeAlias(alias, (TransactedContainer*)this);
}

void Container::sync()
{
	configuration_->sync();
	dictionary_->sync();
	documentDb_->sync();
	int j = 0;
	const Syntax *syntax = SyntaxManager::getInstance()->getNextSyntax(j);
	while (syntax != 0) {
		SyntaxDatabase *sdb = indexes_[syntax->getType()].get();
		if (sdb)
			sdb->sync();
		syntax = SyntaxManager::getInstance()->getNextSyntax(j);
	}
}

void Container::postNotify(bool commit)
{
	if(!commit) {
		// The user has aborted the transaction this container
		// opened under, meaning it has been made invalid.
		// Therefore, we must remove it from the list of open
		// containers in Database
		((Manager &)mgr_).openContainers_.closeContainer((TransactedContainer*)this, 0);
	} else {
		configuration_->resetSequenceCache();
	}
}

int Container::setIndexSpecification(Transaction *txn, const IndexSpecification &newis, UpdateContext &context)
{
	// The current indexing specification
	//
	IndexSpecification oldis;
	int err = configuration_->getIndexSpecification(txn, oldis, /*lock=*/true);
	if (err == 0) {
		// The indexing strategies to add
		//
		IndexSpecification addis(newis);
		addis.disableIndex(oldis);
		//
		// The indexing strategies to delete
		//
		IndexSpecification delis(oldis);
		delis.disableIndex(newis);
		// remove deleted strategies
		if (delis.isIndexed(Index::NONE, Index::NONE)) {
			err = removeIndexes(txn, delis, context);
		}
		// reindex with new strategies (will walk the entire
		// container)
		if (err == 0 &&
		    addis.isIndexed(Index::NONE, Index::NONE)) {
			err = reindex(txn, addis, context);
		}
		if (err == 0) {
			err = configuration_->
				putIndexSpecification(txn, newis);
		}
		if (err == 0) {
			string uri, name, index;
			XmlIndexSpecification::Type stype;
			XmlValue::Type vtype;
			// create index databases for new
			// types.  Safer/more concurrent to
			// do this now, rather than when adding
			// initial entries.
			// TBD: is there a better way to get Syntax::Type?
			IndexSpecificationIterator i0(addis);
			while (i0.next(uri, name, stype, vtype)) {
				Syntax::Type synType =
					AtomicTypeValue::convertToSyntaxType(
						vtype);
				getIndexDB(synType, txn, true);
			}
			//
			// Log the indexing changes.
			//

			IndexSpecificationIterator i1(addis);
			while (i1.next(uri, name, index)) {
				ostringstream oss;
				oss << "Add '" << index <<
					"' index for node '" << uri <<
					"," << name << "'";
				log(C_CONTAINER, L_INFO, oss);
			}
			IndexSpecificationIterator i2(delis);
			while (i2.next(uri, name, index)) {
				ostringstream oss;
				oss << "Delete '" << index <<
					"' index for node '" << uri <<
					"," << name << "'";
				log(C_CONTAINER, L_INFO, oss);
			}
		}
	}
	return err;
}

Results *Container::lookupIndex(Transaction *txn,
				XmlQueryContext &context,
				const IndexLookup &il,
				u_int32_t flags)
{
	checkFlags(Log::misc_flag_info, "lookupIndex()", flags,
		   DB_READ_UNCOMMITTED|DB_RMW|DB_READ_COMMITTED|DBXML_LAZY_DOCS|
		   DBXML_REVERSE_ORDER|DBXML_INDEX_VALUES|DBXML_NO_INDEX_NODES|
		   DBXML_CACHE_DOCUMENTS|DB_TXN_SNAPSHOT);
	try {
		Index index;
		if(!index.set(il.getIndex())) {
			throw XmlException(
				XmlException::UNKNOWN_INDEX,
				"Unknown index specification, '" +
				(std::string)il.getIndex() + ".");
		}
		if(index.getKey() == Index::KEY_SUBSTRING) {
			throw XmlException(
				XmlException::UNKNOWN_INDEX,
				"Index lookup on a substring index is not available.");
		}
		if((index.getPath() == Index::PATH_NODE) && il.hasParent()) {
			// Deal with an edge lookup on a node
			// index as a special case
			return new ValueResults();
		}
		
		if(context.getEvaluationType() ==
		   XmlQueryContext::Eager) {
			return new ValueResults(
				new LazyIndexResults(
					*this, context, txn, index, il, flags));
		} else {
			return new LazyIndexResults(
				*this, context, txn, index, il, flags);
		}
	} catch (DbException &e) {
		throw XmlException(e);
	}
	return 0;
}

Statistics *Container::lookupStatistics(Transaction *txn,
					Name *child,
					Name *parent,
					const std::string &indexString,
					const XmlValue &value)
{
	try {
		std::string pUri = (parent ? parent->getURIName() : "");
		const char *p_uri = pUri.length() ? pUri.c_str() : 0;
		Index index_obj;
		if(!index_obj.set(indexString)) {
			throw XmlException(
				XmlException::UNKNOWN_INDEX,
				"Unknown index specification, '" +
				(std::string)indexString + ".");
		}
		return new Statistics(*this, txn, index_obj,
				      child->getURIName().c_str(),
				      p_uri, value);
	} catch (DbException &e) {
		throw XmlException(e);
	}
	return 0;
}

int Container::reindex(Transaction *txn, const IndexSpecification &is,
		       UpdateContext &context)
{
	context.init(txn, this);
	Indexer &indexer = context.getIndexer();
	KeyStash &stash = context.getKeyStash();
	OperationContext &oc = context.getOperationContext();
	indexer.resetContext(this, &oc);
	const Manager &mgr = (Manager&)getManager();
	// Use DB_READ_COMMITTED locking to avoid lots of locks/lockers
	// during reindex
	u_int32_t flags = 0;
	if (mgr.isTransactedEnv() ||  mgr.isLockingEnv()) {
		flags = DB_READ_COMMITTED;
	}

	ScopedPtr<DocumentCursor> cursor;
	int err = documentDb_->createDocumentCursor(txn, cursor, flags);
	if(err == 0) {
		XmlDocument document;
		DocID id;
		err = cursor->first(id);
		while(id != 0) {
			indexer.reset();
			stash.reset();
			if ((err = getDocument(oc, id, document, flags)) == 0) {
				documentDb_->reindex(is, document,
						     context, stash);
				err = stash.updateIndex(oc, this);
			}
			if(err == 0) err = cursor->next(id);
			else break;
		}
	}

	if(err == DB_NOTFOUND)
		err = 0;

	return err;
}

int Container::removeIndexes(Transaction *txn, const IndexSpecification &is,
			     UpdateContext &context)
{
	int err = 0;
	context.init(txn, this);
	OperationContext &oc = context.getOperationContext();
	const IndexVector *iv = is.getDefaultIndex();
	err = removeIndexes(oc, *iv, context, true);
	
	IndexSpecification::const_iterator current = is.begin();
	IndexSpecification::const_iterator end = is.end();
	while (current != end) {
		iv = current->second;
		err = removeIndexes(oc, *iv, context, false);
		current++;
	}
	return err;
}

int Container::removeIndexes(OperationContext &oc, const IndexVector &iv,
			     UpdateContext &context, bool isDefault)
{
	int err = 0;
	NameID id;
	if (!isDefault) {
		err = getDictionaryDB()->lookupIDFromName(
			oc, iv.getNameObject(), id, false);
		if(err == DB_NOTFOUND) {
			// DB_NOTFOUND means there's no data in the index yet
			return 0;
		}
		else if (err != 0) return err;
	}
	IndexVector::const_iterator i;
	for (i = iv.begin(); i != iv.end(); ++i) {
		SyntaxDatabase *sdb = getIndexDB(i->getSyntax(),
						 oc.txn(), false);
		if (sdb) {
			err = sdb->removeIndex(oc, *i, id);
			if(err == DB_NOTFOUND) {
				// DB_NOTFOUND just means there's no data in the index
				err = 0;
			}
			else if(err != 0) break;
		}
	}

	return err;
}

//
// add a document as an XmlEventWriter.  Return an object to use.
// caller *must* call XmlEventWriter::close() to not only destroy
// the object, but to perform final cleanup of state.
// This is true whether or not an exception is thrown during processing.
//
NsEventWriter &Container::addDocumentAsEventWriter(
	Transaction *txn, Document &document,
	UpdateContext &context,
	u_int32_t flags)
{
	// do not return events object, we'll create it here
	(void)prepareAddDocument(txn, document, context, flags, false);

	NsDocumentDatabase *ndd = documentDb_->getNsDocumentDatabase();
	NsEventWriter *writer =
		new NsEventWriter(this, &context, &document, flags,
				  Globals::defaultMemoryManager);
	writer->setBaseEventWriter(&(context.getIndexer()));
	if (!ndd)
		writer->createNsWriter(); // write wholedoc content, as well
	return *writer;
}

int Container::addDocumentAsEventReader(Transaction *txn, Document &document,
					UpdateContext &context, u_int32_t flags)
{
	DBXML_ASSERT(document.getDefinitiveContent() == Document::READER);
	// NOTE: this code is virtually identical to above, in ...AsEventWriter
	// except for the end
	
	// do not return events object, we'll create it here
	(void)prepareAddDocument(txn, document, context, flags, false);

	NsDocumentDatabase *ndd = documentDb_->getNsDocumentDatabase();
	NsEventWriter *writer =
		new NsEventWriter(this, &context, &document, flags,
				  Globals::defaultMemoryManager);
	writer->setBaseEventWriter(&(context.getIndexer()));
	if (!ndd)
		writer->createNsWriter(); // write wholedoc content, as well
	XmlEventReaderToWriter r2w(document.getContentAsEventReader(),
				   *writer);
	// reader is a one-time use only; it's now owned by the r2w object
	DBXML_ASSERT(document.getDefinitiveContent() == Document::NONE);
	r2w.start();
	return 0;
}

int Container::addDocument(Transaction *txn, Document &document,
			   UpdateContext &context, u_int32_t flags)
{
	if(isLogEnabled(C_CONTAINER, L_INFO))
		logDocumentOperation(document, "Adding document: ");
	
	if (document.getDefinitiveContent() == Document::READER)
		return addDocumentAsEventReader(txn, document, context, flags);
	NsPushEventSource *events =
		prepareAddDocument(txn, document, context, flags);
	int err = 0;
	if (events) {
		// events is donated to this call, which deletes it
		err = indexAddDocument(events, document, context);
	}
	if (!err)
		err = completeAddDocument(document, context);
	if(!err && isLogEnabled(C_CONTAINER, L_INFO))
		logDocumentOperation(document, "Added document successfully: ");
	return err;
}

// part 1 of adding a document: generate id (and optionally name),
// index metadata, and prepare for indexing content.
// Most intermediate state is maintained in UpdateContext
NsPushEventSource *Container::prepareAddDocument(Transaction *txn,
						 Document &document,
						 UpdateContext &context,
						 u_int32_t flags,
						 bool createEvents)
{
	document.setEagerMetaData(); // Just in case
	document.setContainer((TransactedContainer*)this); // also, just in case
	if ((flags & DBXML_WELL_FORMED_ONLY) != 0)
		document.setValidation(Document::WF_ONLY);
	else if (doValidation_)
		document.setValidation(Document::VALIDATE);

	context.init(txn, this);
	OperationContext &oc = context.getOperationContext();
	(void) ensureDocName(oc, document, flags); // throws on error

	KeyStash &stash = context.getKeyStash();
	Indexer &indexer = context.getIndexer();
	NsPushEventSource *events = 0;
	try {
		// index metadata
		indexer.indexMetaData(context.getIndexSpecification(),
				      document, stash, false /*checkModified*/);

		if (createEvents) {
			// If using wholedoc storage, and document content is a
			// stream, convert it to a dbt -- streams can only
			// be used once...
			NsDocumentDatabase *ndd = documentDb_->
				getNsDocumentDatabase();
			if (!ndd && (document.getDefinitiveContent() ==
				     Document::INPUTSTREAM))
				document.getContentAsDbt();


			// create event source, and initialize indexer, but
			// only if there is content to index
			if (document.getDefinitiveContent() != Document::NONE)
				events = document.getContentAsEventSource(
					oc.txn(),
					/*needsValidation*/true,
					getIndexNodes());
		}
		// it's ok for events to be null
		indexer.initIndexContent(
			context.getIndexSpecification(),
			document.getID(),
			events, stash,
			(contType_ == XmlContainer::WholedocContainer));
	}
	catch (XmlException &xe) {
		if (xe.getExceptionCode() == XmlException::UNIQUE_ERROR) {
			throwUnique(xe, document.getName(), true);
		} else
			throw; // re-throw
	}
	
	return events;
}

// Perform the actual indexing step.  This is where these errors are caught:
// o parse errors
// o unique index errors on content
// o database errors
//
// Call the underlying DocumentDatabase instances on error and success to
// allow for cleanup.
//
int Container::indexAddDocument(NsPushEventSource *events, Document &document,
				UpdateContext &context)
{
	try {
		events->start(); // run the indexer
	}
	catch (XmlException &xe) {
		delete events;
		bool isDbExc = (xe.getExceptionCode() ==
				XmlException::DATABASE_ERROR);
		documentDb_->addContentException(document, context, isDbExc);
		if (xe.getExceptionCode() == XmlException::UNIQUE_ERROR) {
			throwUnique(xe, document.getName(), false);
		} else
			throw; // re-throw
	}
	catch (DbException &de) {
		delete events;
		documentDb_->addContentException(document, context, true);
		throw;
	}
	catch (...) {
		delete events;
		documentDb_->addContentException(document, context, false);
		throw;
	}
	delete events;
	// success, finish adding content (if there's anything left)
	int err = documentDb_->addContent(document, context);
	if (err == 0)
		document.setContentModified(false);
	return 0;
}

int Container::completeAddDocument(Document &document, UpdateContext &context)
{
	// indexing is done
	// write metadata, data, and index keys
	OperationContext &oc = context.getOperationContext();
	
	int err = documentDb_->addMetaData(oc, getDictionaryDB(), document);
	if(!err) {
		// Write the index keys
		err = context.getKeyStash(false).updateIndex(oc, this);
		if (!err) {
			document.setMetaDataFromContainer(
				(TransactedContainer*)this,
				document.getID(),
				oc.txn(), 0 /* no flags */);
		}
	}
	return err;
}

// generates ID and makes sure that the document has a name
int Container::ensureDocName(OperationContext &context, Document &document,
			     u_int32_t flags) const
{
	string name = document.getName();
	DocID &id = document.getIDToSet();
	id = 0;
	int err = configuration_->generateID(context.txn(), id);
	if (err)
		throw XmlException(XmlException::INVALID_VALUE,
				   "Unable to generated a document ID");
	if ((flags & DBXML_GEN_NAME) != 0) {
		static const char *genPrefix = "dbxml_";
		if (name == (std::string)"")
			name = genPrefix;
		else
			name += "_";
		name += id.asString();
		document.setName(name, /*modified*/true);
	} else {
		// name must be set if DBXML_GEN_NAME is not used
		const char *tname = name.c_str();
		if (tname && (*tname == 0))
			throw XmlException(XmlException::INVALID_VALUE,
					   "A valid name, or specification of DBXML_GEN_NAME, is required to put a XmlDocument into a container");
	}
	return 0;
}

int Container::getDocumentID(OperationContext &context,
			     const std::string &name, DocID &id) const
{
	// Using built in index "unique-metadata-equality-string" on dbxml:name
	Key key(0);
	key.getIndex().set(Index::NME | Syntax::STRING, Index::PNKS_MASK);
	key.setID1(dictionary_->getNIDForName());
	key.setValue(name.c_str(), name.length());

	const SyntaxDatabase *database = getIndexDB(Syntax::STRING);

	IndexEntry ie;
	key.setDbtFromThis(context.key());
	int err = database->getIndexDB()->getIndexEntry(context, context.key(), ie);
	if(err == 0) {
		id = ie.getDocID();
	}
	return err;
}

int Container::getDocument(OperationContext &context,
			   const std::string &name,
			   XmlDocument &document, u_int32_t flags) const
{
	DocID id;
	int err = getDocumentID(context, name, id);
	if(err != 0) return err;

	err = getDocument(context, id, document, flags);
	if (err == 0)
		((Document&)document).setName(name, /*modified*/false);

	return err;
}

int Container::getDocument(OperationContext &context, const DocID &did,
			   XmlDocument &document, u_int32_t flags) const
{
	if(document.isNull()) {
		// Make a document, if there isn't one already
		document = const_cast<XmlManager&>(mgr_).createDocument();
	} else {
		((Document*)document)->reset();
	}
	bool wfonly = false;
	if ((flags & DBXML_WELL_FORMED_ONLY) != 0) {
		wfonly = true;
		flags &= ~DBXML_WELL_FORMED_ONLY;
	}
	((Document*)document)->setMetaDataFromContainer((TransactedContainer*)this, did, context.txn(), flags);
	if (wfonly)
		((Document*)document)->setValidation(Document::WF_ONLY);
	return 0;
}

int Container::deleteDocument(Transaction *txn, const std::string &name,
			      UpdateContext &context)
{
	XmlDocument document;
	OperationContext &oc = context.getOperationContext();
	oc.set(txn);
	// LAZY here may be a no-op, since the doc will need to be
	// re-indexed for delete, anyway.  Could consider DB_RMW.
	int err = getDocument(oc, name, document, DBXML_LAZY_DOCS);
	if(err == 0) {
		err = deleteDocument(txn, document, context);
	}
	return err;
}

int Container::deleteDocument(Transaction *txn, Document &document,
			      UpdateContext &context)
{
	checkSameContainer(*this, document);
	if (document.getDefinitiveContent() != Document::NONE)
		return deleteDocument(txn, document.getName(), context);
	if(isLogEnabled(C_CONTAINER, L_INFO))
		logDocumentOperation(document, "Deleting document: ");

	context.init(txn, this);
	OperationContext &oc = context.getOperationContext();
	KeyStash &stash = context.getKeyStash();
	DocID id = document.getID();

	//
	// We may have been given an empty document, or a document that has
	// already been deleted, or a document with a mangled ID. Db::del
	// returns EINVAL if it can't delete a key/data pair. We change this
	// to DB_NOTFOUND, which makes more sense.
	//
	int err = 0;
	try {
		// Index the (possibly lazy) document and remove it's content
		err = documentDb_->removeContentAndIndex(document, context,
							 stash);
	} catch (DbException &e) {
		err = e.get_errno();
		if (err == EINVAL) {
			err = DB_NOTFOUND;
		}
	}
	if(err != 0) return err;

	// Delete meta-data items from the document secondary database.
	err = documentDb_->removeMetaData(oc, id, 0);
	if(err != 0) return err;

	// Remove the document's index entries
	err = stash.updateIndex(oc, this);
	if(!err && isLogEnabled(C_CONTAINER, L_INFO))
		logDocumentOperation(document, "Deleted document successfully: ");
	return err;
}

int Container::updateDocument(Transaction *txn, Document &new_document,
			      UpdateContext &context)
{
	if (new_document.getDefinitiveContent() == Document::READER) {
		// do not (yet) support updateDocument using XmlEventReader
		// content.  More code reorganization is necessary to do this.
		throw XmlException(XmlException::INVALID_VALUE,
				   "updateDocument is not supported for XmlEventReader content");
	}
	if(isLogEnabled(C_CONTAINER, L_INFO))
		logDocumentOperation(new_document, "Updating document: ");
	
	checkSameContainer(*this, new_document);
	context.init(txn, this);
	new_document.setContainer((TransactedContainer*)this); // just in case
	OperationContext &oc = context.getOperationContext();
	KeyStash &stash = context.getKeyStash();

	// Generate index keys for the new document and update the content
	int err = documentDb_->updateContentAndIndex(new_document,
						     context, stash);
	if(err != 0) return err;

	// Update the metadata
	err = documentDb_->updateMetaData(oc, getDictionaryDB(),
					  new_document);
	if(err != 0) return err;

	// Make the index changes necessary
	err = stash.updateIndex(oc, this);
	if(!err && isLogEnabled(C_CONTAINER, L_INFO))
		logDocumentOperation(new_document, "Updated document successfully: ");
	return err;
}

void Container::logDocumentOperation(const Document &doc, const char *msg)
{
	string logmsg = msg;
	if (doc.getName() != (string)"")
		logmsg += doc.getName();
	else
		logmsg += "(no name)";
	log(C_CONTAINER, L_INFO, logmsg);
}

void Container::log(ImplLogCategory c, ImplLogLevel l, const ostringstream &s) const
{
	Log::log(environment_, c, l, name_.c_str(), s.str().c_str());
}

void Container::log(ImplLogCategory c, ImplLogLevel l, const std::string &s) const
{
	Log::log(environment_, c, l, name_.c_str(), s.c_str());
}

void Container::checkFlags(const FlagInfo *flag_info, const char *function,
			   u_int32_t flags, u_int32_t mask) const
{
	Log::checkFlags(environment_, C_CONTAINER, name_.c_str(),
			function, flag_info, flags, mask);
}

void Container::dump(Manager &mgr, const std::string &name,
		     std::ostream *out)
{
	int err = 0;
	try {
		if(err == 0)
			err = ConfigurationDatabase::dump(mgr.getDbEnv(),
							  name, out);
		if(err == 0)
			err = DictionaryDatabase::dump(mgr.getDbEnv(),
						       name, out);
		if(err == 0) {
			switch(ConfigurationDatabase::readContainerType(
				       mgr.getDbEnv(), name)) {
			case XmlContainer::WholedocContainer: {
				err = DocumentDatabase::dump(
					mgr.getDbEnv(), name,
					XmlContainer::WholedocContainer,
					out);
				break;
			}
			case XmlContainer::NodeContainer: {
				err = NsDocumentDatabase::dump(mgr.getDbEnv(),
							       name, out);
				break;
			}
			default: DBXML_ASSERT(false);
			}
		}

	}
	catch(DbException &e) {
		throw XmlException(e);
	}
	if(err) {
		throw XmlException(err);
	}
	else {
		Log::log(mgr.getDbEnv(), C_CONTAINER, L_DEBUG,
			 name.c_str(), "Container dumped");
	}
}

void Container::load(Manager &mgr, const std::string &name,
		     std::istream *in, unsigned long *lineno,
		     UpdateContext &context)
{
	int ret = 0;
	try {
		XmlContainer::ContainerType ctype;
		if(ret == 0)
			ret = ConfigurationDatabase::load(mgr.getDbEnv(), name,
							  in, lineno);
		if(ret == 0)
			ret = DictionaryDatabase::load(mgr.getDbEnv(), name,
						       in, lineno);
		if(ret == 0) {
			ctype = ConfigurationDatabase::readContainerType(
				mgr.getDbEnv(), name);
			switch(ctype) {
			case XmlContainer::WholedocContainer: {
				ret = DocumentDatabase::load(
					mgr.getDbEnv(), name,
					XmlContainer::WholedocContainer,
					in, lineno);
				break;
			}
			case XmlContainer::NodeContainer: {
				ret = NsDocumentDatabase::load(mgr.getDbEnv(), name,
							       in, lineno);
				break;
			}
			default: DBXML_ASSERT(false);
			}
		}

		if(ret == 0) {
			TransactedContainer *container =
				new TransactedContainer(
					mgr, name, 0, DB_CREATE, 0, 0,
					0666, ctype, /*doVersionCheck=*/true);
			// Deletes the container at the end of this scope
			XmlContainer containerWrapper(container);

			IndexSpecification is;
			ret = container->getConfigurationDB()->
				getIndexSpecification((Transaction*)0, is);
			if(ret == 0)
				ret = container->reindex(0, is, context);
		}
	}
	catch(DbException &e) {
		throw XmlException(e);
	}
	if(ret) {
		throw XmlException(ret);
	}
	else {
		Log::log(mgr.getDbEnv(), C_CONTAINER, L_DEBUG,
			 name.c_str(), "Container loaded");
	}
}

//
// Verify is really 2 interfaces:
// 1. verify
// 2. salvage
// The verify behavior needs 2 passes on the databases in a container:
//   1.  for each database, verify it using DB_NOORDERCHK.
//   2.  for each database, verify it using DB_ORDERCHKONLY, after
// Salvage behavior is different. In this case, there is no attempt
// to salvage index databases, and the *CHK* flags are irrelevant,
// so it iterates over the content databases, creating key/value pair
// output for user salvage.
//
// Salvage order is configuration db, dictionary primary, secondary,
// document primary, secondary.
//
// NOTE: according to the Db::verify() interface, the Db handle used
// for a call to verify() cannot be used after that call, regardless of
// return.
//
void Container::verify(Manager &mgr, const std::string &name,
		       std::ostream *out, u_int32_t flags)
{
	int ret = 0;
	try {
		if (!(flags & DB_SALVAGE)) {
			// only done for verify w/o salvage
			flags |= DB_NOORDERCHK;
			flags &= ~DB_ORDERCHKONLY;
			Db allDbs(mgr.getDbEnv(), 0);
			ret = allDbs.verify((name.length() == 0 ? 0 : name.c_str()),
					    NULL, out, flags);
		}
		if (ret == 0) {
			if (!(flags & DB_SALVAGE)) {
				flags &= ~DB_NOORDERCHK;
				flags |= DB_ORDERCHKONLY;
			}
			// this pass is done for both the salvage and
			// verify cases.
			ret = ConfigurationDatabase::verify(
				mgr.getDbEnv(), name, out, flags);
			if (ret == 0) {
				ret = DictionaryDatabase::verify(
					mgr.getDbEnv(), name, out, flags);
			}
			if (ret == 0) {
				switch(ConfigurationDatabase::readContainerType(
					       mgr.getDbEnv(), name)) {
				case XmlContainer::WholedocContainer: {
					ret = DocumentDatabase::verify(
						mgr.getDbEnv(), name,
						XmlContainer::WholedocContainer,
						out, flags);
					break;
				}
				case XmlContainer::NodeContainer: {
					ret = NsDocumentDatabase::verify(
						mgr.getDbEnv(), name, out, flags);
					break;
				}
				default: DBXML_ASSERT(false);
				}
			}
		}
		if ((ret == 0) && !(flags & DB_SALVAGE)) {
			// only done for verify -- do ordercheck on index dbs
			int j = 0;
			const Syntax *syntax =
				SyntaxManager::getInstance()->getNextSyntax(j);
			while(syntax != 0 && ret == 0) {
				ret = SyntaxDatabase::verify(syntax,
							     mgr.getDbEnv(),
							     name, NULL, flags);
				syntax = SyntaxManager::getInstance()->
					getNextSyntax(j);
			}
		}
	}
	catch(DbException &e) {
		throw XmlException(e);
	}
	if(ret) {
		throw XmlException(ret);
	}
	else {
		Log::log(mgr.getDbEnv(), C_CONTAINER, L_DEBUG,
			 name.c_str(), "Container verified");
	}
}

int Container::writeHeader(const std::string &name, std::ostream *out)
{
	(*out) << "xml_database=" << name << endl;
	return 0;
}

int Container::verifyHeader(const std::string &name, std::istream *in)
{
	int ret = 0;
	char keyname[64], dbname[256];

	if(!in->get(keyname, sizeof keyname, '=') || strcmp(keyname, "xml_database") != 0 || in->get() != '=' ||
	   !in->get(dbname, sizeof dbname) || in->get() != '\n'|| name != dbname) {
		ret = EINVAL;
	}

	return ret;
}

// (re) set the indexNodes state
void Container::setIndexNodes(Transaction *txn, bool indexNodes)
{
	getConfigurationDB()->
		setIndexNodes(txn, indexNodes);
	indexNodes_ = indexNodes;
}

// reindex container (closed container)
//static
void Container::reindexContainer(Transaction *txn,
				 Manager &mgr,
				 const std::string &name,
				 UpdateContext &context,
				 u_int32_t flags)
{
	if (flags & DBXML_INDEX_NODES && flags & DBXML_NO_INDEX_NODES)
		throw XmlException(XmlException::INVALID_VALUE,
				   "XmlManger::reindexContainer: cannot specify both DBXML_INDEX_NODES and DBXML_NO_INDEX_NODES");
	Log::log(mgr.getDbEnv(), C_CONTAINER, L_INFO,
		 name.c_str(), "Reindexing container");

	// need an open container (will throw if container
	// doesn't exist)
	u_int32_t tflags = (txn ? DBXML_TRANSACTIONAL : 0);
	// don't use top-level txn for opening the container
	XmlContainer cont(new TransactedContainer(
				  mgr, name, 0, tflags, 0, 0, 0,
				  (XmlContainer::ContainerType)0,
				  true));
	bool indexNodes = ((flags & DBXML_INDEX_NODES) ? true : false);
	
	// if mismatch, reset the container
	if (indexNodes != cont.getIndexNodes())
		((Container&)cont).setIndexNodes(txn, indexNodes);
	
	// reindex
	((Container&)cont).reloadIndexes(txn, context);
	Log::log(mgr.getDbEnv(), C_CONTAINER, L_INFO,
		 name.c_str(), "Reindex complete");
}

class truncator : public DbWrapper::DbFunctionRunner
{
public:
	truncator() {}
	void run(Transaction *txn, DbWrapper &database) {
		try {
			u_int32_t count;
			DbTxn *dbTxn = (txn ? txn->getDbTxn() : 0);
			Db &db = database.getDb();
			db.truncate(dbTxn, &count, 0);
		}
		catch (DbException &de) {
			throw XmlException(de);
		}
	}
};

class compactor : public DbWrapper::DbFunctionRunner
{
public:
	compactor() {}
	void run(Transaction *txn, DbWrapper &database) {
		try {
			DbTxn *dbTxn = (txn ? txn->getDbTxn() : 0);
			// TBD: get the statistics, and log them
#if DBVER > 43
			database.getDb().compact(dbTxn, 0, 0, 0, 0, 0);
#endif			
		}
		catch (DbException &de) {
			throw XmlException(de);
		}
	}
};

// truncate container (closed container)
//static
void Container::truncateContainer(Transaction *txn,
				  Manager &mgr,
				  const std::string &name,
				  UpdateContext &context)
{
	Log::log(mgr.getDbEnv(), C_CONTAINER, L_INFO,
		 name.c_str(), "Truncating container");
	// need an open container (will throw if container
	// doesn't exist)
	u_int32_t tflags = (txn ? DBXML_TRANSACTIONAL : 0);
	// don't use top-level txn for opening the container
	XmlContainer cont(new TransactedContainer(
				  mgr, name, 0, tflags, 0, 0, 0,
				  (XmlContainer::ContainerType)0,
				  true));
	truncator trunc;
	// skip core dbs, or the container is corrupt
	((Container&)cont).runOnAllDatabases(txn, trunc, true);
}

// compact container (closed container)
//static
void Container::compactContainer(Transaction *txn,
				 Manager &mgr,
				 const std::string &name,
				 UpdateContext &context)
{
	Log::log(mgr.getDbEnv(), C_CONTAINER, L_INFO,
		 name.c_str(), "Compacting container");
	// need an open container (will throw if container
	// doesn't exist)
	u_int32_t tflags = (txn ? DBXML_TRANSACTIONAL : 0);
	// don't use top-level txn for opening the container
	XmlContainer cont(new TransactedContainer(
				  mgr, name, 0, tflags, 0, 0, 0,
				  (XmlContainer::ContainerType)0,
				  true));
	compactor comp;
	((Container&)cont).runOnAllDatabases(txn, comp, false);
}

// upgrade functions

// assumes container is not open. Caller is responsible
// for checking (XmlManager)
//static
void Container::upgradeContainer(const std::string &name,
				 Manager &mgr,
				 UpdateContext &context)
{
	int err = 0;
	// check for existence of container, and version first.
	unsigned int old_version = Container::checkContainer(name, *mgr.getDbEnv());
	if (old_version == version) // version is a class static
		return;
	else if (!old_version) {
		ostringstream s;
		s << "Cannot upgrade non-existent container: " << name;
		throw XmlException(XmlException::INVALID_VALUE, s.str());
	}
	if (version < old_version) {
		ostringstream s;
		s << "Container version '";
		s << old_version;
		s << "' is more recent than the bdbxml library version '";
		s << version;
		s << "'.  Use a more recent release of the bdbxml library";
		throw XmlException(XmlException::VERSION_MISMATCH, s.str());
	}
	// Versions and supported upgrades
	// 3 (2.0)
	// 4 (2.1)
	// 5 (2.2)
	// Current version is 6 (2.3)
	// 2.0 or 2.1 must be upgraded to 2.2 first
	if (old_version != VERSION_20 &&
	    old_version != VERSION_21 &&
	    old_version != VERSION_22) {
		throw XmlException(
			XmlException::VERSION_MISMATCH,
			"Upgrade is not supported from release 1.2.x to release 2.x.");
	}

	// Start upgrade...

#if 0
	// 4.3 and 4.4 do not require DB upgrade, and
	// DB upgrade requires matching endianism of host,
	// while BDB XML upgrade does not, so leave out for now.
	//
	// Upgrade Berkeley DB
	// NOTE: maybe this needs to be done first...
	//
	{
		Db db(mgr.getDbEnv(), 0);
		err = db.upgrade(name.c_str(), 0);
	}
#endif	
	//
	// Upgrade Berkeley DB XML
	//
	if(err == 0) {
		try {
			doUpgrade(name, mgr, old_version, version);
			// reload indexes requires an open container
			XmlContainer cont(new TransactedContainer(
						  mgr, name, 0, 0, 0, 0, 0,
						  (XmlContainer::ContainerType)0,
						  true));
			((Container&)cont).reloadIndexes(0, context);
			Log::log(mgr.getDbEnv(), C_CONTAINER, L_INFO,
				 name.c_str(), "Upgrade complete");
		}
		catch (DbException &de) {
			throw XmlException(de);
		}
	}
}

// 2.0 -> 2.1
//  o  Databases created on big-endian machines --
//     IDs need byte-swapping in dictionary secondary data
//  o  Containers created on big-endian machines --
//     IDs need byte-swapping in document secondary keys
//  o  NOTE: if the "current" machine is little-endian, it needs to
//     detect the big-endian nature of the container and byte-swap, anway,
//     and big-endian machines need to do the reverse -- detect that
//     the container was created little-endian, and *not* swap...
//     In other words, the check cannot be based on the current machine's
//     endian-ness.
//  o  upgrade version (ConfigurationDatabase)
//  o  dump/load indexes
// 2.1 -> 2.2
//  o upgrade version in configuration DB
//  o configuration DB change:
//    -- added upgrade state item to config. DB to track upgrade
//    -- removed DbSequence record; it is now in its own
//       btree database -- "secondary_sequence"
//  o Changed doc and name ids, and keys to a marshaled form, and use
//  custom comparison function.  This affects these databases:
//    -- content_document (key)
//    -- node_nodestorage (key)
//    -- secondary_document (metadata keys)
//    -- secondary_dictionary (metadata values)
//  Index formats have also changed, so reindexing is necessary.
// 2.2 -> 2.3
//  o version number in configuration DB
//  o node storage format change (including node ID now in BTree key)
//  o remove index types (anyURI, QName, NOTATION)
//

static void doUpgrade(const std::string &name, Manager &mgr,
		      unsigned int old_version, unsigned int current_version)
{
	char buf[4096];
	sprintf(buf, "Upgrading container %s from format version %d to version %d\n",
		name.c_str(), old_version, current_version);
	Log::log(mgr.getDbEnv(), Log::C_CONTAINER, Log::L_INFO,
		 name.c_str(), buf);

	ConfigurationDatabase::upgrade(name, mgr, old_version, current_version);
	Log::log(mgr.getDbEnv(), Log::C_CONTAINER, Log::L_INFO,
		 name.c_str(), "Configuration upgrade complete");

	// It's safe to open configuration DB to read container type
	// now that it's upgraded
	XmlContainer::ContainerType ctype =
		ConfigurationDatabase::readContainerType(mgr.getDbEnv(), name);
	
	DictionaryDatabase::upgrade(name, mgr, old_version, current_version);
	Log::log(mgr.getDbEnv(), Log::C_CONTAINER, Log::L_INFO,
		 name.c_str(), "Dictionary upgrade complete");

	DocumentDatabase::upgrade(name, mgr, old_version, current_version);
	Log::log(mgr.getDbEnv(), Log::C_CONTAINER, Log::L_INFO,
		 name.c_str(), "Document upgrade complete");
	if (ctype == XmlContainer::NodeContainer) {
		NsDocumentDatabase::upgrade(name, mgr,
					    old_version, current_version);
		Log::log(mgr.getDbEnv(), Log::C_CONTAINER, Log::L_INFO,
			 name.c_str(), "Node Storage upgrade complete");
	}
}

// This class could create Index databases on demand,
// from the Indexer and KeyStash code.
// All the code is here, except for this problem:
//  o if the transaction in which the DB(s) is/are created is
//  aborted, the new DBs must be cleared.
//  o this means using tracking new DBS using
//    the abort hooks for txn.  Just a little more code.
// Once this is done, just un-comment out the flags &= line
void Container::openIndexDbs(Transaction *txn, u_int32_t flags, int mode)
{
	// Iterate over the registered syntax types, creating a
	// SyntaxDatabase for each.
	indexes_.resize(SyntaxManager::getInstance()->size());
	int j = 0;
	u_int32_t origFlags = flags;
	flags &= ~(DB_CREATE|DB_EXCL); // don't allow creation
	const Syntax *syntax = SyntaxManager::getInstance()->getNextSyntax(j);
	while (syntax != 0) {
		SyntaxDatabase *sdb = 0;
		u_int32_t tflags = (syntax->getType() == Syntax::STRING) ?
			origFlags : flags;
		try {
			sdb = new SyntaxDatabase(syntax, environment_,
				txn, name_, indexNodes_, pageSize_, tflags, mode);
		} catch (XmlException &xe) {
			// ENOENT is not an error
			if (xe.getDbErrno() != ENOENT)
				throw;
		}
		indexes_[syntax->getType()].reset(sdb);
		syntax = SyntaxManager::getInstance()->getNextSyntax(j);
	}
}

//
// upgrade for indexes -- equivalent to dump/load, but in-place
// 1. truncate indexes
// 2. grab IndexSpecification and reindex
//
void Container::reloadIndexes(Transaction *txn, UpdateContext &uc)
{
	int j = 0;
	const Syntax *syntax = SyntaxManager::getInstance()->getNextSyntax(j);
	while (syntax != 0) {
		u_int32_t count = 0;
		SyntaxDatabase *sdb = indexes_[syntax->getType()].get();
		if (sdb) {
			sdb->getIndexDB()->truncate(txn, &count, 0);
			sdb->getStatisticsDB()->truncate(txn, &count, 0);
		}
		syntax = SyntaxManager::getInstance()->getNextSyntax(j);
	}

	// reindex (this is the same as code in load()
	XmlIndexSpecification is;
	int ret = getConfigurationDB()->
		getIndexSpecification(txn, is);
	if(ret == 0) {
		ret = reindex(txn, (IndexSpecification&)is, uc);
	}
}

SyntaxDatabase *Container::getIndexDB(Syntax::Type type, Transaction *txn,
				      bool toWrite)
{
	SyntaxDatabase *sdb = indexes_[type].get();
	u_int32_t flags = DB_CREATE;
	if ( flags_ & DBXML_ENCRYPT)
		flags |= DBXML_ENCRYPT;
	if (!sdb && toWrite) {
		indexes_.resize(SyntaxManager::getInstance()->size());
		sdb = new SyntaxDatabase(SyntaxManager::getInstance()->getSyntax(type),
			environment_, txn, name_, indexNodes_, pageSize_, flags, 0);
		indexes_[type].reset(sdb);
		if (txn != 0) {
			// protect initialization among threads
			lock();
			if (indexDbNotify_ == 0) {
				indexDbNotify_ = new IndexDbNotify(this);
				txn->registerNotify(indexDbNotify_);
			}
			indexDbNotify_->add(type);
			unlock();
		}
	}
	return sdb;
}

const SyntaxDatabase * Container::getIndexDB(Syntax::Type type) const
{
	return indexes_[type].get();
}

void IndexDbNotify::postNotify(bool commit)
{
	if (!commit) {
		vector<int>::iterator it;
		for (it = dbs_.begin();
		     it != dbs_.end(); it++) {
			cont_->closeIndexes(*it);
		}
	}
	cont_->lock();
	IndexDbNotify *toDel = cont_->indexDbNotify_;
	DBXML_ASSERT(toDel == this);
	cont_->indexDbNotify_ = 0;
	cont_->unlock();
	delete toDel;
}

static unsigned int _checkVersion(DB *dbp)
{
  /* check version */
  const char *version="version";
  int ret;
  char databuf[20];
  DBT versKey, versData;
  memset(&versKey, 0, sizeof(versKey));
  memset(&versData, 0, sizeof(versData));
  versKey.data = (void*)version;
  versKey.size = strlen(version) + 1;
  versKey.ulen = versKey.size;
  versKey.flags = DB_DBT_USERMEM;
  versData.data = databuf;
  versData.size = 20;
  versData.ulen = 20;
  versData.flags = DB_DBT_USERMEM;
      
  ret = dbp->get(dbp, NULL, &versKey, &versData, 0);
  if (ret == 0)
    return (unsigned int)atoi(databuf);
  return 0;
}

//static
unsigned int Container::checkContainer(const string &name, DbEnv &dbenv)
{
	const char *dbxmlConfig = "secondary_configuration";
	DB *dbp;
	int ret;
	unsigned int version = 0;
	if (name.size() == 0)
		return 0;
	ret = db_create(&dbp, dbenv.get_DB_ENV(), 0);
	if (!ret) {
		ret = dbp->open(dbp, NULL, name.c_str(),
				dbxmlConfig, DB_BTREE, DB_RDONLY, 0);
		if (ret == 0) {
			version = _checkVersion(dbp);
		}
		dbp->close(dbp, 0);
	}
	return version;
}

//static
void Container::runOnAllDatabases(Transaction *txn,
				  DbWrapper::DbFunctionRunner &runner,
				  bool skipCoreDbs)
{
	// databases:
	//   configuration, sequence, dictionary primary/secondary,
	//   document secondary, document content|node storage
	//   [indexes]
	if (!skipCoreDbs) {
		configuration_->run(txn, runner);
		dictionary_->run(txn, runner);
	}
	documentDb_->run(txn, runner);

	// indexes are harder...
	int j = 0;
	const Syntax *syntax = SyntaxManager::getInstance()->getNextSyntax(j);
	while (syntax != 0) {
		SyntaxDatabase *sdb = indexes_[syntax->getType()].get();
		if (sdb)
			sdb->run(txn, runner);
		syntax = SyntaxManager::getInstance()->getNextSyntax(j);
	}
}


static void throwUnique(const XmlException &xe, const std::string name,
			bool isDuplicateMD)
{
	std::string str(xe.what());
	if (isDuplicateMD && str.find("name,http://www.sleepycat.com")) {
		str = "Document exists: ";
	} else {
		str.append(", document: ");
	}
	str.append(name);
	throw XmlException(XmlException::UNIQUE_ERROR, str);
}

