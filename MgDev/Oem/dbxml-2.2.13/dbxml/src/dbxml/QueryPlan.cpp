//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: QueryPlan.cpp,v 1.177 2005/11/30 17:12:14 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include <assert.h>
#include <string.h>
#include <sstream>
#include <set>
#include <algorithm>
#include "QueryPlan.hpp"
#include "UTF8.hpp"
#include "Name.hpp"
#include "Container.hpp"
#include "OperationContext.hpp"
#include "QueryContext.hpp"
#include "SyntaxManager.hpp"
#include "Value.hpp"
#include "QueryExecutionContext.hpp"
#include <dbxml/XmlNamespace.hpp>

#include <pathan/dataItem/DataItem.hpp>
#include <pathan/SequenceType.hpp>

#include <math.h> // for ceil()

using namespace DbXml;
using namespace std;

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

static inline char *char_clone(const char *s, MemoryManager *mm) {
	return XMLString::replicate(s, mm);
}

static inline char *char_clone(const char *s, size_t len, MemoryManager *mm) {
	if(s == 0) return 0;
	char *result = (char*)mm->allocate(len * sizeof(char));
	::strncpy(result, s, len);
	return result;
}

static inline bool char_equals(const char *a, const char *b) {
	return XMLString::equals(a, b);
}

static inline bool char_equals(const char *a, size_t lena, const char *b, size_t lenb) {
	if(lena != lenb) return false;
	return (::strncmp(a, b, lena) == 0) ? true : false;
}

static inline size_t char_len(const char *s) {
	if(s == 0) return 0;
	return ::strlen(s);
}

/////////////////////////////////
// QueryPlan

QueryPlan *QueryPlan::finalizeGlobalRawQueryPlan()
{
	return compress();
}

QueryPlan *QueryPlan::createLocalRawQueryPlan(
	const ImpliedSchemaNode *root, const Log &log,
	DynamicContext *context, bool partiallyOptimize) const
{
	QueryPlan *result = copy(context->getMemoryManager());

	// Only filter if we know the ImpliedSchemaNode root
	if(root != 0) result = result->filter(root);

	if(result != 0) result = result->resolvePaths();
	logQP(log, "RQP", result);

	if(partiallyOptimize) {
		bool success = true;
		if(result != 0)
			result = result->resolveValues(log,
						       context, /*lookupValues*/
						       false, success);
		if(result != 0)
			result = result->removeSubsets(log);

		if(result == 0) {
			// If qp_ is null, this lookup's results do not
			// appear in the result. Therefore we generate
			// a UniverseQP as a query plan.
			result = new (context->getMemoryManager())
				UniverseQP(context->getMemoryManager());
		}

		logQP(log, "POQP", result);
	} else if (result != 0) {
		result = result->removeSubsets(log);
	} else // return UniverseQP
		result = new (context->getMemoryManager())
			UniverseQP(context->getMemoryManager());

	return result;
}

QueryPlan *QueryPlan::createExecutableQueryPlan(
	Transaction *txn, const Container &container,
	DynamicContext *context, bool nodeQP,
	bool lookupValues, bool &success) const
{
	success = true;
	QueryPlan *result = copy(context->getMemoryManager());
	result = result->resolveValues(container, context, lookupValues, success);

	IndexSpecification is;
	container.getConfigurationDB()->getIndexSpecification(txn, is);

	result = result->resolveSteps(container, is, nodeQP);
	if(result != 0) result = result->removeSubsets(container);

	if(result == 0) {
		// If qp_ is null, this lookup's results do not
		// appear in the result. Therefore we generate
		// a UniverseQP as a query plan.
		result = new (context->getMemoryManager())
			UniverseQP(context->getMemoryManager());
		result = result->resolveSteps(container, is, nodeQP);
	}

	if(success) logQP(container, "OQP", result);
	else logQP(container, "POQP", result);

	return result;
}

static inline string shorten(const string &name, unsigned int length) {
	string shortName = name;
	if(shortName.length() > length) {
		shortName = name.substr(0, length - 3) + "...";
	}

	return shortName;
}

void QueryPlan::logLegend(const Log &log) const
{
	static bool outputLegend = true;

	if(outputLegend && Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_INFO)) {
		outputLegend = false;
		ostringstream oss;
		oss
			<< "\n"
			<< "\t Legend for the Query Plan log output\n"
			<< "\n"
			<< "\t RQP  - Raw Query Plan before any optimizations\n"
			<< "\t POQP - Partially optimized Query Plan\n"
			<< "\t OQP  - Optimized Query Plan after optimizations\n"
			<< "\n"
			<< "\t P    - Presence index look up\n"
			<< "\t V    - Value index look up\n"
			<< "\t R    - Range index look up\n"
			<< "\t D    - Document name index look up\n"
			<< "\t U    - Universal set\n"
			<< "\t E    - Empty set\n"
			<< "\t n    - Intersection\n"
			<< "\t u    - Union\n"
			<< "\n";
		log.log(Log::C_OPTIMIZER, Log::L_INFO, oss);
	}
}

void QueryPlan::logQP(const Log &log, const string &title, const QueryPlan *qp) const
{
	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_INFO)) {
		ostringstream oss;

		oss << title << ": ";
		if(qp) oss << qp->toString(false);
		else oss << "none";

		logLegend(log);
		log.log(Log::C_OPTIMIZER, Log::L_INFO, oss);
	}
}

void QueryPlan::logCost(QueryExecutionContext &qec, Cost &cost, const KeyStatistics *stats) const
{
	static const unsigned int MAX_NAME_LENGTH = 80;

	string name = toString();
	if (Log::isLogEnabled(Log::C_QUERY, Log::L_DEBUG)) {
		string shortName = shorten(name, MAX_NAME_LENGTH);

		ostringstream oss;
		oss << shortName;
		oss << " : keys=" << cost.keys;
		oss << ", pages=" << cost.pages;
		if(stats != 0) oss << " : stats(" << stats->asString() << ")";

		logLegend(qec.getContainer());
		qec.getContainer().log(Log::C_QUERY, Log::L_DEBUG, oss);
	}
}

void QueryPlan::logIDs(QueryExecutionContext &qec, const IDS::SharedPtr &ids) const
{
	static const unsigned int MAX_NAME_LENGTH = 80;

	string name = toString();
	if (Log::isLogEnabled(Log::C_QUERY, Log::L_INFO)) {
		string shortName = shorten(name, MAX_NAME_LENGTH);

		ostringstream oss;
		oss << shortName << " : ";
		if (!ids || ids->size() == 0) {
			oss << "NONE";
		} else {
			oss << "[" << ids->size() << "] ";
			int n = 0;
			IDS::const_iterator p;
			for (p = ids->begin();n < 20 && p != ids->end();++p, ++n) {
				oss << *p << " ";
			}
			if (p != ids->end()) {
				oss << "...";
			}
		}

		logLegend(qec.getContainer());
		qec.getContainer().log(Log::C_QUERY, Log::L_INFO, oss);
	}
	qec.addExecutionStep(name);
}

void QueryPlan::logIndexData(QueryExecutionContext &qec, const IndexData::SharedPtr &data) const
{
	static const unsigned int MAX_NAME_LENGTH = 80;

	string name = toString();
	if (Log::isLogEnabled(Log::C_QUERY, Log::L_INFO)) {
		string shortName = shorten(name, MAX_NAME_LENGTH);

		ostringstream oss;
		oss << shortName << " : ";
		if (!data || data->size() == 0) {
			oss << "NONE";
		} else {
			oss << "[" << data->size() << "] ";
			int n = 0;
			IndexData::const_iterator p;
			for (p = data->begin();n < 10 && p != data->end();++p, ++n) {
				oss << **p << " ";
			}
			if (p != data->end()) {
				oss << "...";
			}
		}

		logLegend(qec.getContainer());
		qec.getContainer().log(Log::C_QUERY, Log::L_INFO, oss);
	}
	qec.addExecutionStep(name);
}

/////////////////////////////////
// OperationQP

QueryPlan *OperationQP::addArg(QueryPlan *o)
{
	if(o) {
		if(type_ == o->getType()) {
			OperationQP *op = (OperationQP*)o;
			args_.insert(args_.end(), op->args_.begin(), op->args_.end());
		}
		else {
			args_.push_back(o);
			return o;
		}
	}

	return this;
}

class UniqueArgs
{
public:
	typedef set<QueryPlan*>::iterator iterator;

	iterator begin() {
		return _s.begin();
	}
	iterator end() {
		return _s.end();
	}

	void add(QueryPlan::Type type, QueryPlan *arg) {
		if(arg) {
			if(arg->getType() == type) {
				const OperationQP::Vector &args  = ((OperationQP*)arg)->getArgs();
				OperationQP::Vector::const_iterator end = args.end();
				for(OperationQP::Vector::const_iterator i = args.begin();
				    i != end; ++i) add(type, *i);
			}
			else {
				_s.insert(arg);
			}
		}
	}

	void clear() {
		_s.clear();
	}

private:
	set<QueryPlan*> _s;
};

QueryPlan *OperationQP::compress()
{
	// Remove duplicate arguments
	UniqueArgs uniqueArgs;
	for(Vector::iterator it = args_.begin(); it != args_.end(); ++it) {
		uniqueArgs.add(type_, (*it)->compress());
	}
	args_.clear();
	std::copy(uniqueArgs.begin(), uniqueArgs.end(), back_inserter(args_));

	return dissolve();
}

QueryPlan *OperationQP::dissolve()
{
	// Dissolve if we have none of only one argument
	if(args_.empty()) return 0;
	if(args_.size() == 1) return args_[0];
	return this;
}

QueryPlan *OperationQP::filter(const ImpliedSchemaNode *root)
{
	// Remove duplicate arguments
	UniqueArgs uniqueArgs;
	for(Vector::iterator it = args_.begin(); it != args_.end(); ++it) {
		uniqueArgs.add(type_, (*it)->filter(root));
	}
	args_.clear();
	std::copy(uniqueArgs.begin(), uniqueArgs.end(), back_inserter(args_));

	return dissolve();
}

