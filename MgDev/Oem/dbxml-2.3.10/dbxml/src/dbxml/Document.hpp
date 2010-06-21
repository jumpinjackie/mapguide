//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Document.hpp,v 1.101 2006/10/30 17:45:52 bostic Exp $
//

#ifndef __DOCUMENT_HPP
#define	__DOCUMENT_HPP

#include <vector>
#include <string>
#include <map>
#include "dbxml/XmlValue.hpp"
#include "ReferenceCounted.hpp"
#include "DocID.hpp"
#include "ScopedDbt.hpp"
#include "SharedPtr.hpp"
#include "Value.hpp"
#include "MetaDatum.hpp"
#include "TransactedContainer.hpp"
#include <dbxml/XmlInputStream.hpp>
#include "OperationContext.hpp"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

namespace DbXml
{

class Container;
class DbtOut;
class OperationContext;
class QueryContext;
class NsDocument;
class NsXDOMFactory;
class NsPushEventSource;
class EventReader;
class EventWriter;
class NsEventTranslator;
class ReferenceMinder;
class XmlEventReader;
	
typedef std::vector<MetaDatum*> MetaData;

class MetaDataIterator : public ReferenceCounted
{
public:
	MetaDataIterator(Document *document);
	~MetaDataIterator();
	
	bool next(std::string &uri, std::string &name, XmlValue &value);
	void reset();
	
private:
	// no need for copy and assignment
	MetaDataIterator(const MetaDataIterator&);
	MetaDataIterator &operator=(const MetaDataIterator&);

	Document *document_;
	MetaData::const_iterator i_;
};
	
class Document : public ReferenceCounted
{
public:
 	enum Content {
		// treat these as flags
		NONE = 0,
		DBT = 1,
		INPUTSTREAM = 2,
		DOM = 4,
		READER = 8
	};
	enum Validation {
		VALIDATE = 1,
		DONT_VALIDATE = 2,
		WF_ONLY = 3
	};
public:
	Document(XmlManager &mgr);
	virtual ~Document();

	Document *createCopy() const;

	void reset();
	
	void setMetaDataFromContainer(TransactedContainer *container,
				      const DocID &id, Transaction *txn,
				      u_int32_t flags);
	void setEagerMetaData();
	void setEagerContentAndMetaData();

	void setContainer(TransactedContainer *container);
	TransactedContainer *getContainer() const;

	void setValidation(int validation) {
		validation_ = validation;
	}
	
	/**
	 * Set the ID of a document. The ID is assigned when the document
	 * is first written to a container.
	 */
	void setID(const DocID &id) {
		id_ = id;
	}
	const DocID &getID() const {
		return id_;
	}
	DocID &getIDToSet() {
		return id_;
	}

	const XMLCh *getDocumentURI() const {
		return documentURI_;
	}
	void setDocumentURI(const XMLCh *uri);

	void addReferenceMinder(ReferenceMinder *r);
	void removeReferenceMinder(ReferenceMinder *r);

	// Name
	void setName(const std::string &name, bool modified);
	std::string getName() const;
	void clearModified(const Name &name);

	// Meta-Data
	bool containsMetaData(const Name &name) const;
	bool isMetaDataModified(const Name &name) const;

	// Meta-Data, as XmlValue
	void setMetaData(const Name &name, const XmlValue &value, bool modified);
	bool getMetaData(const Name &name, XmlValue &value);

	// Meta-Data, as Dbt
	void setMetaData(const Name &name, XmlValue::Type type,
			 const Dbt &value, bool modified);
	void setMetaData(const Name &name, XmlValue::Type type,
			 DbtOut **value, bool modified); // Note: Consumes value
	bool getMetaData(const Name &name, Dbt &value);

	// Remove Meta-Data item
	void removeMetaData(const Name &name);

	// Meta-Data, as MetaDatum
	const MetaDatum *getMetaDataPtr(const Name &name) const;
	void setMetaDataPtr(MetaDatum *mdp);
	MetaData::const_iterator metaDataBegin() const;
	MetaData::const_iterator metaDataEnd() const;

