//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Document.cpp,v 1.195 2006/10/30 17:45:52 bostic Exp $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlNamespace.hpp"
#include "dbxml/XmlException.hpp"
#include "dbxml/XmlValue.hpp"
#include "dbxml/XmlData.hpp"
#include "Document.hpp"
#include "OperationContext.hpp"
#include "UTF8.hpp"
#include "Value.hpp"
#include "MetaDatum.hpp"
#include "Manager.hpp"
#include "ReferenceMinder.hpp"
#include "MemBufInputStream.hpp"
#include "nodeStore/NsDomReader.hpp"
#include "nodeStore/NsEventReader.hpp"
#include "nodeStore/EventReaderToWriter.hpp"
#include "nodeStore/NsXercesDom.hpp"
#include "nodeStore/NsDocument.hpp"
#include "nodeStore/NsSAX2Reader.hpp"
#include "nodeStore/NsTransientDomBuilder.hpp"
#include "nodeStore/NsXercesIndexer.hpp"
#include "nodeStore/NsXercesTranscoder.hpp"
#include "nodeStore/NsWriter.hpp"
#include "nodeStore/PullEventInputStream.hpp"
#include <dbxml/XmlEventReaderToWriter.hpp>
#include "Globals.hpp"

#include "db_utils.h"

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

Document::Document(XmlManager &mgr)
	: mgr_(mgr),
	  container_(0),
	  id_(0),
	  documentURI_(0),
	  validation_(DONT_VALIDATE),
	  lazy_(NEITHER),
	  db_flags_(0),
	  definitiveContent_(NONE),
	  contentModified_(false),
	  dbtContent_(0),
	  inputStream_(0),
	  reader_(0),
	  nsDocument_(0),
	  nsDomFactory_(0)
{
}

Document::~Document()
{
	reset();
}

bool Document::operator==(const Document &other) const
{
	// if doc IDs are 0, this is a transient doc,
	// so compare pointers; otherwise, use IDs
	if (id_ == 0)
		return (this == &other);
	return (id_ == other.id_);
}

Document *Document::createCopy() const
{
	Document *doc = new Document(mgr_);
	doc->setID(getID());
	doc->setContainer(container_);
	copyMetaData(*doc);
	return doc;
}

bool
Document::needsCopyForUpdate() const
{
	return (nsDocument_ && !contentModified_);
}

void Document::copyMetaData(Document &doc) const
{
	MetaData::const_iterator i;
	for(i = metaData_.begin(); i != metaData_.end(); ++i) {
		const Dbt *dbt = (*i)->getDbt();
		DbtOut *ddbt = new DbtOut(*dbt);
		MetaDatum *newmd = new MetaDatum((*i)->getName(), (*i)->getType(),
						 &ddbt, (*i)->isModified());
		doc.setMetaDataPtr(newmd);
	}
}

void Document::reset()
{
	if(!refMinders_.empty()) {
		RefMinders::iterator i;
		for(i = refMinders_.begin(); i != refMinders_.end();++i)
			(*i)->removeDocument(this);
		refMinders_.clear();
	}

	id_ = 0;
	setDocumentURI(0);
	validation_ = DONT_VALIDATE;

	lazy_ = NEITHER;
	oc_.set(0);
	db_flags_ = 0;

	definitiveContent_ = NONE;
	contentModified_ = false;

	MetaData::iterator i;
	for(i = metaData_.begin();i != metaData_.end();++i) {
		delete *i;
	}
	metaData_.clear();

	resetContentAsDbt();
	resetContentAsInputStream();
	resetContentAsDOM();
	resetContentAsEventReader();
	// hold reference on Container until done
	setContainer(0);
}

void Document::setDocumentURI(const XMLCh *uri)
{
	if(documentURI_)
		Globals::defaultMemoryManager->deallocate(documentURI_);
	documentURI_ = NsUtil::nsStringDup(Globals::defaultMemoryManager, uri, 0);
}

void Document::addReferenceMinder(ReferenceMinder *r)
{
	DBXML_ASSERT(r != 0);
	refMinders_.insert(r);
}

void Document::removeReferenceMinder(ReferenceMinder *r)
{
	DBXML_ASSERT(r != 0);
	refMinders_.erase(r);
}

