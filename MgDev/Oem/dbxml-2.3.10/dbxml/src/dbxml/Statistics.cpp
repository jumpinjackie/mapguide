//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Statistics.cpp,v 1.63 2006/10/30 17:45:53 bostic Exp $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlException.hpp"
#include "Statistics.hpp"
#include "SyntaxManager.hpp"
#include "Container.hpp"
#include "Manager.hpp"
#include "OperationContext.hpp"
#include "Cursor.hpp"
#include "Value.hpp"

#include <string>
#include <sstream>

using namespace std;
using namespace DbXml;

Statistics::Statistics(const Container &container, Transaction *txn,const Index &index,
		       const char *child, const char *parent, const XmlValue &value)
{
	StatisticsReadCache cache;
	OperationContext oc(txn);

	Key key(((Manager&)((Container&)container).getManager()).getImplicitTimezone());
	key.setIndex(index);

	if(!value.isNull() && AtomicTypeValue::convertToSyntaxType(value.getType()) != key.getSyntaxType()) {
		throw XmlException(XmlException::INVALID_VALUE, "Value type does not match index syntax type.");
	}
	if(!value.isNull() && key.getSyntaxType() == Syntax::NONE) {
		throw XmlException(XmlException::INVALID_VALUE, "A value has been specified for an index that does not require one.");
	}

	key.setIDsFromNames(oc, container, parent, child);
	if(index.getPath() == Index::PATH_EDGE && parent == 0) {
		key.setNodeLookup(true);
	}

	if(!value.isNull()) {
		key.setValue(value);
	}

	const KeyStatistics &stats = cache.getKeyStatistics(container, oc, key);

	numIndexedKeys_ = stats.numIndexedKeys_;
	numUniqueKeys_ = stats.numUniqueKeys_;
	sumKeyValueSize_ = stats.sumKeyValueSize_;

	if(index.getKey() == Index::KEY_EQUALITY && !value.isNull() &&
	   numUniqueKeys_ != 0.0) {
		numIndexedKeys_ /= numUniqueKeys_;
		sumKeyValueSize_ /= numUniqueKeys_;
		numUniqueKeys_ = 1.0;
	}
}

StatisticsWriteCache::StatisticsWriteCache()
	: dkv_(SyntaxManager::getInstance()->size())
{}

StatisticsWriteCache::~StatisticsWriteCache()
{
	reset();
}

void StatisticsWriteCache::reset()
{
	Dbt2KSMapVector::iterator i;
	for (i = dkv_.begin();i != dkv_.end();++i) {
		delete *i;
		*i = 0;
	}
}

// We assume that key has the correct endianness.
void StatisticsWriteCache::addToKeyStatistics(const Index &index, const Dbt &key, const Dbt &data, bool unique)
{
	const Syntax *syntax = SyntaxManager::getInstance()->getSyntax((Syntax::Type)index.getSyntax());

	Dbt2KSMap* dk = dkv_[syntax->getType()];
	if (dk == 0) {
		dk = new Dbt2KSMap;
		dkv_[syntax->getType()] = dk;
	}

	size_t skl = Key::structureKeyLength(index, key);
	if (skl > 0 && skl <= key.get_size()) {
		DbtIn k(key.get_data(), skl);
		Dbt2KSMap::iterator i = dk->find(k);
		KeyStatistics *ps = 0;
		if (i == dk->end()) {
			KeyStatistics s;
			dk->insert(Dbt2KSMap::value_type(k, s));
			i = dk->find(k);
			ps = &i->second;
		} else {
			ps = &i->second;
		}
		if(index.indexerAdd()) {
			ps->numIndexedKeys_++;
			ps->sumKeyValueSize_ += key.get_size() + data.get_size();
			if (unique) {
				ps->numUniqueKeys_++;
			}
		} else {
			ps->numIndexedKeys_--;
			ps->sumKeyValueSize_ -= key.get_size() + data.get_size();
			if (unique) {
				ps->numUniqueKeys_--;
			}
		}
	}
}

int StatisticsWriteCache::updateContainer(OperationContext &context, Container &container) const
{
	int err = 0;
	int syntax = 0;
	Dbt2KSMapVector::const_iterator i;
	for (i = dkv_.begin();err == 0 && i != dkv_.end();++i) {
		Dbt2KSMap* dk = *i;
		if (dk != 0) {
			Dbt2KSMap::iterator i2;
			for (i2 = dk->begin();i2 != dk->end();++i2) {
				DbtIn &k = (DbtIn&)i2->first;  // jcm const_cast
				SyntaxDatabase *database =
					container.getIndexDB(
						(Syntax::Type)syntax);
				if (database)
					err = database->
						updateStatistics(
							context, k, i2->second);
			}
		}
		syntax++;
	}
	return err;
}

