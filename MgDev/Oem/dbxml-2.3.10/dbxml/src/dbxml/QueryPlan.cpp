//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryPlan.cpp,v 1.200.2.1 2007/02/05 15:46:20 jsnelson Exp $
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
#include "Manager.hpp"
#include "QueryExecutionContext.hpp"
#include "PrintDataItemTree.hpp"
#include "dataItem/DbXmlContext.hpp"
#include <dbxml/XmlNamespace.hpp>

#include <xqilla/exceptions/XPath2TypeMatchException.hpp>

#include <math.h> // for ceil()

using namespace DbXml;
using namespace std;

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

static const int INDENT = 1;

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
// QueryPlan::Cost

void QueryPlan::Cost::unionOp(const Cost &cost)
{
	pages += cost.pages;
	keys += cost.keys;
}

void QueryPlan::Cost::intersectOp(const Cost &cost)
{
	pages += cost.pages;
	if(cost.keys < keys) keys = cost.keys;
}

int QueryPlan::Cost::compare(const Cost &cost)
{
	if(keys < cost.keys) return -1;
	if(keys > cost.keys) return +1;

	if(pages < cost.pages) return -1;
	if(pages > cost.pages) return +1;

	return 0;
}

/////////////////////////////////
// QueryPlan

QueryPlan *QueryPlan::createRawQueryPlan(
	const Log &log, DbXmlContext *context) const
{
	QueryPlan *result = copy(context->getMemoryManager());

	if(result != 0) result = result->resolvePaths();

	if(result != 0) result = result->resolveValues(log, context);

	if(result != 0) result = result->removeSubsets(log);

	if(result == 0) {
		// return UniverseQP
		result = new (context->getMemoryManager()) UniverseQP(context->getMemoryManager());
	}

	logQP(log, "RQP", result);
	return result;
}

QueryPlan *QueryPlan::createPartiallyOptimisedQueryPlan(
	Transaction *txn, const Container &container,
	DbXmlContext *context, bool nodeQP,
	bool &fullyOptimised, bool &exact) const
{
	QueryPlan *result = copy(context->getMemoryManager());

	if(result != 0) result = result->resolvePaths();

	if(result != 0) result = result->resolveValues(container, context);

	logQP(container, "RQP", result);

	IndexSpecification is;
	container.getConfigurationDB()->getIndexSpecification(txn, is);

	fullyOptimised = true;
	exact = true;
	if(result != 0) {
		result = result->resolveIndexes(container, is, nodeQP,
			/*success*/fullyOptimised, exact);
	}

	if(result != 0) result = result->removeSubsets(container);

	if(result == 0) {
		// If qp_ is null, this lookup's results do not
		// appear in the result. Therefore we generate
		// a UniverseQP as a query plan.
		result = new (context->getMemoryManager())
			UniverseQP(context->getMemoryManager());
		fullyOptimised = true;
		exact = true;
		result = result->resolveIndexes(container, is, nodeQP,
			/*success*/fullyOptimised, exact);
	}

	if(fullyOptimised) logQP(container, "OQP", result);
	else logQP(container, "POQP", result);

	return result;
}

QueryPlan *QueryPlan::createExecutableQueryPlan(
	Transaction *txn, const Container &container,
	DbXmlContext *context, bool nodeQP, bool &exact) const
{
	QueryPlan *result = copy(context->getMemoryManager());

	result = result->resolveSyntax(container, context);

	IndexSpecification is;
	container.getConfigurationDB()->getIndexSpecification(txn, is);

	bool success = true;
	exact = true;
	result = result->resolveIndexes(container, is, nodeQP, success, exact);
	if(result != 0) result = result->removeSubsets(container);

	if(result == 0) {
		// If qp_ is null, this lookup's results do not
		// appear in the result. Therefore we generate
		// a UniverseQP as a query plan.
		result = new (context->getMemoryManager())
			UniverseQP(context->getMemoryManager());
		exact = true;
		result = result->resolveIndexes(container, is, nodeQP, success, exact);
	}

	logQP(container, "OQP", result);
	return result;
}