void Document::setMetaDataFromContainer(TransactedContainer *container,
					const DocID &id, Transaction *txn,
					u_int32_t flags)
{
	// Set up lazy meta data
	setContainer(container);
	setID(id);
	oc_.set(txn);
	db_flags_ = flags & ~DBXML_LAZY_DOCS;
	lazy_ = BOTH;

	if((flags & DBXML_LAZY_DOCS) == 0) {
		setEagerMetaData();
	}
}

void Document::setEagerMetaData()
{
	if(lazy_ == BOTH) {
		if(definitiveContent_ == NONE) {
			if(container_->getContainerType() ==
			   XmlContainer::WholedocContainer)
				id2dbt();
			// This was "else id2dom()" for NLS; however,
			// that makes the definitiveContent_ DOM,
			// which bypasses the most efficient way to
			// materialize NLS -- via NsDocumentDatabase::getContent
		}
		int err = container_->getDocumentDB()->
			getAllMetaData(oc_, container_->getDictionaryDB(),
				       this, db_flags_);
		if(err != 0)
			throw XmlException(err);
		// NLS docs are still lazy at this point
		if(container_->getContainerType() ==
		   XmlContainer::WholedocContainer) {
			lazy_ = NEITHER;
			//oc_.set(0);
		}
		else lazy_ = CONTENT;
	}
}

void Document::setEagerContentAndMetaData()
{
	// force materialization (as Dbt) of Node Storage documents.
	// NOTE: this may change definitiveContent_ to Dbt (from DOM).
	// If definitiveContent_ is already DOM, it will use the DOM to
	// generate events, rather than the raw document.  This means
	// that modifications, if they exist, will be seen.
	//
	// TBD: consider a fetchAllDataAsDom() interface
	setEagerMetaData();
	getContentAsDbt();
	resetContentAsDOM();
	resetContentAsEventReader();
	lazy_ = NEITHER;
	//oc_.set(0);
}

void Document::setContainer(TransactedContainer *container)
{
	if(container_ != container) {
		if(container_) {
			container_->release();
		}
		container_ = container;
		if(container_) {
			container_->acquire();
			validation_ = (container_->getDoValidation()) ? VALIDATE :
				DONT_VALIDATE;
		} else {
			validation_ = DONT_VALIDATE;
		}
	}
}

TransactedContainer *Document::getContainer() const
{
	return container_;
}

void Document::setMetaData(const Name &name, const XmlValue &value, bool modified)
{
	switch (value.getType()) {
	case XmlValue::NODE:
		throw XmlException(XmlException::INVALID_VALUE,
				   "setMetaData expects a typed value, not a node");
		break;
	case XmlValue::NONE:
		throw XmlException(XmlException::INVALID_VALUE,
				   "setMetaData expects a typed value");
		break;
	case XmlValue::BINARY: {
		DbtOut *ddbt = new DbtOut(value.asBinary().getDbt());
		setMetaDataPtr(new MetaDatum(name, value.getType(),
					     &ddbt, modified));
		return;
	}
	default:
		// one of the atomic types
		break;
	}
	std::string v(value.asString());
	DbtOut *dbt = new DbtOut((void*)v.c_str(), v.length() + 1); // Include the terminating 0
	setMetaDataPtr(new MetaDatum(name, value.getType(), &dbt, modified));
}

void Document::setMetaData(const Name &name, XmlValue::Type type, const Dbt &value, bool modified)
{
	DbtOut *dbt = new DbtOut(value);
	setMetaData(name, type, &dbt, modified);
}

void Document::setMetaData(const Name &name, XmlValue::Type type,
			   DbtOut **value, bool modified) // Note: Consumes value
{
	setMetaDataPtr(new MetaDatum(name, type, value, modified)); // Note: Consumes Dbt.
}

void Document::setMetaDataPtr(MetaDatum *pmd)
{
	MetaData::iterator i;
	for (i = metaData_.begin();i != metaData_.end();++i) {
		if ((*i)->getName() == pmd->getName()) {
			(*i)->setDbt(pmd);
			return;
		}
	}
	metaData_.push_back(pmd);
}

bool Document::getMetaData(const Name &name, XmlValue &value)
{
	const MetaDatum *md = getMetaDataPtr(name);
	if (md != 0) {
		if(md->getDbt() == 0) return false;
		try {
			md->asValue(value);
		}
		catch(XmlException &e) {
			return false;  // cannot convert binary metadata to an XmlValue
		}
		return true;
	}

	return false;
}

