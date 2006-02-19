//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: ReferenceMinder.cpp,v 1.11 2005/09/07 09:36:55 jsnelson Exp $
//

#include "ReferenceMinder.hpp"
#include "Document.hpp"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/XMLString.hpp>
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

void ReferenceMinder::addDocument(const XmlDocument &doc)
{
	Document *impl = (Document *)doc;

	if(!impl->getIdentifier().empty() && !findDocument(impl->getIdentifier())) {
		ids2documents_.insert(ID2DocumentMap::value_type(&impl->getIdentifier(), impl));
		impl->addReferenceMinder(this);
	}
}

void ReferenceMinder::removeDocument(const std::string &id)
{
	ID2DocumentMap::iterator i = ids2documents_.find(&id);
	if(i != ids2documents_.end()) ids2documents_.erase(i);
}

Document *ReferenceMinder::findDocument(const std::string &id)
{
	ID2DocumentMap::iterator i = ids2documents_.find(&id);
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

	containers_.clear();
	ids2documents_.clear();
}

bool ReferenceMinder::stringCompare::operator()(const std::string *s1, const std::string *s2) const
{
	return *s1 < *s2;
}
