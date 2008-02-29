//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: ReferenceMinder.hpp,v 1.10 2006/11/21 18:49:43 jsnelson Exp $
//

#ifndef __REFERENCEMINDER_HPP
#define	__REFERENCEMINDER_HPP

#include "DbXmlInternal.hpp"
#include <dbxml/XmlContainer.hpp>

#include "DocID.hpp"

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMDocument.hpp>

#include <map>

namespace DbXml
{

class Document;
class Container;

class ReferenceMinder
{
public:
	struct DocMapKey
	{
		DocMapKey(const Container *c, DocID i)
			: container(c), id(i) {}

		const Container *container;
		DocID id;
	};

	ReferenceMinder() {}
	~ReferenceMinder();

	void addContainer(TransactedContainer *container);
	void removeContainer(TransactedContainer *container);

	void addDocument(Document *doc);
	void removeDocument(Document *doc);
	/// Returns 0 if none found
	Document *findDocument(const XMLCh *uri);
	/// Returns 0 if none found
	Document *findDocument(const Container *container, DocID id);

	void resetMinder();

private:
	ReferenceMinder(const ReferenceMinder&);
	ReferenceMinder &operator=(const ReferenceMinder&);

	struct docMapCompare
	{
		bool operator()(const DocMapKey &s1, const DocMapKey &s2) const;
	};

	struct xmlchCompare
	{
		bool operator()(const XMLCh *s1, const XMLCh *s2) const;
	};

	typedef std::map<TransactedContainer*, XmlContainer> ContainerMap;
	typedef std::map<DocMapKey, Document*, docMapCompare> ID2DocumentMap;
	typedef std::map<const XMLCh*, Document*, xmlchCompare> URI2DocumentMap;

	ContainerMap containers_;
	ID2DocumentMap ids2documents_;
	URI2DocumentMap uris2documents_;
};

}

#endif
