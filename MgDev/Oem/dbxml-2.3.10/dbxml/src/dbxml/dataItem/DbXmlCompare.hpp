//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlCompare.hpp,v 1.19 2006/11/21 18:49:43 jsnelson Exp $
//

#ifndef __DBXMLCOMPARE_HPP
#define	__DBXMLCOMPARE_HPP

#include "QueryPlanHolder.hpp"
#include "QueryPlanResultImpl.hpp"
#include "DbXmlASTNode.hpp"
#include "Join.hpp"

#include <xqilla/operators/GeneralComp.hpp>
#include <xqilla/operators/Equals.hpp>
#include <xqilla/operators/NotEquals.hpp>
#include <xqilla/operators/LessThan.hpp>
#include <xqilla/operators/LessThanEqual.hpp>
#include <xqilla/operators/GreaterThan.hpp>
#include <xqilla/operators/GreaterThanEqual.hpp>

namespace DbXml
{

class DbXmlCompare : public DbXmlASTNode, public QueryPlanHolder
{
public:
	class ValueCompareResult : public ResultImpl
	{
	public:
		ValueCompareResult(const Result &parent, const DbXmlCompare *comp);
		Item::Ptr next(DynamicContext *context);
		void skip();
		std::string asString(DynamicContext *context, int indent) const;
	private:
		Result parent_;
		const DbXmlCompare *compare_;
	};

	class GeneralCompareResult : public ResultImpl
	{
	public:
		GeneralCompareResult(const Result &parent, const DbXmlCompare *comp);
		Item::Ptr next(DynamicContext *context);
		void skip();
		std::string asString(DynamicContext *context, int indent) const;
	private:
		Result parent_;
		const DbXmlCompare *compare_;
	};

	DbXmlCompare(GeneralComp::ComparisonOperation op, bool generalComp, ASTNode *arg, XPath2MemoryManager* memMgr);
	DbXmlCompare(Join::Type join, const DbXmlCompare *cmp, XPath2MemoryManager* memMgr);
	DbXmlCompare(Join::Type join, DbXmlNodeTest *nodeTest, const DbXmlCompare *cmp,
		XPath2MemoryManager* memMgr);

	virtual ASTNode *staticTyping(StaticContext *context);
	virtual DbXmlResult createResult(const DbXmlResult &contextItems, unsigned int props,
		DynamicContext* context) const;

	Join::Type getJoinType() const { return joinType_; }
	const DbXmlNodeTest *getNodeTest() const { return nodeTest_; }

	bool isGeneralComp() const { return generalComp_; }
	GeneralComp::ComparisonOperation getOperation() const { return operation_; }
	const ASTNode *getArgument() const { return arg_; }
	void setArgument(ASTNode *a) { arg_ = a; }

	const XMLCh *getCastURI() const { return castURI_; }
	const XMLCh *getCastName() const { return castName_; }
	void setCastType(const XMLCh *uri, const XMLCh *name) { castURI_ = uri; castName_ = name; }
	
private:
	class QueryPlanValueCompareResult : public QueryPlanResultImpl
	{
	public:
		QueryPlanValueCompareResult(const DbXmlResult &parent, const DbXmlCompare *comp, bool useQP);
		void setResult(const DbXmlNodeImpl *node, DynamicContext *context);
		std::string asString(DynamicContext *context, int indent) const;
	private:
		const DbXmlCompare *compare_;
	};

	class QueryPlanGeneralCompareResult : public QueryPlanResultImpl
	{
	public:
		QueryPlanGeneralCompareResult(const DbXmlResult &parent, const DbXmlCompare *comp, bool useQP);
		void setResult(const DbXmlNodeImpl *node, DynamicContext *context);
		std::string asString(DynamicContext *context, int indent) const;
	private:
		const DbXmlCompare *compare_;
	};

	Join::Type joinType_;
	DbXmlNodeTest *nodeTest_;
	GeneralComp::ComparisonOperation operation_;
	bool generalComp_;
	ASTNode *arg_;

	const XMLCh *castURI_, *castName_;
};

class DbXmlGeneralComp : public GeneralComp
{
public:
	DbXmlGeneralComp(GeneralComp *comp, XPath2MemoryManager *mm);

	QueryPlanHolder &getArg0QPH() { return arg0qph_; }
	QueryPlanHolder &getArg1QPH() { return arg1qph_; }

private:
	QueryPlanHolder arg0qph_;
	QueryPlanHolder arg1qph_;
};

class DbXmlEquals : public Equals
{
public:
	DbXmlEquals(Equals *comp, XPath2MemoryManager *mm);

	QueryPlanHolder &getArg0QPH() { return arg0qph_; }
	QueryPlanHolder &getArg1QPH() { return arg1qph_; }

private:
	QueryPlanHolder arg0qph_;
	QueryPlanHolder arg1qph_;
};

class DbXmlNotEquals : public NotEquals
{
public:
	DbXmlNotEquals(NotEquals *comp, XPath2MemoryManager *mm);

	QueryPlanHolder &getArg0QPH() { return arg0qph_; }
	QueryPlanHolder &getArg1QPH() { return arg1qph_; }

private:
	QueryPlanHolder arg0qph_;
	QueryPlanHolder arg1qph_;
};

class DbXmlLessThan : public LessThan
{
public:
	DbXmlLessThan(LessThan *comp, XPath2MemoryManager *mm);

	QueryPlanHolder &getArg0QPH() { return arg0qph_; }
	QueryPlanHolder &getArg1QPH() { return arg1qph_; }

private:
	QueryPlanHolder arg0qph_;
	QueryPlanHolder arg1qph_;
};

class DbXmlLessThanEqual : public LessThanEqual
{
public:
	DbXmlLessThanEqual(LessThanEqual *comp, XPath2MemoryManager *mm);

	QueryPlanHolder &getArg0QPH() { return arg0qph_; }
	QueryPlanHolder &getArg1QPH() { return arg1qph_; }

private:
	QueryPlanHolder arg0qph_;
	QueryPlanHolder arg1qph_;
};

class DbXmlGreaterThan : public GreaterThan
{
public:
	DbXmlGreaterThan(GreaterThan *comp, XPath2MemoryManager *mm);

	QueryPlanHolder &getArg0QPH() { return arg0qph_; }
	QueryPlanHolder &getArg1QPH() { return arg1qph_; }

private:
	QueryPlanHolder arg0qph_;
	QueryPlanHolder arg1qph_;
};

class DbXmlGreaterThanEqual : public GreaterThanEqual
{
public:
	DbXmlGreaterThanEqual(GreaterThanEqual *comp, XPath2MemoryManager *mm);

	QueryPlanHolder &getArg0QPH() { return arg0qph_; }
	QueryPlanHolder &getArg1QPH() { return arg1qph_; }

private:
	QueryPlanHolder arg0qph_;
	QueryPlanHolder arg1qph_;
};

}

#endif
