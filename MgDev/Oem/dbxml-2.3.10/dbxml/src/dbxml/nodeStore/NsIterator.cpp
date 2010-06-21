//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsIterator.cpp,v 1.4 2006/10/30 17:45:57 bostic Exp $
//

#include "NsIterator.hpp"

using namespace DbXml;
using namespace std;

NsItem::Relationship NsItem::isAncestorOf(const NsItem &d) const
{
	// TBD check the container is the same - jpcs

	if (getDocID() < d.getDocID()) return REL_TOO_BIG;
	if (getDocID() > d.getDocID()) return REL_TOO_SMALL;

	long cmp = getNodeID().compareNids(&d.getNodeID());
	if(cmp >= 0) return REL_TOO_SMALL;

	cmp = getLastDescendant().compareNids(&d.getNodeID());
	if(cmp < 0) return REL_TOO_BIG;

	return REL_TRUE;
}

NsStructuralJoin::NsStructuralJoin(NsIterator *ancestors, NsIterator *descendants)
	: ancestors_(ancestors),
	  descendants_(descendants),
	  state_(INIT)
{
}

NsStructuralJoin::~NsStructuralJoin()
{
	delete ancestors_;
	delete descendants_;
}

bool NsStructuralJoin::next(NsItem &result)
{
	switch(state_) {
	case INIT: {
		state_ = RUNNING;
		if(!ancestors_->next(ancestor_)) {
			state_ = DONE;
			return false;
		}
		// Fall through
	}
	case RUNNING: {
		NsItem descendant;
		if(!descendants_->next(descendant))
			state_ = DONE;
		return doJoin(descendant, result);
	}
	case DONE: break;
	}
	return false;
}

bool NsStructuralJoin::seek(DocID did, const NsNid &nid, NsItem &result)
{
	switch(state_) {
	case INIT: {
		state_ = RUNNING;
		if(!ancestors_->next(ancestor_)) {
			state_ = DONE;
			return false;
		}
		// Fall through
	}
	case RUNNING: {
		NsItem descendant;
		if(!descendants_->seek(did, nid, descendant))
			state_ = DONE;
		return doJoin(descendant, result);
	}
	case DONE: break;
	}
	return false;
}

bool NsStructuralJoin::doJoin(NsItem &descendant, NsItem &result)
{
	while(state_ != DONE) {
		switch(ancestor_.isAncestorOf(descendant)) {
		case NsItem::REL_TRUE: {
			result = descendant;
			return true;
		}
		case NsItem::REL_TOO_BIG: {
			if(!ancestors_->seek(ancestor_.getDocID(), ancestor_.getLastDescendant(),
				   ancestor_)) state_ = DONE;
			// loop
			break;
		}
		case NsItem::REL_TOO_SMALL: {
			if(!descendants_->seek(ancestor_.getDocID(), ancestor_.getNodeID(),
				   descendant)) state_ = DONE;
			// loop
			break;
		}
		}
	}
	return false;
}

void NsStructuralJoin::reset()
{
	ancestors_->reset();
	descendants_->reset();
	ancestor_.clear();
	state_ = INIT;
}