bool Document::getMetaData(const Name &name, Dbt &value)
{
	const MetaDatum *md = getMetaDataPtr(name);
	if (md != 0) {
		const Dbt *dbt = md->getDbt();
		if(dbt == 0) return false;
		value.set_data(dbt->get_data());
		value.set_size(dbt->get_size());
		return true;
	}
	return false;
}

void Document::removeMetaData(const Name &name)
{
	MetaData::iterator i;
	bool removed = false;
	while (!removed) {
		for(i = metaData_.begin(); i != metaData_.end(); ++i) {
			if((*i)->getName() == name) {
				if (name == Name::dbxml_colon_name) {
					throw XmlException(
						XmlException::INVALID_VALUE,
						"You cannot remove the name metadata item");
				}
				(*i)->setRemoved();
				removed = true;
				break;
			}
		}
		if (!removed && (lazy_ == BOTH)) {
			// create a "removed" item for this name
			// type doesn't matter for removal
			XmlValue val("x");
			setMetaData(name, val, true);
		} else
			break;
	}
}

bool Document::containsMetaData(const Name &name) const
{
	MetaData::const_iterator i;
	for(i = metaData_.begin(); i != metaData_.end(); ++i) {
		if((*i)->getName() == name) {
			return (*i)->getDbt() != 0;
		}
	}
	return false;
}

bool Document::isMetaDataModified(const Name &name) const
{
	MetaData::const_iterator i;
	for(i = metaData_.begin(); i != metaData_.end(); ++i) {
		if((*i)->getName() == name) {
			return (*i)->isModified();
		}
	}
	return false;
}

const MetaDatum *Document::getMetaDataPtr(const Name &name) const
{
	const MetaDatum *r = 0;
	MetaData::const_iterator i;
	for(i = metaData_.begin(); i != metaData_.end(); ++i) {
		if((*i)->getName() == name) {
			if (!(*i)->isRemoved()) {
				r = *i;
			}
			// doc may be lazy_
			return r;
		}
	}
	if(r == 0 && lazy_ == BOTH) {
		// If we can't find it, then try looking it up in the database
		int err = 0;
		DbtOut *data = new DbtOut();
		try {
			XmlValue::Type type;
			err = container_->getDocumentDB()->
				getMetaData(oc_, container_->getDictionaryDB(), name,
					id_, type, data, db_flags_);
			if(err == 0) const_cast<Document*>(this)->
					     setMetaData(name, type, &data, /*modified*/false);
			delete data; // If not consumed by setThisFromDbt
		}
		catch(...) {
			delete data; // If not consumed by setThisFromDbt
			throw;
		}

		if(err == DB_NOTFOUND) {
			// Add a dummy MetaDatum, to avoid doing the database lookup again
			const_cast<Document*>(this)->setMetaDataPtr(new MetaDatum(name, XmlValue::NONE));
			err = 0;
		}
		else if(err != 0) {
			throw XmlException(err);
		}

		r = metaData_.back(); // The added MetaDatum will be last in the vector
	}
	return r;
}

std::string Document::getName() const
{
	const MetaDatum *name = getMetaDataPtr(Name::dbxml_colon_name);
	return name != 0 ? (name->getValue() != 0 ? name->getValue() : "") : "";
}

void Document::setName(const std::string &name, bool modified)
{
	DbtOut *dbt = new DbtOut(name.c_str(), name.length() + 1); // Store the trailing '\0'
	setMetaDataPtr(new MetaDatum(Name::dbxml_colon_name, XmlValue::STRING, &dbt, modified)); // Note: Consumes Dbt.
}

void Document::clearModified(const Name &name)
{
	MetaData::iterator i;
	for (i = metaData_.begin();i != metaData_.end();++i) {
		if ((*i)->getName() == name) {
			(*i)->setModified(false);
			return;
		}
	}
}

void Document::setContentAsDbt(DbtOut **value, bool setOnly) // Note: Consumes Dbt.
{

	if(*value != dbtContent_) {
		resetContentAsDbt();
		dbtContent_ = *value;
	} else {
		delete *value;
	}
	*value = 0;
	if (setOnly)
		return;
	definitiveContent_ = DBT;
	contentModified_ = true;

	resetContentAsInputStream();
	resetContentAsDOM();
	resetContentAsEventReader();
}