QueryPlan *OperationQP::resolvePaths()
{
	UniqueArgs uniqueArgs;
	for(Vector::iterator it = args_.begin(); it != args_.end(); ++it) {
		uniqueArgs.add(type_, (*it)->resolvePaths());
	}
	args_.clear();
	std::copy(uniqueArgs.begin(), uniqueArgs.end(), back_inserter(args_));

	return dissolve();
}

QueryPlan *OperationQP::resolveValues(const Log &log, DynamicContext *context, bool lookupValues, bool &success)
{
	UniqueArgs uniqueArgs;
	for(Vector::iterator it = args_.begin(); it != args_.end(); ++it) {
		uniqueArgs.add(type_, (*it)->resolveValues(log, context, lookupValues, success));
	}
	args_.clear();
	std::copy(uniqueArgs.begin(), uniqueArgs.end(), back_inserter(args_));

	return dissolve();
}

QueryPlan *OperationQP::resolveSteps(const Container &container, const IndexSpecification &is, bool nodeQP)
{
	UniqueArgs uniqueArgs;
	for(Vector::iterator it = args_.begin(); it != args_.end(); ++it) {
		uniqueArgs.add(type_, (*it)->resolveSteps(container, is, nodeQP));
	}
	args_.clear();
	std::copy(uniqueArgs.begin(), uniqueArgs.end(), back_inserter(args_));

	return dissolve();
}

void OperationQP::logSubset(const Log &log, const QueryPlan *subset, const QueryPlan *superset) const
{
	static const unsigned int MAX_NAME_LENGTH = 50;

	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_DEBUG)) {
		string subset_name = shorten(subset->toString(), MAX_NAME_LENGTH);
		string superset_name = shorten(superset->toString(), MAX_NAME_LENGTH);

		ostringstream oss;

		if(type_ == QueryPlan::UNION) {
			oss << "u: ";
		}
		else {
			oss << "n: ";
		}

		oss << subset_name << " <isSubsetOf> " << superset_name;

		oss << " -> Removing ";

		if(type_ == QueryPlan::UNION) {
			oss << subset_name;
		}
		else {
			oss << superset_name;
		}

		logLegend(log);
		log.log(Log::C_OPTIMIZER, Log::L_DEBUG, oss);
	}
}

void OperationQP::logMerge(const Log &log, const QueryPlan *l, const QueryPlan *r, const QueryPlan *result) const
{
	static const unsigned int MAX_NAME_LENGTH = 50;

	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_DEBUG)) {
		string l_name = shorten(l->toString(), MAX_NAME_LENGTH);
		string r_name = shorten(r->toString(), MAX_NAME_LENGTH);
		string result_name = shorten(result->toString(), MAX_NAME_LENGTH);

		ostringstream oss;

		if(type_ == QueryPlan::UNION) {
			oss << "u: ";
		}
		else {
			oss << "n: ";
		}

		oss << "Merged: " << l_name << ", " << r_name;
		oss << " -> " << result_name;

		logLegend(log);
		log.log(Log::C_OPTIMIZER, Log::L_DEBUG, oss);
	}
}

/////////////////////////////////
// UnionQP

UnionQP::UnionQP(QueryPlan *l, QueryPlan *r, MemoryManager *mm)
	: OperationQP(QueryPlan::UNION, mm)
{
	addArg(l);
	addArg(r);
}

class keys_compare_more
{
public:
	keys_compare_more(OperationContext &context, QueryExecutionContext &qec)
		: context_(context), qec_(qec) {}

	bool operator()(const QueryPlan *l, const QueryPlan *r) const
	{
		return l->cost(context_, qec_).keys > r->cost(context_, qec_).keys;
	}

private:
	OperationContext &context_;
	QueryExecutionContext &qec_;
};

IDS::SharedPtr UnionQP::execute(OperationContext &context, QueryExecutionContext &qec) const
{
	vector<QueryPlan*> sortedArgs(args_.begin(), args_.end());
	sort(sortedArgs.begin(), sortedArgs.end(), keys_compare_more(context, qec));

	IDS::SharedPtr result;
	vector<QueryPlan*>::iterator it = sortedArgs.begin();
	if(it != sortedArgs.end()) {
		result = (*it)->execute(context, qec);
		for(++it; it != sortedArgs.end(); ++it) {
			result = result->set_union((*it)->execute(context, qec));
		}
	}
	logIDs(qec, result);
	return result;
}

IndexData::SharedPtr UnionQP::nodes_execute(OperationContext &context, QueryExecutionContext &qec) const
{
	vector<QueryPlan*> sortedArgs(args_.begin(), args_.end());
	sort(sortedArgs.begin(), sortedArgs.end(), keys_compare_more(context, qec));

	IndexData::SharedPtr result(new IndexData);
	vector<QueryPlan*>::iterator it = sortedArgs.begin();
	if(it != sortedArgs.end()) {
		result->set_union((*it)->nodes_execute(context, qec));
		for(++it; it != sortedArgs.end(); ++it) {
			result->set_union((*it)->nodes_execute(context, qec));
		}
	}
	logIndexData(qec, result);
	return result;
}

QueryPlan::Cost UnionQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	Cost result;
	Vector::const_iterator it = args_.begin();
	if(it != args_.end()) {
		result = (*it)->cost(context, qec);
		for(++it; it != args_.end(); ++it) {
			Cost childcost = (*it)->cost(context, qec);
			result.pages += childcost.pages;
			result.keys += childcost.keys;
		}
	}
	logCost(qec, result, 0);
	return result;
}

QueryPlan *UnionQP::removeSubsets(const Log &log)
{
	UniqueArgs uniqueArgs;
	Vector::iterator it;
	for(it = args_.begin(); it != args_.end(); ++it) {
		uniqueArgs.add(type_, (*it)->removeSubsets(log));
	}
	args_.clear();
	std::copy(uniqueArgs.begin(), uniqueArgs.end(), back_inserter(args_));

	uniqueArgs.clear();
	for(it = args_.begin(); it != args_.end(); ++it) {

		bool store = true;
		for(UniqueArgs::iterator it3 = uniqueArgs.begin();
		    it3 != uniqueArgs.end(); ++it3) {
			if((*it)->isSubsetOf(*it3)) {
				logSubset(log, *it, *it3);
				store = false;
				break;
			}
		}

		if(store) {
			for(Vector::iterator it2 = it + 1; it2 != args_.end(); ++it2) {
				if((*it)->isSubsetOf(*it2)) {
					logSubset(log, *it, *it2);
					store = false;
					break;
				}
			}

			if(store) uniqueArgs.add(type_, *it);
		}
	}
	args_.clear();
	std::copy(uniqueArgs.begin(), uniqueArgs.end(), back_inserter(args_));

	return dissolve();
}

bool UnionQP::isSubsetOf(const QueryPlan *o) const
{
	for(Vector::const_iterator it = args_.begin(); it != args_.end(); ++it) {
		if(!(*it)->isSubsetOf(o)) return false;
	}
	return true;
}

bool UnionQP::isSupersetOf(const QueryPlan *o) const
{
	for(Vector::const_iterator it = args_.begin(); it != args_.end(); ++it) {
		if(o->isSubsetOf(*it)) return true;
	}
	return false;
}

QueryPlan *UnionQP::copy(MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	UnionQP *result = new (mm) UnionQP(mm);

	for(Vector::const_iterator it = args_.begin(); it != args_.end(); ++it) {
		result->addArg((*it)->copy(mm));
	}

	return result;
}

string UnionQP::toString(bool brief) const
{
	ostringstream s;

	s << "u(";

	bool addComma = false;
	for(Vector::const_iterator it2 = args_.begin(); it2 != args_.end(); ++it2) {
		if(addComma) { s << ","; } else { addComma = true; }
		s << (*it2)->toString(brief);
	}

	s << ")";

	return s.str();
}

/////////////////////////////////
// IntersectQP

IntersectQP::IntersectQP(QueryPlan *l, QueryPlan *r, MemoryManager *mm)
	: OperationQP(QueryPlan::INTERSECT, mm)
{
	addArg(l);
	addArg(r);
}

class keys_compare_less
{
public:
	keys_compare_less(OperationContext &context, QueryExecutionContext &qec)
		: context_(context), qec_(qec) {}

	bool operator()(const QueryPlan *l, const QueryPlan *r) const
	{
		return l->cost(context_, qec_).keys < r->cost(context_, qec_).keys;
	}

private:
	OperationContext &context_;
	QueryExecutionContext &qec_;
};

IDS::SharedPtr IntersectQP::execute(OperationContext &context, QueryExecutionContext &qec) const
{
	vector<QueryPlan*> sortedArgs(args_.begin(), args_.end());
	sort(sortedArgs.begin(), sortedArgs.end(), keys_compare_less(context, qec));

	IDS::SharedPtr result;
	Cost cost;
	vector<QueryPlan*>::iterator it = sortedArgs.begin();
	if(it != sortedArgs.end()) {
		result = (*it)->execute(context, qec);
		for(++it; !result->empty() && it != sortedArgs.end(); ++it) {
			// If we can filter more cheaply than intersecting the next
			// argument, don't intersect it!
			cost = (*it)->cost(context, qec);
			if(cost.pages < qec.costToFilter(result)) {
				result = result->set_intersection((*it)->execute(context, qec));
			}
		}
	}
	logIDs(qec, result);
	return result;
}

IndexData::SharedPtr IntersectQP::nodes_execute(OperationContext &context, QueryExecutionContext &qec) const
{
	vector<QueryPlan*> sortedArgs(args_.begin(), args_.end());
	sort(sortedArgs.begin(), sortedArgs.end(), keys_compare_less(context, qec));

	IndexData::SharedPtr result(new IndexData);
	vector<QueryPlan*>::iterator it = sortedArgs.begin();
	if(it != sortedArgs.end()) {
		result->set_union((*it)->nodes_execute(context, qec));
		for(++it; !result->empty() && it != sortedArgs.end(); ++it) {
			result->set_intersection((*it)->nodes_execute(context, qec));
		}
	}
	logIndexData(qec, result);
	return result;
}