QueryPlan *QueryPlan::createUniverseQueryPlan(
	Transaction *txn, const Container &container,
	DbXmlContext *context)
{
	IndexSpecification is;
	container.getConfigurationDB()->getIndexSpecification(txn, is);

	bool success = true;
	bool exact = true;
	QueryPlan *result = new (context->getMemoryManager())
		UniverseQP(context->getMemoryManager());
	result = result->resolveIndexes(container, is, /*nodeQP*/false, success, exact);

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
			<< "\t nd   - DocIDs Intersection\n"
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

void QueryPlan::logIDs(QueryExecutionContext &qec, const IDS::Ptr &ids) const
{
	static const unsigned int MAX_NAME_LENGTH = 80;

	if (Log::isLogEnabled(Log::C_QUERY, Log::L_INFO)) {
		string name = toString();
		string shortName = shorten(name, MAX_NAME_LENGTH);

		ostringstream oss;
		oss << shortName << " : ";
		if (!ids || ids->size() == 0) {
			oss << "NONE";
		} else {
			oss << "[" << ids->size() << "] ";
			int n = 0;
			IDS::const_iterator p;
			for (p = ids->begin();n < 20 && p != ids->end();
			     ++p, ++n) {
				oss << (*p).asString() << " ";
			}
			if (p != ids->end()) {
				oss << "...";
			}
		}

		logLegend(qec.getContainer());
		qec.getContainer().log(Log::C_QUERY, Log::L_INFO, oss);
	}
}

void QueryPlan::logIndexData(QueryExecutionContext &qec, const IndexData::Ptr &data) const
{
	static const unsigned int MAX_NAME_LENGTH = 80;

	if (Log::isLogEnabled(Log::C_QUERY, Log::L_INFO)) {
		string name = toString();
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

QueryPlan *OperationQP::rootFilter(const ImpliedSchemaNode *root)
{
	// Remove duplicate arguments
	UniqueArgs uniqueArgs;
	for(Vector::iterator it = args_.begin(); it != args_.end(); ++it) {
		uniqueArgs.add(type_, (*it)->rootFilter(root));
	}
	args_.clear();
	std::copy(uniqueArgs.begin(), uniqueArgs.end(), back_inserter(args_));

	return dissolve();
}

QueryPlan *OperationQP::nodeFilter(const ImpliedSchemaNode::Vector &nodes)
{
	// Remove duplicate arguments
	UniqueArgs uniqueArgs;
	for(Vector::iterator it = args_.begin(); it != args_.end(); ++it) {
		uniqueArgs.add(type_, (*it)->nodeFilter(nodes));
	}
	args_.clear();
	std::copy(uniqueArgs.begin(), uniqueArgs.end(), back_inserter(args_));

	return dissolve();
}

void OperationQP::findQueryPlanHolders(QPHSet &qphset) const
{
	for(Vector::const_iterator it = args_.begin(); it != args_.end(); ++it) {
		(*it)->findQueryPlanHolders(qphset);
	}
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

QueryPlan *OperationQP::resolveValues(const Log &log, DbXmlContext *context)
{
	UniqueArgs uniqueArgs;
	for(Vector::iterator it = args_.begin(); it != args_.end(); ++it) {
		uniqueArgs.add(type_, (*it)->resolveValues(log, context));
	}
	args_.clear();
	std::copy(uniqueArgs.begin(), uniqueArgs.end(), back_inserter(args_));

	return dissolve();
}

QueryPlan *OperationQP::resolveSyntax(const Log &log, DbXmlContext *context)
{
	UniqueArgs uniqueArgs;
	for(Vector::iterator it = args_.begin(); it != args_.end(); ++it) {
		uniqueArgs.add(type_, (*it)->resolveSyntax(log, context));
	}
	args_.clear();
	std::copy(uniqueArgs.begin(), uniqueArgs.end(), back_inserter(args_));

	return dissolve();
}

QueryPlan *OperationQP::resolveIndexes(const Container &container, const IndexSpecification &is, bool nodeQP,
	bool &success, bool &exact)
{
	UniqueArgs uniqueArgs;
	for(Vector::iterator it = args_.begin(); it != args_.end(); ++it) {
		uniqueArgs.add(type_, (*it)->resolveIndexes(container, is, nodeQP, success, exact));
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

UnionQP::UnionQP(QueryPlan *l, QueryPlan *r, XPath2MemoryManager *mm)
	: OperationQP(QueryPlan::UNION, mm)
{
	addArg(l);
	addArg(r);
}

QueryPlan *UnionQP::staticTyping(StaticContext *context)
{
	_src.clear();

	// Remove duplicate arguments
	UniqueArgs uniqueArgs;
	for(Vector::iterator it = args_.begin(); it != args_.end(); ++it) {
		QueryPlan *arg = (*it)->staticTyping(context);
		_src.add(arg->getStaticResolutionContext());
		_src.getStaticType().typeUnion(arg->getStaticResolutionContext().getStaticType());
		uniqueArgs.add(type_, arg);
	}
	args_.clear();
	std::copy(uniqueArgs.begin(), uniqueArgs.end(), back_inserter(args_));

	return dissolve();
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

IDS::Ptr UnionQP::execute(OperationContext &context, QueryExecutionContext &qec) const
{
	vector<QueryPlan*> sortedArgs(args_.begin(), args_.end());
	sort(sortedArgs.begin(), sortedArgs.end(), keys_compare_more(context, qec));

	IDS::Ptr result;
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

IndexData::Ptr UnionQP::nodes_execute(OperationContext &context, QueryExecutionContext &qec) const
{
	vector<QueryPlan*> sortedArgs(args_.begin(), args_.end());
	sort(sortedArgs.begin(), sortedArgs.end(), keys_compare_more(context, qec));

	IndexData::Ptr result(new IndexData);
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
			result.unionOp(childcost);
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

QueryPlan *UnionQP::removeNonConstant()
{
	UniqueArgs uniqueArgs;
	Vector::iterator it;
	for(it = args_.begin(); it != args_.end(); ++it) {
		QueryPlan *qp = (*it)->removeNonConstant();
		if(qp == 0) return 0;

		uniqueArgs.add(type_, qp);
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

QueryPlan *UnionQP::copy(XPath2MemoryManager *mm) const
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

string UnionQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<UnionQP>" << endl;
	for(Vector::const_iterator it2 = args_.begin(); it2 != args_.end(); ++it2) {
		s << (*it2)->printQueryPlan(context, indent + INDENT);
	}
	s << in <<  "</UnionQP>" << endl;

	return s.str();
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

IntersectQP::IntersectQP(QueryPlan *l, QueryPlan *r, XPath2MemoryManager *mm)
	: OperationQP(QueryPlan::INTERSECT, mm)
{
	addArg(l);
	addArg(r);
}

QueryPlan *IntersectQP::staticTyping(StaticContext *context)
{
	_src.clear();

	// Remove duplicate arguments
	UniqueArgs uniqueArgs;
	for(Vector::iterator it = args_.begin(); it != args_.end(); ++it) {
		QueryPlan *arg = (*it)->staticTyping(context);
		_src.add(arg->getStaticResolutionContext());
		_src.getStaticType().typeIntersect(arg->getStaticResolutionContext().getStaticType());
		uniqueArgs.add(type_, arg);
	}
	args_.clear();
	std::copy(uniqueArgs.begin(), uniqueArgs.end(), back_inserter(args_));

	return dissolve();
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

IDS::Ptr IntersectQP::execute(OperationContext &context, QueryExecutionContext &qec) const
{
	vector<QueryPlan*> sortedArgs(args_.begin(), args_.end());
	sort(sortedArgs.begin(), sortedArgs.end(), keys_compare_less(context, qec));

	IDS::Ptr result;
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

IndexData::Ptr IntersectQP::nodes_execute(OperationContext &context, QueryExecutionContext &qec) const
{
	vector<QueryPlan*> sortedArgs(args_.begin(), args_.end());
	sort(sortedArgs.begin(), sortedArgs.end(), keys_compare_less(context, qec));

	IndexData::Ptr result(new IndexData);
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
			result.intersectOp((*it)->cost(context, qec));
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

QueryPlan *IntersectQP::removeNonConstant()
{
	UniqueArgs uniqueArgs;
	Vector::iterator it;
	for(it = args_.begin(); it != args_.end(); ++it) {
		QueryPlan *qp = (*it)->removeNonConstant();
		if(qp != 0) uniqueArgs.add(type_, qp);
	}

	args_.clear();
	std::copy(uniqueArgs.begin(), uniqueArgs.end(), back_inserter(args_));

	return dissolve();
}

QueryPlan *IntersectQP::removeSupersetsOf(const QueryPlan *o, const Log &log)
{
	UniqueArgs uniqueArgs;
	Vector::iterator it;
	for(it = args_.begin(); it != args_.end(); ++it) {
		if(!o->isSubsetOf(*it)) {
			uniqueArgs.add(type_, *it);
		} else {
			logSubset(log, o, *it);
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
	   l->getValue().getASTNode() == 0 &&
	   r->getValue().getASTNode() == 0) {

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

QueryPlan *IntersectQP::copy(XPath2MemoryManager *mm) const
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

string IntersectQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<IntersectQP>" << endl;
	for(Vector::const_iterator it2 = args_.begin(); it2 != args_.end(); ++it2) {
		s << (*it2)->printQueryPlan(context, indent + INDENT);
	}
	s << in <<  "</IntersectQP>" << endl;

	return s.str();
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
// IntersectDocsQP

IntersectDocsQP::IntersectDocsQP(QueryPlan *l, QueryPlan *r, XPath2MemoryManager *mm)
	: OperationQP(QueryPlan::INTERSECT_DOCS, mm)
{
	args_.push_back(l);
	args_.push_back(r);
}

QueryPlan *IntersectDocsQP::addArg(QueryPlan *o)
{
	DBXML_ASSERT(false);

// 	if(o) {
// 		if(type_ == o->getType()) {
// 			OperationQP *op = (OperationQP*)o;
// 			args_.insert(args_.end(), op->args_.begin(), op->args_.end());
// 		}
// 		else {
// 			args_.push_back(o);
// 			return o;
// 		}
// 	}

	return this;
}

QueryPlan *IntersectDocsQP::staticTyping(StaticContext *context)
{
	_src.clear();

	args_[0] = args_[0]->staticTyping(context);
	_src.add(args_[0]->getStaticResolutionContext());

	args_[1] = args_[1]->staticTyping(context);
	_src.add(args_[1]->getStaticResolutionContext());
	_src.getStaticType() = args_[1]->getStaticResolutionContext().getStaticType();

	return dissolve();
}

IDS::Ptr IntersectDocsQP::execute(OperationContext &context, QueryExecutionContext &qec) const
{
	IDS::Ptr result = args_[0]->execute(context, qec);

	if(!result->empty()) {
		result = result->set_intersection(args_[1]->execute(context, qec));
	}

	logIDs(qec, result);
	return result;
}

IndexData::Ptr IntersectDocsQP::nodes_execute(OperationContext &context, QueryExecutionContext &qec) const
{
	IDS::Ptr docIDs = args_[0]->execute(context, qec);

	IndexData::Ptr result;
	if(!docIDs->empty()) {
		IndexData::Ptr nodes(args_[1]->nodes_execute(context, qec));
		result = new IndexData(*nodes);
		result->set_intersection(docIDs);
	}
	else {
		result = new IndexData;
	}

	logIndexData(qec, result);
	return result;
}

QueryPlan::Cost IntersectDocsQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	Cost result;
	Vector::const_iterator it = args_.begin();
	if(it != args_.end()) {
		result = (*it)->cost(context, qec);
		for(++it; it != args_.end(); ++it) {
			result.intersectOp((*it)->cost(context, qec));
		}
	}
	logCost(qec, result, 0);
	return result;
}

QueryPlan *IntersectDocsQP::removeSubsets(const Log &log)
{
	args_[0] = args_[0]->removeSubsets(log);
	args_[1] = args_[1]->removeSubsets(log);

	if(args_[1]->isSubsetOf(args_[0])) {
		return args_[1];
	}

	if(args_[0]->getType() == INTERSECT) {
		args_[0] = ((IntersectQP*)args_[0])->removeSupersetsOf(args_[1], log);
		if(args_[0] == 0) {
			return args_[1];
		}
	} else if(args_[0]->getType() == DOCUMENT) {
		DocumentQP *docqp = (DocumentQP*)args_[0];
		QueryPlan *arg = docqp->getArgument();

		if(arg != 0) {
			if(args_[1]->isSubsetOf(arg)) {
				arg = 0;
			} else if(arg->getType() == INTERSECT) {
				arg = ((IntersectQP*)arg)->removeSupersetsOf(args_[1], log);
			}

			docqp->setArgument(arg);
		}
	}

	return this;
}

QueryPlan *IntersectDocsQP::removeNonConstant()
{
	args_[0] = args_[0]->removeNonConstant();
	args_[1] = args_[1]->removeNonConstant();

	if(args_[0] == 0) return args_[1];
	if(args_[1] == 0) return 0;

	return this;
}

bool IntersectDocsQP::isSubsetOf(const QueryPlan *o) const
{
	return false;
}

bool IntersectDocsQP::isSupersetOf(const QueryPlan *o) const
{
	return false;
}

QueryPlan *IntersectDocsQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	IntersectDocsQP *result = new (mm) IntersectDocsQP(args_[0]->copy(mm), args_[1]->copy(mm), mm);

	return result;
}

string IntersectDocsQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<IntersectDocsQP>" << endl;
	for(Vector::const_iterator it2 = args_.begin(); it2 != args_.end(); ++it2) {
		s << (*it2)->printQueryPlan(context, indent + INDENT);
	}
	s << in <<  "</IntersectDocsQP>" << endl;

	return s.str();
}

string IntersectDocsQP::toString(bool brief) const
{
	ostringstream s;

	s << "nd(";

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

QueryPlan *PathsQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	PathsQP *result = new (mm) PathsQP(mm);

	result->paths_.insert(result->paths_.begin(), paths_.begin(), paths_.end());

	return result;
}

string PathsQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<PathsQP>";
	bool addComma = false;
	for(Paths::const_iterator it = paths_.begin(); it != paths_.end(); ++it) {
		if(addComma) { s << ","; } else { addComma = true; }
		s << (*it)->getStepName();
	}
	s << "</PathsQP>" << endl;

	return s.str();
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

QueryPlan *PathsQP::rootFilter(const ImpliedSchemaNode *root)
{
	set<ImpliedSchemaNode*> uniquePaths;
	for(Paths::iterator it = paths_.begin(); it != paths_.end(); ++it) {
		if((*it)->getRoot() == root) uniquePaths.insert(*it);
	}
	paths_.clear();
	std::copy(uniquePaths.begin(), uniquePaths.end(), back_inserter(paths_));

	return dissolve();
}

QueryPlan *PathsQP::nodeFilter(const ImpliedSchemaNode::Vector &nodes)
{
	set<ImpliedSchemaNode*> uniquePaths;
	for(Paths::iterator it = paths_.begin(); it != paths_.end(); ++it) {

		// Using the base node here won't work, because moving equality
		// checks to the prior step interferes with cardinality senstive
		// operations - jpcs
// 		ImpliedSchemaNode *baseNode = (*it)->getBaseNode();

		ImpliedSchemaNode *baseNode = *it;

		for(ImpliedSchemaNode::Vector::const_iterator it2 = nodes.begin();
		    it2 != nodes.end(); ++it2) {

			if((*it2) == baseNode) {
				uniquePaths.insert(*it);
				break;
			}
		}
	}
	paths_.clear();
	std::copy(uniquePaths.begin(), uniquePaths.end(), back_inserter(paths_));

	return dissolve();
}

void PathsQP::findQueryPlanHolders(QPHSet &qphset) const
{
	for(Paths::const_iterator it = paths_.begin(); it != paths_.end(); ++it) {
		qphset.insert((*it)->getRoot()->getQueryPlanHolder());
	}
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

QueryPlan *StepQP::rootFilter(const ImpliedSchemaNode *root)
{
	return this;
}

QueryPlan *StepQP::nodeFilter(const ImpliedSchemaNode::Vector &nodes)
{
	return this;
}

void StepQP::findQueryPlanHolders(QPHSet &qphset) const
{
}

QueryPlan *StepQP::resolvePaths()
{
	return this;
}

QueryPlan *StepQP::removeSubsets(const Log &log)
{
	return this;
}

QueryPlan *StepQP::removeNonConstant()
{
	return this;
}

QueryPlan *StepQP::resolveValues(const Log &log, DbXmlContext *context)
{
	return this;
}

QueryPlan *StepQP::resolveSyntax(const Log &log, DbXmlContext *context)
{
	return this;
}

QueryPlan *StepQP::resolveIndexes(const Container &container, const IndexSpecification &is, bool nodeQP,
	bool &success, bool &exact)
{
	return this;
}

QueryPlan *StepQP::createStep(const ImpliedSchemaNode *child, XPath2MemoryManager *mm)
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

		return new (mm) ValueQP(type, parentName, childName, op, child->getGeneralComp(),
			child->getSyntaxType(), child->getASTNode(), mm);
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
bool StepQP::getStepNames(const ImpliedSchemaNode *child, char *&parentName, char *&childName, XPath2MemoryManager *mm)
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
// IndexLookups

namespace DbXml {
class IndexLookups
{
public:
	IndexLookups(bool intersect = false)
		: intersect_(intersect), op_(DbWrapper::NONE), key_(0) {}

	void add(DbWrapper::Operation op, const Key &key)
	{
		values_.push_back(IndexLookups(op, key));
	}

	void add(const IndexLookups &okv)
	{
		values_.push_back(okv);
	}

	IDS::Ptr execute(const PresenceQP *pqp, OperationContext &context, QueryExecutionContext &qec)
	{
		IDS::Ptr ids;
		if(op_ != DbWrapper::NONE) {
			pqp->lookupIndex(op_, key_, ids, context, qec);
		}
		else {
			std::vector<IndexLookups>::iterator i = values_.begin();
			if(i != values_.end()) {
				ids = i->execute(pqp, context, qec);
				for(++i; i != values_.end(); ++i) {
					if(intersect_) {
						ids = ids->set_intersection(i->execute(pqp, context, qec));
					}
					else {
						ids = ids->set_union(i->execute(pqp, context, qec));
					}
				}
			}
		}
			
		if(!ids) ids = new IDS();
		return ids;
	}

	IndexData::Ptr nodes_execute(const PresenceQP *pqp, OperationContext &context, QueryExecutionContext &qec)
	{
		IndexData::Ptr data;
		if(op_ != DbWrapper::NONE) {
			pqp->lookupIndexNodes(op_, key_, data, context, qec);
		}
		else {
			std::vector<IndexLookups>::iterator i = values_.begin();
			if(i != values_.end()) {
				data = i->nodes_execute(pqp, context, qec);
				for(++i; i != values_.end(); ++i) {
					if(intersect_) {
						data->set_intersection(i->nodes_execute(pqp, context, qec));
					}
					else {
						data->set_union(i->nodes_execute(pqp, context, qec));
					}
				}
			}
		}

		if(!data) data = new IndexData();

		return data;
	}

	QueryPlan::Cost cost(const PresenceQP *pqp, OperationContext &context, QueryExecutionContext &qec)
	{
		QueryPlan::Cost result;
		if(op_ != DbWrapper::NONE) {
			result = pqp->lookupCost(op_, key_, context, qec);
		}
		else {
			std::vector<IndexLookups>::iterator i = values_.begin();
			if(i != values_.end()) {
				result = i->cost(pqp, context, qec);
				for(++i; i != values_.end(); ++i) {
					if(intersect_) {
						result.intersectOp(i->cost(pqp, context, qec));
					}
					else {
						result.unionOp(i->cost(pqp, context, qec));
					}
				}
			}
		}

		return result;
	}

private:
	IndexLookups(DbWrapper::Operation op, const Key &key)
		: intersect_(false), op_(op), key_(key) {}

	bool intersect_;
	std::vector<IndexLookups> values_;
	DbWrapper::Operation op_;
	Key key_;
};
}

/////////////////////////////////
// PresenceQP

PresenceQP::PresenceQP(ImpliedSchemaNode::Type type, const char *parent, const char *child, XPath2MemoryManager *mm)
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
		       XPath2MemoryManager *mm)
	: StepQP(qpType, mm),
	  nodeType_(type),
	  parentUriName_(parent),
	  childUriName_(child),
	  key_(key),
	  operation_(operation)
{
}

void PresenceQP::lookupIndex(DbWrapper::Operation operation, Key &key, IDS::Ptr &ids,
	OperationContext &context, QueryExecutionContext &qec) const
{
	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		key.setIDsFromNames(context, qec.getContainer(), parentUriName_, childUriName_);
		const_cast<QPKey&>(key_).setID1(key.getID1());
		const_cast<QPKey&>(key_).setID2(key.getID2());
	}

	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		ids.reset(new IDS(0));
	}
	else {
		ids = qec.getIndexReadCache().getIDS(qec.getContainer(), context, operation, key);
	}
}

void PresenceQP::lookupIndexNodes(DbWrapper::Operation operation, Key &key, IndexData::Ptr &data,
	OperationContext &context, QueryExecutionContext &qec) const
{
	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		key.setIDsFromNames(context, qec.getContainer(), parentUriName_, childUriName_);
		const_cast<QPKey&>(key_).setID1(key.getID1());
		const_cast<QPKey&>(key_).setID2(key.getID2());
	}

	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		data.reset(new IndexData());
	}
	else {
		data = qec.getIndexReadCache().getIndexData(qec.getContainer(), context, operation, key);
	}
}

QueryPlan::Cost PresenceQP::lookupCost(DbWrapper::Operation operation, Key &key,
	OperationContext &context, QueryExecutionContext &qec) const
{
	// usable_page_size= (pagesize-overhead)*page_fill_factor
	//
	// DB4.1 has btree page overhead of 26. Fill factor for a full page
	// is between 50% and 100%... let's guess 75%.
	//
	const Container &container = qec.getContainer();
	double pageSize = (container.getDocumentDB()->getPageSize() - 26) * 0.75;

	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		key.setIDsFromNames(context, qec.getContainer(), parentUriName_, childUriName_);
		const_cast<QPKey&>(key_).setID1(key.getID1());
		const_cast<QPKey&>(key_).setID2(key.getID2());
	}

	KeyStatistics statistics = qec.getStatisticsReadCache().getKeyStatistics(qec.getContainer(), context, key);

	Cost result;
	// Let's just assume that the distribution of keys across
	// the domain of possible values is flat. There are probably
	// more clever things we could do.
	//
	switch(operation) {
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
		double percentageOfKeys = qec.getStatisticsReadCache().getPercentage(qec.getContainer(), context, operation, key);
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
	if(!qec.getIndexReadCache().containsIndexData(qec.getContainer(), operation, key)) {
		result.pages = ceil((result.keys * (statistics.averageKeyValueSize() + 10)) / pageSize);
	}

	return result;
}

void PresenceQP::getKeys(IndexLookups &keys, DbXmlContext *context) const
{
	int timezone = ((Manager&)context->getQueryContext().getManager()).getImplicitTimezone();
	keys.add(operation_, key_.createKey(timezone));
}

void PresenceQP::getKeysForCost(IndexLookups &keys, DbXmlContext *context) const
{
	int timezone = ((Manager&)context->getQueryContext().getManager()).getImplicitTimezone();
	keys.add(operation_, key_.createKey(timezone));
}

QueryPlan *PresenceQP::staticTyping(StaticContext *context)
{
	_src.clear();

	if(nodeType_ == ImpliedSchemaNode::METADATA) {
		_src.getStaticType().flags = StaticType::DOCUMENT_TYPE;
	} else if(nodeType_ == ImpliedSchemaNode::ATTRIBUTE) {
		_src.getStaticType().flags = StaticType::ATTRIBUTE_TYPE;
	} else {
		_src.getStaticType().flags = StaticType::ELEMENT_TYPE;
	}

	return this;
}

IDS::Ptr PresenceQP::execute(OperationContext &context, QueryExecutionContext &qec) const
{
	DBXML_ASSERT(key_.getIndex() != 0); // We assert that resolveIndexes has already been called

	IndexLookups keys;
	getKeys(keys, qec.getDbXmlContext());

	IDS::Ptr ids = keys.execute(this, context, qec);

	logIDs(qec, ids);
	return ids;
}

IndexData::Ptr PresenceQP::nodes_execute(OperationContext &context, QueryExecutionContext &qec) const
{
	DBXML_ASSERT(key_.getIndex() != 0); // We assert that resolveIndexes has already been called

	IndexLookups keys;
	getKeys(keys, qec.getDbXmlContext());

	IndexData::Ptr data = keys.nodes_execute(this, context, qec);

	logIndexData(qec, data);
	return data;
}

QueryPlan::Cost PresenceQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	DBXML_ASSERT(key_.getIndex() != 0); // We assert that resolveIndexes has already been called

	IndexLookups keys;
	getKeysForCost(keys, qec.getDbXmlContext());

	Cost result = keys.cost(this, context, qec);

	logCost(qec, result, 0);
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
QueryPlan *PresenceQP::resolveIndexes(const Container &container, const IndexSpecification &is, bool nodeQP,
	bool &success, bool &exact)
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
				return rootPresenceCheck(container, is, nodeQP, success, exact);
			}

			// edge-*-presence-none (prefix)
			index.set(Index::PATH_EDGE | Index::KEY_PRESENCE | Index::SYNTAX_NONE, Index::PKS_MASK);
			operation_ = DbWrapper::PREFIX;
			if(iv->isEnabled(index, Index::PNKS_MASK)) {
				key_.setNodeLookup(true);
				return rootPresenceCheck(container, is, nodeQP, success, exact);
			}

			// node-*-equality-* (prefix)
			index.set(Index::PATH_NODE | Index::KEY_EQUALITY, Index::PK_MASK);
			Index prefix_index = iv->getIndex(index, Index::PNK_MASK);
			if(prefix_index != 0) {
				index = prefix_index;
				return rootPresenceCheck(container, is, nodeQP, success, exact);
			}

			// edge-*-equality-* (prefix)
			index.set(Index::PATH_EDGE | Index::KEY_EQUALITY, Index::PK_MASK);
			prefix_index = iv->getIndex(index, Index::PNK_MASK);
			if(prefix_index != 0) {
				index = prefix_index;
				key_.setNodeLookup(true);
				return rootPresenceCheck(container, is, nodeQP, success, exact);
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
			return pres->resolveIndexes(container, is, nodeQP, success, exact);
		} else {
			// else convert to n(node-*-presence-*, node-*-presence) and try again
			IntersectQP *intersectOp = new (memMgr_) IntersectQP(memMgr_);
			intersectOp->addArg(new (memMgr_) PresenceQP(ImpliedSchemaNode::CHILD, 0, parentUriName_, memMgr_));
			intersectOp->addArg(new (memMgr_) PresenceQP(nodeType_, 0, childUriName_, memMgr_));

			logTransformation(container, intersectOp);
			return intersectOp->resolveIndexes(container, is, nodeQP, success, exact);
		}
	}
	else {
		UniverseQP *result = new (memMgr_) UniverseQP(memMgr_);
		logTransformation(container, result);
		return result->resolveIndexes(container, is, nodeQP, success, exact);
	}
}

QueryPlan *PresenceQP::rootPresenceCheck(const Container &container, const IndexSpecification &is, bool nodeQP,
	bool &success, bool &exact)
{
	if(!isParentSet() && char_equals(childUriName_, metaDataName_uri_root)) {
		// A lookup on dbxml:root resolves to U
		UniverseQP *result = new (memMgr_) UniverseQP(memMgr_);
		logTransformation(container, result);
		return result->resolveIndexes(container, is, nodeQP, success, exact);
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

QueryPlan *PresenceQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	PresenceQP *result = new (mm) PresenceQP(QueryPlan::PRESENCE, nodeType_,
						 parentUriName_, childUriName_,
						 key_, operation_, mm);
	return result;
}

string PresenceQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<PresenceQP";
	if(key_.getIndex() != 0) {
		s << " index=\"" << key_.getIndex().asString() << "\"";
	}
	if(operation_ != DbWrapper::NONE) {
		s << " operation=\"" << DbWrapper::operationToWord(operation_) << "\"";
	}
	if(parentUriName_ != 0) {
		s << " parent=\"" << parentUriName_ << "\"";
	}
	if(childUriName_ != 0) {
		s << " child=\"" << childUriName_ << "\"";
	}
	s << "/>" << endl;

	return s.str();
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
	DbWrapper::Operation operation, bool generalComp,
	Syntax::Type syntax, const ASTNode *value,
	XPath2MemoryManager *mm)
	: PresenceQP(QueryPlan::VALUE, type, parent, child, QPKey(syntax), operation, mm),
	  value_(syntax, value, generalComp)
{
}

ValueQP::ValueQP(ImpliedSchemaNode::Type type,
	const char *parent, const char *child,
	Syntax::Type syntax, DbWrapper::Operation operation,
	bool generalComp, const char *value, size_t vlen,
	XPath2MemoryManager *mm)
	: PresenceQP(QueryPlan::VALUE, type, parent, child, QPKey(syntax), operation, mm),
	  value_(syntax, value, vlen, generalComp)
{
}

ValueQP::ValueQP(QueryPlan::Type qpType, ImpliedSchemaNode::Type type,
	const char *parent, const char *child,
	const QPKey &key, DbWrapper::Operation operation,
	const QPValue &value,
	XPath2MemoryManager *mm)
	: PresenceQP(qpType, type, parent, child, key, operation, mm),
	  value_(value)
{
}

QueryPlan *ValueQP::staticTyping(StaticContext *context)
{
	DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);

	_src.clear();

	if(value_.getASTNode() != 0) {
		AutoRunQueryPlansReset reset(dbContext);
		value_.setASTNode(const_cast<ASTNode*>(value_.getASTNode())->staticTyping(context));
		_src.add(value_.getASTNode()->getStaticResolutionContext());
	}

	if(nodeType_ == ImpliedSchemaNode::METADATA) {
		_src.getStaticType().flags = StaticType::DOCUMENT_TYPE;
	} else if(nodeType_ == ImpliedSchemaNode::ATTRIBUTE) {
		_src.getStaticType().flags = StaticType::ATTRIBUTE_TYPE;
	} else {
		_src.getStaticType().flags = StaticType::ELEMENT_TYPE;
	}

	return this;
}

QueryPlan *ValueQP::removeNonConstant()
{
	if(value_.getASTNode() != 0) {
		// convert to node-*-presence
		return new (memMgr_) PresenceQP(nodeType_, parentUriName_, childUriName_, memMgr_);
	}
	return this;
}

void ValueQP::getKeysForCost(IndexLookups &keys, DbXmlContext *context) const
{
	int timezone = ((Manager&)context->getQueryContext().getManager()).getImplicitTimezone();
	if(value_.getASTNode() != 0) {
		const Index &index = key_.getIndex();
		if(index.getKey() == Index::KEY_SUBSTRING) {
			// TBD How can we do this better, given that we don't want to lookup
			// the value? - jpcs

			// Guess that the substring lookup will result in 5 index retrievals
			IndexLookups intersect(/*intersect*/true);
			intersect.add(DbWrapper::EQUALITY, key_.createKey(timezone));
			intersect.add(DbWrapper::EQUALITY, key_.createKey(timezone));
			intersect.add(DbWrapper::EQUALITY, key_.createKey(timezone));
			intersect.add(DbWrapper::EQUALITY, key_.createKey(timezone));
			intersect.add(DbWrapper::EQUALITY, key_.createKey(timezone));

			keys.add(intersect);
		}
		else {
			keys.add(operation_, key_.createKey(timezone));
		}
	}
	else {
		getKeysImpl(keys, value_.getValue(), value_.getLength(), timezone);
	}
}

void ValueQP::getKeys(IndexLookups &keys, DbXmlContext *context) const
{
	int timezone = ((Manager&)context->getQueryContext().getManager()).getImplicitTimezone();
	if(value_.getASTNode() != 0) {

		AutoRunQueryPlansReset reset(context);
		Result result = value_.getASTNode()->collapseTree(context);

		Item::Ptr item = result->next(context);
		while(!item.isNull()) {
			if(item->isNode()) {
				XMLChToUTF8 transcoded(((const Node*)item.get())->dmStringValue(context));
				getKeysImpl(keys, transcoded.str(), transcoded.len(), timezone);
			}
			else {
				XMLChToUTF8 transcoded(item->asString(context));
				getKeysImpl(keys, transcoded.str(), transcoded.len(), timezone);
			}

			item = result->next(context);
			if(!value_.isGeneralComp() && !item.isNull()) {
				XQThrow3(XPath2TypeMatchException,X("ValueQP::getKeys"),
					X("A parameter of the operator is not a single atomic value [err:XPTY0004]"),
					value_.getASTNode());
			}
		}
	}
	else {
		getKeysImpl(keys, value_.getValue(), value_.getLength(), timezone);
	}
}

void ValueQP::getKeysImpl(IndexLookups &keys, const char *value, size_t length, int timezone) const
{
	const Index &index = key_.getIndex();
	if(index.getKey() == Index::KEY_SUBSTRING) {
		// Split the value using the KeyGenerator from the syntax
		const Syntax *syntax = SyntaxManager::getInstance()->getSyntax((Syntax::Type)index.getSyntax());
		KeyGenerator::Ptr kg = syntax->getKeyGenerator(index, value, length);

		if(kg->noOfKeys() == 0) {
			// do a presence lookup when no substring keys are generated

			// TBD maybe we shouldn't use the substring index to do
			// a presence lookup here - jpcs

			keys.add(DbWrapper::PREFIX, key_.createKey(timezone));
		}
		else {
			IndexLookups intersect(/*intersect*/true);
			const char *keyBuffer = 0;
			size_t keyLength = 0;
			while(kg->next(keyBuffer, keyLength)) {
				intersect.add(DbWrapper::EQUALITY, key_.createKey(keyBuffer, keyLength, timezone));
			}
			keys.add(intersect);
		}
	}
	else {
		keys.add(operation_, key_.createKey(value, length, timezone));
	}
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
QueryPlan *ValueQP::resolveIndexes(const Container &container, const IndexSpecification &is, bool nodeQP,
	bool &success, bool &exact)
{
	const IndexVector *iv = is.getIndexOrDefault(childUriName_);

	if(iv) {
		Index::Type searchMask = Index::PNKS_MASK;
		if(value_.getSyntax() == Syntax::NONE) {
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
					else success = false;
					return this;
				}
			}

			if(operation_ == DbWrapper::EQUALITY ||
			   operation_ == DbWrapper::PREFIX ||
			   operation_ == DbWrapper::SUBSTRING) {
				// edge-*-substring-* (if equality, prefix or substring)
				index.set(Index::PATH_EDGE | Index::KEY_SUBSTRING, Index::PK_MASK);
				if(iv->isEnabled(index, searchMask)) {
					if(searchMask == Index::PNKS_MASK) {
						exact = false;
						logIndexUse(container, key_.getIndex(), operation_);
					}
					else success = false;
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
					else success = false;
					return this;
				}
			}

			if(operation_ == DbWrapper::EQUALITY ||
			   operation_ == DbWrapper::PREFIX ||
			   operation_ == DbWrapper::SUBSTRING) {
				// node-*-substring-* (if equality, prefix or substring)
				index.set(Index::PATH_NODE | Index::KEY_SUBSTRING, Index::PK_MASK);
				if(iv->isEnabled(index, searchMask)) {
					if(searchMask == Index::PNKS_MASK) {
						exact = false;
						logIndexUse(container, key_.getIndex(), operation_);
					}
					else success = false;
					return this;
				}
			}

			if(nodeQP) {
				return 0;
			}
			else {
				// else convert to node-*-presence and try again
				PresenceQP *result = new (memMgr_) PresenceQP(nodeType_, parentUriName_, childUriName_, memMgr_);
				logTransformation(container, result);
				return result->resolveIndexes(container, is, nodeQP, success, exact);
			}
		}
	}

	if(isParentSet()) {
		if(nodeQP) {
			// else convert to node-*-equality-* and try again
			ValueQP *val = new (memMgr_) ValueQP(QueryPlan::VALUE, nodeType_, 0, childUriName_,
				key_, operation_, value_, memMgr_);

			logTransformation(container, val);
			return val->resolveIndexes(container, is, nodeQP, success, exact);
		} else {
			// else convert to n(node-*-equality-*, edge-*-presence) and try again
			IntersectQP *intersectOp = new (memMgr_) IntersectQP(memMgr_);
			intersectOp->addArg(new (memMgr_) PresenceQP(nodeType_, parentUriName_, childUriName_, memMgr_));
			intersectOp->addArg(new (memMgr_) ValueQP(QueryPlan::VALUE, nodeType_, 0, childUriName_,
						    key_, operation_, value_, memMgr_));

			logTransformation(container, intersectOp);
			return intersectOp->resolveIndexes(container, is, nodeQP, success, exact);
		}
	}
	else {
		UniverseQP *result = new (memMgr_) UniverseQP(memMgr_);
		logTransformation(container, result);
		return result->resolveIndexes(container, is, nodeQP, success, exact);
	}
}

QueryPlan *ValueQP::resolveFromResult(Result &result, const Log &log, DbXmlContext *context, const LocationInfo *location)
{
	Item::Ptr item = result->next(context);
	if(item.isNull()) {
		EmptyQP *empty = new (memMgr_) EmptyQP(memMgr_);
		logTransformation(log, empty);
		return empty;
	}

	UnionQP *unionOp = new (memMgr_) UnionQP(memMgr_);
	while(!item.isNull()) {
		Syntax::Type syntax = value_.getSyntax();
		int len;
		char *value;
		if(item->isNode()) {
			XMLChToUTF8 transcoded(((const Node*)item.get())->dmStringValue(context));
			len = transcoded.len();
			value = char_clone(transcoded.str(), len, memMgr_);
			if(syntax == Syntax::NONE) {
				syntax = Syntax::STRING;
			}
		}
		else {
			XMLChToUTF8 transcoded(item->asString(context));
			len = transcoded.len();
			value = char_clone(transcoded.str(), len, memMgr_);
			if(syntax == Syntax::NONE) {
				syntax = Value::getSyntaxType(item);
			}
		}

		unionOp->addArg(new (memMgr_) ValueQP(nodeType_, parentUriName_, childUriName_,
					syntax, operation_, value_.isGeneralComp(), value, len, memMgr_));

		item = result->next(context);
		if(!value_.isGeneralComp() && !item.isNull()) {
			XQThrow3(XPath2TypeMatchException,X("ValueQP::getKeys"),
				X("A parameter of the operator is not a single atomic value [err:XPTY0004]"),
				location);
			}
	}

	unionOp->compress();
	logTransformation(log, unionOp);
	return unionOp;
}

QueryPlan *ValueQP::resolveValues(const Log &log, DbXmlContext *context)
{
	AutoRunQueryPlansReset reset(context);

	bool success = true;
	Result result(0);

	const ASTNode *values = value_.getASTNode();
	if(values != 0 && values->isConstant()) {
		try {
			result = values->collapseTree(context);
		}
		catch(...) {
			success = false;
		}
	} else {
		success = false;
	}

	if(!success) return this;

	return resolveFromResult(result, log, context, value_.getASTNode());
}

QueryPlan *ValueQP::resolveSyntax(const Log &log, DbXmlContext *context)
{
	if(value_.getSyntax() != Syntax::NONE) return this;

	AutoRunQueryPlansReset reset(context);

	Result result(0);
	if(value_.getASTNode() != 0) {
		result = value_.getASTNode()->collapseTree(context);
	}

	return resolveFromResult(result, log, context, value_.getASTNode());
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

QueryPlan *ValueQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	ValueQP *result = new (mm) ValueQP(QueryPlan::VALUE, nodeType_,
					   parentUriName_, childUriName_,
					   key_, operation_, value_, mm);
	return result;
}

string ValueQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<ValueQP";
	if(key_.getIndex() != 0) {
		s << " index=\"" << key_.getIndex().asString() << "\"";
	}
	if(operation_ != DbWrapper::NONE) {
		s << " operation=\"" << DbWrapper::operationToWord(operation_) << "\"";
	}
	if(parentUriName_ != 0) {
		s << " parent=\"" << parentUriName_ << "\"";
	}
	if(childUriName_ != 0) {
		s << " child=\"" << childUriName_ << "\"";
	}

	if(value_.getASTNode() == 0) {
		s << " value=\"" << string(value_.getValue(), value_.getLength()) << "\"";
		s << "/>" << endl;
	} else {
		s << ">" << endl;
		s << DbXmlPrintXQTree::print(value_.getASTNode(), context, indent + INDENT);
		s << in << "</ValueQP>" << endl;
	}

	return s.str();
}

string ValueQP::toString(bool brief) const
{
	ostringstream s;

	s << "V(";

	if(!brief && value_.getSyntax() != Syntax::NONE && key_.getIndex() != 0) {
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
	const QPValue &value2, XPath2MemoryManager *mm)
	: ValueQP(QueryPlan::RANGE, type, parent, child, key1, operation1, value1, mm),
	  value2_(value2),
	  operation2_(operation2)
{
}

RangeQP::RangeQP(const ValueQP *gt, const ValueQP *lt, XPath2MemoryManager *mm)
	: ValueQP(QueryPlan::RANGE, gt->getNodeType(), gt->getParentName(), gt->getChildName(),
		  gt->getKey(), gt->getOperation(), gt->getValue(), mm),
	  value2_(lt->getValue()),
	  operation2_(lt->getOperation())
{
}

QueryPlan *RangeQP::staticTyping(StaticContext *context)
{
	DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);

	_src.clear();

	if(value_.getASTNode() != 0) {
		AutoRunQueryPlansReset reset(dbContext);
		value_.setASTNode(const_cast<ASTNode*>(value_.getASTNode())->staticTyping(context));
		_src.add(value_.getASTNode()->getStaticResolutionContext());
	}

	if(value2_.getASTNode() != 0) {
		AutoRunQueryPlansReset reset(dbContext);
		value2_.setASTNode(const_cast<ASTNode*>(value2_.getASTNode())->staticTyping(context));
		_src.add(value2_.getASTNode()->getStaticResolutionContext());
	}

	if(nodeType_ == ImpliedSchemaNode::METADATA) {
		_src.getStaticType().flags = StaticType::DOCUMENT_TYPE;
	} else if(nodeType_ == ImpliedSchemaNode::ATTRIBUTE) {
		_src.getStaticType().flags = StaticType::ATTRIBUTE_TYPE;
	} else {
		_src.getStaticType().flags = StaticType::ELEMENT_TYPE;
	}

	return this;
}

IDS::Ptr RangeQP::execute(OperationContext &context, QueryExecutionContext &qec) const
{
	DBXML_ASSERT(key_.getIndex() != 0); // We assert that resolveIndexes has already been called

	int timezone = ((Manager&)qec.getContext().getManager()).getImplicitTimezone();
	Key key1 = key_.createKey(value_, timezone);
	if(key1.getID1() == 0 || (isParentSet() && key1.getID2() == 0)) {
		key1.setIDsFromNames(context, qec.getContainer(), parentUriName_, childUriName_);
		const_cast<QPKey&>(key_).setID1(key1.getID1());
		const_cast<QPKey&>(key_).setID2(key1.getID2());
	}

	IDS::Ptr ids;
	if(key1.getID1() == 0 || (isParentSet() && key1.getID2() == 0)) {
		ids.reset(new IDS(0));
	}
	else {
		Key key2 = key_.createKey(value2_, timezone);
		ids = qec.getIndexReadCache().getIDS(qec.getContainer(), context, operation_, key1, operation2_, key2);
	}
	logIDs(qec, ids);
	return ids;
}

IndexData::Ptr RangeQP::nodes_execute(OperationContext &context, QueryExecutionContext &qec) const
{
	DBXML_ASSERT(key_.getIndex() != 0); // We assert that resolveIndexes has already been called

	int timezone = ((Manager&)qec.getContext().getManager()).getImplicitTimezone();
	Key key1 = key_.createKey(value_, timezone);
	if(key1.getID1() == 0 || (isParentSet() && key1.getID2() == 0)) {
		key1.setIDsFromNames(context, qec.getContainer(), parentUriName_, childUriName_);
		const_cast<QPKey&>(key_).setID1(key1.getID1());
		const_cast<QPKey&>(key_).setID2(key1.getID2());
	}

	IndexData::Ptr data;
	if(key1.getID1() == 0 || (isParentSet() && key1.getID2() == 0)) {
		data.reset(new IndexData());
	}
	else {
		Key key2 = key_.createKey(value2_, timezone);
		data = qec.getIndexReadCache().getIndexData(qec.getContainer(), context, operation_, key1, operation2_, key2);
	}
	logIndexData(qec, data);
	return data;
}

QueryPlan::Cost RangeQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	DBXML_ASSERT(key_.getIndex() != 0); // We assert that resolveIndexes has already been called

	// usable_page_size= (pagesize-overhead)*page_fill_factor
	//
	// DB4.1 has btree page overhead of 26. Fill factor for a full page
	// is between 50% and 100%... let's guess 75%.
	//
	const Container &container = qec.getContainer();
	double pageSize = (container.getDocumentDB()->getPageSize() - 26) * 0.75;

	int timezone = ((Manager&)qec.getContext().getManager()).getImplicitTimezone();
	Key key = key_.createKey(value_, timezone);
	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		key.setIDsFromNames(context, qec.getContainer(), parentUriName_, childUriName_);
		const_cast<QPKey&>(key_).setID1(key.getID1());
		const_cast<QPKey&>(key_).setID2(key.getID2());
	}

	KeyStatistics statistics = qec.getStatisticsReadCache().getKeyStatistics(qec.getContainer(), context, key);

	Key key2 = key_.createKey(value2_, timezone);

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
QueryPlan *RangeQP::resolveIndexes(const Container &container, const IndexSpecification &is, bool nodeQP,
	bool &success, bool &exact)
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
			return result->resolveIndexes(container, is, nodeQP, success, exact);
		}
	}

	if(isParentSet()) {
		// else convert to n(node-*-equality-*, edge-*-presence) and try again
		IntersectQP *intersectOp = new (memMgr_) IntersectQP(memMgr_);
		intersectOp->addArg(new (memMgr_) PresenceQP(nodeType_, parentUriName_, childUriName_, memMgr_));
		intersectOp->addArg(new (memMgr_) RangeQP(nodeType_, 0, childUriName_, key_, operation_,
							  value_, operation2_, value2_, memMgr_));

		logTransformation(container, intersectOp);
		return intersectOp->resolveIndexes(container, is, nodeQP, success, exact);
	}
	else {
		UniverseQP *result = new (memMgr_) UniverseQP(memMgr_);
		logTransformation(container, result);
		return result->resolveIndexes(container, is, nodeQP, success, exact);
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

QueryPlan *RangeQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	RangeQP *result = new (mm) RangeQP(nodeType_, parentUriName_, childUriName_, key_,
					   operation_, value_, operation2_, value2_, mm);
	return result;
}

string RangeQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<RangeQP";
	if(key_.getIndex() != 0) {
		s << " index=\"" << key_.getIndex().asString() << "\"";
	}
	if(operation_ != DbWrapper::NONE) {
		s << " operation=\"" << DbWrapper::operationToWord(operation_) << "\"";
	}
	if(operation2_ != DbWrapper::NONE) {
		s << " operation2=\"" << DbWrapper::operationToWord(operation2_) << "\"";
	}
	if(parentUriName_ != 0) {
		s << " parent=\"" << parentUriName_ << "\"";
	}
	if(childUriName_ != 0) {
		s << " child=\"" << childUriName_ << "\"";
	}

	if(value_.getASTNode() == 0) {
		s << " value=\"" << string(value_.getValue(), value_.getLength()) << "\"";
	}
	if(value2_.getASTNode() == 0) {
		s << " value2=\"" << string(value2_.getValue(), value2_.getLength()) << "\"";
	}
	if(value_.getASTNode() == 0 && value2_.getASTNode() == 0) {
		s << "/>" << endl;
	} else {
		s << ">" << endl;
		if(value_.getASTNode() != 0) {
			s << DbXmlPrintXQTree::print(value_.getASTNode(), context, indent + INDENT);
		}
		if(value2_.getASTNode() != 0) {
			s << DbXmlPrintXQTree::print(value2_.getASTNode(), context, indent + INDENT);
		}
		s << in << "</RangeQP>" << endl;
	}

	return s.str();
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

DocumentQP::DocumentQP(const std::string &documentName, QueryPlan *qp, XPath2MemoryManager *mm)
	: ValueQP(QueryPlan::DOCUMENT, ImpliedSchemaNode::METADATA, 0, DbXml::metaDataName_uri_name,
		QPKey(Syntax::STRING), DbWrapper::EQUALITY,
		QPValue(Syntax::STRING, (documentName.empty() ? 0 :
				char_clone(documentName.c_str(), mm)),
			documentName.length(), /*generalComp*/false), mm),
	  qp_(qp)
{
	// The metadata-equality-string index on dbxml:name
	// is mandatory, so there's no need to check if it's there.
	Index &index = key_.getIndex();
	index.set(Index::NME | Syntax::STRING);

	if(documentName.empty()) {
		operation_ = DbWrapper::PREFIX;
	}
}

DocumentQP::DocumentQP(const QPKey &key, const QPValue &value, QueryPlan *qp, XPath2MemoryManager *mm)
	: ValueQP(QueryPlan::DOCUMENT, ImpliedSchemaNode::METADATA, 0, DbXml::metaDataName_uri_name,
		key, DbWrapper::EQUALITY, value, mm),
	  qp_(qp)
{
	// The metadata-equality-string index on dbxml:name
	// is mandatory, so there's no need to check if it's there.
	Index &index = key_.getIndex();
	index.set(Index::NME | Syntax::STRING);

	if(value.getValue() == 0) {
		operation_ = DbWrapper::PREFIX;
	}
}

QueryPlan *DocumentQP::staticTyping(StaticContext *context)
{
	ValueQP::staticTyping(context);

	qp_ = qp_->staticTyping(context);
	_src.add(qp_->getStaticResolutionContext());
	_src.getStaticType() = qp_->getStaticResolutionContext().getStaticType();

	return this;
}

IDS::Ptr DocumentQP::execute(OperationContext &context, QueryExecutionContext &qec) const
{
	DBXML_ASSERT(key_.getIndex() != 0); // We assert that resolveIndexes has already been called

	IndexLookups keys;
	getKeys(keys, qec.getDbXmlContext());

	IDS::Ptr result = keys.execute(this, context, qec);

	if(qp_ != 0) {
		Cost cost = qp_->cost(context, qec);
		if(cost.pages < qec.costToFilter(result)) {
			result = result->set_intersection(qp_->execute(context, qec));
			logIDs(qec, result);
		}
	}

	logIDs(qec, result);
	return result;
}

IndexData::Ptr DocumentQP::nodes_execute(OperationContext &context, QueryExecutionContext &qec) const
{
	DBXML_ASSERT(key_.getIndex() != 0); // We assert that resolveIndexes has already been called

	IndexLookups keys;
	getKeys(keys, qec.getDbXmlContext());

	IDS::Ptr docIDs = keys.execute(this, context, qec);

	IndexData::Ptr result;
	if(!docIDs->empty()) {
		IndexData::Ptr nodes(qp_->nodes_execute(context, qec));
		result = new IndexData(*nodes);
		result->set_intersection(docIDs);
	}
	else {
		result = new IndexData;
	}
	
	logIndexData(qec, result);
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

QueryPlan *DocumentQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	DocumentQP *result = new (mm) DocumentQP(key_, value_, (qp_ != 0 ? qp_->copy(mm) : 0), mm);
	return result;
}

string DocumentQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<DocumentQP";
	if(key_.getIndex() != 0) {
		s << " index=\"" << key_.getIndex().asString() << "\"";
	}
	if(operation_ != DbWrapper::NONE) {
		s << " operation=\"" << DbWrapper::operationToWord(operation_) << "\"";
	}
	if(childUriName_ != 0) {
		s << " child=\"" << childUriName_ << "\"";
	}

	if(value_.getASTNode() == 0) {
		s << " value=\"" << string(value_.getValue(), value_.getLength()) << "\"";
	}

	if(value_.getASTNode() == 0 && qp_ == 0) {
		s << "/>" << endl;
	} else {
		s << ">" << endl;
		if(value_.getASTNode() != 0) {
			s << DbXmlPrintXQTree::print(value_.getASTNode(), context, indent + INDENT);
		}
		if(qp_ != 0) {
			s << qp_->printQueryPlan(context, indent + INDENT);
		}
		s << in << "</DocumentQP>" << endl;
	}

	return s.str();
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

QueryPlan *DocumentQP::rootFilter(const ImpliedSchemaNode *root)
{
	if(qp_ != 0) qp_ = qp_->rootFilter(root);
	return this;
}

QueryPlan *DocumentQP::nodeFilter(const ImpliedSchemaNode::Vector &nodes)
{
	if(qp_ != 0) qp_ = qp_->nodeFilter(nodes);
	return this;
}

void DocumentQP::findQueryPlanHolders(QPHSet &qphset) const
{
	if(qp_ != 0) qp_->findQueryPlanHolders(qphset);
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

QueryPlan *DocumentQP::removeNonConstant()
{
	if(qp_ != 0) qp_ = qp_->removeNonConstant();
	return this;
}

QueryPlan *DocumentQP::resolveValues(const Log &log, DbXmlContext *context)
{
	if(qp_ != 0) qp_ = qp_->resolveValues(log, context);
	return this;
}

QueryPlan *DocumentQP::resolveSyntax(const Log &log, DbXmlContext *context)
{
	if(qp_ != 0) qp_ = qp_->resolveSyntax(log, context);
	return this;
}

QueryPlan *DocumentQP::resolveIndexes(const Container &container, const IndexSpecification &is, bool nodeQP,
	bool &success, bool &exact)
{
	if(qp_ != 0) qp_ = qp_->resolveIndexes(container, is, nodeQP, success, exact);
	return this;
}

/////////////////////////////////
// UniverseQP

UniverseQP::UniverseQP(XPath2MemoryManager *mm)
	: PresenceQP(QueryPlan::UNIVERSE, ImpliedSchemaNode::METADATA, 0,
		DbXml::metaDataName_uri_name, QPKey(Syntax::STRING),
		DbWrapper::PREFIX, mm)
{
}

IndexData::Ptr UniverseQP::nodes_execute(OperationContext &context, QueryExecutionContext &qec) const
{
	// You shouldn't execute a universe query plan in nodes mode
	DBXML_ASSERT(false);
	return IndexData::Ptr(new IndexData());
}

bool UniverseQP::isSubsetOf(const QueryPlan *o) const
{
	if(o->getType() == QueryPlan::UNIVERSE) {
		return true;
	}
	return false;
}

QueryPlan *UniverseQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	return new (mm) UniverseQP(mm);
}

string UniverseQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<UniverseQP/>" << endl;

	return s.str();
}

