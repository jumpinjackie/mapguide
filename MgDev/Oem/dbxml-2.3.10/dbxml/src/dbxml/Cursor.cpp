//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Cursor.cpp,v 1.67 2006/10/30 17:45:52 bostic Exp $
//

#include "DbXmlInternal.hpp"
#include "Cursor.hpp"
#include "Key.hpp"
#include "Log.hpp"
#include "IndexEntry.hpp"
#include <dbxml/XmlException.hpp>

#include <cassert>
#include <sstream>

using namespace DbXml;
using namespace std;

#define MINIMUM_BULK_GET_BUFFER 256 * 1024

// determine if two keys refer to the same index, based
// on initial structure bytes.
static bool isSameIndex(const Dbt &k1, const Dbt &k2)
{
	const xmlbyte_t *p1 = (const xmlbyte_t*)k1.get_data();
	const xmlbyte_t *p2 = (const xmlbyte_t*)k2.get_data();
	return (Key::compareStructure(p1, p1 + k1.get_size(),
				      p2, p2 + k2.get_size()) == 0);
}

// Cursor
Cursor::Cursor(DbWrapper &db, Transaction *txn,
	       CursorType type, u_int32_t flags)
	: dbc_(0),
	error_( -1)
{
	if (db.getEnvironment() != 0 && type==CURSOR_WRITE) {
		// If we're in a CDS environment then the cursor used to
		// delete the keys must be created with the DB_WRITECURSOR flag.
		//
		if (db.getEnvironment()->get_DB_ENV()->flags & DB_ENV_CDB) {
			flags |= DB_WRITECURSOR;
		}
	}
	try {
		error_ = db.getDb().cursor(Transaction::toDbTxn(txn), &dbc_,
			DB_CURSOR_CREATE_MASK(flags));
	}
	catch(DbException &de) {
		error_ = de.get_errno();
	}
}

// IndexCursor

IndexCursor::IndexCursor(IndexDatabase &db, Transaction *txn, bool initBulk)
	: cursor_(db, txn, CURSOR_READ,
		  ((db.getEnvironment()->get_DB_ENV()->open_flags & DB_INIT_LOCK) ? DB_READ_COMMITTED : 0)),
	  done_(false)
{
	if (initBulk) {
		// The bulk get buffer should be a multiple of the pagesize
		u_int32_t pagesize;
		db.getDb().get_pagesize(&pagesize);
		
		while(pagesize < MINIMUM_BULK_GET_BUFFER) {
			pagesize <<= 1;
		}
		
		bulk_.set_data(new char[pagesize / sizeof(char)]);
		bulk_.set_ulen(pagesize / sizeof(char));
		bulk_.set_flags(DB_DBT_USERMEM);
	}
}

IndexCursor::~IndexCursor()
{
	if (bulk_.get_data())
		delete [] (char*)bulk_.get_data();
}

int IndexCursor::nextEntries(u_int32_t flags)
{
	tmpKey_.set_data(key_.get_data());
	tmpKey_.set_size(key_.get_size());
	int err = cursor_.get(&tmpKey_, &bulk_, flags);

	while(err == DB_BUFFER_SMALL) {
		// If the buffer is too small, retry with a bigger buffer
		// bulk_.get_size() has the necessary number in it
		delete [] (char*)bulk_.get_data();
		u_int32_t newsize = bulk_.get_ulen();
		while (newsize < bulk_.get_size())
			newsize <<= 1;
		bulk_.set_data(new char[newsize / sizeof(char)]);
		bulk_.set_ulen(newsize / sizeof(char));
		
		tmpKey_.set_data(key_.get_data());
		tmpKey_.set_size(key_.get_size());
		err = cursor_.get(&tmpKey_, &bulk_, flags);
	}
	if(err != 0) {
		if(err == DB_NOTFOUND || err == DB_KEYEMPTY) {
			err = 0;
		}
		done_ = true;
	}

	return err;
}