const Dbt *Document::getContentAsDbt() const
{
	switch(definitiveContent_) {
	case DBT: break;
	case INPUTSTREAM: stream2dbt(); break;
	case DOM: dom2dbt(); break;
	case READER: reader2dbt(); break;
	case NONE: id2dbt(); break;
	}

	return dbtContent_;
}

void Document::resetContentAsDbt() const
{
	delete dbtContent_;
	dbtContent_ = 0;
}

void Document::setContentAsInputStream(XmlInputStream **adopted_str)
{
	resetContentAsInputStream();
	inputStream_ = *adopted_str;
	*adopted_str = 0;

	definitiveContent_ = INPUTSTREAM;
	contentModified_ = true;

	resetContentAsDbt();
	resetContentAsDOM();
	resetContentAsEventReader();
}

void Document::setContentAsEventReader(XmlEventReader &reader)
{
	resetContentAsEventReader();
	reader_ = &reader;

	definitiveContent_ = READER;
	contentModified_ = true;

	resetContentAsDbt();
	resetContentAsDOM();
	resetContentAsInputStream();
}

// the object is donated to caller.  This is because
// input stream objects cannot be reset, so they are
// "one-use only"
XmlInputStream *Document::getContentAsInputStream() const
{
	switch(definitiveContent_) {
	case DBT: dbt2stream(); break;
	case INPUTSTREAM: break;
	case DOM: dom2stream(); break;
	case READER: reader2stream(); break;
	case NONE: id2stream(); break;
	}

	XmlInputStream *ret = inputStream_;
	inputStream_ = 0;
	if(definitiveContent_ == INPUTSTREAM) {
		definitiveContent_ = NONE;
	}
	return ret;
}

void Document::resetContentAsInputStream() const
{
	delete inputStream_;
	inputStream_ = 0;
}

void Document::resetContentAsEventReader() const
{
	if (reader_) {
		reader_->close();
		reader_ = 0;
	}
}

void Document::createNsObjects() const
{
	resetContentAsDOM();
	DictionaryDatabase *ddb = (container_ ? container_->getDictionaryDB() :
				   ((Manager&)mgr_).getDictionary());
	nsDomFactory_ = new NsXDOMFactory(Globals::defaultMemoryManager);
	nsDocument_ = new NsDocument(this, Globals::defaultMemoryManager);
	nsDocument_->initDOM(nsDomFactory_, id_, ddb);
	nsDocument_->setDocumentUri(documentURI_);
}

void Document::initNsObjects() const
{
	DBXML_ASSERT(nsDocument_ && container_);
	nsDocument_->initDoc(
		oc_.txn(),
		container_->getDocumentDB()->getNsDocumentDatabase(),
		container_->getDictionaryDB(),
		id_, db_flags_);
}

void Document::createBlankDOM() const
{
	createNsObjects();
	NsTransientDomBuilder builder(Globals::defaultMemoryManager,
		nsDocument_, nsDomFactory_);
	builder.startDocument(0);
	builder.endDocument();
}