	// Content
	bool isContentModified() const;
	void setContentModified(bool modified) const
	{
		contentModified_ = modified;
	}
	Content getDefinitiveContent() const
	{
		return definitiveContent_;
	}
	void domModified() const;

	bool needsCopyForUpdate() const;
	
	// Content as Dbt
	void setContentAsDbt(DbtOut **value, bool setOnly = false); // Consumes value
	const Dbt *getContentAsDbt() const;
	void resetContentAsDbt() const;

	// Content as InputStream
	void setContentAsInputStream(XmlInputStream **adopted_str);
	XmlInputStream *getContentAsInputStream() const; // donated to caller
	void resetContentAsInputStream() const;

	// Content as DOM
	void changeContentToDOM() const;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *getContentAsDOM() const;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *getElement(const NsNid *nid) const;
	void setContentAsDOM(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc);
	void resetContentAsDOM() const;

	// Content as XmlEventReader
	void setContentAsEventReader(XmlEventReader &reader);
	XmlEventReader &getContentAsEventReader() const;
	void resetContentAsEventReader() const;

	// Content as XmlEventWriter
	void getContentAsEventWriter(XmlEventWriter &writer);

	NsDocument *getNsDocument() const
	{
		return nsDocument_;
	}

	// Content as events
	EventReader *getElementAsReader(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *element) const;
	NsPushEventSource *getContentAsEventSource(
		Transaction *txn,
		bool needsValidation, // true on initial input (wholedoc only)
		bool nodeEvents, // Provide NsNode objects in the events
		bool useId = false // force use of ID, not definitiveContent_
		) const;

	bool isNodeDocument() const
	{
		return container_ && container_->getContainerType() ==
			XmlContainer::NodeContainer;
	}
	void initNsObjects() const;

	bool operator==(const Document &other) const;
private:
	// no need for copy and assignment
	Document(const Document&);
	Document &operator=(const Document &);

	void copyMetaData(Document &to) const;
	
	/** @name Content Conversion Methods */
	// @{

	void id2dbt() const;
	void id2dom(bool includeEntityInfo = false) const;
	void id2stream() const;
	void id2reader() const;
	void dbt2stream() const;
	void dbt2dom(Transaction *txn, bool includeEntityInfo = false) const;
	void dbt2reader() const;
	void dom2dbt() const;
	void dom2stream() const;
	void dom2reader() const;
	void stream2dom(Transaction *txn, bool includeEntityInfo = false) const;
	void stream2dbt() const;
	void stream2reader() const;

	void reader2dbt() const;
	void reader2stream() const;

	NsPushEventSource *id2events(Transaction *txn, bool needsValidation,
		bool nodeEvents) const;
	NsPushEventSource *dbt2events(Transaction *txn, bool needsValidation,
		bool nodeEvents) const;
	NsPushEventSource *dom2events(Transaction *txn, bool needsValidation,
		bool nodeEvents) const;
	NsPushEventSource *stream2events(Transaction *txn, bool needsValidation,
		bool nodeEvents) const;

	// @}

	void createBlankDOM() const;
	void createNsObjects() const;

private:
	/** @name Document ID attributes */

	mutable XmlManager mgr_;
	TransactedContainer *container_;
	DocID id_;
	XMLCh *documentURI_;
	int validation_;

	typedef std::set<ReferenceMinder*> RefMinders;
	RefMinders refMinders_; ///< The ReferenceMinder objects that are pointing to us

	/** @name Metadata attributes */

	MetaData metaData_;

	/** @name Lazy document attributes */

	enum LazyMode {
		NEITHER,
		CONTENT,
		BOTH
	};

	LazyMode lazy_;
	mutable OperationContext oc_;
	u_int32_t db_flags_;

	/** @name content management attributes */

	mutable Content definitiveContent_;
	mutable bool contentModified_;

	/** @name content as dbt attributes */

	mutable DbtOut *dbtContent_;

	/** @name content as input stream attributes */

	mutable XmlInputStream *inputStream_;

	/** @name content as input stream attributes */

	mutable XmlEventReader *reader_;

	/** @name content as DOM attributes */

	mutable NsDocument *nsDocument_;
	mutable NsXDOMFactory *nsDomFactory_;
};

}

#endif

