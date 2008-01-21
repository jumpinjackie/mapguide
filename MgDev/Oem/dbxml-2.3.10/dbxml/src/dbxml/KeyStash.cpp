//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: KeyStash.cpp,v 1.17 2006/11/17 18:18:25 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include "KeyStash.hpp"
#include "Log.hpp"
#include "Document.hpp"
#include "Container.hpp"
#include "IndexEntry.hpp"

#include <string>
#include <sstream>

using namespace DbXml;
using namespace std;

KeyStash::KeyStash()
	: buffer_(0, 1024)
{
}

KeyStash::~KeyStash()
{
	reset();
}

void KeyStash::reset()
{
	EntrySet::iterator end = keys_.end();
	for(EntrySet::iterator i = keys_.begin();
	    i != end; ++i) {
		if((*i)->otherEntry != 0)
			delete (*i)->otherEntry;
		delete *i;
	}
	keys_.clear();
	buffer_.reset();
}

void KeyStash::addEntry(Entry *toAdopt)
{
	pair<EntrySet::iterator, bool> inserted =
		keys_.insert(toAdopt);

	if(!inserted.second) {
		// the Entry is a duplicate, see if it cancels
		// out the existing entry
		if((*inserted.first)->index.indexerAdd() !=
		   toAdopt->index.indexerAdd()) {
			if((*inserted.first)->equals(toAdopt)) {
				// They cancel each other out.  Rather than
				// removing the cancelled entry, mark it
				// as deleted, so later entries can find
				// it [#14276]
				(*inserted.first)->setDeleted();
				delete toAdopt;
			} else {
				// They combine to form an update
				(*inserted.first)->otherEntry = toAdopt;
			}
		} else {
			delete toAdopt;
		}
	}
}

KeyStash::Entry *KeyStash::addUniqueEntry(Entry *toAdopt)
{
	pair<EntrySet::iterator, bool> inserted =
		keys_.insert(toAdopt);
	if(!inserted.second) {
		delete toAdopt;
		return 0;
	}
	return toAdopt;
}

KeyStash::Entry *KeyStash::addUniqueKey(const Key &key, const char *value,
	size_t length)
{
	return addUniqueEntry(new Entry(key, value, length, 0, buffer_));
}

void KeyStash::addKey(const Key &key, const char *value,
		      size_t length, const IndexEntry &ie)
{
	addEntry(new Entry(key, value, length, &ie, buffer_));
}

void KeyStash::addConsolationKey(const Key &key, const IndexEntry &ie)
{
	addEntry(new Entry(key, 0, 0, &ie, buffer_));
}

enum EntryAction {
	ENTRY_ADD,
	ENTRY_DELETE,
	ENTRY_UPDATE
};