QueryPlan::Cost IntersectQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	Cost result;
	Vector::const_iterator it = args_.begin();
	if(it != args_.end()) {
		result = (*it)->cost(context, qec);
		for(++it; it != args_.end(); ++it) {
			Cost childcost = (*it)->cost(context, qec);
			result.pages += childcost.pages;
			if(childcost.keys < result.keys) result.keys = childcost.keys;
		}
	}
	logCost(qec, result, 0);
	return result;
}

static inline bool isLessThanOrGreaterThan(const QueryPlan *l)
{
	return l->getType() == QueryPlan::VALUE &&
		(((ValueQP *)l)->getOperation() == DbWrapper::LTX ||
		 ((ValueQP *)l)->getOperation() == DbWrapper::LTE ||
		 ((ValueQP *)l)->getOperation() == DbWrapper::GTX ||
		 ((ValueQP *)l)->getOperation() == DbWrapper::GTE);
}

QueryPlan *IntersectQP::removeSubsets(const Log &log)
{
	UniqueArgs uniqueArgs;
	Vector::iterator it;
	for(it = args_.begin(); it != args_.end(); ++it) {
		if(isLessThanOrGreaterThan(*it)) {

			for(Vector::iterator it2 = it + 1;
			    it2 != args_.end(); ++it2) {
				if(isLessThanOrGreaterThan(*it2)) {
					QueryPlan *range = createRange((ValueQP*)*it, (ValueQP*)*it2);
					if(range != 0) {
						logMerge(log, *it, *it2, range);
						uniqueArgs.add(type_, range);
					}
				}
			}

		}

		uniqueArgs.add(type_, (*it)->removeSubsets(log));
	}

	args_.clear();
	std::copy(uniqueArgs.begin(), uniqueArgs.end(), back_inserter(args_));

	uniqueArgs.clear();
	for(it = args_.begin(); it != args_.end(); ++it) {

		bool store = true;
		for(UniqueArgs::iterator it3 = uniqueArgs.begin();
		    it3 != uniqueArgs.end(); ++it3) {
			if((*it3)->isSubsetOf(*it)) {
				logSubset(log, *it3, *it);
				store = false;
				break;
			}
		}

		if(store) {
			for(Vector::iterator it2 = it + 1; it2 != args_.end(); ++it2) {
				if((*it2)->isSubsetOf(*it)) {
					logSubset(log, *it2, *it);
					store = false;
					break;
				}
			}

			if(store) uniqueArgs.add(type_, *it);
		}
	}

	args_.clear();
	std::copy(uniqueArgs.begin(), uniqueArgs.end(), back_inserter(args_));

	return dissolve();
}

QueryPlan *IntersectQP::createRange(const ValueQP *l, const ValueQP *r)
{
	if(l->getNodeType() == r->getNodeType() &&
	   l->isParentSet() == r->isParentSet() &&
	   char_equals(l->getChildName(), r->getChildName()) &&
	   char_equals(l->getParentName(), r->getParentName()) &&
	   l->getValue().getDataItem() == 0 &&
	   r->getValue().getDataItem() == 0 ) {

		switch(l->getOperation()) {
		case DbWrapper::LTX:
		case DbWrapper::LTE: {
			if(r->getOperation() == DbWrapper::GTX || r->getOperation() == DbWrapper::GTE) {
				return new (memMgr_) RangeQP(r, l, memMgr_);
			}
			break;
		}
		case DbWrapper::GTX:
		case DbWrapper::GTE: {
			if(r->getOperation() == DbWrapper::LTX || r->getOperation() == DbWrapper::LTE) {
				return new (memMgr_) RangeQP(l, r, memMgr_);
			}
			break;
		}
		default: break;
		}
	}
	return 0;
}

bool IntersectQP::isSubsetOf(const QueryPlan *o) const
{
	if(o->getType() == QueryPlan::INTERSECT) {
		const IntersectQP *oi = (const IntersectQP*)o;
		for(Vector::const_iterator oit = oi->args_.begin(); oit != oi->args_.end(); ++oit) {
			bool found = false;
			for(Vector::const_iterator it = args_.begin(); it != args_.end(); ++it) {
				if((*it)->isSubsetOf(*oit)) {
					found = true;
					break;
				}
			}
			if(found == false) return false;
		}
		return true;
	} else {
		for(Vector::const_iterator it = args_.begin(); it != args_.end(); ++it) {
			if((*it)->isSubsetOf(o)) return true;
		}
		return false;
	}
}

bool IntersectQP::isSupersetOf(const QueryPlan *o) const
{
	for(Vector::const_iterator it = args_.begin(); it != args_.end(); ++it) {
		if(!o->isSubsetOf(*it)) return false;
	}
	return true;
}

QueryPlan *IntersectQP::copy(MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	IntersectQP *result = new (mm) IntersectQP(mm);

	for(Vector::const_iterator it = args_.begin(); it != args_.end(); ++it) {
		result->addArg((*it)->copy(mm));
	}

	return result;
}

string IntersectQP::toString(bool brief) const
{
	ostringstream s;

	s << "n(";

	bool addComma = false;
	for(Vector::const_iterator it2 = args_.begin(); it2 != args_.end(); ++it2) {
		if(addComma) { s << ","; } else { addComma = true; }
		s << (*it2)->toString(brief);
	}

	s << ")";

	return s.str();
}

/////////////////////////////////
// PathsQP

void PathsQP::addPaths(const Paths &o)
{
	Paths::const_iterator it = o.begin();
	Paths::const_iterator end = o.end();
	for(; it != end; ++it) {
		paths_.push_back(*it);
	}
}

bool PathsQP::sameTree() const
{
	Paths::const_iterator it = paths_.begin();
	Paths::const_iterator end = paths_.end();
	if(it != end) {
		ImpliedSchemaNode *root = (*it)->getRoot();
		for(++it; it != end; ++it) {
			if((*it)->getRoot() != root)
				return false;
		}
	}

	return true;
}

QueryPlan *PathsQP::copy(MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	PathsQP *result = new (mm) PathsQP(mm);

	result->paths_.insert(result->paths_.begin(), paths_.begin(), paths_.end());

	return result;
}

string PathsQP::toString(bool brief) const
{
	ostringstream s;

	s << "p(";

	bool addComma = false;
	for(Paths::const_iterator it = paths_.begin(); it != paths_.end(); ++it) {
		if(addComma) { s << ","; } else { addComma = true; }
		s << "\"" << (*it)->getStepName() << "\"";
	}

	s << ")";
	return s.str();
}

QueryPlan *PathsQP::compress()
{
	set<ImpliedSchemaNode*> uniquePaths;
	for(Paths::iterator it = paths_.begin(); it != paths_.end(); ++it) {
		uniquePaths.insert(*it);
	}
	paths_.clear();
	std::copy(uniquePaths.begin(), uniquePaths.end(), back_inserter(paths_));

	return dissolve();
}

QueryPlan *PathsQP::dissolve()
{
	if(paths_.empty()) return 0;
	return this;
}

QueryPlan *PathsQP::filter(const ImpliedSchemaNode *root)
{
	set<ImpliedSchemaNode*> uniquePaths;
	for(Paths::iterator it = paths_.begin(); it != paths_.end(); ++it) {
		if((*it)->getRoot() == root) uniquePaths.insert(*it);
	}
	paths_.clear();
	std::copy(uniquePaths.begin(), uniquePaths.end(), back_inserter(paths_));

	return dissolve();
}

QueryPlan *PathsQP::resolvePaths()
{
	UnionQP *unionOp = new (memMgr_) UnionQP(memMgr_);
	for(Paths::iterator it = paths_.begin(); it != paths_.end(); ++it) {
		unionOp->addArg(StepQP::createStep(*it, memMgr_));
	}
	return unionOp->compress();
}

/////////////////////////////////
// StepQP

QueryPlan *StepQP::compress()
{
	return this;
}

QueryPlan *StepQP::dissolve()
{
	return this;
}

QueryPlan *StepQP::filter(const ImpliedSchemaNode *root)
{
	return this;
}

QueryPlan *StepQP::resolvePaths()
{
	return this;
}

QueryPlan *StepQP::removeSubsets(const Log &log)
{
	return this;
}

QueryPlan *StepQP::resolveValues(const Log &log, DynamicContext *context, bool lookupValues, bool &success)
{
	return this;
}

QueryPlan *StepQP::resolveSteps(const Container &container, const IndexSpecification &is, bool nodeQP)
{
	return this;
}

QueryPlan *StepQP::createStep(const ImpliedSchemaNode *child, MemoryManager *mm)
{
	switch(child->getType()) {
	case ImpliedSchemaNode::ROOT:
	case ImpliedSchemaNode::DESCENDANT:
	case ImpliedSchemaNode::ATTRIBUTE:
	case ImpliedSchemaNode::CHILD:
	case ImpliedSchemaNode::METADATA: {
		char *parentName, *childName;
		if(getStepNames(child, parentName, childName, mm)) {
			return new (mm) UniverseQP(mm);
		}

		ImpliedSchemaNode::Type type = ImpliedSchemaNode::CHILD;
		switch(child->getType()) {
		case ImpliedSchemaNode::ATTRIBUTE: type = ImpliedSchemaNode::ATTRIBUTE; break;
		case ImpliedSchemaNode::METADATA: type = ImpliedSchemaNode::METADATA; break;
		default: break;
		}

		return new (mm) PresenceQP(type, parentName, childName, mm);
		break;
	}
	case ImpliedSchemaNode::LTX:
	case ImpliedSchemaNode::LTE:
	case ImpliedSchemaNode::GTX:
	case ImpliedSchemaNode::GTE:
	case ImpliedSchemaNode::EQUALS:
	case ImpliedSchemaNode::PREFIX:
	case ImpliedSchemaNode::SUBSTRING: {
		DbWrapper::Operation op;
		switch(child->getType()) {
		case ImpliedSchemaNode::LTX: op = DbWrapper::LTX; break;
		case ImpliedSchemaNode::LTE: op = DbWrapper::LTE; break;
		case ImpliedSchemaNode::GTX: op = DbWrapper::GTX; break;
		case ImpliedSchemaNode::GTE: op = DbWrapper::GTE; break;
		case ImpliedSchemaNode::EQUALS: op = DbWrapper::EQUALITY; break;
		case ImpliedSchemaNode::PREFIX: op = DbWrapper::PREFIX; break;
		case ImpliedSchemaNode::SUBSTRING: op = DbWrapper::SUBSTRING; break;
		default: break;
		}

		const ImpliedSchemaNode *parent = child->getParent();

		char *parentName, *childName;
		if(getStepNames(parent, parentName, childName, mm)) {
			return new (mm) UniverseQP(mm);
		}

		ImpliedSchemaNode::Type type = ImpliedSchemaNode::CHILD;
		switch(parent->getType()) {
		case ImpliedSchemaNode::ATTRIBUTE: type = ImpliedSchemaNode::ATTRIBUTE; break;
		case ImpliedSchemaNode::METADATA: type = ImpliedSchemaNode::METADATA; break;
		default: break;
		}

		return new (mm) ValueQP(type, parentName, childName, op, child->getValue(), mm);
		break;
	}
	}

	return 0;
}

