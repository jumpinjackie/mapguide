// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: IndexEntry.cpp,v 1.23 2005/12/19 16:47:37 jsnelson Exp $
//

#include "nodeStore/NsUtil.hpp"
#include "IndexEntry.hpp"
#include "ScopedDbt.hpp"
#include "SyntaxDatabase.hpp"
#include "Container.hpp"
#include "Globals.hpp"
#include "dataItem/DbXmlNodeImpl.hpp"

using namespace DbXml;
using namespace std;

XERCES_CPP_NAMESPACE_USE

IndexEntry::IndexEntry()
{
	memset(this, 0, sizeof(IndexEntry));
}

IndexEntry::~IndexEntry()
{
	reset();
}

void IndexEntry::reset()
{
	NsUtil::nsFreeId(Globals::defaultMemoryManager, &node_id_);
	NsUtil::nsFreeId(Globals::defaultMemoryManager, &last_descendant_);
	memset(this, 0, sizeof(IndexEntry));
}

IndexEntry::Relationship IndexEntry::isSelf(const DbXmlNodeImpl *node) const
{
	DBXML_ASSERT(isSpecified(NODE_ID));
	DBXML_ASSERT(isSpecified(NODE_LEVEL));

	// We assume that the nodes come from the same
	// container and the same document

	if(node->getLevel() != getNodeLevel()) return REL_FALSE;

	int compare = NsUtil::nsCompareIds(&getNodeID(), node->getNID());
	if(compare < 0) return REL_TOO_SMALL;
	if(compare > 0) return REL_TOO_BIG;
	return REL_TRUE;
}

IndexEntry::Relationship IndexEntry::isChildOf(const DbXmlNodeImpl *parent) const
{
	DBXML_ASSERT(isSpecified(NODE_ID));
	DBXML_ASSERT(isSpecified(NODE_LEVEL));

	// We assume that the nodes come from the same
	// container and the same document

	if(!parent->hasElemChildren()) return REL_FALSE;

	if((parent->getLevel() + 1) != getNodeLevel()) return REL_FALSE;

	if(parent->getLevel() == 0) {
		// The document node only has one child,
		// and this already fulfilled the criteria
		return REL_TRUE;
	}

	if(NsUtil::nsCompareIds(&getNodeID(), parent->getNID()) <= 0) return REL_TOO_SMALL;

	const nid_t *maxNID = parent->getLastElemDescendantNID();
	if(maxNID == 0) {
		maxNID = parent->getLastElemChildNID(/*fetch*/true);
		if(maxNID == 0) return REL_TOO_BIG;
	}
	return (NsUtil::nsCompareIds(&getNodeID(), maxNID) <= 0) ? REL_TRUE : REL_TOO_BIG;
}

IndexEntry::Relationship IndexEntry::isDescendantOf(const DbXmlNodeImpl *parent, bool orSelf, const DynamicContext *context) const
{
	DBXML_ASSERT(isSpecified(NODE_ID));
	DBXML_ASSERT(isSpecified(NODE_LEVEL));

	// We assume that the nodes come from the same
	// container and the same document

	if(!orSelf && !parent->hasElemChildren()) {
		return REL_FALSE;
	}

	if(parent->getLevel() == 0) {
		// Everything is a descendant of the document node
		return REL_TRUE;
	}
	if(parent->getLevel() > getNodeLevel()) {
		return REL_FALSE;
	}
	if(!orSelf && parent->getLevel() == getNodeLevel()) {
		return REL_FALSE;
	}

	int compare = NsUtil::nsCompareIds(&getNodeID(), parent->getNID());
	if(compare < 0) {
		return REL_TOO_SMALL;
	}
	if(compare == 0) {
		return orSelf ? REL_TRUE : REL_TOO_SMALL;
	}

	const nid_t *maxNID = parent->getLastElemDescendantNID();
	if(maxNID == 0) {
		DbXmlNodeImpl::Ptr lastChild = parent;
		while(lastChild.notNull()) {
			if(!lastChild->hasElemChildren()) {
				return REL_TOO_BIG;
			}

			maxNID = lastChild->getLastElemChildNID(/*fetch*/true);
			if(maxNID == 0) {
				return REL_TOO_BIG;
			}

			if(NsUtil::nsCompareIds(&getNodeID(), maxNID) <= 0) {
				return REL_TRUE;
			}
			if(lastChild->getLevel() >= getNodeLevel()) {
				return REL_TOO_BIG;
			}

			lastChild = lastChild->getLastElemChild(context);
		}
		return REL_TOO_BIG;
	}

	return (NsUtil::nsCompareIds(&getNodeID(), maxNID) <= 0) ? REL_TRUE : REL_TOO_BIG;
}