int IndexCursor::prevEntry()
{
	tmpKey_.set_data(key_.get_data());
	tmpKey_.set_size(key_.get_size());
	int err = cursor_.get(&tmpKey_, &data_, DB_PREV);
	if(err != 0) {
		if(err == DB_NOTFOUND || err == DB_KEYEMPTY) {
			err = 0;
		}
		done_ = true;
	}
	return err;
}

// find the last key value that matches the prefix
// Algorithm:
//  1.  first use DB_SET_RANGE to make sure there is
//  at least one match.
//  2.  Add 1 to the least significant digit
//  of the key, do DB_SET_RANGE, and DB_PREV until
//  a match is found.  This finds the last match.
//  NOTE: this algorithm ONLY works for prefix matches,
//  where the key is <index_type,id[,id]>.  It will not
//  work correctly if there is a value at the end of the
//  key, because adding one may generate a type format error.
//  Things to handle:
//   o what if lsb is 255? In that case, add a digit
//     rather than 1.  For now, I think that this is not
//     possible, as 255 isn't going to show up.
//   o no entries -- means initial get will fail
//   o no entries after the range
int IndexCursor::findLastPrefix(Dbt &key, IndexEntry *ie)
{
	int err;
	u_int32_t keySize = key.get_size();
	Dbt *lookupKey = &tmpKey_;
	tmpKey_.set_data(key.get_data());
	tmpKey_.set_size(keySize);
	err = cursor_.get(&tmpKey_, &data_, DB_SET_RANGE);
	if (err != 0) {
		done_ = true;
		return err;
	}
	// one or more matches in DB
	// reset tmpKey_ size, add one to key
	tmpKey_.set_size(keySize);
	unsigned char *keyArray = (unsigned char *)tmpKey_.get_data();
	unsigned char last = keyArray[keySize-1];
	DbtOut keyPlusOne;
	if ((unsigned int)last < 255)
		keyArray[keySize-1] += 1;
	else {
		// create a Dbt with an extra byte.  set() method
		// will copy/realloc
		keyPlusOne.set(keyArray, keySize + 1);
		keyArray = (unsigned char *)keyPlusOne.get_data();
		keyArray[keySize] = 1; //put something in
		lookupKey = &keyPlusOne;
	}

	err = cursor_.get(lookupKey, &data_, DB_SET_RANGE);
	// can use tmpKey_ from here down
	if (err == DB_NOTFOUND) {
		// handle case where there are no entries past
		// the prefix requested
		err = cursor_.get(&tmpKey_, &data_, DB_LAST);
		if ((err == 0) &&
		    (memcmp(key.get_data(), tmpKey_.get_data(),
			    keySize) == 0)) {
			if (ie)
				ie->setThisFromDbt(data_);
			return err;
		}
	}
	while ((err = cursor_.get(&tmpKey_, &data_, DB_PREV)) == 0) {
		DBXML_ASSERT(tmpKey_.get_size() >= keySize);
		if (memcmp(key.get_data(), tmpKey_.get_data(),
			   keySize) == 0)
			break;
	}
	if (err == 0 && ie)
		ie->setThisFromDbt(data_);
	return err;
}

// InequalityIndexCursor

InequalityIndexCursor::InequalityIndexCursor(
	IndexDatabase &db, Transaction *txn, DbWrapper::Operation operation,
	const Key *k1, const Syntax *syntax)
	: IndexCursor(db, txn),
	  syntax_(syntax),
	  operation_(operation),
	  greaterThanOperation_(DbWrapper::NONE),
	  lessThanOperation_(DbWrapper::NONE),
	  it_(bulk_)
{
	DBXML_ASSERT(operation_ != DbWrapper::EQUALITY && operation_ != DbWrapper::PREFIX && operation_ != DbWrapper::RANGE);
	DBXML_ASSERT(syntax != 0);

	k1->setDbtFromThis(key_);
}