/*
 * parent        child         result
 * -------------------------------------------------------
 *
 * CHILD(x)      CHILD(y)       =  P(x.y)
 * DESCENDANT(x) CHILD(y)       =  P(x.y)
 * ROOT          CHILD(x)       =  P(#.x)
 * ...           CHILD(x)       =  P(x)
 * ...           CHILD(*)       =  U
 *
 * CHILD(x)      ATTRIBUTE(y)   =  P(x.y)
 * DESCENDANT(x) ATTRIBUTE(y)   =  P(x.y)
 * ROOT          ATTRIBUTE(x)   =  P(#.x)
 * ...           ATTRIBUTE(x)   =  P(x)
 * ...           ATTRIBUTE(*)   =  U
 *
 * ...           DESCENDANT(x)  =  P(x)
 * ...           DESCENDANT(*)  =  U
 *
 *               ROOT           =  U
 *
 * Returns true if the step should be a U
 */
bool StepQP::getStepNames(const ImpliedSchemaNode *child, char *&parentName, char *&childName, MemoryManager *mm)
{
	const ImpliedSchemaNode *parent = child->getParent();

	parentName = 0;
	childName = 0;

	if(child->isWildcard()) {
		return true;
	}

	switch(child->getType()) {
	case ImpliedSchemaNode::ROOT: {
		return true;
	}
	case ImpliedSchemaNode::METADATA:
	case ImpliedSchemaNode::DESCENDANT: {
		childName = char_clone(child->getUriName().c_str(), mm);
		break;
	}
	case ImpliedSchemaNode::ATTRIBUTE:
	case ImpliedSchemaNode::CHILD: {
		childName = char_clone(child->getUriName().c_str(), mm);

		if(parent->getType() == ImpliedSchemaNode::ROOT) {
			parentName = char_clone(Name::dbxml_colon_root.getURIName().c_str(), mm);
		}
		else if(!parent->isWildcard()) {
			parentName = char_clone(parent->getUriName().c_str(), mm);
		}

		break;
	}
	default: break;
	}

	return false;
}

void StepQP::logIndexUse(const Log &log, const Index &index, const DbWrapper::Operation op) const
{
	static const unsigned int MAX_NAME_LENGTH = 80;

	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_DEBUG)) {
		string qp_name = shorten(toString(), MAX_NAME_LENGTH);

		ostringstream oss;

		oss << qp_name << " using: ";
		oss << index.asString() << ",";
		oss << DbWrapper::operationToString(op);

		logLegend(log);
		log.log(Log::C_OPTIMIZER, Log::L_DEBUG, oss);
	}
}

void StepQP::logTransformation(const Log &log, const QueryPlan *transformed) const
{
	static const unsigned int MAX_NAME_LENGTH = 80;

	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_DEBUG)) {
		string start_name = shorten(toString(), MAX_NAME_LENGTH);
		string end_name = shorten(transformed->toString(), MAX_NAME_LENGTH);

		ostringstream oss;
		oss << start_name << " -> " << end_name;

		logLegend(log);
		log.log(Log::C_OPTIMIZER, Log::L_DEBUG, oss);
	}
}

/////////////////////////////////
// PresenceQP

PresenceQP::PresenceQP(ImpliedSchemaNode::Type type, const char *parent, const char *child, MemoryManager *mm)
	: StepQP(QueryPlan::PRESENCE, mm),
	  nodeType_(type),
	  parentUriName_(parent),
	  childUriName_(child),
	  key_(),
	  operation_(DbWrapper::NONE)
{
}

PresenceQP::PresenceQP(QueryPlan::Type qpType, ImpliedSchemaNode::Type type,
		       const char *parent, const char *child,
		       const QPKey &key, DbWrapper::Operation operation,
		       MemoryManager *mm)
	: StepQP(qpType, mm),
	  nodeType_(type),
	  parentUriName_(parent),
	  childUriName_(child),
	  key_(key),
	  operation_(operation)
{
}

IDS::SharedPtr PresenceQP::execute(OperationContext &context, QueryExecutionContext &qec) const
{
	DBXML_ASSERT(key_.getIndex() != 0); // We assert that resolveSteps has already been called

	Key key = key_.createKey();
	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		key.setIDsFromNames(context, qec.getContainer(), parentUriName_, childUriName_);
		const_cast<QPKey&>(key_).setID1(key.getID1());
		const_cast<QPKey&>(key_).setID2(key.getID2());
	}

	IDS::SharedPtr ids;
	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		ids.reset(new IDS(0));
	}
	else {
		ids = qec.getIndexReadCache().getIDS(qec.getContainer(), context, operation_, key);
	}
	logIDs(qec, ids);
	return ids;
}

IndexData::SharedPtr PresenceQP::nodes_execute(OperationContext &context, QueryExecutionContext &qec) const
{
	DBXML_ASSERT(key_.getIndex() != 0); // We assert that resolveSteps has already been called

	Key key = key_.createKey();
	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		key.setIDsFromNames(context, qec.getContainer(), parentUriName_, childUriName_);
		const_cast<QPKey&>(key_).setID1(key.getID1());
		const_cast<QPKey&>(key_).setID2(key.getID2());
	}

	IndexData::SharedPtr data;
	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		data.reset(new IndexData());
	}
	else {
		data = qec.getIndexReadCache().getIndexData(qec.getContainer(), context, operation_, key);
	}
	logIndexData(qec, data);
	return data;
}

QueryPlan::Cost PresenceQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	DBXML_ASSERT(key_.getIndex() != 0); // We assert that resolveSteps has already been called

	// usable_page_size= (pagesize-overhead)*page_fill_factor
	//
	// DB4.1 has btree page overhead of 26. Fill factor for a full page
	// is between 50% and 100%... let's guess 75%.
	//
	const Container &container = qec.getContainer();
	double pageSize = (container.getDocumentDB()->getPageSize() - 26) * 0.75;

	Key key = key_.createKey();
	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		key.setIDsFromNames(context, qec.getContainer(), parentUriName_, childUriName_);
		const_cast<QPKey&>(key_).setID1(key.getID1());
		const_cast<QPKey&>(key_).setID2(key.getID2());
	}

	const KeyStatistics &statistics = qec.getStatisticsReadCache().getKeyStatistics(qec.getContainer(), context, key);

	//
	// (number_of_keys * (average_key_value_pair_size+pair_overhead) / usable_page_size
	//
	// DB4.1 has btree key value pair overhead of 5 bytes per item, so 10 bytes for a pair.
	//
	Cost result;
	result.keys = statistics.numIndexedKeys_;
	if(!qec.getIndexReadCache().containsIndexData(qec.getContainer(), operation_, key)) {
		result.pages = ceil((statistics.numIndexedKeys_ * (statistics.averageKeyValueSize() + 10)) / pageSize);
	}

	logCost(qec, result, &statistics);
	return result;
}

inline Index::Type nodeTypeToIndexType(ImpliedSchemaNode::Type t) {
	switch(t) {
	case ImpliedSchemaNode::ATTRIBUTE: return Index::NODE_ATTRIBUTE;
	case ImpliedSchemaNode::METADATA: return Index::NODE_METADATA;
	case ImpliedSchemaNode::CHILD: return Index::NODE_ELEMENT;
	default: break;
	}
	return (Index::Type)-1;
}

