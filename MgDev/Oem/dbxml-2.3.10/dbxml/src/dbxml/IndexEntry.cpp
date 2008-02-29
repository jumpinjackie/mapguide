//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: IndexEntry.cpp,v 1.40 2006/11/17 18:18:25 jsnelson Exp $
//

#include "nodeStore/NsNode.hpp"
#include "IndexEntry.hpp"
#include "ScopedDbt.hpp"
#include "SyntaxDatabase.hpp"
#include "Container.hpp"
#include "Globals.hpp"
#include "Document.hpp"
#include "dataItem/DbXmlNodeImpl.hpp"
#include "nodeStore/NsFormat.hpp"
#include "nodeStore/NsXercesDom.hpp"

#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMAttr.hpp>

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
	node_id_.freeNid(Globals::defaultMemoryManager);
	last_descendant_.freeNid(Globals::defaultMemoryManager);
	memset(this, 0, sizeof(IndexEntry));
}

IndexEntry::Relationship IndexEntry::isSelf(const DbXmlNodeImpl *node) const
{
	DBXML_ASSERT(isSpecified(NODE_ID));

	// We assume that the nodes come from the same
	// container and the same document

	int compare = getNodeID().compareNids(node->getNID());
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

	if(parent->getLevel() == 0) {
		// The document node only has one child,
		// and this already fulfilled the criteria
		return REL_TRUE;
	}

	if(getNodeID().compareNids(parent->getNID()) <= 0)
		return REL_TOO_SMALL;

	const NsNid *maxNID = parent->getLastElemDescendantNID();
	if(maxNID == 0) return REL_TOO_BIG;

	if(getNodeID().compareNids(maxNID) > 0) return REL_TOO_BIG;

	if((parent->getLevel() + 1) != getNodeLevel())
		return REL_FALSE;

	return REL_TRUE;
}

IndexEntry::Relationship IndexEntry::isDescendantOf(const DbXmlNodeImpl *parent, bool orSelf, const DynamicContext *context) const
{
	DBXML_ASSERT(isSpecified(NODE_ID));
	DBXML_ASSERT(isSpecified(NODE_LEVEL));

	// We assume that the nodes come from the same
	// container and the same document

	if(parent->getLevel() == 0) {
		// Everything is a descendant of the document node
		return REL_TRUE;
	}

	int compare = getNodeID().compareNids(parent->getNID());
	if(compare < 0) {
		return REL_TOO_SMALL;
	}
	if(compare == 0) {
		return orSelf ? REL_TRUE : REL_TOO_SMALL;
	}

	const NsNid *maxNID = parent->getLastElemDescendantNID();
	if(maxNID == 0) return REL_TOO_BIG;

	return (getNodeID().compareNids(maxNID) <= 0) ? REL_TRUE : REL_TOO_BIG;
}

DOMNode *IndexEntry::fetchNode(const Document *document) const
{
	if(!isSpecified(NODE_ID)) {
		// Return the document
		return document->getContentAsDOM();
	}

	DOMElement *element = document->getElement(&getNodeID());
	if(element == 0) {
		ostringstream s;
		s << "Invalid node index values found during query (element 0x";
		getNodeID().displayNid(s);
		s << " not found)";

		const_cast<Document*>(document)->getContainer()->log(Log::C_QUERY, Log::L_ERROR, s.str());
		throw XmlException(XmlException::INTERNAL_ERROR, "Invalid node indexes");
	}

	if(isSpecified(ATTRIBUTE_INDEX)) {
		DOMAttr *attr = (DOMAttr*)element->getAttributes()->item(index_);
		if(attr == 0) {
			const_cast<Document*>(document)->getContainer()->log(Log::C_QUERY, Log::L_ERROR,
				"Invalid index values found during query (attribute not found)");
			throw XmlException(XmlException::INTERNAL_ERROR, "Invalid node indexes");
		}
		return attr;
	}
	if(isSpecified(TEXT_INDEX) || isSpecified(COMMENT_INDEX) || isSpecified(PI_INDEX)) {
		NsDomElement *nselem = (NsDomElement*)fakeDynamicCastNsDomNode(element);

		NsDomText *text = nselem->getNsTextNode(index_);
		if(text == 0) {
			const_cast<Document*>(document)->getContainer()->log(Log::C_QUERY, Log::L_ERROR,
				"Invalid index values found during query (text not found)");
			throw XmlException(XmlException::INTERNAL_ERROR, "Invalid node indexes");
		}
		return fakeDynamicCastDOMNode(text);
	}

	return element;
}

const NsNid &IndexEntry::getNodeID() const
{
	if(node_) {
		return *(node_->getNid());
	} else {
		return node_id_;
	}
}

void IndexEntry::setNodeID(const NsNid *nid)
{
	node_id_.copyNid(Globals::defaultMemoryManager, nid);
}

u_int32_t IndexEntry::getNodeLevel() const
{
	if(node_) {
		return node_->getLevel();
	} else {
		return node_level_;
	}
}

void IndexEntry::setNode(const NsNode *node)
{
	node_ = (NsNode*)node;
}