InequalityIndexCursor::InequalityIndexCursor(
	IndexDatabase &db, Transaction *txn, DbWrapper::Operation gto,
	const Key *gtk, DbWrapper::Operation lto, const Key *ltk,
	const Syntax *syntax)
	: IndexCursor(db, txn),
	  syntax_(syntax),
	  operation_(DbWrapper::RANGE),
	  greaterThanOperation_(gto),
	  lessThanOperation_(lto),
	  it_(bulk_)
{
	DBXML_ASSERT(lto == DbWrapper::LTX || lto == DbWrapper::LTE);
	DBXML_ASSERT(gto == DbWrapper::GTX || gto == DbWrapper::GTE);
	DBXML_ASSERT(syntax != 0);

	gtk->setDbtFromThis(key_);
	ltk->setDbtFromThis(key2_);
}

int InequalityIndexCursor::first(IndexEntry &ie)
{
	int flags = 0;
	int err = 0;
	// For a RANGE we start with the greaterThanOperation_.
	switch (operation_ == DbWrapper::RANGE ? greaterThanOperation_ : operation_) {
	case DbWrapper::ALL:
	case DbWrapper::LTE:
	case DbWrapper::LTX: {
		// Unmarshal the index
		Index index;
		index.setFromPrefix(*(char*)key_.get_data());

		// Set the tmpKey_ to the structure prefix of the key
		tmpKey_.set_data(key_.get_data());
		tmpKey_.set_size(Key::structureKeyLength(index, key_));

		// Do a DB_SET_RANGE lookup on the prefix
		err = cursor_.get(&tmpKey_, &data_, DB_SET_RANGE);

		flags = DB_CURRENT;
		break;
	}
	case DbWrapper::GTX:
		// Skip the first matching duplicate keys.
		// no throw for NOTFOUND and KEYEMPTY
		err = cursor_.get(&key_, &data_, DB_SET);
		if(err == 0) {
			// save key structure for comparison below
			DbtOut tmp(key_.get_data(), key_.get_size());
			// Do the DB_NEXT_NODUP without the DB_MULTIPLE_KEY,
			// otherwise the multiple get will get all of it's keys
			// with the NODUP flag...
			err = cursor_.get(&key_, &data_, DB_NEXT_NODUP);
			if ((err == 0) && !isSameIndex(key_, tmp)) {
				done_ = true;
				return 0;
			}
			flags = DB_CURRENT;
		} else if(err == DB_NOTFOUND) {
			err = 0;
			flags = DB_SET_RANGE;
		}
		break;
	case DbWrapper::GTE:
		flags = DB_SET_RANGE;
		break;
	default:
		// This just won't happen.
		err = DB_NOTFOUND;
		DBXML_ASSERT(0);
		break;
	}
	if(err != 0) {
		if (err == DB_NOTFOUND || err == DB_KEYEMPTY) {
			err = 0;
		}
		done_ = true;
		ie.reset();
		return err;
	}

	err = nextEntries(flags | DB_MULTIPLE_KEY);
	if(err != 0 || done_) {
		ie.reset();
		return err;
	} else {
		// placement new to initialize multiple iter
		(void) new (&it_) DbMultipleKeyDataIterator(bulk_);
	}

	if(operation_ == DbWrapper::RANGE) {
		// From now on, behave like the less than operator
		operation_ = lessThanOperation_;
		key_.set(key2_.get_data(), key2_.get_size());
	}

	return next(ie);
}