// Order of preference for indexes
//
// edge-*-presence-none (equality)
// edge-*-equality-* (prefix)
// else convert to n(node-*-presence-*, node-*-presence) and try again
// ---- or ----
// node-*-presence-none (equality)
// edge-*-presence-none (prefix)
// node-*-equality-* (prefix)
// edge-*-equality-* (prefix)
//
QueryPlan *PresenceQP::resolveSteps(const Container &container, const IndexSpecification &is, bool nodeQP)
{
	const IndexVector *iv = is.getIndexOrDefault(childUriName_);

	if(iv) {
		Index &index = key_.getIndex();
		index.set(nodeTypeToIndexType(nodeType_), Index::NODE_MASK);

		if(isParentSet()) {
			// edge-*-presence-none (equality)
			index.set(Index::PATH_EDGE | Index::KEY_PRESENCE | Index::SYNTAX_NONE, Index::PKS_MASK);
			operation_ = DbWrapper::EQUALITY;
			if(iv->isEnabled(index, Index::PNKS_MASK)) {
				logIndexUse(container, key_.getIndex(), operation_);
				return this;
			}

			// edge-*-equality-* (prefix)
			index.set(Index::KEY_EQUALITY, Index::KEY_MASK);
			operation_ = DbWrapper::PREFIX;
			Index prefix_index = iv->getIndex(index, Index::PNK_MASK);
			if(prefix_index != 0) {
				index = prefix_index;
				logIndexUse(container, key_.getIndex(), operation_);
				return this;
			}

			// Don't use substring indexes for presence lookups,
			// as they are too slow.
		}
		else { // !isParentSet()

			// node-*-presence-none (equality)
			index.set(Index::PATH_NODE | Index::KEY_PRESENCE | Index::SYNTAX_NONE, Index::PKS_MASK);
			operation_ = DbWrapper::EQUALITY;
			if(iv->isEnabled(index, Index::PNKS_MASK)) {
				return rootPresenceCheck(container, is, nodeQP);
			}

			// edge-*-presence-none (prefix)
			index.set(Index::PATH_EDGE | Index::KEY_PRESENCE | Index::SYNTAX_NONE, Index::PKS_MASK);
			operation_ = DbWrapper::PREFIX;
			if(iv->isEnabled(index, Index::PNKS_MASK)) {
				key_.setNodeLookup(true);
				return rootPresenceCheck(container, is, nodeQP);
			}

			// node-*-equality-* (prefix)
			index.set(Index::PATH_NODE | Index::KEY_EQUALITY, Index::PK_MASK);
			Index prefix_index = iv->getIndex(index, Index::PNK_MASK);
			if(prefix_index != 0) {
				index = prefix_index;
				return rootPresenceCheck(container, is, nodeQP);
			}

			// edge-*-equality-* (prefix)
			index.set(Index::PATH_EDGE | Index::KEY_EQUALITY, Index::PK_MASK);
			prefix_index = iv->getIndex(index, Index::PNK_MASK);
			if(prefix_index != 0) {
				index = prefix_index;
				key_.setNodeLookup(true);
				return rootPresenceCheck(container, is, nodeQP);
			}

			// Don't use substring indexes for presence lookups,
			// as they are too slow.
		}
	}

	if(isParentSet()) {
		if(nodeQP) {
			// else convert to node-*-presence-* and try again
			PresenceQP *pres = new (memMgr_) PresenceQP(nodeType_, 0, childUriName_, memMgr_);

			logTransformation(container, pres);
			return pres->resolveSteps(container, is, nodeQP);
		} else {
			// else convert to n(node-*-presence-*, node-*-presence) and try again
			IntersectQP *intersectOp = new (memMgr_) IntersectQP(memMgr_);
			intersectOp->addArg(new (memMgr_) PresenceQP(ImpliedSchemaNode::CHILD, 0, parentUriName_, memMgr_));
			intersectOp->addArg(new (memMgr_) PresenceQP(nodeType_, 0, childUriName_, memMgr_));

			logTransformation(container, intersectOp);
			return intersectOp->resolveSteps(container, is, nodeQP);
		}
	}
	else {
		UniverseQP *result = new (memMgr_) UniverseQP(memMgr_);
		logTransformation(container, result);
		return result->resolveSteps(container, is, nodeQP);
	}
}

QueryPlan *PresenceQP::rootPresenceCheck(const Container &container, const IndexSpecification &is, bool nodeQP)
{
	if(!isParentSet() && char_equals(childUriName_, metaDataName_uri_root)) {
		// A lookup on dbxml:root resolves to U
		UniverseQP *result = new (memMgr_) UniverseQP(memMgr_);
		logTransformation(container, result);
		return result->resolveSteps(container, is, nodeQP);
	}

	logIndexUse(container, key_.getIndex(), operation_);
	return this;
}

bool PresenceQP::isSubsetOf(const QueryPlan *o) const
{
	if(o->getType() == QueryPlan::PRESENCE) {
		const PresenceQP *step = (const PresenceQP*)o;

		if(char_equals(step->childUriName_, childUriName_) &&
		   step->nodeType_ == nodeType_) {

			// x-y-presence <isSubsetOf> x-y-presence
			if(isParentSet() && step->isParentSet() &&
			   char_equals(parentUriName_, step->parentUriName_)) {
				return true;
			}

			// *-x-presence <isSubsetOf> node-x-presence
			if(!step->isParentSet()) {
				return true;
			}
		}

		// P(a.b) <isSubsetOf> P(a)
		if(step->nodeType_ == ImpliedSchemaNode::CHILD &&
		   isParentSet() && !step->isParentSet() &&
		   char_equals(step->childUriName_, parentUriName_)) {
			return true;
		}
	}
	else if(o->getType() == QueryPlan::UNION ||
		o->getType() == QueryPlan::INTERSECT) {
		return ((const OperationQP*)o)->isSupersetOf(this);
	}
	else if(o->getType() == QueryPlan::UNIVERSE) {
		// *-*-presence <isSubsetOf> Universe
		return true;
	}

	return false;
}

QueryPlan *PresenceQP::copy(MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	PresenceQP *result = new (mm) PresenceQP(QueryPlan::PRESENCE, nodeType_,
						 parentUriName_, childUriName_,
						 key_, operation_, mm);
	return result;
}

string PresenceQP::toString(bool brief) const
{
	ostringstream s;

	s << "P(";

	if(!brief && operation_ != DbWrapper::NONE) {
		s << key_.getIndex().asString() << ",";
		s << DbWrapper::operationToString(operation_) << ",";
	}

	if(parentUriName_ != 0) {
		s << parentUriName_ << ".";
	}

	if(nodeType_ == ImpliedSchemaNode::ATTRIBUTE) {
		s << "@";
	}
	else if(nodeType_ == ImpliedSchemaNode::METADATA) {
		s << "metadata::";
	}
	else if(nodeType_ == ImpliedSchemaNode::DESCENDANT) {
		s << "descendant::";
	}

	if(childUriName_ != 0) {
		s << childUriName_ << ")";
	}
	else {
		s << "null)";
	}

	return s.str();
}

/////////////////////////////////
// ValueQP

ValueQP::ValueQP(ImpliedSchemaNode::Type type,
		 const char *parent, const char *child,
		 DbWrapper::Operation operation, const DataItem *value,
		 MemoryManager *mm)
	: PresenceQP(QueryPlan::VALUE, type, parent, child, QPKey(), operation, mm),
	  value_(value)
{
}

ValueQP::ValueQP(ImpliedSchemaNode::Type type, const char *parent, const char *child, Syntax::Type syntax,
		DbWrapper::Operation operation, const char *value, size_t vlen, MemoryManager *mm)
	: PresenceQP(QueryPlan::VALUE, type, parent, child, QPKey(syntax), operation, mm),
	  value_(syntax, value, vlen)
{
}

ValueQP::ValueQP(QueryPlan::Type qpType, ImpliedSchemaNode::Type type,
		 const char *parent, const char *child,
		 const QPKey &key, DbWrapper::Operation operation,
		 const QPValue &value, MemoryManager *mm)
	: PresenceQP(qpType, type, parent, child, key, operation, mm),
	  value_(value)
{
}

IDS::SharedPtr ValueQP::execute(OperationContext &context, QueryExecutionContext &qec) const
{
	DBXML_ASSERT(value_.getDataItem() == 0); // assert that resolveValues has already been called
	DBXML_ASSERT(key_.getIndex() != 0); // We assert that resolveSteps has already been called

	Key key = key_.createKey(value_);
	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		key.setIDsFromNames(context, qec.getContainer(), parentUriName_, childUriName_);
		const_cast<QPKey&>(key_).setID1(key.getID1());
		const_cast<QPKey&>(key_).setID2(key.getID2());
	}

	IDS::SharedPtr ids;
	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		ids.reset(new IDS(0));
	}
	else {
		ids = qec.getIndexReadCache().getIDS(qec.getContainer(), context, operation_, key);
	}
	logIDs(qec, ids);
	return ids;
}

IndexData::SharedPtr ValueQP::nodes_execute(OperationContext &context, QueryExecutionContext &qec) const
{
	DBXML_ASSERT(value_.getDataItem() == 0); // assert that resolveValues has already been called
	DBXML_ASSERT(key_.getIndex() != 0); // We assert that resolveSteps has already been called

	Key key = key_.createKey(value_);
	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		key.setIDsFromNames(context, qec.getContainer(), parentUriName_, childUriName_);
		const_cast<QPKey&>(key_).setID1(key.getID1());
		const_cast<QPKey&>(key_).setID2(key.getID2());
	}

	IndexData::SharedPtr data;
	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		data.reset(new IndexData());
	}
	else {
		data = qec.getIndexReadCache().getIndexData(qec.getContainer(), context, operation_, key);
	}
	logIndexData(qec, data);
	return data;
}

QueryPlan::Cost ValueQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	DBXML_ASSERT(value_.getDataItem() == 0); // assert that resolveValues has already been called
	DBXML_ASSERT(key_.getIndex() != 0); // We assert that resolveSteps has already been called

	// usable_page_size= (pagesize-overhead)*page_fill_factor
	//
	// DB4.1 has btree page overhead of 26. Fill factor for a full page
	// is between 50% and 100%... let's guess 75%.
	//
	const Container &container = qec.getContainer();
	double pageSize = (container.getDocumentDB()->getPageSize() - 26) * 0.75;

	Key key = key_.createKey(value_);
	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		key.setIDsFromNames(context, qec.getContainer(), parentUriName_, childUriName_);
		const_cast<QPKey&>(key_).setID1(key.getID1());
		const_cast<QPKey&>(key_).setID2(key.getID2());
	}

	const KeyStatistics &statistics = qec.getStatisticsReadCache().getKeyStatistics(qec.getContainer(), context, key);

	Cost result;
	// Let's just assume that the distribution of keys across
	// the domain of possible values is flat. There are probably
	// more clever things we could do.
	//
	switch (operation_) {
	case DbWrapper::EQUALITY: {
		if(key.getIndex().getKey() == Index::KEY_EQUALITY) {
			// jcm - could use key_range... but is this more accurate?
			// (number of equality keys / number of unique keys)
			result.keys =
				(statistics.numUniqueKeys_ > 0
				 ? (double)statistics.numIndexedKeys_ / (double)statistics.numUniqueKeys_
				 : 0);
		}
		else {
			result.keys = statistics.numIndexedKeys_;
		}
		break;
	}
	case DbWrapper::PREFIX: {
		if (key.getValueSize() == 0) {
			// Doing a prefix search on just the structure.
			result.keys = statistics.numIndexedKeys_;
			// BREAK!
			break;
		} else {
			// Doing a prefix search on the structure, and the start of the value.
			// FALL THROUGH!
		}
	}
	case DbWrapper::LTX:
	case DbWrapper::LTE:
	case DbWrapper::GTX:
	case DbWrapper::GTE: {
		double percentageOfKeys = qec.getStatisticsReadCache().getPercentage(qec.getContainer(), context, operation_, key);
		result.keys = (double)statistics.numIndexedKeys_ * percentageOfKeys;
		break;
	}
	default: {  // keep compilers quiet
		break;
	}
	}
	//
	// (number_of_keys * (average_key_value_pair_size+pair_overhead) / usable_page_size
	//
	// DB4.1 has btree key value pair overhead of 5 bytes per item, so 10 bytes for a pair.
	//
	if(!qec.getIndexReadCache().containsIndexData(qec.getContainer(), operation_, key)) {
		result.pages = ceil((result.keys * (statistics.averageKeyValueSize() + 10)) / pageSize);
	}

	logCost(qec, result, &statistics);
	return result;
}

