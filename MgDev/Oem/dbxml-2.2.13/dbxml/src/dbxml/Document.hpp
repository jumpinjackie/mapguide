//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: Document.hpp,v 1.81 2005/09/20 14:35:51 jsnelson Exp $
//

#ifndef __DOCUMENT_HPP
#define	__DOCUMENT_HPP

#include <vector>
#include <string>
#include <map>
#include "dbxml/XmlValue.hpp"
#include "ReferenceCounted.hpp"
#include "ID.hpp"
#include "ScopedDbt.hpp"
#include "shared_ptr.hpp"
#include "Value.hpp"
#include "MetaDatum.hpp"
#include "TransactedContainer.hpp"
#include <dbxml/XmlInputStream.hpp>
#include "OperationContext.hpp"

#if defined(DBXML_DOM_XERCES2)
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#endif

namespace DbXml
{

class Container;
class DbtOut;
class OperationContext;
class QueryContext;
class NsDocument;
class NsDocumentDatabase;
class NsXDOMFactory;
class NsPushEventSource8;
class NsPullEventSource8;
class NsEventTranslator;
class ReferenceMinder;
	
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
		DOM = 4
	};
public:
	Document(XmlManager &db);
	virtual ~Document();

	Document *createCopy() const;
	
	void reset();
	
	void setMetaDataFromContainer(TransactedContainer *container, const ID &id, Transaction *txn, u_int32_t flags);
	void setEagerMetaData();
	void setEagerContentAndMetaData();

	void setContainer(TransactedContainer *container);
	TransactedContainer *getContainer();

	void setValidate(bool validate) {
		validate_ = validate;
	}
	
	/**
	 * Set the ID of a document. The ID is assigned when the document
	 * is first written to a container.
	 */
	void setID(const ID &id) {
		id_ = id;
	}
	const ID &getID() const {
		return id_;
	}
	ID &getIDToSet() {
		return id_;
	}

	// Identifier
	const std::string &getIdentifier() const {
		return identifier_;
	}
	void setIdentifier(const std::string &id) {
		identifier_ = id;
	}
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
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *getContentAsDOM() const;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *getElement(const nid_t *nid) const;
	void setContentAsDOM(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc);
	void resetContentAsDOM() const;
	NsDocument *getNsDocument() const
	{
		return nsDocument_;
	}

	// Content as SAX
	NsPullEventSource8 *getElementAsSAX(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *element) const;
	NsPushEventSource8 *getContentAsSAX(Transaction *txn,
		bool needsValidation, // true on initial input (wholedoc only)
		bool nodeEvents, // Provide nsNode_t objects in the events
		NsDocumentDatabase *ddb // If provided, will have the nodes stored in it
		) const;

	bool isNodeDocument() const
	{
		return container_ && container_->getContainerType() ==
			XmlContainer::NodeContainer;
	}
	void initNsObjects() const;

private:
	// no need for copy and assignment
	Document(const Document&);
	Document &operator=(const Document &);

	void copyMetaData(Document &to) const;
	
	/** @name Content Conversion Methods */
	// @{

	void id2dbt() const;
	void id2dom() const;
	void id2stream() const;
	void dbt2stream() const;
	void dbt2dom(Transaction *txn) const;
	void dom2dbt() const;
	void dom2stream() const;
	void stream2dom(Transaction *txn) const;
	void stream2dbt() const;

	NsPushEventSource8 *id2sax(Transaction *txn, bool needsValidation,
		bool nodeEvents, NsDocumentDatabase *ddb) const;
	NsPushEventSource8 *dbt2sax(Transaction *txn, bool needsValidation,
		bool nodeEvents, NsDocumentDatabase *ddb) const;
	NsPushEventSource8 *dom2sax(Transaction *txn, bool needsValidation,
		bool nodeEvents, NsDocumentDatabase *ddb) const;
	NsPushEventSource8 *stream2sax(Transaction *txn, bool needsValidation,
		bool nodeEvents, NsDocumentDatabase *ddb) const;

	// @}

	void createBlankDOM() const;
	void createNsObjects() const;

private:
	/** @name Document ID attributes */

	mutable XmlManager mgr_;
	TransactedContainer *container_;
	ID id_;
	std::string identifier_; ///< Made from id and container name
	bool validate_;

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

	/** @name content as DOM attributes */

	mutable NsDocument *nsDocument_;
	mutable NsXDOMFactory *nsDomFactory_;
	mutable XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *document_;
};

}

#endif