int InequalityIndexCursor::next(IndexEntry &ie)
{
	if(done_) {
		ie.reset();
		return 0;
	}

	while(!it_.next(tmpKey_, data_)) {
		int err = nextEntries(DB_NEXT | DB_MULTIPLE_KEY);
		if(err != 0 || done_) {
			ie.reset();
			return err;
		} else {
			// placement new to initialize multiple iter
			(void) new (&it_) DbMultipleKeyDataIterator(
				bulk_);
		}
	}

	switch (operation_) {
	case DbWrapper::ALL:
	case DbWrapper::GTX:
	case DbWrapper::GTE: {
		// Check the Prefix and VIDs are the same.
		if (!isSameIndex(key_, tmpKey_)) {
			done_ = true;
			ie.reset();
			return 0;
		}
		break;
	}
	case DbWrapper::LTX: {
		if(syntax_->get_bt_compare()(0, &tmpKey_, &key_) >= 0) {
			done_ = true;
			ie.reset();
			return 0;			
		}
		break;
	}
	case DbWrapper::LTE: {
		if(syntax_->get_bt_compare()(0, &tmpKey_, &key_) > 0) {
			done_ = true;
			ie.reset();
			return 0;			
		}
		break;
	}
	default:
		// This just won't happen.
		DBXML_ASSERT(0);
		break;
	}

	ie.setThisFromDbt(data_);
	return 0;
}

// PrefixIndexCursor

PrefixIndexCursor::PrefixIndexCursor(IndexDatabase &db, Transaction *txn,
				     const Key *k1)
	: IndexCursor(db, txn),
	  it_(bulk_)
{
	k1->setDbtFromThis(key_);
}

int PrefixIndexCursor::first(IndexEntry &ie)
{
	int err;
	err = nextEntries(DB_SET_RANGE | DB_MULTIPLE_KEY);
	
	if(err != 0 || done_) {
		ie.reset();
		return err;
	} else {
		(void) new (&it_) DbMultipleKeyDataIterator(bulk_);
		err = next(ie);
	}
	return err;
}

int PrefixIndexCursor::next(IndexEntry &ie)
{
	if(done_) {
		ie.reset();
		return 0;
	}

	while(!it_.next(tmpKey_, data_)) {
		int err = nextEntries(DB_NEXT | DB_MULTIPLE_KEY);
		if(err != 0 || done_) {
			ie.reset();
			return err;
		} else {
			// placement new to initialize multiple iter
			(void) new (&it_) DbMultipleKeyDataIterator(
				bulk_);
		}
	}

	if (tmpKey_.get_size() < key_.get_size() ||
	    memcmp(key_.get_data(), tmpKey_.get_data(), key_.get_size()) != 0) {
		done_ = true;
		ie.reset();
	} else {
		ie.setThisFromDbt(data_);
	}

	return 0;
}

// ReversePrefixIndexCursor

ReversePrefixIndexCursor::ReversePrefixIndexCursor(IndexDatabase &db,
						   Transaction *txn,
						   const Key *k1)
	: IndexCursor(db, txn, false)
{
	k1->setDbtFromThis(key_);
}

int ReversePrefixIndexCursor::first(IndexEntry &ie)
{
	int err;
	err = findLastPrefix(key_, &ie);
	
	if(err != 0 || done_) {
		ie.reset();
		return err;
	}
	return err;
}

int ReversePrefixIndexCursor::next(IndexEntry &ie)
{
	if(done_) {
		ie.reset();
		return 0;
	}

	int err;
	err = prevEntry();
	if(err != 0 || done_) {
		ie.reset();
		return err;
	}

	if (tmpKey_.get_size() < key_.get_size() ||
	    memcmp(key_.get_data(), tmpKey_.get_data(), key_.get_size()) != 0) {
		done_ = true;
		ie.reset();
	} else {
		ie.setThisFromDbt(data_);
	}

	return 0;
}

// EqualsIndexCursor

EqualsIndexCursor::EqualsIndexCursor(IndexDatabase &db, Transaction *txn,
				     const Key *k1)
	: IndexCursor(db, txn),
	  it_(bulk_)
{
	k1->setDbtFromThis(key_);
}

int EqualsIndexCursor::first(IndexEntry &ie)
{
	int err = nextEntries(DB_SET | DB_MULTIPLE);
	if(err != 0 || done_) {
		ie.reset();
		return err;
	} else {
		// placement new to initialize multiple iter
		(void) new (&it_) DbMultipleDataIterator(bulk_);
	}
	return next(ie);
}