const nid_t &IndexEntry::getNodeID() const
{
	if(node_) {
		return *nsGetNid(node_);
	} else {
		return node_id_;
	}
}

u_int32_t IndexEntry::getNodeLevel() const
{
	if(node_) {
		return node_->nd_level;
	} else {
		return node_level_;
	}
}

void IndexEntry::setNode(const nsNode_t *node)
{
	node_ = (nsNode_t*)node;
}

void IndexEntry::setLastDescendant(const nid_t *last_descendant)
{
	if(last_descendant == 0) {
		NsUtil::nsFreeId(Globals::defaultMemoryManager, &last_descendant_);
		memset(&last_descendant_, 0, sizeof(nid_t));		
	}
	else {
		NsUtil::nsCopyId(Globals::defaultMemoryManager, &last_descendant_, last_descendant);
	}
}

void IndexEntry::setDbtFromThis(DbtOut &dbt) const
{
	int count = marshal(0, /*count*/true);
	dbt.set(0, count);
	marshal((xmlbyte_t*)dbt.get_data(), /*count*/false);
}

void IndexEntry::setThisFromDbt(const Dbt &dbt)
{
	unmarshal(dbt);
}

int IndexEntry::marshal(xmlbyte_t *ptr, bool count) const
{
	int size = 0;

	if(count) {
		size += 1; // For the prefix byte
		size += docid_.marshalSize();

		if(isSpecified(NODE_ID)) {
			size += NsUtil::nsCountId(&getNodeID());
		} else {
			// We'll store a null instead, for ease of sorting
			size += 1;
		}
		if(isSpecified(LAST_CHILD_ID)) {
			const nid_t *lastchild = nsLastChildNid(node_);
			if(lastchild == 0) size += 1; // For a single null
			else size += NsUtil::nsCountId(lastchild);
		}
		if(isSpecified(NODE_LEVEL))
			size += NsUtil::nsCountInt(getNodeLevel());
		if(isSpecified(PARENT_ID))
			size += NsUtil::nsCountId(nsGetParentNid(node_));
		if(isSpecified(LAST_DESCENDANT_ID))
			size += NsUtil::nsCountId(&last_descendant_);
		if(isSpecified(ATTRIBUTE_INDEX))
			size += NsUtil::nsCountInt(attr_index_);
	}
	else {
		*ptr++ = (xmlbyte_t)format_;
		ptr += docid_.marshal(ptr);

		if(isSpecified(NODE_ID)) {
			ptr += NsUtil::nsMarshId(ptr, &getNodeID());
		} else {
			// We'll store a null instead, for ease of sorting
			*ptr++ = 0;
		}
		if(isSpecified(LAST_CHILD_ID)) {
			const nid_t *lastchild = nsLastChildNid(node_);
			if(lastchild == 0) *ptr++ = 0; // Store a single null
			else ptr += NsUtil::nsMarshId(ptr, lastchild);
		}
		if(isSpecified(NODE_LEVEL))
			ptr += NsUtil::nsMarshInt(ptr, getNodeLevel());
		if(isSpecified(PARENT_ID))
			ptr += NsUtil::nsMarshId(ptr, nsGetParentNid(node_));
		if(isSpecified(LAST_DESCENDANT_ID)) {
			if(last_descendant_.idLen == 0) *ptr++ = 0; // Store a single null
			else ptr += NsUtil::nsMarshId(ptr, &last_descendant_);
		}
		if(isSpecified(ATTRIBUTE_INDEX))
			ptr += NsUtil::nsMarshInt(ptr, attr_index_);
	}

	return size;
}

