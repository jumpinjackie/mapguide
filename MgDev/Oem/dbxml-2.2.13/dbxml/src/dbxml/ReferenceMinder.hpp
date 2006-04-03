//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: ReferenceMinder.hpp,v 1.6 2005/08/16 13:29:31 jsnelson Exp $
//

#ifndef __REFERENCEMINDER_HPP
#define	__REFERENCEMINDER_HPP

#include "DbXmlInternal.hpp"
#include <dbxml/XmlDocument.hpp>
#include <dbxml/XmlContainer.hpp>

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMDocument.hpp>

#include <map>

namespace DbXml
{

class Document;

class ReferenceMinder
{
public:
	ReferenceMinder() {}
	~ReferenceMinder();

	void addContainer(TransactedContainer *container);
	void removeContainer(TransactedContainer *container);

	void addDocument(const XmlDocument &doc);
	void removeDocument(const std::string &id);
	/// Returns 0 if none found
	Document *findDocument(const std::string &id);

	void resetMinder();

private:
	ReferenceMinder(const ReferenceMinder&);
	ReferenceMinder &operator=(const ReferenceMinder&);

	struct stringCompare
	{
		bool operator()(const std::string *s1, const std::string *s2) const;
	};

	typedef std::map<TransactedContainer*, XmlContainer> ContainerMap;
	typedef std::map<const std::string*, Document*, stringCompare> ID2DocumentMap;

	ContainerMap containers_;
	ID2DocumentMap ids2documents_;
};

}

#endif
