//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: Indexer.cpp,v 1.119 2005/09/13 17:51:04 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlException.hpp"
#include "Indexer.hpp"
#include "Log.hpp"
#include "Container.hpp"
#include "SyntaxManager.hpp"
#include "UTF8.hpp"
#include "Document.hpp"
#include "Manager.hpp"
#include "KeyStash.hpp"
#include "scoped_ptr.hpp"
#include "IndexEntry.hpp"

#include "nodeStore/NsUtil.hpp"
#include "nodeStore/NsDocument.hpp"
#include "nodeStore/NsXercesTranscoder.hpp"
#include "nodeStore/NsXercesIndexer.hpp"
#include "nodeStore/NsDom.hpp"
#include "nodeStore/NsDocumentDatabase.hpp"

#if defined(XERCES_HAS_CPP_NAMESPACE)
  XERCES_CPP_NAMESPACE_USE
#endif

#include <string>

using namespace DbXml;

// IndexerState

IndexerState::IndexerState()
	: key_(0),
	  iv_(0)
{}

IndexerState::~IndexerState()
{
	if (key_)
		delete key_;
}

void IndexerState::initialize()
{
	if (!key_)
		key_ = new Key;
	else
		key_->reset();
}

void IndexerState::saveUriname(const char* const uri, const char* const localname)
{
	initialize();
	size_t u_len = uri == 0 ? 0 : NsUtil::nsStringLen((const unsigned char*)uri);
	size_t l_len = NsUtil::nsStringLen((const unsigned char*)localname);
	// need to ensure null termination in buffer.
	if (u_len)
		++u_len;
	else
		++l_len;
	buffer_.reset();
	Name::writeToBuffer(buffer_, localname, l_len,
			    uri, u_len);
}

const char *IndexerState::getName() const
{
	return (const char*)buffer_.getBuffer();
}

void IndexerState::startNode(const IndexSpecification &indexSpecification,
			     const char *const uri, const char *const localname)
{
	saveUriname(uri, localname);
	iv_ = indexSpecification.getIndexOrDefault(getName());
}

void IndexerState::characters(const char *s, size_t l)
{
	key_->addValue(s, l);
}

bool IndexerState::isIndexed(const Index::Type &type) const
{
	return iv_ && iv_->isEnabled(type, Index::NODE_MASK);
}

Key &IndexerState::getKey(Container &container, OperationContext &context)
{
	if (!key_)
		key_ = new Key();
	ID &id = key_->getID1();
	if (id == 0)
		container.getDictionaryDB()->lookupName(context, getName(),
							id, /*define=*/true);
	return *key_;
}

void IndexerState::reset()
{
	if (key_)
		key_->reset();
	iv_ = 0;
}

// IndexerStateStack

IndexerStateStack::IndexerStateStack()
	: top_(0)
{
	v_.reserve(16);
}

IndexerStateStack::~IndexerStateStack()
{
	std::vector<IndexerState*>::iterator i;
	for(i = v_.begin(); i != v_.end(); ++i) {
		delete *i;
	}
}

IndexerState *IndexerStateStack::push()
{
	IndexerState *is = 0;
	if (top_ == v_.size()) {
		v_.push_back(new IndexerState);
		is = v_[top_];
		++top_;
	} else {
		is = v_[top_];
		++top_;
		is->reset();
	}
	return is;
}

IndexerState *IndexerStateStack::top(unsigned int index)
{
	IndexerState *is = 0;
	if (top_ > index && top_ != 0) {
		is = v_[top_ - index - 1];
	}
	return is;
}

void IndexerStateStack::pop()
{
	if (top_ != 0) {
		--top_;
	}
}

bool IndexerStateStack::empty()
{
	return (top_ == 0);
}

void IndexerStateStack::reset()
{
  top_ = 0;
}

// Indexer

Indexer::Indexer()
	: container_(0),
	  indexSpecification_(0),
	  attributesIndexed_(false),
	  elementsIndexed_(false)
{
}

Indexer::~Indexer()
{
}