// Order of preference for indexes
//
// edge-*-equality-* (if not substring)
// edge-*-substring-* (if equality, prefix or substring)
// else convert to n(node-*-equality-*, edge-*-presence) and try again
// ---- or ----
// node-*-equality-* (if not substring)
// node-*-substring-* (if equality, prefix or substring)
// else convert to node-*-presence and try again
QueryPlan *ValueQP::resolveSteps(const Container &container, const IndexSpecification &is, bool nodeQP)
{
	const IndexVector *iv = is.getIndexOrDefault(childUriName_);

	if(iv) {
		Index::Type searchMask = Index::PNKS_MASK;
		if(value_.getDataItem() != 0) {
			// We don't know the syntax yet - so we won't include it
			// in our search for an index. If we find indexes that
			// potentially match, we won't convert ourselves.
			searchMask = Index::PNK_MASK;
		}

		Index &index = key_.getIndex();
		index.set(nodeTypeToIndexType(nodeType_), Index::NODE_MASK);

		if(isParentSet()) {
			if(operation_ != DbWrapper::SUBSTRING) {
				// edge-*-equality-* (if not substring)
				index.set(Index::PATH_EDGE | Index::KEY_EQUALITY, Index::PK_MASK);
				if(iv->isEnabled(index, searchMask)) {
					if(searchMask == Index::PNKS_MASK)
						logIndexUse(container, key_.getIndex(), operation_);
					return this;
				}
			}

			if(operation_ == DbWrapper::EQUALITY ||
			   operation_ == DbWrapper::PREFIX ||
			   operation_ == DbWrapper::SUBSTRING) {
				// edge-*-substring-* (if equality, prefix or substring)
				index.set(Index::PATH_EDGE | Index::KEY_SUBSTRING, Index::PK_MASK);
				if(iv->isEnabled(index, searchMask)) {
					if(searchMask == Index::PNKS_MASK)
						return splitSubstring(container, is, nodeQP);
					return this;
				}
			}

		}
		else { // !isParentSet()

			if(operation_ != DbWrapper::SUBSTRING) {
				// node-*-equality-* (if not substring)
				index.set(Index::PATH_NODE | Index::KEY_EQUALITY, Index::PK_MASK);
				if(iv->isEnabled(index, searchMask)) {
					if(searchMask == Index::PNKS_MASK)
						logIndexUse(container, key_.getIndex(), operation_);
					return this;
				}
			}

			if(operation_ == DbWrapper::EQUALITY ||
			   operation_ == DbWrapper::PREFIX ||
			   operation_ == DbWrapper::SUBSTRING) {
				// node-*-substring-* (if equality, prefix or substring)
				index.set(Index::PATH_NODE | Index::KEY_SUBSTRING, Index::PK_MASK);
				if(iv->isEnabled(index, searchMask)) {
					if(searchMask == Index::PNKS_MASK)
						return splitSubstring(container, is, nodeQP);
					return this;
				}
			}

			// else convert to node-*-presence and try again
			PresenceQP *result = new (memMgr_) PresenceQP(nodeType_, parentUriName_, childUriName_, memMgr_);
			logTransformation(container, result);
			return result->resolveSteps(container, is, nodeQP);
		}
	}

	if(isParentSet()) {
		if(nodeQP) {
			// else convert to node-*-equality-* and try again
			ValueQP *val = new (memMgr_) ValueQP(QueryPlan::VALUE, nodeType_, 0, childUriName_,
				key_, operation_, value_, memMgr_);

			logTransformation(container, val);
			return val->resolveSteps(container, is, nodeQP);
		} else {
			// else convert to n(node-*-equality-*, edge-*-presence) and try again
			IntersectQP *intersectOp = new (memMgr_) IntersectQP(memMgr_);
			intersectOp->addArg(new (memMgr_) PresenceQP(nodeType_, parentUriName_, childUriName_, memMgr_));
			intersectOp->addArg(new (memMgr_) ValueQP(QueryPlan::VALUE, nodeType_, 0, childUriName_,
						    key_, operation_, value_, memMgr_));

			logTransformation(container, intersectOp);
			return intersectOp->resolveSteps(container, is, nodeQP);
		}
	}
	else {
		UniverseQP *result = new (memMgr_) UniverseQP(memMgr_);
		logTransformation(container, result);
		return result->resolveSteps(container, is, nodeQP);
	}
}

QueryPlan *ValueQP::splitSubstring(const Container &container, const IndexSpecification &is, bool nodeQP)
{
	// Split the value using the KeyGenerator from the syntax
	const Syntax *syntax = SyntaxManager::getInstance()->getSyntax((Syntax::Type)key_.getIndex().getSyntax());
	KeyGenerator::Ptr kg = syntax->getKeyGenerator(key_.getIndex(), value_.getValue(), value_.getLength());

	if(kg->noOfKeys() == 0) {
		// return (edge|node)-*-presence to cover cases where
		// no substring keys are generated
		PresenceQP *result = new (memMgr_) PresenceQP(nodeType_, parentUriName_, childUriName_, memMgr_);

		logTransformation(container, result);
		return result->resolveSteps(container, is, nodeQP);
	}
	else {
		IntersectQP *intersectOp = new (memMgr_) IntersectQP(memMgr_);

		const char *keyBuffer = 0;
		size_t keyLength = 0;
		while(kg->next(keyBuffer, keyLength)) {
			ValueQP *value = new (memMgr_) ValueQP(QueryPlan::VALUE, nodeType_,
							       parentUriName_, childUriName_,
							       key_, DbWrapper::EQUALITY,
							       QPValue(value_.getSyntax(),
								       char_clone(keyBuffer, keyLength, memMgr_),
								       keyLength),
							       memMgr_);
			intersectOp->addArg(value);
		}

		logTransformation(container, intersectOp);
		return intersectOp;
	}
}

QueryPlan *ValueQP::resolveValues(const Log &log, DynamicContext *context, bool lookupValues, bool &success)
{
	const DataItem *values = value_.getDataItem();
	if(values == 0) return this;

	if(values->isConstant() || lookupValues) {
		static const SequenceType stringType(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
						     SchemaSymbols::fgDT_STRING,
						     SequenceType::QUESTION_MARK);

		Result constants = values->collapseTree(context);
		try {
			if(nodeType_ == ImpliedSchemaNode::SUBSTRING) {
				// SUBSTRING comes from a function, so convert the argument
				// like a function does
				constants = constants.convertFunctionArg(&stringType,
					values->getStaticResolutionContext().getStaticType(), context);
			}
			else {
				// Other operations come from operators, so convert the argument
				// like an operator does
				if(values->getStaticResolutionContext().getStaticType().flags &
					StaticType::NODE_TYPE) {
					constants = constants.atomize(context);
				}
			}
		}
		catch(...) {
			// could throw - in which case we generate a presence lookup
			PresenceQP *result = new (memMgr_) PresenceQP(nodeType_, parentUriName_, childUriName_, memMgr_);
			logTransformation(log, result);
			return result;
		}

		UnionQP *unionOp = new (memMgr_) UnionQP(memMgr_);

		Item::Ptr item;
		while((item = constants.next(context)) != NULLRCP) {
			DBXML_ASSERT(item->isAtomicValue()); // Because we've called atomize()

			XMLChToUTF8 transcoded(item->asString(context));
			int len = char_len(transcoded.str());
			char *value = char_clone(transcoded.str(), len, memMgr_);

			unionOp->addArg(new (memMgr_) ValueQP(nodeType_, parentUriName_, childUriName_,
							      Value::getSyntaxType(item),
							      operation_, value, len, memMgr_));
		}

		logTransformation(log, unionOp);
		return unionOp;
	}
	else {
		success = false;
		return this;
	}
}

static bool operationSubset(DbWrapper::Operation l, DbWrapper::Operation r)
{
	if(l == r) return true;

	switch(l) {
	case DbWrapper::LTX: {
		if(r == DbWrapper::LTE) return true;
		break;
	}
	case DbWrapper::GTX: {
		if(r == DbWrapper::GTE) return true;
		break;
	}
	case DbWrapper::PREFIX: {
		if(r == DbWrapper::SUBSTRING) return true;
		break;
	}
	case DbWrapper::EQUALITY: {
		return true;
	}
	default: break;
	}
	return false;
}