int EqualsIndexCursor::next(IndexEntry &ie)
{
	if (done_) {
		ie.reset();
		return 0;
	}
	while(!it_.next(data_)) {
		int err = nextEntries(DB_NEXT_DUP | DB_MULTIPLE);
		if(err != 0 || done_) {
			ie.reset();
			return err;
		} else {
			// placement new to initialize multiple iter
			(void) new (&it_) DbMultipleDataIterator(bulk_);
		}
	}

	ie.setThisFromDbt(data_);
	return 0;
}

// ReverseInequalityIndexCursor

ReverseInequalityIndexCursor::ReverseInequalityIndexCursor(
	IndexDatabase &db, Transaction *txn, DbWrapper::Operation operation,
	const Key *k1, const Syntax *syntax)
	: IndexCursor(db, txn, false),
	  syntax_(syntax),
	  operation_(operation),
	  greaterThanOperation_(DbWrapper::NONE),
	  lessThanOperation_(DbWrapper::NONE)
{
	DBXML_ASSERT(operation_ != DbWrapper::EQUALITY &&
		     operation_ != DbWrapper::PREFIX &&
		     operation_ != DbWrapper::RANGE);
	DBXML_ASSERT(syntax != 0);
	k1->setDbtFromThis(key_);
}

ReverseInequalityIndexCursor::ReverseInequalityIndexCursor(
	IndexDatabase &db, Transaction *txn, DbWrapper::Operation gto,
	const Key *gtk, DbWrapper::Operation lto, const Key *ltk,
	const Syntax *syntax)
	: IndexCursor(db, txn),
	  syntax_(syntax),
	  operation_(DbWrapper::RANGE),
	  greaterThanOperation_(gto),
	  lessThanOperation_(lto)
{
	DBXML_ASSERT(lto == DbWrapper::LTX || lto == DbWrapper::LTE);
	DBXML_ASSERT(gto == DbWrapper::GTX || gto == DbWrapper::GTE);
	DBXML_ASSERT(syntax != 0);

	gtk->setDbtFromThis(key_);
	ltk->setDbtFromThis(key2_);
}

int ReverseInequalityIndexCursor::first(IndexEntry &ie)
{
	int err = 0;
	DbtOut *key = &key_;
	DbWrapper::Operation op = operation_;
	// For a RANGE we start with the lessThanOperation_,
	// and key2_, which is the opposite of the forward cursor
	if (op == DbWrapper::RANGE) {
		op = lessThanOperation_;
		DBXML_ASSERT((op == DbWrapper::LTE) ||
			     (op == DbWrapper::LTX));
		key = &key2_;
	}
	switch (op) {
	case DbWrapper::ALL:
	case DbWrapper::LTE:
	case DbWrapper::LTX: {
		// this case covers both single values and ranges
		// find last matching key
		err = findLastValue(*key, (op == DbWrapper::LTX));
		break;
	}
	case DbWrapper::GTE:
	case DbWrapper::GTX: {
		// valid for single values only.
		// Unmarshal the index
		Index index;
		index.setFromPrefix(*(char*)key_.get_data());
		Dbt tkey;
		// Set the tkey to the structure prefix of the key
		tkey.set_data(key_.get_data());
		tkey.set_size(Key::structureKeyLength(index, key_));

		err = findLastPrefix(tkey, 0);
		break;
	}
	default:
		// This just won't happen.
		err = DB_NOTFOUND;
		DBXML_ASSERT(0);
		break;
	}
	if (err != 0 || done_ == true) {
		if (err == DB_NOTFOUND || err == DB_KEYEMPTY) {
			err = 0;
		}
		done_ = true;
		ie.reset();
		return err;
	}

	if(operation_ == DbWrapper::RANGE) {
		// From now on, behave like the greater than operator
		operation_ = greaterThanOperation_;
	}

	return compare(ie);
}

int ReverseInequalityIndexCursor::next(IndexEntry &ie)
{
	if(done_) {
		ie.reset();
		return 0;
	}

	int err = prevEntry();
	if(err != 0 || done_) {
		ie.reset();
		return err;
	}
	return compare(ie);
}

