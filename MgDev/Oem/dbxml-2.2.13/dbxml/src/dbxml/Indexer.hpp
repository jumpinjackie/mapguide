//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: Indexer.hpp,v 1.66 2005/09/13 17:51:04 jsnelson Exp $
//

#ifndef __INDEXER_HPP
#define	__INDEXER_HPP

#include <string>
#include <vector>

#include "IndexEntry.hpp"
#include "IndexSpecification.hpp"
#include "nodeStore/NsEvent.hpp"
#include "OperationContext.hpp"
#include "KeyStash.hpp"

namespace DbXml
{

class Transaction;
class Container;
class Buffer;
class Document;
class Key;
class NsDomElement;
class ID;

class IndexerState
{
public:
	IndexerState();
	~IndexerState();
	void initialize();
	void startNode(const IndexSpecification &indexSpecification,
		       const char *const uri, const char *const localname);
	void characters(const char *s, size_t l);
	/// type should be NODE_ELEMENT, NODE_ATTRIBUTE, or NODE_METADATA
	bool isIndexed(const Index::Type &type) const;
	Key &getKey(Container &container, OperationContext &context);
	void reset();
	const IndexVector &iv() const
	{
		return *iv_;
	}
private:
	IndexerState(const IndexerState&);
	void operator=(const IndexerState&);

	void saveUriname(const char* const uri, const char* const localname); // copies
	const char *getName() const;

	Key *key_;
	const IndexVector *iv_;
	Buffer buffer_;
};

class IndexerStateStack
{
public:
	IndexerStateStack();
	~IndexerStateStack();

	IndexerState *push();
	IndexerState *top(unsigned int index = 0);
	void pop();
	bool empty();
	void reset();

private:
	IndexerStateStack(const IndexerStateStack&);
	void operator=(const IndexerStateStack &);

	std::vector<IndexerState*> v_;
	size_t top_;
};

class Indexer : public NsEventHandler8
{
public:
	Indexer();
	~Indexer();

	/**
	 * Generates a set of index keys for the document metadata based on
	 * the indexing specification. reset() must be called with a valid
	 * txn and container before this method is called.
	 *
	 * This method is not thread safe. One indexer is needed per thread.
	 *
	 * After the document is indexed the KeyStash::updateIndex() method
	 * must be called to write the keys to the index databases.
	 *
	 * \param indexSpecification The index specification to index for.
	 * \param document the document whose metadata is to be indexed
	 * \param stash the KeyStash to write the keys to
	 * \param checkModified check the modified flags of the metadata
	 * before indexing them
	 * \exception XmlException
	 */
	void indexMetaData(const IndexSpecification &indexSpecification,
			   const Document &document, KeyStash &stash,
			   bool checkModified);
	/**
	 * Generates a set of index keys for the document content based on
	 * the indexing specification. reset() must be called with a valid
	 * txn and container before this method is called.
	 *
	 * This method is not thread safe. One indexer is needed per thread.
	 *
	 * After the document is indexed the KeyStash::updateIndex() method
	 * must be called to write the keys to the index databases.
	 *
	 * \param indexSpecification The index specification to index for.
	 * \param did the document id
	 * \param source the NsPushEventSource8 that represents the content
	 * \param stash the KeyStash to write the keys to
	 * \exception XmlException
	 */
	void indexContent(const IndexSpecification &indexSpecification,
			  const ID &did, NsPushEventSource8 *source,
			  KeyStash &stash); // throws XmlException

	void reset(Transaction *txn, Container *container);

	Container *getContainer()
	{
		return container_;
	}

private:
	// no need for copy and assignment
	Indexer(const Indexer&);
	Indexer &operator=(const Indexer &);

	// NsEventHandler8 interface
	virtual void startElement(const xmlbyte_t *localName,
				  const xmlbyte_t *prefix,
				  const xmlbyte_t *uri,
				  int32_t uriId,
				  NsEventAttrList8 *attrs,
				  const uint32_t attrCount,
				  bool isEmpty,
				  bool isRoot,
				  const nsNode_t *node);
	virtual void endElement(const xmlbyte_t *localName,
				const xmlbyte_t *prefix,
				const xmlbyte_t *uri,
				bool isRoot,
				const nsNode_t *node);
	virtual void characters(const xmlbyte_t *characters,
				uint32_t len, bool isCDATA,
				bool needsEscape);
	virtual void comment(const xmlbyte_t *comment, uint32_t len) {}
	virtual void processingInstruction(const xmlbyte_t *target,
					   const xmlbyte_t *data) {}

	void generateKeys(const IndexVector &iv, Index::Type pnk,
			  Index::Type mask, Key &key, KeyStash &stash);
	void checkUniqueConstraint(const Key &key);

	// The operation context within which the index keys are added to the container.
	mutable OperationContext context_;

	// Reference to the Container that this Indexer indexes for.
	Container *container_;

	// The index keys that are to be created.
	const IndexSpecification *indexSpecification_;

	// The default indexes.
	const IndexVector *default_;

	// We check the index specification up front to see if there are any
	// attribute indexes. We can avoid some code if they're not needed.
	bool attributesIndexed_;

	// We check the index specification up front to see if there are any
	// element indexes. We can avoid some code if they're not needed.
	bool elementsIndexed_;

	// A mutable IndexEntry for constructing the indexes
	IndexEntry indexEntry_;

	// Data structures maintained whilst parsing the XML document.
	IndexerStateStack stateStack_;

	// The index keys are buffered up in a 'keyStash'.
	KeyStash *keyStash_;

	// For storing unique index keys seen in this document
	KeyStash uniqueKeysStash_;
};

}

#endif
