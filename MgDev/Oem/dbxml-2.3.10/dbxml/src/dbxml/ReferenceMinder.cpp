//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: ReferenceMinder.cpp,v 1.14 2006/10/30 17:45:53 bostic Exp $
//

#include "ReferenceMinder.hpp"
#include "Document.hpp"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/XMLString.hpp>

#include <xqilla/utils/XPath2Utils.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

ReferenceMinder::~ReferenceMinder()
{
	resetMinder();
}

void ReferenceMinder::addContainer(TransactedContainer *container)
{
	containers_.insert(ContainerMap::value_type(container, container));
}

void ReferenceMinder::removeContainer(TransactedContainer *container)
{
	ContainerMap::iterator i = containers_.find(container);
	if(i != containers_.end()) containers_.erase(i);
}

void ReferenceMinder::addDocument(Document *doc)
{
	if(doc->getContainer()) {
		DocMapKey key(doc->getContainer(), doc->getID());
		if(ids2documents_.insert(ID2DocumentMap::value_type(key, doc)).second) {
			doc->addReferenceMinder(this);
		}
	} else {
		if(uris2documents_.insert(URI2DocumentMap::value_type(doc->getDocumentURI(), doc)).second) {
			doc->addReferenceMinder(this);
		}
	}
}

void ReferenceMinder::removeDocument(Document *doc)
{
	if(doc->getContainer()) {
		ids2documents_.erase(DocMapKey(doc->getContainer(), doc->getID()));
	} else {
		uris2documents_.erase(doc->getDocumentURI());
	}
}

Document *ReferenceMinder::findDocument(const XMLCh *uri)
{
	URI2DocumentMap::iterator i = uris2documents_.find(uri);
	if(i != uris2documents_.end()) {
		return i->second;
	}
	return 0;
}

Document *ReferenceMinder::findDocument(const Container *container, DocID id)
{
	ID2DocumentMap::iterator i = ids2documents_.find(DocMapKey(container, id));
	if(i != ids2documents_.end()) {
		return i->second;
	}
	return 0;
}

void ReferenceMinder::resetMinder()
{
	// Let the Document objects know that we don't hold a
	// reference to them anymore
	ID2DocumentMap::iterator i = ids2documents_.begin();
	for(; i != ids2documents_.end(); ++i) {
		i->second->removeReferenceMinder(this);
	}
	URI2DocumentMap::iterator j = uris2documents_.begin();
	for(; j != uris2documents_.end(); ++j) {
		j->second->removeReferenceMinder(this);
	}

	containers_.clear();
	ids2documents_.clear();
	uris2documents_.clear();
}

bool ReferenceMinder::docMapCompare::operator()(const DocMapKey &s1, const DocMapKey &s2) const
{
	if(s1.id < s2.id) return true;
	if(s1.id > s2.id) return false;
	return s1.container < s2.container;
}

bool ReferenceMinder::xmlchCompare::operator()(const XMLCh *s1, const XMLCh *s2) const
{
	return XPath2Utils::compare(s1, s2) < 0;
}
