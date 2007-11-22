//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Statistics.hpp,v 1.35 2006/10/30 17:45:53 bostic Exp $
//

#ifndef __STATISTICS_HPP
#define	__STATISTICS_HPP

#include <map>
#include <vector>
#include "KeyStatistics.hpp"
#include "ScopedDbt.hpp"
#include "Key.hpp"
#include "ReferenceCounted.hpp"

namespace DbXml
{

class Transaction;
class Container;
class Index;
class Syntax;
class OperationContext;

class Statistics : public ReferenceCounted
{
public:
	Statistics(const Container &container, Transaction *txn,
		   const Index &index, const char *child,
		   const char *parent, const XmlValue &value);

	double getNumberOfIndexedKeys() const
	{
		return numIndexedKeys_;
	}
	double getNumberOfUniqueKeys() const
	{
		return numUniqueKeys_;
	}
	double getSumKeyValueSize() const
	{
		return sumKeyValueSize_;
	}

private:
	Statistics(const Statistics &);
	Statistics &operator=(const Statistics &);

	double numIndexedKeys_;
	double numUniqueKeys_;
	double sumKeyValueSize_;
};

/** The indexer makes use of this class to maintain statistics about
    the keys being generated. */
class StatisticsWriteCache
{
public:
	StatisticsWriteCache();
	~StatisticsWriteCache();
	// Reset the statistics object so that it can be reused.
	void reset();

	/// Add a key to the statistics. We assume that key has the correct endianness.
	void addToKeyStatistics(const Index &index, const Dbt &key,
				const Dbt &data, bool unique);

	/// Write the statistics to the statistics databases
	int updateContainer(OperationContext &context,
			    Container &container) const;

private:
	StatisticsWriteCache(const StatisticsWriteCache&);
	StatisticsWriteCache &operator=(const StatisticsWriteCache &);

	typedef std::map<DbtIn, KeyStatistics> Dbt2KSMap;
	typedef std::vector<Dbt2KSMap*> Dbt2KSMapVector;
	Dbt2KSMapVector dkv_;
};

class StatisticsReadCache
{
public:
	StatisticsReadCache();

	void reset();

	KeyStatistics getKeyStatistics(const Container &container,
					      OperationContext &context,
					      const Key &key);
	double getPercentage(const Container &container,
			     OperationContext &context,
			     SecondaryDatabase::Operation op1,
			     Key k1,
			     SecondaryDatabase::Operation op2 =
			     SecondaryDatabase::NONE, Key k2 = Key(0));
private:
	struct StatsMapKey {
		StatsMapKey(const Container *c)
			: container(c), key(0) {}
		StatsMapKey(const Container *c, const Key &k)
			: container(c), key(k) {}
		bool operator<(const StatsMapKey &o) const;

		const Container *container;
		Key key;
	};

	struct PercentageMapKey {
		PercentageMapKey(const Container *c,
				 SecondaryDatabase::Operation op1,
				 const Key &k1,
				 SecondaryDatabase::Operation op2,
				 const Key &k2)
			: container(c), operation1(op1),
			  operation2(op2), key1(k1), key2(k2) {}
		bool operator<(const PercentageMapKey &o) const;

		const Container *container;
		SecondaryDatabase::Operation operation1, operation2;
		Key key1, key2;
	};

	KeyStatistics populateStatistics(OperationContext &context, const StatsMapKey &key);
	void putKeyStatistics(const StatsMapKey &key, const KeyStatistics &stats);
	void putPercentage(const PercentageMapKey &pk, const double &p);

	typedef std::map<StatsMapKey, KeyStatistics> StatsMap;
	typedef std::map<PercentageMapKey, double> PercentageMap;

	StatsMap statsMap_;
	PercentageMap percentageMap_;
};

}
#endif