void Document::changeContentToDOM() const
{
	switch(definitiveContent_) {
	case DBT: dbt2dom(oc_.txn()); break;
	case INPUTSTREAM: stream2dom(lazy_ != NEITHER ? oc_.txn() : 0); break;
	case DOM: break;
	case READER:
	{
		reader2stream();
		stream2dom((lazy_ != NEITHER ? oc_.txn() : 0), true);
		break;
	}
	case NONE: id2dom(); break;
	}
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *Document::getContentAsDOM() const
{
	changeContentToDOM();

	if(nsDocument_ == 0) {
		createBlankDOM();
	}

	return nsDomFactory_->getDocumentNode(nsDocument_);
}

DOMElement *Document::getElement(const NsNid *nid) const
{
	changeContentToDOM();
	return NsXDOMFactory::
		CastToDOMElement(nsDocument_->getDomElement(nid));
}

void Document::setContentAsDOM(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc)
{
	DOMDocument *documentNode = getContentAsDOM();
	if(doc != documentNode) {
		DOMNode *child;
		// Remove our document's children
		while((child = documentNode->getFirstChild()) != 0) {
			documentNode->removeChild(child);
			child->release();
		}

		child = doc->getFirstChild();
		while(child != 0) {
			// allow importNode to fail with null for
			// certain nodes that don't affect the info set
			DOMNode *cnode = documentNode->
				importNode(child, /*deep*/true);
			if (cnode)
				documentNode->appendChild(cnode);
			child = child->getNextSibling();
		}
	}

	definitiveContent_ = DOM;
	contentModified_ = true;

	resetContentAsDbt();
	resetContentAsInputStream();
}

void Document::domModified() const
{
	definitiveContent_ = DOM;
	contentModified_ = true;

	resetContentAsDbt();
	resetContentAsInputStream();
}

void Document::resetContentAsDOM() const
{
	if (nsDocument_)
		delete nsDocument_; // must precede nsDomFactory_ del
	if (nsDomFactory_)
		delete nsDomFactory_;
	nsDocument_ = 0;
	nsDomFactory_ = 0;
}

EventReader *Document::getElementAsReader(DOMElement *element) const
{
	NsDomElement *el = (NsDomElement*)((NsDomNode*)element->
		getInterface(_nsDomString));

	if(container_ == 0 ||
		container_->getContainerType() == XmlContainer::WholedocContainer ||
		isContentModified()) {
		return new NsDomReader(el);
	} else {
		DBXML_ASSERT(el->getNsDocument()->getDocId() == id_);
		// Otherwise get the data directly from the database
		return new NsEventReader(
			oc_.txn(),
			container_->getDocumentDB()->getNsDocumentDatabase(),
			container_->getDictionaryDB(),
			id_, db_flags_, Globals::defaultMemoryManager, el->getNodeId());
	}
}

NsPushEventSource *Document::getContentAsEventSource(
	Transaction *txn,
	bool needsValidation,
	bool nodeEvents,
	bool useID) const
{
	Content src = (useID ? NONE : definitiveContent_);
	switch (src) {
	case DBT: return dbt2events(txn, needsValidation, nodeEvents);
	case INPUTSTREAM: return stream2events(txn, needsValidation,
					       nodeEvents);
	case DOM: return dom2events(txn, needsValidation, nodeEvents);
	case READER:
	{
		DBXML_ASSERT(reader_);
		EventReaderToWriter *r2w = new EventReaderToWriter(
			*reader_,
			true, //owned
			false); //not internal
		reader_ = 0;
		definitiveContent_ = NONE;
		return r2w;
	}
	case NONE: return id2events(txn, needsValidation, nodeEvents);
	}
	return 0;
}

//
// Event translations...
//
XmlEventReader &Document::getContentAsEventReader() const
{
	switch(definitiveContent_) {
	case DBT: dbt2reader(); break;
	case INPUTSTREAM: stream2reader(); break;
	case DOM: dom2reader(); break;
	case READER: break;
	case NONE: id2reader(); break;
	}

	XmlEventReader *ret = reader_;
	reader_ = 0;
	if(definitiveContent_ == READER) {
		definitiveContent_ = NONE;
	}
	DBXML_ASSERT(ret);
	return *ret;
}

// write content to the writer, then close it
void Document::getContentAsEventWriter(XmlEventWriter &writer)
{
	// DBT or stream: parse directly to writer
	// DOM or READER: get XmlEventReader, use EventReaderToWriter.
	switch(definitiveContent_) {
	case DOM:
		dom2reader();
		break;
	case READER:
		break;
	case NONE:
		if(container_->getContainerType() ==
		   XmlContainer::NodeContainer)
			id2reader();
		break;
#if 1
        // TBD: see below -- try to avoid intermediate reader
        // by writing parse events directly to the writer
	case DBT: dbt2reader(); break;
	case INPUTSTREAM: stream2reader(); break;
#endif		
	default:
		break;
	}
	XmlEventReader *reader = reader_;
	if (reader)  {
		reader_ = 0;
		if(definitiveContent_ == READER)
			definitiveContent_ = NONE;
		XmlEventReaderToWriter r2w(*reader, writer, true);
		r2w.start();
	} else {
		// TBD: implement this -- it allows
		// the parse events to be written directly to the
		// writer, rather than using an intermediate reader
		// need to parse to events
		switch(definitiveContent_) {
		case DBT:
			dbt2stream();
			break;
		case INPUTSTREAM:
			break;
		case NONE:
			id2stream();
			break;
		default:
			DBXML_ASSERT(0);
			break;
		}
		// parse stream into writer
		DBXML_ASSERT(inputStream_ != 0);
		ScopedPtr<NsPushEventSource>
			pushSource(stream2events(
					   (lazy_ != NEITHER ? oc_.txn() : 0),
					   false, // needs validation
					   false)); // nodeEvents
#if 0
		// GMF TBD
		NsEventHandlerToWriter h2w(writer);
		pushSource->setNsEventHandler(&h2w);
		pushSource->start();
#endif		
	}
}

bool Document::isContentModified() const
{
	return contentModified_;
}

void Document::id2dbt() const
{
	if(lazy_ != NEITHER && dbtContent_ == 0) {
		int err = container_->getDocumentDB()->getContent(
			oc_, const_cast<Document*>(this), db_flags_);
		if(err != 0) {
			throw XmlException(err);
		}
		if (dbtContent_ && !isNodeDocument()) // no-content docs..
			definitiveContent_ = DBT;
		// this method wipes out any possible
		// modifications that were present in memory
		contentModified_ = false;
	}
}

void Document::id2dom(bool includeEntityInfo) const
{
	if(lazy_ != NEITHER && nsDocument_ == 0) {
		if(container_->getContainerType() == XmlContainer::NodeContainer) {
			createNsObjects();
			initNsObjects();
			definitiveContent_ = DOM;
			contentModified_ = false;
		} else {
			id2dbt();
			dbt2stream();
			stream2dom(oc_.txn(), includeEntityInfo);
		}
	}
}

void Document::id2stream() const
{
	if(lazy_ != NEITHER && inputStream_ == 0) {
		if(container_->getContainerType() == XmlContainer::NodeContainer) {
			resetContentAsInputStream();
			inputStream_ = new PullEventInputStream(
				new EventReaderToWriter(
					*(new NsEventReader(
					  oc_.txn(),
					  container_->getDocumentDB()->
					  getNsDocumentDatabase(),
					  container_->getDictionaryDB(),
					  id_, db_flags_,
					  Globals::defaultMemoryManager)),
					true,
					true /* owns reader */)
				);
		} else {
			id2dbt();
			dbt2stream();
		}
	}
}

void Document::dbt2stream() const
{
	if(inputStream_ == 0) {
		if (!dbtContent_ || (dbtContent_->get_size() == 0))
			return; // no-content doc
		resetContentAsInputStream();

		inputStream_ =
			new MemBufInputStream(
				(const char*)dbtContent_->get_data(),
				dbtContent_->get_size(), getName().c_str(),
				/*adoptBuffer*/false);
	}
}

void Document::dbt2dom(Transaction *txn, bool includeEntityInfo) const
{
	dbt2stream();
	stream2dom(txn, includeEntityInfo);
}

void Document::dom2dbt() const
{
	if(dbtContent_ == 0) {
		DBXML_ASSERT(nsDocument_);
		// if the user has modified the DOM, in place,
		// and called setContentAsDOM(), it will be noted
		// as contentModified_, so generate output from the DOM.
		// Otherwise, generate it directly from the database
		if(lazy_ == NEITHER || isContentModified()) {
			// Make output writer
			BufferNsStream output;
			NsWriter writer(&output);

			// Make event generator
			NsDomReader reader(nsDocument_->getDocumentNode());
			writer.writeFromReader(reader);

			resetContentAsDbt();
			// directly donate memory from Buffer to DbtOut
			dbtContent_ = new DbtOut();
			dbtContent_->setNoCopy(output.buffer.donateBuffer(),
					       output.buffer.getOccupancy());
		} else {
			id2dbt();
		}
	}
	// Do NOT change definitiveContent_, definitiveContent_ is still
	// DOM; dbtContent_ is a cache
}

void Document::dom2stream() const
{
	if(inputStream_ == 0) {
		DBXML_ASSERT(nsDocument_);

		// if the user has modified the DOM, in place,
		// and called setContentAsDOM(), it will be noted
		// as contentModified_, so generate output from the DOM.
		// Otherwise, generate it directly from the database
		if(lazy_ == NEITHER || isContentModified()) {
			resetContentAsInputStream();
			inputStream_ = new PullEventInputStream(
				new EventReaderToWriter(
					*(new NsDomReader(
						  nsDocument_->getDocumentNode())),
					true, // internal
					true /* owns reader */));
		} else {
			id2stream();
		}
	}
}

void Document::stream2dom(Transaction *txn, bool includeEntityInfo) const
{
	if(nsDocument_ == 0) {
		if(!inputStream_) // no content doc...
			return;

		//
		// This is the path used for creating DOM for querying
		// for wholedoc containers, and also for node containers
		// if the current document's definitive format is DBT or
		// stream.
		//
		createNsObjects();
		// TBD: figure out if this is for query, and pass that
		// info along, to suppress attempts to add to dictionary.
		// For now, false is safe.
		nsDocument_->initDOMParse(txn, db_flags_, false /* TBD */);
		try {
			unsigned int pflags = 0;
			if (validation_ == WF_ONLY)
				pflags |=
				  NsParserEventSource::NS_PARSER_WF_ONLY;
			else if (validation_ != VALIDATE)
				pflags |=
				  NsParserEventSource::NS_PARSER_WELL_FORMED;
			NsSAX2Reader parser(mgr_, txn, pflags);
			NsTransientDomBuilder builder(
				Globals::defaultMemoryManager,
				nsDocument_,
				nsDomFactory_,
				includeEntityInfo);
			parser.setNsEventHandler(&builder);

			parser.parse(&inputStream_);
		} catch (XMLException &e) {
			// shouldn't happen -- parser catches Xerces exceptions
			throw XmlException(XmlException::DOM_PARSER_ERROR,
					   XMLChToUTF8(e.getMessage()).str(),
					   e.getSrcFile(), e.getSrcLine());
		}

		// An inputstream can only be used once
		resetContentAsInputStream();
		if(definitiveContent_ == INPUTSTREAM) {
			definitiveContent_ = DOM;
		}
	}
}

#define TMP_BUF_SIZE (4 * 1024)
void Document::stream2dbt() const
{
	if(dbtContent_ == 0) {
		DBXML_ASSERT(inputStream_);

		char tmp[TMP_BUF_SIZE];
		Buffer buffer(0, 16 * 1024); // 16k buffer, to start

		unsigned int length;
		while((length = inputStream_->
		       readBytes(tmp, TMP_BUF_SIZE * sizeof(char))) != 0) {
			buffer.write((void*)tmp, length);
		}

		resetContentAsDbt();
		dbtContent_ =
			new DbtOut(buffer.getBuffer(), buffer.getOccupancy());

		// An inputstream can only be used once
		resetContentAsInputStream();
		if(definitiveContent_ == INPUTSTREAM) {
			definitiveContent_ = DBT;
		}
	}
}

NsPushEventSource *Document::id2events(Transaction *txn, bool needsValidation,
				       bool nodeEvents) const
{
	NsPushEventSource *result = 0;
	if(lazy_ != NEITHER) {
		if(container_->getContainerType() == XmlContainer::NodeContainer
		   && !needsValidation) {
			NsDocumentDatabase *ddb = container_->getDocumentDB()->
				getNsDocumentDatabase();
			result = new EventReaderToWriter(
				*(new NsEventReader(
					  txn, ddb,
					  container_->getDictionaryDB(),
					  id_, db_flags_,
					  Globals::defaultMemoryManager)),
				true, // internal
				true /* owns reader */);
		} else {
			id2stream();
			return stream2events(txn, needsValidation,
					     nodeEvents);
		}
	}
	return result;
}

NsPushEventSource *Document::dbt2events(Transaction *txn, bool needsValidation,
					bool nodeEvents) const 
{
	dbt2stream();
	return stream2events(txn, needsValidation, nodeEvents);
}

NsPushEventSource *Document::dom2events(Transaction *txn, bool needsValidation,
					bool nodeEvents) const 
{
	DBXML_ASSERT(nsDocument_);
	// may need to parse the document
	// in order to (1) validate if necessary, and (2) get it
	// into a node storage container.
	if (!needsValidation) {
		return 	new EventReaderToWriter(
			*(new NsDomReader(
				  nsDocument_->getDocumentNode())),
			true, //internal
			true /* owns reader */);
	} else {
		dom2stream();
		return stream2events(txn, needsValidation, nodeEvents);
	}
}

NsPushEventSource *Document::stream2events(Transaction *txn,
					   bool needsValidation,
					   bool nodeEvents) const
{
	if (!inputStream_) return 0; // no-content doc

	NsDocumentDatabase *ddb = 0;
	if (container_)
		ddb = container_->getDocumentDB()->
			getNsDocumentDatabase();
	NsEventTranslator *translator;
	if(nodeEvents || ddb != 0) {
		DBXML_ASSERT(container_);
		translator = new NsXercesTranscoder(txn, container_, id_, 0,
						    Globals::defaultMemoryManager);
	} else {
		translator = new NsXercesIndexer();
	}

	unsigned int pflags = 0;
	if (validation_ == WF_ONLY)
		pflags |= NsParserEventSource::NS_PARSER_WF_ONLY;
	else if (validation_ == DONT_VALIDATE)
		pflags |= NsParserEventSource::NS_PARSER_WELL_FORMED;
	else if (needsValidation)
		pflags |= NsParserEventSource::NS_PARSER_MUST_VALIDATE;
	NsParserEventSource *pes = new NsParserEventSource(mgr_, txn, pflags,
							   &inputStream_);

	if(definitiveContent_ == INPUTSTREAM) {
		definitiveContent_ = NONE;
	}

	return new NsPushEventSourceTranslator(pes, translator);
}

// XmlEventReader methods

//
// Generate an XmlEventReader from the id
//
void Document::id2reader() const
{
	if(lazy_ != NEITHER && reader_ == 0) {
		DBXML_ASSERT(container_);
		if(container_->getContainerType() ==
		   XmlContainer::NodeContainer) {
			reader_ = new NsEventReader(
				oc_.txn(),
				container_->getDocumentDB()->
				getNsDocumentDatabase(),
				container_->getDictionaryDB(),
				id_, db_flags_,
				Globals::defaultMemoryManager);
		} else {
			// wholedoc requires parsing into dom, which serves
			// as the source for the events to be read
			// The returned reader is not part of the document object
			id2dom(true);
			dom2reader();
		}
	}
}

void Document::dbt2reader() const
{
	// txn may be needed to lookup IDs in dictionary w/o deadlock
	dbt2dom(oc_.txn(), true);
	dom2reader();
}

void Document::dom2reader() const
{
	DBXML_ASSERT(nsDocument_);
	reader_ = new NsDomReader(nsDocument_->getDocumentNode());
}

void Document::stream2reader() const
{
	stream2dom(oc_.txn(), true);
	dom2reader();
}

void Document::reader2stream() const
{
	DBXML_ASSERT(reader_);
	resetContentAsInputStream();

	inputStream_ = new PullEventInputStream(
		new EventReaderToWriter(*reader_, true, true));
	if (definitiveContent_ == READER)
		definitiveContent_ = INPUTSTREAM;
	reader_ = 0;
}

void Document::reader2dbt() const
{
	DBXML_ASSERT(reader_);
	// write from reader to NsWriter
	BufferNsStream output;
	NsWriter writer(&output);
	EventReaderToWriter r2e(*reader_, writer, true);
	reader_ = 0;
	resetContentAsDbt();
	r2e.start();
	dbtContent_ = new DbtOut();
	dbtContent_->setNoCopy(output.buffer.donateBuffer(),
			       output.buffer.getOccupancy());
	if (definitiveContent_ == READER)
		definitiveContent_ = DBT;
}
	
MetaData::const_iterator Document::metaDataBegin() const {
	return metaData_.begin();
}

MetaData::const_iterator Document::metaDataEnd() const {
	return metaData_.end();
}

MetaDataIterator::MetaDataIterator(Document *document)
	: document_(document)
{
	document_->acquire();
	document_->setEagerMetaData();
	i_ = document_->metaDataBegin();
}

MetaDataIterator::~MetaDataIterator()
{
	document_->release();
}

bool MetaDataIterator::next(std::string &uri, std::string &name, XmlValue &value)
{
	if(i_ == document_->metaDataEnd()) {
		uri= "";
		name= "";
		value = XmlValue();
		return false;
	}
	else {
		const Name &mdName = (*i_)->getName();
		uri = mdName.getURI();
		name = mdName.getName();
		(*i_)->asValue(value);
		++i_;
		return true;
	}
}

void MetaDataIterator::reset()
{
	i_ = document_->metaDataBegin();
}