bool ValueQP::isSubsetOfValue(const PresenceQP *step, DbWrapper::Operation myOp, DbWrapper::Operation hisOp) const
{
	if(char_equals(step->getChildName(), childUriName_) &&
	   step->getNodeType() == nodeType_) {

		if(hisOp == myOp) {

			// x-y-(equality|substring) <isSubsetOf> x-y-(equality|substring)
			if(isParentSet() && step->isParentSet() &&
			   char_equals(parentUriName_, step->getParentName())) {
				return true;
			}

			// *-x-(equality|substring) <isSubsetOf> node-x-(equality|substring)
			if(!step->isParentSet()) {
				return true;
			}
		}

		// x-y-(equality|substring) (op1) <isSubsetOf> x-y-(equality|substring) (op2) iff operationSubset(op1, op2)
		if(isParentSet() == step->isParentSet() &&
		   char_equals(parentUriName_, step->getParentName()) &&
		   operationSubset(myOp, hisOp)) {
			return true;
		}
	}

	// V(a.b) <isSubsetOf> V(a)
	if(step->getNodeType() == ImpliedSchemaNode::CHILD &&
	   isParentSet() && !step->isParentSet() &&
	   char_equals(step->getChildName(), parentUriName_)) {
		return true;
	}

	return false;
}

bool ValueQP::isSubsetOf(const QueryPlan *o) const
{
	if(o->getType() == QueryPlan::VALUE) {
		const ValueQP *step = (const ValueQP*)o;

		if(value_.equals(step->value_) &&
		   isSubsetOfValue(step, operation_, step->operation_)) {
			return true;
		}
	}
	else if(o->getType() == QueryPlan::RANGE) {
		const RangeQP *step = (const RangeQP*)o;

		if(value_.equals(step->getValue()) &&
		   isSubsetOfValue(step, operation_, step->getOperation()) &&
		   value_.equals(step->getValue2()) &&
		   isSubsetOfValue(step, operation_, step->getOperation2())) {
			return true;
		}
	}
	else {
		return PresenceQP::isSubsetOf(o);
	}

	return false;
}

QueryPlan *ValueQP::copy(MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	ValueQP *result = new (mm) ValueQP(QueryPlan::VALUE, nodeType_,
					   parentUriName_, childUriName_,
					   key_, operation_, value_, mm);
	return result;
}

string ValueQP::toString(bool brief) const
{
	ostringstream s;

	s << "V(";

	if(!brief && value_.getDataItem() == 0 && key_.getIndex() != 0) {
		s << key_.getIndex().asString() << ",";
	}

	if(isParentSet()) {
		s << parentUriName_ << ".";
	}

	if(nodeType_ == ImpliedSchemaNode::ATTRIBUTE) {
		s << "@";
	}
	else if(nodeType_ == ImpliedSchemaNode::METADATA) {
		s << "metadata::";
	}
	else if(nodeType_ == ImpliedSchemaNode::DESCENDANT) {
		s << "descendant::";
	}

	s << childUriName_ << "," << DbWrapper::operationToString(operation_);
	s << ",'" << value_.asString() << "')";

	return s.str();
}

/////////////////////////////////
// RangeQP

RangeQP::RangeQP(ImpliedSchemaNode::Type type,
		 const char *parent, const char *child,
		 const QPKey &key1, DbWrapper::Operation operation1,
		 const QPValue &value1, DbWrapper::Operation operation2,
		 const QPValue &value2, MemoryManager *mm)
	: ValueQP(QueryPlan::RANGE, type, parent, child, key1, operation1, value1, mm),
	  value2_(value2),
	  operation2_(operation2)
{
}

RangeQP::RangeQP(const ValueQP *gt, const ValueQP *lt, MemoryManager *mm)
	: ValueQP(QueryPlan::RANGE, gt->getNodeType(), gt->getParentName(), gt->getChildName(),
		  gt->getKey(), gt->getOperation(), gt->getValue(), mm),
	  value2_(lt->getValue()),
	  operation2_(lt->getOperation())
{
}

IDS::SharedPtr RangeQP::execute(OperationContext &context, QueryExecutionContext &qec) const
{
	DBXML_ASSERT(key_.getIndex() != 0); // We assert that resolveSteps has already been called

	Key key1 = key_.createKey(value_);
	if(key1.getID1() == 0 || (isParentSet() && key1.getID2() == 0)) {
		key1.setIDsFromNames(context, qec.getContainer(), parentUriName_, childUriName_);
		const_cast<QPKey&>(key_).setID1(key1.getID1());
		const_cast<QPKey&>(key_).setID2(key1.getID2());
	}

	IDS::SharedPtr ids;
	if(key1.getID1() == 0 || (isParentSet() && key1.getID2() == 0)) {
		ids.reset(new IDS(0));
	}
	else {
		Key key2 = key_.createKey(value2_);
		ids = qec.getIndexReadCache().getIDS(qec.getContainer(), context, operation_, key1, operation2_, key2);
	}
	logIDs(qec, ids);
	return ids;
}

IndexData::SharedPtr RangeQP::nodes_execute(OperationContext &context, QueryExecutionContext &qec) const
{
	DBXML_ASSERT(key_.getIndex() != 0); // We assert that resolveSteps has already been called

	Key key1 = key_.createKey(value_);
	if(key1.getID1() == 0 || (isParentSet() && key1.getID2() == 0)) {
		key1.setIDsFromNames(context, qec.getContainer(), parentUriName_, childUriName_);
		const_cast<QPKey&>(key_).setID1(key1.getID1());
		const_cast<QPKey&>(key_).setID2(key1.getID2());
	}

	IndexData::SharedPtr data;
	if(key1.getID1() == 0 || (isParentSet() && key1.getID2() == 0)) {
		data.reset(new IndexData());
	}
	else {
		Key key2 = key_.createKey(value2_);
		data = qec.getIndexReadCache().getIndexData(qec.getContainer(), context, operation_, key1, operation2_, key2);
	}
	logIndexData(qec, data);
	return data;
}

QueryPlan::Cost RangeQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	DBXML_ASSERT(key_.getIndex() != 0); // We assert that resolveSteps has already been called

	// usable_page_size= (pagesize-overhead)*page_fill_factor
	//
	// DB4.1 has btree page overhead of 26. Fill factor for a full page
	// is between 50% and 100%... let's guess 75%.
	//
	const Container &container = qec.getContainer();
	double pageSize = (container.getDocumentDB()->getPageSize() - 26) * 0.75;

	Key key = key_.createKey(value_);
	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		key.setIDsFromNames(context, qec.getContainer(), parentUriName_, childUriName_);
		const_cast<QPKey&>(key_).setID1(key.getID1());
		const_cast<QPKey&>(key_).setID2(key.getID2());
	}

	const KeyStatistics &statistics = qec.getStatisticsReadCache().getKeyStatistics(qec.getContainer(), context, key);

	Key key2 = key_.createKey(value2_);

	Cost result;
	double percentageOfKeys = qec.getStatisticsReadCache().getPercentage(qec.getContainer(), context, operation_, key, operation2_, key2);
	result.keys = (double)statistics.numIndexedKeys_ * percentageOfKeys;

	//
	// (number_of_keys * (average_key_value_pair_size+pair_overhead) / usable_page_size
	//
	// DB4.1 has btree key value pair overhead of 5 bytes per item, so 10 bytes for a pair.
	//
	if(!qec.getIndexReadCache().containsIndexData(qec.getContainer(), operation_, key, operation2_, key2)) {
		result.pages = ceil((result.keys * (statistics.averageKeyValueSize() + 10)) / pageSize);
	}

	logCost(qec, result, &statistics);
	return result;
}

// Order of preference for indexes
//
// edge-*-equality-*
// else convert to n(node-*-equality-*, edge-*-presence) and try again
// ---- or ----
// node-*-equality-*
// else convert to node-*-presence and try again
QueryPlan *RangeQP::resolveSteps(const Container &container, const IndexSpecification &is, bool nodeQP)
{
	const IndexVector *iv = is.getIndexOrDefault(childUriName_);

	if(iv) {
		Index &index = key_.getIndex();
		index.set(nodeTypeToIndexType(nodeType_), Index::NODE_MASK);

		if(isParentSet()) {
			// edge-*-equality-*
			index.set(Index::PATH_EDGE | Index::KEY_EQUALITY, Index::PK_MASK);
			if(iv->isEnabled(index, Index::PNKS_MASK)) {
				logIndexUse(container, key_.getIndex(), operation_);
				logIndexUse(container, key_.getIndex(), operation2_);
				return this;
			}

		}
		else { // !isParentSet()

			// node-*-equality-*
			index.set(Index::PATH_NODE | Index::KEY_EQUALITY, Index::PK_MASK);
			if(iv->isEnabled(index, Index::PNKS_MASK)) {
				logIndexUse(container, key_.getIndex(), operation_);
				logIndexUse(container, key_.getIndex(), operation2_);
				return this;
			}

			// else convert to node-*-presence and try again
			PresenceQP *result = new (memMgr_) PresenceQP(nodeType_, parentUriName_, childUriName_, memMgr_);
			logTransformation(container, result);
			return result->resolveSteps(container, is, nodeQP);
		}
	}

	if(isParentSet()) {
		// else convert to n(node-*-equality-*, edge-*-presence) and try again
		IntersectQP *intersectOp = new (memMgr_) IntersectQP(memMgr_);
		intersectOp->addArg(new (memMgr_) PresenceQP(nodeType_, parentUriName_, childUriName_, memMgr_));
		intersectOp->addArg(new (memMgr_) RangeQP(nodeType_, 0, childUriName_, key_, operation_,
							  value_, operation2_, value2_, memMgr_));

		logTransformation(container, intersectOp);
		return intersectOp->resolveSteps(container, is, nodeQP);
	}
	else {
		UniverseQP *result = new (memMgr_) UniverseQP(memMgr_);
		logTransformation(container, result);
		return result->resolveSteps(container, is, nodeQP);
	}
}