static inline void ieUnmarshId(MemoryManager *mmgr, const xmlbyte_t *&ptr, nid_t &nid)
{
	if(*ptr == 0) {
		// There is no NID
		++ptr;
		NsUtil::nsFreeId(mmgr, &nid);
		nid.idStore.idPtr = 0;
		nid.idLen = 0;
	}
	else {
		nid_t tmp;
		ptr += NsUtil::nsUnmarshId(ptr, &tmp, 0, /*copyStrings*/false);

		if(tmp.idLen > NID_BYTES_SIZE) {
			if(nid.idLen <= tmp.idLen && nid.idLen > NID_BYTES_SIZE) {
				// Reuse the existing memory
				nid.idLen = tmp.idLen;
				memcpy(nid.idStore.idPtr, tmp.idStore.idPtr, tmp.idLen);
			} else {
				NsUtil::nsFreeId(mmgr, &nid);
				nid.idStore.idPtr = (xmlbyte_t *)mmgr->allocate(tmp.idLen);
				if (!nid.idStore.idPtr)
					NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
						"copyId failed to allocate memory",
						__FILE__, __LINE__);
				memcpy(nid.idStore.idPtr, tmp.idStore.idPtr, tmp.idLen);
				nid.idLen = tmp.idLen;
			}
		} else {
			NsUtil::nsFreeId(mmgr, &nid);
			memcpy(nid.idStore.idBytes, tmp.idStore.idBytes, tmp.idLen);
			nid.idLen = tmp.idLen;
		}
	}
}

void IndexEntry::unmarshal(const Dbt &dbt)
{
	const xmlbyte_t *ptr = (const xmlbyte_t *)dbt.get_data();

	format_ = (Format)*ptr++;
	if(format_ >= KNOWN_FORMATS)
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "unknown format in index entry",
					 __FILE__, __LINE__);
	ptr += docid_.unmarshal(ptr);

	if(isSpecified(NODE_ID)) {
		ieUnmarshId(Globals::defaultMemoryManager, ptr, node_id_);
	} else {
		// Skip the null that was marshalled
		++ptr;
	}
	if(isSpecified(LAST_CHILD_ID)) {
		// We don't keep the last child id around
		// as it's not used
		nid_t tmp;
		ptr += NsUtil::nsUnmarshId(ptr, &tmp, 0, /*copyStrings*/false);
	}
	if(isSpecified(NODE_LEVEL)) {
		ptr += NsUtil::nsUnmarshInt(ptr, &node_level_);
	}
	if(isSpecified(PARENT_ID)) {
		// We don't keep the parent id around
		// as it's not used
		nid_t tmp;
		ptr += NsUtil::nsUnmarshId(ptr, &tmp, 0, /*copyStrings*/false);
	}
	if(isSpecified(LAST_DESCENDANT_ID)) {
		ieUnmarshId(Globals::defaultMemoryManager, ptr, last_descendant_);
	}
	if(isSpecified(ATTRIBUTE_INDEX)) {
		ptr += NsUtil::nsUnmarshInt(ptr, &attr_index_);
	}
}

bool IndexEntry::operator<(const IndexEntry &o) const
{
	if(docid_ != o.docid_)
		return docid_ < o.docid_;

	if(!isSpecified(NODE_ID)) {
		return o.isSpecified(NODE_ID);
	}
	if(!o.isSpecified(NODE_ID)) {
		return false;
	}

	return NsUtil::nsCompareIds(&getNodeID(),
				    &o.getNodeID()) < 0;
}

const bool IndexEntry::indexFormats_[IndexEntry::KNOWN_FORMATS][IndexEntry::INFO_MAX] = {
	/*                     NODE_ID,     NODE_LEVEL,   L_DESC_ID         */
	/*                          L_CHILD_ID,    PARENT_ID     ATTR_INDEX */
	/* D_FORMAT         */ {false, false, false, false, false, false},
	/* DSEL_OLD_FORMAT  */ {true,  true,  true,  false, false, false},
	/* DSEL_FORMAT      */ {true,  false, true,  false, true,  false},
	/* ATTR_OLD_FORMAT  */ {true,  false, false, false, false, true },
	/* ATTRIBUTE_FORMAT */ {true,  false, true,  false, false, true }
};

std::ostream& DbXml::operator<<(std::ostream& s, const IndexEntry &ie)
{
	s << ie.getDocID();
	if(ie.isSpecified(IndexEntry::NODE_ID)) {
		s << "(0x";

// 		bool doDot = false;
		const xmlbyte_t *nid = NsUtil::nsNode(&ie.getNodeID());
		while(*nid != 0) {
// 			if(doDot) s << '.';
// 			else doDot = true;

			xmlbyte_t hi = *nid >> 4;
			xmlbyte_t lo = *nid & 0xf;
			s << (hi > 9 ? (char)(hi - 9 + 'A') : (char)(hi + '0'));
			s << (lo > 9 ? (char)(lo - 9 + 'A') : (char)(lo + '0'));

			++nid;
		}

		s << ")";
	}
	return s;
}