StatisticsReadCache::StatisticsReadCache()
{
}

void StatisticsReadCache::reset()
{
	statsMap_.clear();
}

KeyStatistics StatisticsReadCache::getKeyStatistics(const Container &container, OperationContext &context, const Key &key)
{
	StatsMapKey noValueKey(&container);
	if(key.getIndex().getKey() == Index::KEY_EQUALITY) {
		noValueKey.key.set(key, 0, 0);
	} else {
		noValueKey.key.set(key);
	}

	StatsMap::const_iterator it = statsMap_.find(noValueKey);
	if(it != statsMap_.end()) {
		StatsMap::value_type v = *it;
		return v.second;
	} else {
		// Not found, so look it up
		return populateStatistics(context, noValueKey);
	}
}

double StatisticsReadCache::getPercentage(const Container &container, OperationContext &context, DbWrapper::Operation op1, Key k1, DbWrapper::Operation op2, Key k2)
{
	PercentageMapKey key(&container, op1, k1, op2, k2);

	PercentageMap::const_iterator it = percentageMap_.find(key);
	if(it != percentageMap_.end()) {
		return it->second;
	}

	const SyntaxDatabase *database = container.getIndexDB(k1.getSyntaxType());
	if (!database)
		return 0.0;
	
	double percentage;
	if(op2 == DbWrapper::NONE) {
		percentage = database->getStatisticsDB()->percentage(context, op1, DbWrapper::NONE, DbWrapper::NONE, k1, k2);
	} else {
		percentage = database->getStatisticsDB()->percentage(context, DbWrapper::RANGE, op1, op2, k1, k2);
	}
	putPercentage(key, percentage);
	return percentage;
}

void StatisticsReadCache::putKeyStatistics(const StatsMapKey &key, const KeyStatistics &stats)
{
	statsMap_.insert(StatsMap::value_type(key, stats));
}

void StatisticsReadCache::putPercentage(const PercentageMapKey &pk, const double &p)
{
	percentageMap_.insert(PercentageMap::value_type(pk, p));
}

KeyStatistics StatisticsReadCache::populateStatistics(OperationContext &context, const StatsMapKey &key)
{
	SyntaxDatabase *database = const_cast<SyntaxDatabase*>(key.container->getIndexDB(key.key.getSyntaxType()));

	if (!database)
		return KeyStatistics();
	
	key.key.setDbtFromThis(context.key());

	int structureLength = Key::structureKeyLength(key.key.getIndex(),
						      context.key());
	if(structureLength < (int)context.key().get_size()) {
		context.key().set_size(structureLength); // trim the value off
	}

	KeyStatistics tmpStats;
	KeyStatistics result;

	// Behave like a prefix cursor, adding all the statistics with a key whose prefix matches our key
	int found = 0;
	Cursor cursor(*database->getStatisticsDB(), context.txn(), CURSOR_READ);
	int err = cursor.error();
	if(err == 0) {
		DbtOut original;
		original.set(context.key().get_data(), context.key().get_size());

		err = cursor.get(&context.key(), &context.data(), DB_SET_RANGE);
		while(err == 0) {
			if(context.key().get_size() < original.get_size() ||
			   memcmp(original.get_data(), context.key().get_data(), original.get_size()) != 0) {
				// We've reached the end, so set a flag saying so
				err = DB_NOTFOUND;
			}
			else {
				++found;

				tmpStats.setThisFromDbt(context.data());

				// Fix the unique keys value, if necessary
				if(tmpStats.numUniqueKeys_ == 0 &&
				   tmpStats.numIndexedKeys_ != 0) {
					tmpStats.numUniqueKeys_ = 1;
				}

				// add the value it to the results
				result.add(tmpStats);

				// Get the next key/data pair
				context.key().set(original.get_data(), original.get_size());
				err = cursor.get(&context.key(), &context.data(), DB_NEXT);
			}
		}
		if(err == DB_NOTFOUND || err == DB_KEYEMPTY) {
			err = 0;
		}
	}
	if(err) throw XmlException(err);

	putKeyStatistics(key, result);
	return result;
}

bool StatisticsReadCache::StatsMapKey::operator<(const StatsMapKey &o) const
{
	if(container < o.container) return true;
	if(o.container < container) return false;
	if(key < o.key) return true;
	if(o.key < key) return false;
	return false;
}

bool StatisticsReadCache::PercentageMapKey::operator<(const PercentageMapKey &o) const
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