string UniverseQP::toString(bool brief) const
{
	ostringstream s;

	s << "U";

	return s.str();
}

/////////////////////////////////
// EmptyQP

IDS::Ptr EmptyQP::execute(OperationContext &context, QueryExecutionContext &qec) const
{
	IDS::Ptr result(new IDS(0));
	logIDs(qec, result);
	return result;
}

IndexData::Ptr EmptyQP::nodes_execute(OperationContext &context, QueryExecutionContext &qec) const
{
	IndexData::Ptr result(new IndexData());
	logIndexData(qec, result);
	return result;
}

bool EmptyQP::isSubsetOf(const QueryPlan *o) const
{
	return true;
}

QueryPlan *EmptyQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	return new (mm) EmptyQP(mm);
}

string EmptyQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<EmptyQP/>" << endl;

	return s.str();
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
	if(syntax != Syntax::NONE)
		index_.set(syntax, Index::SYNTAX_MASK);
}

Key QPKey::createKey(int timezone) const
{
	Key result(timezone);
	result.setNodeLookup(nodeLookup_);
	result.setIndex(index_);
	result.setID1(id1_);
	result.setID2(id2_);
	return result;
}

Key QPKey::createKey(const QPValue &value, int timezone) const
{
	Key result(timezone);
	result.setNodeLookup(nodeLookup_);
	result.setIndex(index_);
	result.setID1(id1_);
	result.setID2(id2_);
	result.setValue(value.getValue(), value.getLength());
	return result;
}