bool RangeQP::isSubsetOf(const QueryPlan *o) const
{
	// Behaves like the intersection of two ValueQP objects
	// (which is what it is, really)

	if(o->getType() == QueryPlan::VALUE) {
		const ValueQP *step = (const ValueQP*)o;

		if(value_.equals(step->getValue()) &&
		   isSubsetOfValue(step, operation_, step->getOperation())) {
			return true;
		}
		if(key_.getIndex().getSyntax() == step->getKey().getIndex().getSyntax() &&
		   step->getValue().equals(value2_) &&
		   isSubsetOfValue(step, operation2_, step->getOperation())) {
			return true;
		}
	}
	else if(o->getType() == QueryPlan::RANGE) {
		const RangeQP *step = (const RangeQP*)o;

		if(value_.equals(step->getValue()) &&
		   isSubsetOfValue(step, operation_, step->getOperation()) &&
		   value2_.equals(step->getValue2()) &&
		   isSubsetOfValue(step, operation2_, step->getOperation2())) {
			return true;
		}
		if(key_.getIndex().getSyntax() == step->getKey().getIndex().getSyntax() &&
		   value2_.equals(step->getValue()) &&
		   isSubsetOfValue(step, operation2_, step->getOperation()) &&
		   value_.equals(step->getValue2()) &&
		   isSubsetOfValue(step, operation_, step->getOperation2())) {
			return true;
		}
	}
	else {
		return ValueQP::isSubsetOf(o);
	}

	return false;
}

QueryPlan *RangeQP::copy(MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	RangeQP *result = new (mm) RangeQP(nodeType_, parentUriName_, childUriName_, key_,
					   operation_, value_, operation2_, value2_, mm);
	return result;
}

string RangeQP::toString(bool brief) const
{
	ostringstream s;

	s << "R(";

	if(!brief && key_.getIndex() != 0) {
		s << key_.getIndex().asString() << ",";
	}

	if(isParentSet()) {
		s << parentUriName_ << ".";
	}

	if(nodeType_ == ImpliedSchemaNode::ATTRIBUTE) {
		s << "@";
	}
	else if(nodeType_ == ImpliedSchemaNode::METADATA) {
		s << "metadata::";
	}
	else if(nodeType_ == ImpliedSchemaNode::DESCENDANT) {
		s << "descendant::";
	}

	s << childUriName_ << ",";
	s << DbWrapper::operationToString(operation_) << ",'" << value_.asString() << "',";
	s << DbWrapper::operationToString(operation2_) << ",'" << value2_.asString() << "')";

	return s.str();
}

/////////////////////////////////
// DocumentQP

DocumentQP::DocumentQP(const std::string documentName, QueryPlan *qp, MemoryManager *mm)
	: ValueQP(QueryPlan::DOCUMENT, ImpliedSchemaNode::METADATA, 0, DbXml::metaDataName_uri_name,
		  QPKey(Syntax::STRING), DbWrapper::EQUALITY,
		  QPValue(Syntax::STRING, (documentName.empty() ? 0 :
					   char_clone(documentName.c_str(), mm)),
			  documentName.length()),
		  mm),
	  qp_(qp)
{
	if(documentName.empty()) {
		operation_ = DbWrapper::PREFIX;
	}
}

DocumentQP::DocumentQP(const QPKey &key, const QPValue &value, QueryPlan *qp, MemoryManager *mm)
	: ValueQP(QueryPlan::DOCUMENT, ImpliedSchemaNode::METADATA, 0, DbXml::metaDataName_uri_name,
		  key, DbWrapper::EQUALITY, value, mm),
	  qp_(qp)
{
	if(value.getValue() == 0) {
		operation_ = DbWrapper::PREFIX;
	}
}

IDS::SharedPtr DocumentQP::execute(OperationContext &context, QueryExecutionContext &qec) const
{
	IDS::SharedPtr result = ValueQP::execute(context, qec);

	if(qp_ != 0) {
		Cost cost = qp_->cost(context, qec);
		if(cost.pages < qec.costToFilter(result)) {
			result = result->set_intersection(qp_->execute(context, qec));
			logIDs(qec, result);
		}
	}

	return result;
}

IndexData::SharedPtr DocumentQP::nodes_execute(OperationContext &context, QueryExecutionContext &qec) const
{
	IndexData::SharedPtr result(new IndexData);
	result->set_union(ValueQP::nodes_execute(context, qec));

	if(qp_ != 0) {
		Cost cost = qp_->cost(context, qec);
		if(cost.pages < qec.costToFilter(result)) {
			result->set_intersection(qp_->nodes_execute(context, qec));
			logIndexData(qec, result);
		}
	}

	return result;
}

QueryPlan::Cost DocumentQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	Cost result = ValueQP::cost(context, qec);

	if(qp_ != 0) {
		Cost childcost = qp_->cost(context, qec);
		result.pages += childcost.pages;
		if(childcost.keys < result.keys) result.keys = childcost.keys;
	}

	logCost(qec, result, 0);
	return result;
}

QueryPlan *DocumentQP::copy(MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	DocumentQP *result = new (mm) DocumentQP(key_, value_, (qp_ != 0 ? qp_->copy(mm) : 0), mm);
	return result;
}

string DocumentQP::toString(bool brief) const
{
	ostringstream s;

	s << "D(";

	if(!brief && key_.getIndex() != 0) {
		s << key_.getIndex().asString() << ",";
		if(value_.getValue() == 0) {
			s << DbWrapper::operationToString(operation_) << "," << childUriName_ << ",";
		}
		else {
			s << childUriName_ << "," << DbWrapper::operationToString(operation_) << ",";
		}
	}

	if(value_.getValue() != 0) {
		s << "'" << value_.asString() << "'";
	}
	if(qp_ != 0) {
		s << "," << qp_->toString(brief);
	}
	s << ")";

	return s.str();
}

QueryPlan *DocumentQP::compress()
{
	qp_ = qp_->compress();
	return this;
}

QueryPlan *DocumentQP::dissolve()
{
	if(qp_ != 0) qp_ = qp_->dissolve();
	return this;
}

QueryPlan *DocumentQP::filter(const ImpliedSchemaNode *root)
{
	if(qp_ != 0) qp_ = qp_->filter(root);
	return this;
}

QueryPlan *DocumentQP::resolvePaths()
{
	if(qp_ != 0) qp_ = qp_->resolvePaths();
	return this;
}

QueryPlan *DocumentQP::removeSubsets(const Log &log)
{
	if(qp_ != 0) qp_ = qp_->removeSubsets(log);
	return this;
}

QueryPlan *DocumentQP::resolveValues(const Log &log, DynamicContext *context, bool lookupValues, bool &success)
{
	if(qp_ != 0) qp_ = qp_->resolveValues(log, context, lookupValues, success);
	return this;
}

QueryPlan *DocumentQP::resolveSteps(const Container &container, const IndexSpecification &is, bool nodeQP)
{

	// The metadata-equality-string index on dbxml:name
	// is mandatory, so there's no need to check if it's there.
	Index &index = key_.getIndex();
	index.set(Index::NME | Syntax::STRING);

	if(qp_ != 0) qp_ = qp_->resolveSteps(container, is, nodeQP);
	return this;
}

/////////////////////////////////
// UniverseQP

UniverseQP::UniverseQP(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm)
	: PresenceQP(QueryPlan::UNIVERSE, ImpliedSchemaNode::METADATA, 0,
		DbXml::metaDataName_uri_name, QPKey(Syntax::STRING),
		DbWrapper::PREFIX, mm)
{
}

IndexData::SharedPtr UniverseQP::nodes_execute(OperationContext &context, QueryExecutionContext &qec) const
{
	// You shouldn't execute a universe query plan in nodes mode
	DBXML_ASSERT(false);
	return IndexData::SharedPtr(new IndexData());
}

bool UniverseQP::isSubsetOf(const QueryPlan *o) const
{
	if(o->getType() == QueryPlan::UNIVERSE) {
		return true;
	}
	return false;
}

QueryPlan *UniverseQP::copy(MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	return new (mm) UniverseQP(mm);
}

string UniverseQP::toString(bool brief) const
{
	ostringstream s;

	s << "U";

	return s.str();
}

/////////////////////////////////
// EmptyQP

IDS::SharedPtr EmptyQP::execute(OperationContext &context, QueryExecutionContext &qec) const
{
	IDS::SharedPtr result(new IDS(0));
	logIDs(qec, result);
	return result;
}

IndexData::SharedPtr EmptyQP::nodes_execute(OperationContext &context, QueryExecutionContext &qec) const
{
	IndexData::SharedPtr result(new IndexData());
	logIndexData(qec, result);
	return result;
}

bool EmptyQP::isSubsetOf(const QueryPlan *o) const
{
	return true;
}

QueryPlan *EmptyQP::copy(MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	return new (mm) EmptyQP(mm);
}

string EmptyQP::toString(bool brief) const
{
	ostringstream s;

	s << "E";

	return s.str();
}

/////////////////////////////////
// QPKey

QPKey::QPKey()
	: nodeLookup_(false),
	  index_(Index::NONE),
	  id1_(0),
	  id2_(0)
{
}

QPKey::QPKey(Syntax::Type syntax)
	: nodeLookup_(false),
	  index_(Index::NONE),
	  id1_(0),
	  id2_(0)
{
	index_.set(syntax, Index::SYNTAX_MASK);
}

Key QPKey::createKey() const
{
	Key result;
	result.setNodeLookup(nodeLookup_);
	result.setIndex(index_);
	result.setID1(id1_);
	result.setID2(id2_);
	return result;
}

Key QPKey::createKey(const QPValue &value) const
{
	Key result;
	result.setNodeLookup(nodeLookup_);
	result.setIndex(index_);
	result.setID1(id1_);
	result.setID2(id2_);
	result.setValue(value.getValue(), value.getLength());
	return result;
}

/////////////////////////////////
// QPValue

QPValue::QPValue(Syntax::Type syntax, const char *value, size_t vlen)
	: syntax_(syntax),
	  value_(value),
	  len_(vlen),
	  di_(0)
{
}

QPValue::QPValue(const DataItem *di)
	: syntax_(Syntax::NONE),
	  value_(0),
	  len_(0),
	  di_(di)
{
}

bool QPValue::equals(const QPValue &o) const
{
	if(di_ != 0) return di_ == o.di_;
	else return syntax_ == o.syntax_ &&
		     char_equals(value_, len_, o.value_, o.len_);
}