void Indexer::indexMetaData(const IndexSpecification &indexSpecification,
			    const Document &document, KeyStash &stash,
			    bool checkModified)
{
	//
	// Build index keys for the document metadata. We use the
	// IndexSpecification::getIndexed() method to avoid using
	// a metadata iterator, which would cancel any benefit
	// gained from a lazy document.
	//
	// Note that metadata indexes don't have node/edge set on them.
	//
	IndexSpecification::NameList md_names = indexSpecification.
		getIndexed(Index::NODE_METADATA, Index::NODE_MASK);
	if(!md_names.empty()) {
		indexEntry_.setDocID(document.getID().raw());
		indexEntry_.setNode(0);

		Key mdkey;
		IndexSpecification::NameList::iterator end = md_names.end();
		for(IndexSpecification::NameList::iterator it = md_names.begin();
		    it != end; ++it) {
			const Name &mdname = *it;
			if(!checkModified || document.isMetaDataModified(mdname)) {
				const MetaDatum *md = document.getMetaDataPtr(mdname);
				if(md != 0 && md->canBeIndexed()) {
					const IndexVector *iv = indexSpecification.
						getIndexOrDefault(mdname.getURIName().c_str());

					if(iv && iv->isEnabled(Index::NODE_METADATA, Index::NODE_MASK)) {
						mdkey.reset();
						container_->getDictionaryDB()->
							lookupName(context_, mdname, mdkey.getID1(), /*define=*/true);
						mdkey.setValue((const char*)md->getDbt()->get_data(),
							       md->getDbt()->get_size() - 1);

						mdkey.setIndex(Index::NM);
						generateKeys(*iv, Index::NM, Index::PN_MASK, mdkey, stash);
					}
				}
			}
		}
	}
}

void Indexer::reset(Transaction *txn, Container *container)
{
	context_.set(txn);
	container_ = container;

	// Cleanup
	indexEntry_.reset();
	indexSpecification_ = 0;
	stateStack_.reset();

	uniqueKeysStash_.reset();
}

void Indexer::startElement(const xmlbyte_t *localName,
			   const xmlbyte_t *prefix,
			   const xmlbyte_t *uri,
			   int32_t uriId,
			   NsEventAttrList8 *attrs,
			   const uint32_t attrCount,
			   bool isEmpty,
			   bool isRoot,
			   const nsNode_t *node)
{
	if (elementsIndexed_ || attributesIndexed_) {
		// Maintain the last descendant NID
		indexEntry_.setLastDescendant(nsGetNid(node));

		// only push state if there is indexing going on
		IndexerState *eis = stateStack_.push();
		eis->startNode(*indexSpecification_, (const char*)uri, (const char*)localName);
		if (attributesIndexed_ && (attrCount != 0)) {
			//
			// node-attribute-presence
			// edge-attribute-presence
			// edge-attribute-equality-*
			// node-attribute-equality-*
			// edge-attribute-substring-*
			// node-attribute-substring-*
			//
			IndexerState *ais = stateStack_.push();
			for (unsigned int i = 0; i < attrCount; ++i) {
				ais->startNode(*indexSpecification_,
					       (const char*)attrs->uri(i),
					       (const char*)attrs->localName(i));
				if (ais->isIndexed(Index::NODE_ATTRIBUTE)) {
					const char *v =
						(const char *)attrs->value(i);
					// do NOT add trailing null
					ais->characters(v, ::strlen(v));

					indexEntry_.setNode(node);
					indexEntry_.setAttrIndex(i);

					//
					// node-attribute-*
					//
					Key &k = ais->getKey(*container_, context_);
					k.setIndex(Index::NA);
					generateKeys(ais->iv(), Index::NA, Index::PN_MASK, k, *keyStash_);
					//
					// edge-attribute-*
					//
					k.set(Index::EA, k.getID1(),
						eis->getKey(*container_, context_).getID1());
					generateKeys(ais->iv(), Index::EA, Index::PN_MASK, k, *keyStash_);
				}
			}
			stateStack_.pop();
		}
	}

	if(isEmpty)
		endElement(localName, prefix, uri, isRoot, node);
}

/**
 * Note - Elements with multiple text node descendants are
 * concatenated into a single key.
 *
 * Eg <a>x<b>y</b>z</a> is a=xyz, b=y
 */
void Indexer::characters(const xmlbyte_t *chars, uint32_t len,
			 bool isCDATA, bool needsEscape)
{
	if (elementsIndexed_ && len != 0) {
		//
		// *-element-equality-*
		// *-element-substring-*
		//
		unsigned int index = 0;
		for(IndexerState *is = stateStack_.top(index); is != 0;
		    is = stateStack_.top(++index)) {
			if(is->isIndexed(Index::NODE_ELEMENT) &&
			   (is->iv().isEnabled(Index::XEE, Index::NK_MASK) ||
			    is->iv().isEnabled(Index::XES, Index::NK_MASK))) {
				// Note that an element value can be split into
				// multiple calls to characterData. First time
				// past the key goes from Presence to Equality,
				// there after the data is just appended to
				// the Equality key.
				//
				// Note that we do not trim leading whitespace.
				// trailing null must NOT be added to len
				//
				is->characters((const char *)chars, len);
			}
		}
	}
}