void IndexEntry::setLastDescendant(const NsNid *last_descendant)
{
	if(last_descendant == 0) {
		last_descendant_.freeNid(Globals::defaultMemoryManager);
		memset(&last_descendant_, 0, sizeof(NsNid));		
	} else {
		last_descendant_.copyNid(Globals::defaultMemoryManager,
					last_descendant);
	}
}

string IndexEntry::getNodeHandle() const
{
	int size = marshal(0, /*count*/true);
	size += 1; // The checksum

	Buffer rawBuf(0, size);
	size_t offset;
	rawBuf.reserve(offset, size);

	xmlbyte_t *ptr = (xmlbyte_t*)rawBuf.getBuffer(offset);
	ptr += marshal(ptr, /*count*/false);

	// Calculate the checksum
	xmlbyte_t chk = 0;
	for(xmlbyte_t *cp = (xmlbyte_t*)rawBuf.getBuffer(offset);
	    cp < ptr; ++cp) chk += *cp;
	*ptr++ = (chk & 0xFF); // Checksum byte

	Buffer base64Buf(0, size * 2);
	NsUtil::encodeBase64Binary(&base64Buf, (char*)rawBuf.getBuffer(offset), size);

	return string((char*)base64Buf.getBuffer(), base64Buf.getOccupancy());
}

void IndexEntry::setFromNodeHandle(const string &str)
{
	Buffer rawBuf(0, str.length() >> 1);
	NsUtil::decodeBase64Binary(&rawBuf, str.c_str(), str.length());

	const xmlbyte_t *ptr = (const xmlbyte_t*)rawBuf.getBuffer();

	// Calculate the checksum
	unsigned int chk = 0;
	const xmlbyte_t *end = ptr + rawBuf.getOccupancy() - 1;
	for(const xmlbyte_t *cp = ptr; cp < end; ++cp) chk += *cp;

	if(*end != (chk & 0xFF)) {
		throw XmlException(XmlException::INVALID_VALUE,
			"The node handle has a bad checksum");
	}

	unmarshal(ptr);
}

void IndexEntry::setDbtFromThis(DbtOut &dbt) const
{
	int count = marshal(0, /*count*/true);
	dbt.set(0, count);
	marshal((xmlbyte_t*)dbt.get_data(), /*count*/false);
}

void IndexEntry::setThisFromDbt(const Dbt &dbt)
{
	unmarshal((const xmlbyte_t *)dbt.get_data());
}

int IndexEntry::marshal(xmlbyte_t *ptr, bool count) const
{
	int size = 0;

	if(count) {
		size += 1; // For the prefix byte
		size += docid_.marshalSize();

		if(isSpecified(NODE_ID)) {
			size += NsFormat::countId(&getNodeID());
		} else {
			// We'll store a null instead, for ease of sorting
			size += 1;
		}
		if(isSpecified(LAST_CHILD_ID)) {
			const NsNid *lastchild = node_->getLastChildNid();
			if(lastchild == 0) size += 1; // For a single null
			else size += NsFormat::countId(lastchild);
		}
		if(isSpecified(NODE_LEVEL))
			size += NsFormat::countInt(getNodeLevel());
		if(isSpecified(PARENT_ID))
			size += NsFormat::countId(node_->getParentNid());
		if(isSpecified(LAST_DESCENDANT_ID))
			size += NsFormat::countId(&last_descendant_);
		if(isSpecified(ATTRIBUTE_INDEX) || isSpecified(TEXT_INDEX) ||
			isSpecified(COMMENT_INDEX) || isSpecified(PI_INDEX))
			size += NsFormat::countInt(index_);
	} else {
		xmlbyte_t *orig = ptr;

		*ptr++ = (xmlbyte_t)format_;
		ptr += docid_.marshal(ptr);

		if(isSpecified(NODE_ID)) {
			ptr += NsFormat::marshalId(ptr, &getNodeID());
		} else {
			// We'll store a null instead, for ease of sorting
			*ptr++ = 0;
		}
		if(isSpecified(LAST_CHILD_ID)) {
			const NsNid *lastchild = node_->getLastChildNid();
			if(lastchild == 0) *ptr++ = 0; // Store a single null
			else ptr += NsFormat::marshalId(ptr, lastchild);
		}
		if(isSpecified(NODE_LEVEL))
			ptr += NsFormat::marshalInt(ptr, getNodeLevel());
		if(isSpecified(PARENT_ID))
			ptr += NsFormat::marshalId(ptr, node_->getParentNid());
		if(isSpecified(LAST_DESCENDANT_ID)) {
			if(last_descendant_.getLen() == 0) *ptr++ = 0; // Store a single null
			else ptr += NsFormat::marshalId(ptr, &last_descendant_);
		}
		if(isSpecified(ATTRIBUTE_INDEX) || isSpecified(TEXT_INDEX) ||
			isSpecified(COMMENT_INDEX) || isSpecified(PI_INDEX))
			ptr += NsFormat::marshalInt(ptr, index_);

		size = ptr - orig;
	}

	return size;
}

