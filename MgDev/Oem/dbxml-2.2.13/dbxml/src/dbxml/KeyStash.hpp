//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: KeyStash.hpp,v 1.6 2005/12/14 17:57:07 jsnelson Exp $
//

#ifndef __KEYSTASH_H
#define	__KEYSTASH_H

#include <set>
#include "Buffer.hpp"
#include "IndexSpecification.hpp"
#include "Statistics.hpp"

namespace DbXml
{

class Container;
class Document;

class KeyStash
{
public:
	class Entry
	{
	public:
		Entry(const Key &key, const char *value,
		      size_t length, const IndexEntry *ie,
		      Buffer &b);
		void *getKey() const
		{
			return buffer.getBuffer(key_offset);
		}
		void getKey(DbtIn &key) const
		{
			key.set(buffer.getBuffer(key_offset),
				key_length);
		}
		void *getData() const
		{
			return buffer.getBuffer(data_offset);
		}
		void getData(DbtIn &data) const
		{
			data.set(buffer.getBuffer(data_offset),
				 data_length);
		}
		bool equals(const Entry *o) const;

		Index index;
		size_t key_offset, key_length;
		size_t data_offset, data_length;
		Buffer &buffer;

		Entry *otherEntry;
	};

	KeyStash();
	~KeyStash();

	void reset();

	// Returns the Entry, if the key was unique - null otherwise
	Entry *addUniqueKey(const Key &key, const char *value,
		size_t length);
	void addKey(const Key &key, const char *value,
		size_t length, const IndexEntry &ie);
	void addConsolationKey(const Key &key, const IndexEntry &ie);

	/**
	 * Writes a set of index keys to the index databases.
	 *
	 * \param context The context within which tp perform the update.
	 * \param container The container to write to
	 * \param document The document that the keys are from
	 * \return error code.
	 * \exception XmlException
	 */
	int updateIndex(OperationContext &context, Container *container) const;

private:
	void addEntry(Entry *toAdopt);
	Entry *addUniqueEntry(Entry *toAdopt);
private:
	KeyStash(const KeyStash &);
	KeyStash &operator=(const KeyStash &);

	struct EntryCompare {
		bool operator()(const Entry *a, const Entry *b) const;
	};
	typedef std::set<Entry*, EntryCompare> EntrySet;

	Buffer buffer_;
	EntrySet keys_;

	mutable StatisticsWriteCache statistics_;
};

}

#endif