void IndexData::set_union(const SharedPtr &o)
{
	if(o) {
		iterator otherIt = o->begin();
		iterator otherEnd = o->end();

		if(otherIt != otherEnd) {
			iterator hint = insert(*otherIt).first;
			for(++otherIt; otherIt != otherEnd; ++otherIt) {
				hint = insert(hint, *otherIt);
			}
		}
	}
}

void IndexData::set_intersection(const SharedPtr &o)
{
	if(o) {
		iterator thisIt = begin();
		iterator thisEnd = end();
		iterator otherIt = o->begin();
		iterator otherEnd = o->end();

		iterator tmpIt;
		while(thisIt != thisEnd) {
			if(otherIt == otherEnd || key_comp()(*thisIt, *otherIt)) {
				tmpIt = thisIt;
				++thisIt;
				erase(tmpIt);
			} else if(key_comp()(*otherIt, *thisIt)) {
				++otherIt;
			} else {
				++thisIt;
				++otherIt;
			}
		}
	}
}



IndexReadCache::IndexReadCache()
{
	// do nothing
}

void IndexReadCache::reset()
{
	idsMap_.clear();
	indexMap_.clear();
}

const IDS::SharedPtr &IndexReadCache::getIDS(const Container &container, OperationContext &context,
	SecondaryDatabase::Operation op1, const Key &k1, SecondaryDatabase::Operation op2, const Key &k2)
{
	IndexKey indexKey(&container, op1, k1, op2, k2);

	IDSMap::const_iterator it = idsMap_.find(indexKey);
	if(it != idsMap_.end()) {
		return it->second;
	}

	return lookupIDS(context, indexKey);
}

const IndexData::SharedPtr &IndexReadCache::getIndexData(const Container &container, OperationContext &context,
	SecondaryDatabase::Operation op1, const Key &k1, SecondaryDatabase::Operation op2, const Key &k2)
{
	IndexKey indexKey(&container, op1, k1, op2, k2);

	IndexMap::const_iterator it = indexMap_.find(indexKey);
	if(it != indexMap_.end()) {
		return it->second;
	}

	return lookupIndexData(context, indexKey);
}

bool IndexReadCache::containsIndexData(const Container &container, SecondaryDatabase::Operation op1,
	const Key &k1, SecondaryDatabase::Operation op2, const Key &k2) const
{
	IndexKey indexKey(&container, op1, k1, op2, k2);

	IndexMap::const_iterator it = indexMap_.find(indexKey);
	if(it != indexMap_.end()) {
		return true;
	}

	return false;
}

const IDS::SharedPtr &IndexReadCache::lookupIDS(OperationContext &context, const IndexKey &indexKey)
{
	IndexData::SharedPtr data(0);
	IndexMap::const_iterator it = indexMap_.find(indexKey);
	if(it != indexMap_.end()) {
		data = it->second;
	} else {
		data = lookupIndexData(context, indexKey);
	}

	IDS::SharedPtr ids(new IDS);
	ID id;
	ID lastid(0);
	IndexData::iterator end = data->end();
	for(IndexData::iterator i = data->begin(); i != end; ++i) {
		id = (*i)->getDocID();
		if(id != lastid) {
			ids->push_back(id);
			lastid = id;
		}
	}

	return idsMap_.insert(IDSMap::value_type(indexKey, ids)).first->second;
}

const IndexData::SharedPtr &IndexReadCache::lookupIndexData(OperationContext &context, const IndexKey &indexKey)
{
	IndexData::SharedPtr data(new IndexData);

	const SyntaxDatabase *database = indexKey.container->getIndexDB(indexKey.key1.getSyntaxType());

	int err = database->getIndexData(context, data, indexKey.operation1, indexKey.key1, indexKey.operation2, indexKey.key2);
	if(err) throw XmlException(err);

	return indexMap_.insert(IndexMap::value_type(indexKey, data)).first->second;
}

bool IndexReadCache::IndexKey::operator<(const IndexKey &o) const
{
	if(container < o.container) return true;
	if(o.container < container) return false;
	if(operation1 < o.operation1) return true;
	if(o.operation1 < operation1) return false;
	if(key1 < o.key1) return true;
	if(o.key1 < key1) return false;
	if(operation2 < o.operation2) return true;
	if(o.operation2 < operation2) return false;
	if(key2 < o.key2) return true;
	if(o.key2 < key2) return false;
	return false;
}