int IndexEntry::unmarshal(const xmlbyte_t *ptr)
{
	const xmlbyte_t *orig = ptr;

	format_ = (Format)*ptr++;
	if(format_ >= KNOWN_FORMATS)
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "unknown format in index entry",
					 __FILE__, __LINE__);
	ptr += docid_.unmarshal(ptr);

	if(isSpecified(NODE_ID)) {
		ptr += NsFormat::unmarshalId(Globals::defaultMemoryManager, ptr, &node_id_);
	} else {
		// Skip the null that was marshalled
		++ptr;
	}
	if(isSpecified(LAST_CHILD_ID)) {
		// We don't keep the last child id around
		// as it's not used
		ptr = NsFormat::skipId(ptr);
	}
	if(isSpecified(NODE_LEVEL)) {
		ptr += NsFormat::unmarshalInt(ptr, &node_level_);
	}
	if(isSpecified(PARENT_ID)) {
		// We don't keep the parent id around
		// as it's not used
		ptr = NsFormat::skipId(ptr);
	}
	if(isSpecified(LAST_DESCENDANT_ID)) {
		ptr += NsFormat::unmarshalId(Globals::defaultMemoryManager, ptr, &last_descendant_);
	}
	if(isSpecified(ATTRIBUTE_INDEX) || isSpecified(TEXT_INDEX) ||
		isSpecified(COMMENT_INDEX) || isSpecified(PI_INDEX)) {
		ptr += NsFormat::unmarshalInt(ptr, &index_);
	}

	return ptr - orig;
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

	return getNodeID().compareNids(&o.getNodeID()) < 0;
}

const bool IndexEntry::indexFormats_[IndexEntry::KNOWN_FORMATS][IndexEntry::INFO_MAX] = {
	/*                        NODE_ID,     NODE_LEVEL,   L_DESC_ID     TEXT_INDEX     PI_INDEX */
	/*                             L_CHILD_ID,    PARENT_ID     ATTR_INDEX    CMNT_INDEX       */
	/* D_FORMAT            */ {false, false, false, false, false, false, false, false, false},
	/* DSEL_OLD_FORMAT     */ {true,  true,  true,  false, false, false, false, false, false},
	/* DSEL_FORMAT         */ {true,  false, true,  false, true,  false, false, false, false},
	/* NH_ATTRIBUTE_FORMAT */ {true,  false, false, false, false, true,  false, false, false},
	/* ATTR_OLD_FORMAT2    */ {true,  false, true,  false, false, true,  false, false, false},
	/* ATTRIBUTE_FORMAT    */ {true,  false, true,  false, true,  true,  false, false, false},
	/* NH_ELEMENT_FORMAT   */ {true,  false, false, false, false, false, false, false, false},
	/* NH_TEXT_FORMAT      */ {true,  false, false, false, false, false, true,  false, false},
	/* NH_COMMENT_FORMAT   */ {true,  false, false, false, false, false, false, true,  false},
	/* NH_PI_FORMAT        */ {true,  false, false, false, false, false, false, false, true }
};

std::ostream& DbXml::operator<<(std::ostream& s, const IndexEntry &ie)
{
	s << ie.getDocID().asString();
	if(ie.isSpecified(IndexEntry::NODE_ID)) {
		s << "(0x";

// 		bool doDot = false;
		const xmlbyte_t *nid = ie.getNodeID().getBytes();
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



void IndexData::set_union(const Ptr &o)
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

void IndexData::set_intersection(const Ptr &o)
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

void IndexData::set_intersection(const IDS::Ptr &o)
{
	if(o) {
		iterator thisIt = begin();
		iterator thisEnd = end();
		IDS::iterator otherIt = o->begin();
		IDS::iterator otherEnd = o->end();

		iterator tmpIt;
		while(thisIt != thisEnd) {
			if(otherIt == otherEnd || (*thisIt)->getDocID() < *otherIt) {
				tmpIt = thisIt;
				++thisIt;
				erase(tmpIt);
			} else if((*thisIt)->getDocID() > *otherIt) {
				++otherIt;
			} else {
				++thisIt;
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

const IDS::Ptr &IndexReadCache::getIDS(const Container &container, OperationContext &context,
	SecondaryDatabase::Operation op1, const Key &k1, SecondaryDatabase::Operation op2, const Key &k2)
{
	IndexKey indexKey(&container, op1, k1, op2, k2);

	IDSMap::const_iterator it = idsMap_.find(indexKey);
	if(it != idsMap_.end()) {
		return it->second;
	}

	return lookupIDS(context, indexKey);
}

const IndexData::Ptr &IndexReadCache::getIndexData(const Container &container, OperationContext &context,
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

const IDS::Ptr &IndexReadCache::lookupIDS(OperationContext &context, const IndexKey &indexKey)
{
	IndexData::Ptr data(0);
	IndexMap::const_iterator it = indexMap_.find(indexKey);
	if(it != indexMap_.end()) {
		data = it->second;
	} else {
		data = lookupIndexData(context, indexKey);
	}

	IDS::Ptr ids(new IDS);
	DocID id;
	DocID lastid(0);
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

const IndexData::Ptr &IndexReadCache::lookupIndexData(OperationContext &context, const IndexKey &indexKey)
{
	IndexData::Ptr data(new IndexData);

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