int ReverseInequalityIndexCursor::compare(IndexEntry &ie)
{
		
	switch (operation_) {
	case DbWrapper::ALL:
	case DbWrapper::LTX:
	case DbWrapper::LTE: {
		// Check the Prefix and VIDs are the same.
		if (!isSameIndex(key_, tmpKey_)) {
			done_ = true;
			ie.reset();
			return 0;
		}
		break;
	}
	case DbWrapper::GTX: {
		if(syntax_->get_bt_compare()(0, &tmpKey_, &key_) <= 0) {
			done_ = true;
			ie.reset();
			return 0;			
		}
		break;
	}
	case DbWrapper::GTE: {
		if(syntax_->get_bt_compare()(0, &tmpKey_, &key_) < 0) {
			done_ = true;
			ie.reset();
			return 0;			
		}
		break;
	}
	default:
		// This just won't happen.
		DBXML_ASSERT(0);
		break;
	}

	ie.setThisFromDbt(data_);
	return 0;
}

// find the last value that matches the key
int ReverseInequalityIndexCursor::findLastValue(Dbt &key, bool excl)
{
	int err;
	u_int32_t keySize = key.get_size();
	tmpKey_.set_data(key.get_data());
	tmpKey_.set_size(keySize);
	err = cursor_.get(&tmpKey_, &data_, DB_SET_RANGE);
	if (err != 0) {
		// no exact matches; find last entry that matches prefix
		Index index;
		index.setFromPrefix(*(char*)key.get_data());
		Dbt tkey;
		tkey.set_data(key.get_data());
		tkey.set_size(Key::structureKeyLength(index, key));
		err = findLastPrefix(tkey, 0);		
	} else {
		// if exact match, get to end of duplicates
		if(syntax_->get_bt_compare()(0, &tmpKey_, &key) == 0) {
			while ((err = cursor_.get(&tmpKey_, &data_,
						  DB_NEXT_DUP)) == 0) {
				// just iterate
			}
			// iterate back over match if doing LTX
			if (excl)
				cursor_.get(&tmpKey_, &data_, DB_PREV_NODUP);
		} else {
			// not a match, so we've got an entry beyond the
			// range; move back one
			cursor_.get(&tmpKey_, &data_, DB_PREV);
		}
	}
	return 0;
}

EqualsIndexIterator::EqualsIndexIterator(IndexDatabase &db, Transaction *txn,
	const Key *k1)
	: cursor_(db, txn, CURSOR_READ,
		((db.getEnvironment()->get_DB_ENV()->open_flags & DB_INIT_LOCK) ? DB_READ_COMMITTED : 0)),
	  toDo_(true)
{
	k1->setDbtFromThis(key_);
}

void EqualsIndexIterator::reset()
{
	toDo_ = true;
}

bool EqualsIndexIterator::next(NsItem &result)
{
	int err = 0;

	if(toDo_) {
		toDo_ = false;
		err = cursor_.get(&key_, &data_, DB_SET);
	}
	else {
		err = cursor_.get(&key_, &data_, DB_NEXT_DUP);
	}

	if(err == DB_NOTFOUND) return false;
	if(err != 0) throw XmlException(err);

	result.setThisFromDbt(data_);
	return true;
}

bool EqualsIndexIterator::seek(DocID did, const NsNid &nid, NsItem &result)
{
	int err = 0;

	IndexEntry ie;
	ie.setFormat(IndexEntry::LOOKUP_FORMAT);
	ie.setDocID(did);
	ie.setNodeID(&nid);

	ie.setDbtFromThis(data_);

	toDo_ = false;
	err = cursor_.get(&key_, &data_, DB_GET_BOTH_RANGE);

	if(err == DB_NOTFOUND) return false;
	if(err != 0) throw XmlException(err);

	result.setThisFromDbt(data_);
	return true;
}
