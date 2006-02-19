//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlDocument.hpp,v 1.52 2005/09/07 09:06:22 jsnelson Exp $
//

#ifndef __XMLDOCUMENT_HPP
#define	__XMLDOCUMENT_HPP

#include "XmlPortability.hpp"
#include "DbXmlFwd.hpp"
#include <string>

namespace DbXml
{

class Document;
	
class DBXML_EXPORT XmlDocument
{
public:
	/** @name For Reference Counting */
	XmlDocument();
	XmlDocument(const XmlDocument&);
	XmlDocument &operator=(const XmlDocument &);
	virtual ~XmlDocument();
	bool isNull() const { return document_ == 0; }
	
	/** @name Identifier Methods */
	void setName(const std::string &name);
	std::string getName() const;
	
	/** @name Content Methods */
	std::string &getContent(std::string &content) const;
	void setContent(const std::string &content);

	XmlData getContent() const;
	void setContent(const XmlData &content);

	/** @name ContentAs* Methods */
	void setContentAsXmlInputStream(XmlInputStream *adopted_str);
	// XmlInputStream is donated to caller
	XmlInputStream *getContentAsXmlInputStream() const;
	
	// change namespace with Xerces version
	void setContentAsDOM(XERCESC::DOMDocument *document);
	XERCESC::DOMDocument *getContentAsDOM() const;
	
	/** @name MetaData Methods */
	/// Turns a lazy document into an eager document
	void fetchAllData();
	bool getMetaData(const std::string &uri, const std::string &name,
			 XmlValue &value) const;
	void setMetaData(const std::string &uri, const std::string &name,
			 const XmlValue &value);
	bool getMetaData(const std::string &uri, const std::string &name,
			 XmlData &value) const;
	void setMetaData(const std::string &uri, const std::string &name,
			 const XmlData &value);
	void removeMetaData(const std::string &uri, const std::string &name);

	/** @name MetaData Iterator Methods */
	XmlMetaDataIterator getMetaDataIterator() const;

	/** @name Private Methods (for internal use) */
	// @{
	
	XmlDocument(Document *document);
	operator Document &() const {
		return *document_;
	}
	operator Document *() const {
		return document_;
	}

	bool operator == (const XmlDocument &other) const {
		return (document_ == other.document_);
	}
	bool operator != (const XmlDocument &other) const {
		return (document_ != other.document_);
	}
	
	//@}
	
private:
	Document *document_;
};

}

#endif