int KeyStash::updateIndex(OperationContext &context, Container *container) const
{
	statistics_.reset();

	int err = 0;

	//
	// We iterate over the key stash, adding, or deleting the keys
	// from the indexes. The key buffer comes out of the key stash
	// with the correct endianness.
	//
	Entry *entry, *otherEntry;
	EntryAction action;
	Index index;
	DbtIn key, data;
	// Add each entry to the index.
	EntrySet::const_iterator end = keys_.end();
	for(EntrySet::const_iterator i = keys_.begin();
	    i != end && err == 0; ++i) {

		entry = *i;
		if (entry->isDeleted())
			continue;
		action = entry->index.indexerAdd() ? ENTRY_ADD : ENTRY_DELETE;
		if(entry->otherEntry != 0) {
			// It's an update
			otherEntry = entry->otherEntry;
			// If this is a delete then swap, as we need the add
			if(action == ENTRY_DELETE) {
				otherEntry = entry;
				entry = entry->otherEntry;
			}
			action = ENTRY_UPDATE;
		}

		index = entry->index;
		entry->getKey(key);
		entry->getData(data);

		if (Log::isLogEnabled(Log::C_INDEXER, Log::L_DEBUG)) {
			Buffer k(key.get_data(), key.get_size(),
				 /*wrapper*/true);
			Buffer d(data.get_data(), data.get_size(),
				 /*wrapper*/true);
			std::ostringstream oss;
			switch(action) {
			case ENTRY_ADD: oss << "add "; break;
			case ENTRY_DELETE: oss << "delete "; break;
			case ENTRY_UPDATE: oss << "update "; break;
			}

			oss << index.asString()
			    << ", key={" << k.asStringBrief()
			    << "}, data={" << d.asStringBrief()
			    << "}";
			container->log(Log::C_INDEXER, Log::L_DEBUG, oss);
		}
		bool isEqualityKey =
			(index.getKey() == Index::KEY_EQUALITY);
		SyntaxDatabase *database = container->
			getIndexDB((Syntax::Type)index.getSyntax(),
				   context.txn(), true);
		bool duplicate = true;
		switch(action) {
		case ENTRY_ADD: {
			if (isEqualityKey)
				duplicate = database->getIndexDB()->
					exists(context.txn(), key);
			// endianness of key buffer is correct
			err = database->getIndexDB()->
				putIndexEntry(context, key, data);
			break;
		}
		case ENTRY_DELETE: {
			err = database->getIndexDB()->
				delIndexEntry(context, key, data);
			if (isEqualityKey)
				duplicate = database->getIndexDB()->
					exists(context.txn(), key);
			break;
		}
		case ENTRY_UPDATE: {
			err = database->getIndexDB()->
				updateIndexEntry(context, key, data);
			break;
		}
		}
		if (err == 0) {
			if(action == ENTRY_UPDATE) {
				// We need to use both the delete and add keys
				// in the statistics if this is an update
				statistics_.addToKeyStatistics(index, key, data,
					false);
				otherEntry->getKey(key);
				otherEntry->getData(data);
				statistics_.addToKeyStatistics(otherEntry->index, key, data,
					false);
			} else {
				statistics_.addToKeyStatistics(index, key, data,
					!duplicate);
			}
		}
	}
	//
	// Update the index key statistics stored in the container.
	//
	if (err == 0) {
		err = statistics_.updateContainer(context, *container);
	}

	return err;
}

KeyStash::Entry::Entry(const Key &key, const char *value,
		       size_t length, const IndexEntry *ie,
		       Buffer &b)
	: index(key.getIndex()),
	  key_offset(0),
	  key_length(0),
	  data_offset(0),
	  data_length(0),
	  buffer(b),
	  deleted(false),
	  otherEntry(0)
{
	key_offset = b.getCursorPosition();
	key_length = key.marshal(buffer, value, length);

	if(ie) {
		data_length = b.reserve(data_offset, ie->marshal(0, /*count*/true));
		ie->marshal((xmlbyte_t*)b.getBuffer(data_offset), /*count*/false);
	}
}

bool KeyStash::Entry::equals(const Entry *o) const
{
	return key_length == o->key_length && data_length == o->data_length &&
		::memcmp(getKey(), o->getKey(), key_length) == 0 &&
		::memcmp(getData(), o->getData(), data_length) == 0;
}

bool KeyStash::EntryCompare::operator()(const Entry *a, const Entry *b) const
{
	if(a->key_length != b->key_length) return a->key_length < b->key_length;

	int cmp = ::memcmp(a->getKey(), b->getKey(), a->key_length);
	if(cmp != 0) return cmp < 0;

	if(b->data_length == 0) {
		return false;
	} else if(a->data_length == 0) {
		return true;
	}

	DBT dbt_a;
	dbt_a.data = a->getData();
	dbt_a.size = a->data_length;
	DBT dbt_b;
	dbt_b.data = b->getData();
	dbt_b.size = b->data_length;

	return index_duplicate_compare(0, &dbt_a, &dbt_b) < 0;
}