Key QPKey::createKey(const char *value, size_t length, int timezone) const
{
	Key result(timezone);
	result.setNodeLookup(nodeLookup_);
	result.setIndex(index_);
	result.setID1(id1_);
	result.setID2(id2_);
	result.setValue(value, length);
	return result;
}

/////////////////////////////////
// QPValue

QPValue::QPValue(Syntax::Type syntax, const char *value, size_t vlen, bool generalComp)
	: syntax_(syntax),
	  value_(value),
	  len_(vlen),
	  di_(0),
	  generalComp_(generalComp)
{
}

QPValue::QPValue(Syntax::Type syntax, const ASTNode *di, bool generalComp)
	: syntax_(syntax),
	  value_(0),
	  len_(0),
	  di_(di),
	  generalComp_(generalComp)
{
}

bool QPValue::equals(const QPValue &o) const
{
	if(di_ != 0)
		return di_ == o.di_ &&
			generalComp_ == o.generalComp_;
	else return syntax_ == o.syntax_ &&
		     char_equals(value_, len_, o.value_, o.len_) &&
		     generalComp_ == o.generalComp_;
}

std::string QPValue::asString() const
{
	ostringstream oss;

	if(di_ != 0) {
		oss << "[to be calculated]";
	}
	else if(value_ != 0) {
		oss << string(value_, len_);
	}

	return oss.str();
}