void Indexer::endElement(const xmlbyte_t *localName,
			 const xmlbyte_t *prefix,
			 const xmlbyte_t *uri,
			 bool isRoot,
			 const nsNode_t *node)
{
	UNUSED(uri);
	UNUSED(localName);
	UNUSED(prefix);
	UNUSED(isRoot);

	bool indexed = false;;
	IndexerState *cis = 0; // child indexer state
	if (elementsIndexed_) {
		//
		// node-element-*-*
		//
		cis = stateStack_.top();
		indexed = cis->isIndexed(Index::NODE_ELEMENT);
		if (indexed) {
			Key &k = cis->getKey(*container_, context_);
			indexEntry_.setNode(node);
			generateKeys(cis->iv(), Index::NE, Index::PN_MASK, k, *keyStash_);
		}
	}
	if (elementsIndexed_ || attributesIndexed_)
		stateStack_.pop();

	if(elementsIndexed_ && indexed) {
		//
		// edge-element-*-*
		//
		Key &k = cis->getKey(*container_, context_);
		if(stateStack_.empty()) {
			// It's the document element - it's parent is
			// the special NID for Root
			k.setID2(container_->getDictionaryDB()->getNIDForRoot());
		}
		else {
			IndexerState *pis = stateStack_.top();
			k.setID2(pis->getKey(*container_, context_).getID1());
		}

		indexEntry_.setNode(node);
		generateKeys(cis->iv(), Index::EE, Index::PN_MASK, k, *keyStash_);
	}
}

void Indexer::generateKeys(const IndexVector &iv, Index::Type pnk,
	Index::Type mask, Key &key, KeyStash &stash)
{
	// Iterate over the index vector looking for indexes that match
	// the path-node-key provided (pnk). For each syntax create the
	// keys for that syntax. Note that the key passed through is
	// reused...
	//
	if(indexEntry_.hasNode() && container_->getIndexNodes()) {
		if(pnk & Index::NODE_ELEMENT) {
			indexEntry_.setFormat(IndexEntry::DSEL_FORMAT);
		}
		else if(pnk & Index::NODE_ATTRIBUTE) {
			indexEntry_.setFormat(IndexEntry::ATTRIBUTE_FORMAT);
		}
		else {
			indexEntry_.setFormat(IndexEntry::D_FORMAT);
		}
	} else {
		indexEntry_.setFormat(IndexEntry::D_FORMAT);
	}

	KeyGenerator::Ptr kg;
	int i = 0;
	Index index;
	const Syntax *syntax = iv.getNextSyntax(i, pnk, mask, index);
	while (syntax != 0) {
		key.setIndex(index);
		if(index.getUnique() == Index::UNIQUE_ON && index.indexerAdd()) {
			checkUniqueConstraint(key);
		}

		kg = syntax->getKeyGenerator(index, key.getValue(),
					     key.getValueSize());
		if (kg->noOfKeys() == 0) {
			// Add a consolation key, to make presence lookups work
			stash.addConsolationKey(key, indexEntry_);
		} else {
			const char *keyValue= 0;
			size_t keyLength= 0;
			while(kg->next(keyValue, keyLength)) {
				stash.addKey(key, keyValue, keyLength, indexEntry_);
			}
		}

		syntax = iv.getNextSyntax(i, pnk, mask, index);
	}
}

void Indexer::checkUniqueConstraint(const Key &key)
{
	KeyGenerator::Ptr kg = key.getSyntax()->
		getKeyGenerator(key.getIndex(), key.getValue(),
				key.getValueSize());
	SyntaxDatabase *database = container_->getIndexDB(key.getSyntaxType(),
							  context_.txn(), true);
	DbtIn dbt;
	const char *keyValue= 0;
	size_t keyLength= 0;
	while(kg->next(keyValue, keyLength)) {
		KeyStash::Entry *entry = uniqueKeysStash_.
			addUniqueKey(key, keyValue, keyLength);
		if(entry != 0) {
			entry->getKey(dbt);

			IndexEntry ie;
			int err = database->getIndexDB()->getIndexEntry(context_, dbt, ie);
			if(err != 0 && err != DB_NOTFOUND) throw XmlException(err);

			if(err == DB_NOTFOUND || ie.getDocID() == indexEntry_.getDocID())
				continue;
		}

		Key copy(key);
		copy.setValue(keyValue, keyLength);
		std::ostringstream oss;
		oss << "Uniqueness constraint violation for key: " <<
			copy.asString();
		container_->log(Log::C_INDEXER, Log::L_INFO, oss);
		throw XmlException(XmlException::UNIQUE_ERROR, oss.str());
	}
}

void Indexer::indexContent(const IndexSpecification &indexSpecification,
			   const ID &did, NsPushEventSource8 *source,
			   KeyStash &stash)
{
	indexSpecification_ = &indexSpecification;
	keyStash_ = &stash;

	attributesIndexed_ = indexSpecification_->
		isIndexed(Index::NODE_ATTRIBUTE, Index::NODE_MASK);
	elementsIndexed_ = indexSpecification_->
		isIndexed(Index::NODE_ELEMENT, Index::NODE_MASK);

	indexEntry_.setDocID(did.raw());

	// allow no-content documents
	if(source != 0) {
		source->setNsEventHandler(this);
		source->start();
	}
}
